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

#include "extractionworker.h"
#include "libraries/qslog/QsLog.h"

#include <bzlib.h>
#include <QFile>

ExtractionWorker::ExtractionWorker(QObject *parent) : QObject(parent)
{

}

bool ExtractionWorker::extractBz2(QString compressedFileName, QString extractedFileName)
{
    QLOG_DEBUG() << "Extracting" << compressedFileName << "to" << extractedFileName;

    FILE *compressedFile;
    BZFILE *bzFile;
    int bzError;
    int bufferBytesRead;
    char buffer[1024];
    QByteArray decompressedData;
    QFile extractedFile(extractedFileName);

    //open the destination file
    if(!extractedFile.open(QFile::WriteOnly))
    {
        QLOG_ERROR() << "Unable to open" << extractedFile.fileName();
        return false;
    }

    //open the compressed file
    compressedFile = fopen(compressedFileName.toStdString().c_str(), "rb");
    if(!compressedFile)
    {
        QLOG_ERROR() << "Error opening file: " << compressedFileName << endl;

        return false;
    }

    bzFile = BZ2_bzReadOpen(&bzError, compressedFile, 0, 0, NULL, 0);
    if(bzError != BZ_OK)
    {
        QLOG_ERROR() << "Error opening file to extract:" << compressedFileName;
        return false;
    }

    //decompress the file in segments to save RAM
    while(bzError == BZ_OK)
    {
        //decompress the file in increments of 1024 bytes
        bufferBytesRead = BZ2_bzRead(&bzError, bzFile, buffer, 1024);
        if(bzError == BZ_OK || bzError == BZ_STREAM_END)
        {
            for(int i=0; i<bufferBytesRead; i++)
                decompressedData.append(buffer[i]); //read the buffer into a QByteArray

            //write the data to the local file
            extractedFile.write(decompressedData);

            //clear the QByteArray for the next set of buffer data
            decompressedData.clear();
        }
    }

    //purge the extracted file now that we are done with it
    extractedFile.flush();
    extractedFile.close();
    extractedFile.deleteLater();

    //check if there were any errors while decompressing
    if(bzError != BZ_STREAM_END)
    {
        BZ2_bzReadClose(&bzError, bzFile);
        QLOG_ERROR() << "Error reading compressed file" << compressedFileName << ". BZ error code:" << bzError << endl;

        return false;
    }
    else
    {
        BZ2_bzReadClose(&bzError, bzFile);
    }

    QLOG_DEBUG() << "Finished extracting" << extractedFileName;
    return true;
}
