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

#include "hashworker.h"

#include <QFile>
#include <QCryptographicHash>
#include <QDebug>

HashWorker::HashWorker(QObject *parent) : QObject(parent)
{

}

//Generates the SHA1 checksum of the passed file
QByteArray HashWorker::getHash(QString fileName)
{
    QString hashString;
    QFile file(fileName);
    QByteArray hashArray;
    QCryptographicHash *sha1Hash;

    //Attempt to open the file
    if(!file.open(QFile::ReadOnly))
    {
        qDebug() << "Unable to read file:" << fileName << ". Error:" << file.errorString() << "\n";
    }
    else
    {
        //Load the file's data to process
        QByteArray fileContents = file.readAll();

        hashArray = sha1Hash->hash(fileContents, QCryptographicHash::Sha1);
    }

    //clean up the file since we are done with it
    file.flush();
    file.close();
    file.deleteLater();

    //return the checksum in hexidecimal format since that is what the manifest requires
    return hashArray.toHex();
}
