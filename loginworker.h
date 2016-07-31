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

#ifndef LOGINWORKER_H
#define LOGINWORKER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QEventLoop>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QTimer>
#include <QProcess>

class LoginWorker : public QObject
{
    Q_OBJECT
public:
    explicit LoginWorker(QObject *parent = 0);

signals:
    void sendMessage(QString);
    void gameStarted();
    void gameFinished(int, QByteArray);
    void authenticationFailed();

public slots:
    void initiateLogin(QString, QString);
    void receiveToken(QString);
    void timerFinished();
    void gameHasStarted();
    void gameHasFinished(int);

private:
    QNetworkAccessManager *networkManager;
    QNetworkReply *reply;
    QEventLoop waitForFinished;
    QString receivedToken;
    QString authToken;
    QJsonDocument jsonDocument;
    QJsonObject jsonObject;
    QJsonParseError *parseError;
    QTimer *timer;
    QString lineToken;
    QProcess *gameProcess;
    QString filePath;

    QByteArray loginApiWorker(QByteArray);
    void startTwoFactorAuthentication();
    void authenticationLoop();
    void startGame(QString, QString);
};

#endif // LOGINWORKER_H
