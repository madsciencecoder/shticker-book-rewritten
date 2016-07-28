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
#include "filelocationchooser.h"

#include <QDir>
#include <QMessageBox>
#include <QCloseEvent>
#include <QProcess>
#include <QThread>
#include <QSettings>
#include <QEventLoop>

LauncherWindow::LauncherWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::LauncherWindow)
{
    gameInstances = 0;

    ui->setupUi(this);

    //read the previous settings
    readSettings();

    //check if the user has already chosen a file location for the game files
    while(filePath == "/")
    {
        setFilePath();
    }

    //setup saved toons
    ui->savedToonsBox->addItem("Saved logins");
    ui->savedToonsBox->addItems(savedUsers);

    connect(ui->savedToonsBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(fillCredentials(QString)));

    //setup the webviews
    ui->newsWebview->setUrl(QUrl("https://www.toontownrewritten.com/news/launcher"));
    ui->fishWebview->setUrl(QUrl("http://siggen.toontown-click.de/fishadvisor/en/fishes.html"));
    ui->invasionsWebview->setUrl(QUrl("http://toonhq.org/invasions/"));
    ui->groupsWebview->setUrl(QUrl("http://toonhq.org/groups/"));

    //change news view to a dark background since text is white
    connect(ui->newsWebview->page(), SIGNAL(loadFinished(bool)), this, SLOT(newsViewLoaded()));

    //disable login until files are updated
    emit enableLogin(false);
    loginIsReady = false;

    updateFiles();
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
        qDebug() << "Initiating login sequence\n";

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
        qDebug() << "Login isn't ready, ignoring login event\n";
    }
}

void LauncherWindow::gameHasStarted()
{
    //check whether to save the credentials or not
    if(ui->saveCredentialsBox->isChecked())
    {
        //check to see if it already exists
        if(savedUsers.contains(ui->usernameBox->text()))
        {
            int i;
            i = savedUsers.indexOf(ui->usernameBox->text());

            //replace the password for this username
            savedPasses.replace(i, ui->passwordBox->text());
        }
        else
        {
            savedUsers.append(ui->usernameBox->text());
            savedPasses.append(ui->passwordBox->text());

            ui->savedToonsBox->addItem(ui->usernameBox->text());
        }

        //uncheck the box now
        ui->saveCredentialsBox->setChecked(false);
        writeSettings();
    }

    //clear the username and password boxes to prevent accidental relaunching of the game and to be ready to launch another
    ui->usernameBox->clear();
    ui->passwordBox->clear();
    ui->savedToonsBox->setCurrentIndex(0);

    //increment to show how many instances are running
    gameInstances++;

    qDebug() << "New game instance, there are now" << gameInstances;

    //enable login again now that the game has finished starting
    loginReady();
}

void LauncherWindow::gameHasFinished()
{
    //increment to show how many instances are running
    gameInstances--;

    qDebug() << "Game instance has closed, there are now" << gameInstances;
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
            //save the current settings before quitting
            writeSettings();
            exit(0);
        }
        else
        {
            event->ignore();
        }
    }

    //no running instances so we can just close
    else
    {
        //save the current settings before quitting
        writeSettings();

        exit(0);
    }
}

void LauncherWindow::newsViewLoaded()
{
    ui->newsWebview->page()->runJavaScript(QString("document.body.style.backgroundColor = \"#141618\";"));
}

void LauncherWindow::writeSettings()
{
    QSettings settings("Shticker-Book-Rewritten", "Shticker-Book-Rewritten");

    settings.beginGroup("LauncherWindow");
    settings.setValue("size", size());
    settings.setValue("pos", pos());
    settings.endGroup();

    settings.beginGroup("Logins");
    settings.setValue("username", savedUsers);
    settings.setValue("pass", savedPasses);
    settings.endGroup();
}

void LauncherWindow::readSettings()
{
    QSettings settings("Shticker-Book-Rewritten", "Shticker-Book-Rewritten");

    settings.beginGroup("LauncherWindow");
    resize(settings.value("size", QSize(400, 400)).toSize());
    move(settings.value("pos", QPoint(200, 200)).toPoint());
    settings.endGroup();

    settings.beginGroup("Logins");
    savedUsers = settings.value("username").toStringList();
    savedPasses = settings.value("pass").toStringList();
    settings.endGroup();

    readSettingsPath();
}

void LauncherWindow::readSettingsPath()
{
    QSettings settings("Shticker-Book-Rewritten", "Shticker-Book-Rewritten");

    settings.beginGroup("FilesPath");
    filePath = settings.value("path").toString();
    settings.endGroup();

    filePath = filePath + QString("/");
    cachePath = filePath + QString(".cache/");
}

void LauncherWindow::fillCredentials(QString username)
{
    if(username == "Saved logins")
    {
        ui->usernameBox->clear();
        ui->passwordBox->clear();
        return;
    }

    else
    {
        int i;
        i = savedUsers.indexOf(username);

        ui->usernameBox->setText(username);
        ui->passwordBox->setText(savedPasses.at(i));
    }

}

void LauncherWindow::updateFiles()
{
    //check to make sure the cache directory exists and make it if it doesn't
    if(!QDir(filePath).exists())
    {
        QDir().mkdir(filePath);
    }
    if(!QDir(cachePath).exists())
    {
        QDir().mkdir(cachePath);
    }

    //Begin updating the game files
    updateThread = new QThread(this);
    UpdateWorker *updateWorker = new UpdateWorker();
    //make a new thread for the updating process since it can bog down the main thread and make the window unresponsive
    updateWorker->moveToThread(updateThread);

    connect(updateThread, SIGNAL(started()), updateWorker, SLOT(startUpdating()));
    connect(updateWorker, SIGNAL(updateComplete()), updateThread, SLOT(quit()));

    //allow the update worker to communicate with the main window
    connect(updateWorker, SIGNAL(sendMessage(QString)), this, SLOT(relayMessage(QString)));
    connect(updateWorker, SIGNAL(sendProgressBarReceived(int)), this, SLOT(relayProgressBarReceived(int)));
    connect(updateWorker, SIGNAL(showProgressBar()), this, SLOT(relayShowProgressBar()));
    connect(updateWorker, SIGNAL(hideProgressBar()), this, SLOT(relayHideProgressBar()));
    connect(updateWorker, SIGNAL(updateComplete()), this, SLOT(loginReady()));

    updateThread->start();
}

void LauncherWindow::changeFilePath()
{
    //disable login until files are updated
    emit enableLogin(false);
    loginIsReady = false;

    setFilePath();
    updateFiles();
}

void LauncherWindow::setFilePath()
{
    FileLocationChooser *chooser = new FileLocationChooser;
    chooser->show();
    chooser->activateWindow();

    //wait until a path is chosen
    QEventLoop waitForPath;
    connect(chooser, SIGNAL(finished()), &waitForPath, SLOT(quit()));
    connect(chooser, SIGNAL(rejected()), &waitForPath, SLOT(quit()));
    waitForPath.exec();

    chooser->deleteLater();

    readSettingsPath();
}
