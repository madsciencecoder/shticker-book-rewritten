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

#ifndef UPDATEWORKER_H
#define UPDATEWORKER_H

#include "jsonworker.h"

#include <QObject>
#include <QJsonDocument>


class UpdateWorker : public QObject
{
    Q_OBJECT
public:
    explicit UpdateWorker(QObject *parent = 0);

signals:
    void sendMessage(QString);
    void sendProgressBarReceived(int);
    void hideProgressBar();
    void showProgressBar();
    void updateComplete();

public slots:
    void startUpdating();

private slots:
    void relayDownloadProgress(qint64, qint64);
    void patchManifestReady(QJsonDocument);
    void relayShowProgressBar();
    void relayHideProgressBar();

private:
    JsonWorker *jsonWorker;
    QString filePath;
    QString cachePath;

    void startDownload(QString);
    void getNewFile(QString, QString, QString, QString);
};

#endif // UPDATEWORKER_H
