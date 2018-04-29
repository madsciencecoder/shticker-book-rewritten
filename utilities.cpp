#include "utilities.h"

#include<QString>
#include<QFile>
#include<QCryptographicHash>
#include<QDebug>

QByteArray getHash(QString fileName)
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
