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

#include "downloadworker.h"
#include "globaldefines.h"

#include <QUrl>
#include <QDir>
#include <QNetworkRequest>
#include <QEventLoop>

DownloadWorker::DownloadWorker(QObject *parent) : QObject(parent)
{

}

//returns true if successful and false if there was an error
bool DownloadWorker::getFile(QUrl url, QString fileName)
{
    //Create a temporary file to download into to save RAM
    file = new QFile(fileName);

    //open the file to write into
    if(file->open(QIODevice::WriteOnly))
    {
        qDebug() << "Getting the bz2 file" << file->fileName();
    }
    else
    {
        qDebug() << "Unable to open or write to" << file->fileName();
        delete file;
        file = 0;
        return false;
    }

    //show the progress bar
    emit showProgressBar();

    networkManager = new QNetworkAccessManager(this);
    QEventLoop waitForFinished;

    //start downloading the file
    reply = networkManager->get(QNetworkRequest(url));

    //connect the reply to our event loop to wait for the download to finish
    connect(reply, SIGNAL(finished()), &waitForFinished, SLOT(quit()));
    //send the data information to our progress bar
    connect(reply, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(relayDownloadProgress(qint64, qint64)));
    connect(reply, SIGNAL(readyRead()), this, SLOT(httpReadyRead()));

    //start the event loop to wait for the download to finish
    waitForFinished.exec();

    //hide the progress bar again now that we are done
    emit hideProgressBar();

    //close the file and flush it from memory
    file->flush();
    file->close();
    delete file;

    if(reply->error())
    {
        qDebug() << "There was an error downloading" << url << "Error:" << reply->errorString() << endl;

        reply->deleteLater();

        return false;
    }
    else
    {
        reply->deleteLater();

        return true;
    }
}

void DownloadWorker::relayDownloadProgress(qint64 receivedBytes, qint64 totalBytes)
{
    emit sendDownloadProgress(receivedBytes, totalBytes);
}

void DownloadWorker::httpReadyRead()
{
    //whenever there is new data from the reply we write it to the file to prevent using too much RAM
    if(file)
    {
        file->write(reply->readAll());
    }
}
