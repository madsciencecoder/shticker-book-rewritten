/*
 * Copyright (c) 2015-2016 Joshua Snyder
 * Distributed under the GNU GPL v3. For full terms see the file LICENSE
 *
 * This file is part of Shticker Book Rewritten.
 *
 * Shticker Book Rewritten is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Shticker Book Rewritten is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Shticker Book Rewritten.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "launcherwindow.h"
#include "ui_launcherwindow.h"
#include "globaldefines.h"
#include "updateworker.h"
#include "libraries/qslog/QsLog.h"

#include <QDir>
#include <QMessageBox>
#include <QCloseEvent>
#include <QProcess>
#include <QThread>

LauncherWindow::LauncherWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::LauncherWindow)
{
    gameInstances = 0;

    ui->setupUi(this);

    //check to make sure the cache directory exists and make it if it doesn't
    if(!QDir(FILES_PATH).exists())
    {
        QDir().mkdir(FILES_PATH);
    }
    if(!QDir(CACHE_DIR).exists())
    {
        QDir().mkdir(CACHE_DIR);
    }

    //allow QWebView to store data locally and create a cache for faster loading
    QWebSettings::globalSettings()->setAttribute(QWebSettings::LocalStorageEnabled, true);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::NotificationsEnabled, true);
    QWebSettings::globalSettings()->setLocalStoragePath(CACHE_DIR);
    //enable desktop notifications
    connect(ui->invasionsWebview->page(), SIGNAL(featurePermissionRequested(QWebFrame*,QWebPage::Feature)), this, SLOT(notificationsRequested(QWebFrame*,QWebPage::Feature)));

    QLOG_DEBUG() << "Webview cache is being stored at: " << CACHE_DIR << endl;

    //disable login until files are updated
    emit enableLogin(false);
    loginIsReady = false;

    //Begin updating the game files
    updateThread = new QThread;
    UpdateWorker *updateWorker = new UpdateWorker();
    //make a new thread for the updating process since it can bog down the main thread and make the window unresponsive
    updateWorker->moveToThread(updateThread);

    connect(updateThread, SIGNAL(started()), updateWorker, SLOT(startUpdating()));
    connect(updateWorker, SIGNAL(updateComplete()), updateThread, SLOT(quit()));
    connect(updateWorker, SIGNAL(updateComplete()), updateWorker, SLOT(deleteLater()));
    connect(updateWorker, SIGNAL(destroyed(QObject*)), updateThread, SLOT(deleteLater()));

    //allow the update worker to communicate with the main window
    connect(updateWorker, SIGNAL(sendMessage(QString)), this, SLOT(relayMessage(QString)));
    connect(updateWorker, SIGNAL(sendProgressBarReceived(int)), this, SLOT(relayProgressBarReceived(int)));
    connect(updateWorker, SIGNAL(showProgressBar()), this, SLOT(relayShowProgressBar()));
    connect(updateWorker, SIGNAL(hideProgressBar()), this, SLOT(relayHideProgressBar()));
    connect(updateWorker, SIGNAL(updateComplete()), this, SLOT(loginReady()));

    updateThread->start();
}

LauncherWindow::~LauncherWindow()
{
    delete ui;
}

void LauncherWindow::relayMessage(QString message)
{
    emit sendMessage(message);
}

void LauncherWindow::relayProgressBarReceived(int receivedBytes)
{
    emit sendProgressBarReceived(receivedBytes);
}

void LauncherWindow::relayShowProgressBar()
{
    emit showProgressBar();
}

void LauncherWindow::relayHideProgressBar()
{
    emit hideProgressBar();
}

void LauncherWindow::loginReady()
{
    emit enableLogin(true);
    loginIsReady = true;
}

void LauncherWindow::initiateLogin()
{
    if(loginIsReady)
    {
        QLOG_DEBUG() << "Initiating login sequence\n";

        //disable login again to prevent duplicate logins
        emit enableLogin(false);
        loginIsReady = false;

        loginWorker = new LoginWorker(this);

        connect(loginWorker, SIGNAL(sendMessage(QString)), this, SLOT(relayMessage(QString)));
        connect(loginWorker, SIGNAL(gameStarted()), this, SLOT(gameHasStarted()));
        connect(loginWorker, SIGNAL(gameFinished()), this, SLOT(gameHasFinished()));
        connect(loginWorker, SIGNAL(authenticationFailed()), this, SLOT(authenticationFailed()));

        //start login and then the game
        loginWorker->initiateLogin(ui->usernameBox->text(), ui->passwordBox->text());
    }
    else
    {
        QLOG_DEBUG() << "Login isn't ready, ignoring login event\n";
    }
}

void LauncherWindow::gameHasStarted()
{
    //clear the username and password boxes to prevent accidental relaunching of the game and to be ready to launch another
    ui->usernameBox->clear();
    ui->passwordBox->clear();

    //increment to show how many instances are running
    gameInstances++;

    QLOG_DEBUG() << "New game instance, there are now" << gameInstances;

    //enable login again now that the game has finished starting
    loginReady();
}

void LauncherWindow::gameHasFinished()
{
    //increment to show how many instances are running
    gameInstances--;

    QLOG_DEBUG() << "Game instance has closed, there are now" << gameInstances;
}

void LauncherWindow::authenticationFailed()
{
    emit enableLogin(true);
    loginIsReady = true;
}

//confirm closing of the launcher to alert of closing all running games
void LauncherWindow::closeEvent(QCloseEvent *event)
{
    //check if any game instances are running
    if(gameInstances!=0)
    {
        //create a dialog box to confirm exit and warn about running instances
        QMessageBox::StandardButton dialog;
        dialog = QMessageBox::warning(this,
                                      "Please confirm closing.",
                                      "Are you sure you would like to close?  Closing the launcher when any game instance is running will cause it to close.  There are currently "
                                      + QString::number(gameInstances) + " instances running.",
                                      QMessageBox::Yes | QMessageBox::No);

        if( dialog == QMessageBox::Yes)
        {
            close();
        }
        else
        {
            event->ignore();
        }
    }

    //no running instances so we can just close
    else
    {
        exit(0);
    }
}

void LauncherWindow::notificationsRequested(QWebFrame *frame, QWebPage::Feature feature)
{
    if(feature == QWebPage::Notifications)
    {
        ui->invasionsWebview->page()->setFeaturePermission(frame, feature, QWebPage::PermissionGrantedByUser);
    }
}
