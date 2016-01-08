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

#include "updateworker.h"
#include "globaldefines.h"
#include "extractionworker.h"
#include "downloadworker.h"
#include "patchworker.h"
#include "libraries/qslog/QsLog.h"

#include <QJsonObject>
#include <QJsonArray>
#include <QString>
#include <QByteArray>
#include <QFile>
#include <QUrl>
#include <QDir>

UpdateWorker::UpdateWorker(QObject *parent) : QObject(parent)
{
    hashWorker = new HashWorker(this);
}

void UpdateWorker::startUpdating()
{
    emit sendMessage(QString("Checking for outdated game files."));

    jsonWorker = new JsonWorker(this);

    //download the patch manifest
    connect(jsonWorker, SIGNAL(documentReady(QJsonDocument)), this, SLOT(patchManifestReady(QJsonDocument)));
    jsonWorker->startRequest(QUrl(PATCH_MANIFEST_URL));
}

void UpdateWorker::patchManifestReady(QJsonDocument patchManifest)
{
    QLOG_DEBUG() << "Received patch manifest";

    //begin parsing the patch manifest
    QJsonObject manifestObject = patchManifest.object();
    QStringList fileNamesList = manifestObject.keys();      //get a list of all the filenames in the JSON file
    QString fileName;                                       //used to store the filename we are working on in the following foreach loop
    QByteArray fileHash;

    //stores the file number we are working on in the foreach loop
    int currentFileNum = 1;

    //run through each filename and determine if we need to update that file
    foreach(fileName, fileNamesList)
    {
        QJsonObject fileObject = manifestObject[fileName].toObject();   //create an object for each file in the JSON document
        QJsonArray platforms = fileObject["only"].toArray();            //look for the platform the file is for

        emit sendMessage(QString("Updating file ") + QString::number(currentFileNum) + QString (" of ") + QString::number(fileNamesList.size()) + QString(" (") + fileName + QString(")"));
        QLOG_DEBUG() << "Checking" << fileName << "for updates";

        if(platforms.contains(PLATFORM))
        {
            QString localBz2FileName = CACHE_DIR + fileObject["dl"].toString();
            QString localFileName = FILES_PATH + fileName;
            QFile file(localFileName);
            bool fileIsOld = true;
            int i = 0;

            //loop to run until the file is fully updated or maximum of 5 times to prevent infinite loops
            while(fileIsOld && i<5)
            {
                i++;
            //check to see if the file already exists or not
            if(file.exists())
                {
                    //generate the SHA1 hash of the file to compare with the patch manifest
                    fileHash = hashWorker->getHash(localFileName);
                    QLOG_DEBUG() << "Local file's hash is:" << fileHash;

                    //Check if the hash matches the patch manifest
                    if(fileHash == fileObject["hash"].toString())
                    {
                        QLOG_DEBUG() << "Hash matches patch manifest, skipping update\n";
                        fileIsOld = false;
                    }
                    else
                    {
                        QLOG_DEBUG() << "Hash does not match patch manifest, checking for a patch";

                        QJsonObject patchListObject = fileObject["patches"].toObject();
                        QStringList patchList = patchListObject.keys();
                        QString manifestHash;
                        bool hasBeenPatched = false;

                        foreach(manifestHash, patchList)
                        {
                            //check if the patch applies to our hash
                            if(fileHash == manifestHash)
                            {
                                QJsonObject patchObject = patchListObject[manifestHash].toObject();
                                PatchWorker *patchWorker = new PatchWorker(this);

                                QString patchFileName = CACHE_DIR + patchObject["filename"].toString();
                                QString extractedFileName = patchFileName + ".decomp";

                                //get the patch file
                                getNewFile(patchObject["filename"].toString(), patchFileName, extractedFileName, patchObject["compPatchHash"].toString());

                                //verify the downloaded patch is good
                                if(hashWorker->getHash(extractedFileName) == patchObject["patchHash"].toString())
                                {
                                    QLOG_INFO() << "Downloaded patch matches manifest";
                                }
                                else
                                {
                                    QLOG_ERROR() << "Downloaded patch does not match manifet";
                                }

                                //patch the file
                                QLOG_INFO() << "extracted name:" << extractedFileName << "old name:" << localFileName;
                                patchWorker->patchFile(extractedFileName,localFileName);
                                hasBeenPatched = true;

                                //check if the patch updated the file fully
                                if(hashWorker->getHash(localFileName) == fileObject["hash"].toString())
                                {
                                    QLOG_INFO() << "File has been patched fully\n";
                                    fileIsOld = false;
                                }
                                else
                                {
                                    QLOG_ERROR() << "Downloaded file fails integrity check, looping again to check for patches.  This is try" << i << "of 5";
                                }
                            }
                        }
                        //check if it was patched and if not no patch is available but still out of date so try to get a new copy
                        if(hasBeenPatched == false)
                        {
                            QLOG_DEBUG() << "No patch found, downloading a fresh copy";

                            getNewFile(fileObject["dl"].toString(), localBz2FileName, localFileName, fileObject["compHash"].toString());

                            //check to make sure it is up to date
                            fileHash = hashWorker->getHash(localFileName);
                            if(fileHash == fileObject["hash"].toString())
                            {
                                QLOG_DEBUG() << "Downloaded file's integrity has been verified\n";
                                fileIsOld = false;
                            }
                            else
                            {
                                QLOG_ERROR() << "Downloaded file fails integrity check, looping again to check for patches.  This is try" << i << "of 5";
                            }
                        }
                    }
                }
                else
                {
                    //file doesn't exist, download new one
                    getNewFile(fileObject["dl"].toString(), localBz2FileName, localFileName, fileObject["compHash"].toString());

                    //check to make sure it is up to date
                    fileHash = hashWorker->getHash(localFileName);
                    if(fileHash == fileObject["hash"].toString())
                    {
                        QLOG_DEBUG() << "Downloaded file's integrity has been verified\n";
                        fileIsOld = false;
                    }
                    else
                    {
                        QLOG_ERROR() << "Downloaded file fails integrity check, looping again to check for patches.  This is try" << i << "of 5";
                    }
                }
            }
        }
        else
        {
            QLOG_DEBUG() << "Skipping, it is not for our platform\n";
        }

        currentFileNum++;
    }

    emit sendMessage(QString("All files are up to date!"));
    emit hideProgressBar();
    emit updateComplete();

    //on Linux platforms we need to check if TTREngine is executable since it isn't by default
    #ifdef Q_OS_LINUX
        #include <QFileInfo>

        QFileInfo engine(QString(FILES_PATH + "TTREngine"));

        if(engine.isExecutable())
        {
            QLOG_DEBUG() << "TTREngine is already executable\n";
        }
        else
        {
            QLOG_DEBUG() << "TTREngine is not executable, setting it\n";
            QFile engineFile(QString(FILES_PATH + "TTREngine"));
            engineFile.setPermissions(QFile::ExeOwner | QFile::ReadUser | QFile::WriteOwner);

            engineFile.flush();
            engineFile.close();
            engineFile.deleteLater();
        }
    #endif
}

void UpdateWorker::startDownload(QString fileToDownload)
{
    DownloadWorker *downloadWorker = new DownloadWorker(this);
    QString downloadFileName = CACHE_DIR + fileToDownload;

    QLOG_DEBUG() << "Downloading" << fileToDownload;

    connect(downloadWorker, SIGNAL(hideProgressBar()), this, SLOT(relayHideProgressBar()));
    connect(downloadWorker, SIGNAL(showProgressBar()), this, SLOT(relayShowProgressBar()));
    connect(downloadWorker, SIGNAL(sendDownloadProgress(qint64,qint64)), this, SLOT(relayDownloadProgress(qint64,qint64)));

    //retreive the file
    bool sucessfulDownload = downloadWorker->getFile(CDN_URL + fileToDownload, downloadFileName);

    if (sucessfulDownload)
    {
        QLOG_DEBUG() << "Successfully downloaded file" << downloadFileName;
    }
    else
    {
        QLOG_ERROR() << "Error downloding file" << downloadFileName << endl;
    }
}

void UpdateWorker::getNewFile(QString dlFile, QString localBz2FileName, QString localFileName, QString compHash)
{
    //get the file
    startDownload(dlFile);

    //make sure download succeeded, logging for debugging purposes but still extracting because TTR has been known to not update hashes
    QByteArray fileHash = hashWorker->getHash(localBz2FileName);

    if(fileHash == compHash)
    {
        QLOG_INFO() << "Integrity of" << dlFile << "is valid.";
    }
    else
    {
        QLOG_ERROR() << "Failure verifying the integrity of" << dlFile;
    }

    //extract the file
    emit sendMessage(QString("Extracting " + dlFile));
    ExtractionWorker *extractionWorker = new ExtractionWorker(this);

    extractionWorker->extractBz2(localBz2FileName, localFileName);

    //delete the worker now that we are done with it
    extractionWorker->deleteLater();
}

void UpdateWorker::relayDownloadProgress(qint64 bytesReceived, qint64 totalBytes)
{
    //convert the progress into a percentage since QProgressBar only uses an int
    emit sendProgressBarReceived((bytesReceived * 100) / totalBytes);
}

void UpdateWorker::relayHideProgressBar()
{
    emit hideProgressBar();
}

void UpdateWorker::relayShowProgressBar()
{
    emit showProgressBar();
}
