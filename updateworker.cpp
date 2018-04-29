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
#include "utilities.h"

#include <QJsonObject>
#include <QJsonArray>
#include <QString>
#include <QByteArray>
#include <QFile>
#include <QUrl>
#include <QDir>
#include <QSettings>

UpdateWorker::UpdateWorker(QObject *parent) : QObject(parent)
{
    QSettings settings("Shticker-Book-Rewritten", "Shticker-Book-Rewritten");

    settings.beginGroup("FilesPath");
    filePath = settings.value("path").toString();
    settings.endGroup();

    filePath = filePath + QString("/");
    cachePath = QString(filePath + ".cache/");
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
    qDebug() << "Received patch manifest";

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
        qDebug() << "Checking" << fileName << "for updates";

        if(platforms.contains(PLATFORM))
        {
            QString localBz2FileName = cachePath + fileObject["dl"].toString();
            QString localFileName = filePath + fileName;
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
                    fileHash = getHash(localFileName);
                    qDebug() << "Local file's hash is:" << fileHash;

                    //Check if the hash matches the patch manifest
                    if(fileHash == fileObject["hash"].toString())
                    {
                        qDebug() << "Hash matches patch manifest, skipping update\n";
                        fileIsOld = false;
                    }
                    else
                    {
                        qDebug() << "Hash does not match patch manifest, checking for a patch";

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

                                QString patchFileName = cachePath + patchObject["filename"].toString();
                                QString extractedFileName = patchFileName + ".decomp";

                                //get the patch file
                                getNewFile(patchObject["filename"].toString(), patchFileName, extractedFileName, patchObject["compPatchHash"].toString());

                                //verify the downloaded patch is good
                                if(getHash(extractedFileName) == patchObject["patchHash"].toString())
                                {
                                    qDebug() << "Downloaded patch matches manifest";
                                }
                                else
                                {
                                    qDebug() << "Downloaded patch does not match manifet";
                                }

                                //patch the file
                                qDebug() << "extracted name:" << extractedFileName << "old name:" << localFileName;
                                patchWorker->patchFile(extractedFileName,localFileName);
                                hasBeenPatched = true;

                                //check if the patch updated the file fully
                                if(getHash(localFileName) == fileObject["hash"].toString())
                                {
                                    qDebug() << "File has been patched fully\n";
                                    fileIsOld = false;
                                }
                                else
                                {
                                    qDebug() << "Downloaded file fails integrity check, looping again to check for patches.  This is try" << i << "of 5";
                                }

                                //delete the patch files
                                QFile::remove(patchFileName);
                                QFile::remove(extractedFileName);
                            }
                        }
                        //check if it was patched and if not no patch is available but still out of date so try to get a new copy
                        if(hasBeenPatched == false)
                        {
                            qDebug() << "No patch found, downloading a fresh copy";

                            getNewFile(fileObject["dl"].toString(), localBz2FileName, localFileName, fileObject["compHash"].toString());

                            //check to make sure it is up to date
                            fileHash = getHash(localFileName);
                            if(fileHash == fileObject["hash"].toString())
                            {
                                qDebug() << "Downloaded file's integrity has been verified\n";
                                fileIsOld = false;
                            }
                            else
                            {
                                qDebug() << "Downloaded file fails integrity check, looping again to check for patches.  This is try" << i << "of 5";
                            }
                        }
                        //delete the zipped file
                        QFile::remove(localBz2FileName);
                    }
                }
                else
                {
                    //file doesn't exist, download new one
                    getNewFile(fileObject["dl"].toString(), localBz2FileName, localFileName, fileObject["compHash"].toString());

                    //check to make sure it is up to date
                    fileHash = getHash(localFileName);
                    if(fileHash == fileObject["hash"].toString())
                    {
                        qDebug() << "Downloaded file's integrity has been verified\n";
                        fileIsOld = false;
                    }
                    else
                    {
                        qDebug() << "Downloaded file fails integrity check, looping again to check for patches.  This is try" << i << "of 5";
                    }

                    //delete the zipped file
                    QFile::remove(localBz2FileName);
                }
            }
        }
        else
        {
            qDebug() << "Skipping, it is not for our platform\n";
        }

        currentFileNum++;
    }

    emit sendMessage(QString("All files are up to date!"));
    emit hideProgressBar();
    emit updateComplete();

    //on Linux platforms we need to check if TTREngine is executable since it isn't by default
    #ifdef Q_OS_LINUX
        #include <QFileInfo>

        QFileInfo engine(QString(filePath + "TTREngine"));

        if(engine.isExecutable())
        {
            qDebug() << "TTREngine is already executable\n";
        }
        else
        {
            qDebug() << "TTREngine is not executable, setting it\n";
            QFile engineFile(QString(filePath + "TTREngine"));
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
    QString downloadFileName = cachePath + fileToDownload;

    qDebug() << "Downloading" << fileToDownload;

    connect(downloadWorker, SIGNAL(hideProgressBar()), this, SLOT(relayHideProgressBar()));
    connect(downloadWorker, SIGNAL(showProgressBar()), this, SLOT(relayShowProgressBar()));
    connect(downloadWorker, SIGNAL(sendDownloadProgress(qint64,qint64)), this, SLOT(relayDownloadProgress(qint64,qint64)));

    //retreive the file
    bool sucessfulDownload = downloadWorker->getFile(CDN_URL + fileToDownload, downloadFileName);

    if (sucessfulDownload)
    {
        qDebug() << "Successfully downloaded file" << downloadFileName;
    }
    else
    {
        qDebug() << "Error downloding file" << downloadFileName << endl;
    }
}

void UpdateWorker::getNewFile(QString dlFile, QString localBz2FileName, QString localFileName, QString compHash)
{
    //get the file
    startDownload(dlFile);

    //make sure download succeeded, logging for debugging purposes but still extracting because TTR has been known to not update hashes
    QByteArray fileHash = getHash(localBz2FileName);

    if(fileHash == compHash)
    {
        qDebug() << "Integrity of" << dlFile << "is valid.";
    }
    else
    {
        qDebug() << "Failure verifying the integrity of" << dlFile;
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
