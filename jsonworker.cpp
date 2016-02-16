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

#include "jsonworker.h"
#include "libraries/qslog/QsLog.h"

#include <QByteArray>
#include <QNetworkRequest>
#include <QJsonParseError>
#include <QUrl>

JsonWorker::JsonWorker(QObject *parent) : QObject(parent)
{

}

//Retrieves the passed url and creates a JSON document to work with various APIs
void JsonWorker::startRequest(QUrl url)
{
    QLOG_DEBUG() << "Downloading file:" << url.url();

    networkManager = new QNetworkAccessManager(this);           //Network manager to handle downloading the JSON document

    reply = networkManager->get(QNetworkRequest(url));
    connect(reply, SIGNAL(finished()), this, SLOT(requestFinished()));
}

//Gets called when the request finishes and creates the JSON document then emits it
void JsonWorker::requestFinished()
{
    QByteArray rawData;
    QJsonDocument jsonDocument;
    QJsonParseError *parseError = new QJsonParseError();

    //Check for any errors getting the file
    if(reply->error())
    {
        QLOG_ERROR() << "Error downloading file:" << reply->errorString();
    }
    else
    {
        rawData = reply->readAll();

        QLOG_DEBUG() << "Successfully retreived file";
    }

    reply->deleteLater();   //delete file to prevent using too much ram

    jsonDocument = QJsonDocument::fromJson(rawData, parseError);   //parse the downloaded file into a JSON array

    //Check for errors during parsing
    if(parseError->error != QJsonParseError::NoError)
    {
        QLOG_ERROR() << "Error parsing JSON file:" << parseError->errorString() << endl;
    }
    else
    {
        QLOG_DEBUG() << "Successfully parsed JSON document\n";
    }

    emit documentReady(jsonDocument);
}
