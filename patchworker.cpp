/*-
 * The following code is a derivative work of the code from bsdiff.
 * The original author's copyright and license is stated below.
 *
 * Copyright 2003-2005 Colin Percival
 * All rights reserved
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted providing that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 *__________________________________________________________________________________
 *
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

#include "patchworker.h"
#include "hashworker.h"
#include "libraries/qslog/QsLog.h"

#include <QFile>

#include <bzlib.h>
#include <fcntl.h>
#include <unistd.h>


PatchWorker::PatchWorker(QObject *parent) : QObject(parent)
{

}

int PatchWorker::patchFile(QString patchFile, QString targetFile)
{
    //apply a temporary .tmp extension for the final patched file, will remove when finished
    QString newFileTemp = targetFile + ".tmp";

    bsdiff_patch((char *) patchFile.toStdString().c_str(), (char *) targetFile.toStdString().c_str(), (char *) newFileTemp.toStdString().c_str());

    //replace the old file with the patched one
    QFile::remove(targetFile);
    QFile file(newFileTemp);
    file.rename(targetFile);

//    //check to make sure the patch is good
//    HashWorker *hashWorker = new HashWorker(this);
//    if(hashWorker->getHash(targetFile) != patchedHash)
//    {
//        QLOG_ERROR() << "Hash of extracted file does not match the most up to date";

//        return 1;
//    }

    return 0;
}

int PatchWorker::bsdiff_patch(char *patchFile, char *oldFile, char *newFile)
{
    FILE * f, * cpf, * dpf, * epf;
    BZFILE * cpfbz2, * dpfbz2, * epfbz2;
    int cbz2err, dbz2err, ebz2err;
    int fd;
    ssize_t oldsize,newsize;
    ssize_t bzctrllen,bzdatalen;
    u_char header[32],buf[8];
    u_char *old, *new_;
    off_t oldpos,newpos;
    off_t ctrl[3];
    off_t lenread;
    off_t i;

    /* Open patch file */
    if ((f = fopen(patchFile, "r")) == NULL)
    {
        QLOG_ERROR() << "Unable to open patch file.";

        return 1;
    }

    /*
    File format:
        0	8	"BSDIFF40"
        8	8	X
        16	8	Y
        24	8	sizeof(newfile)
        32	X	bzip2(control block)
        32+X	Y	bzip2(diff block)
        32+X+Y	???	bzip2(extra block)
    with control block a set of triples (x,y,z) meaning "add x bytes
    from oldfile to x bytes from the diff block; copy y bytes from the
    extra block; seek forwards in oldfile by z bytes".
    */

    /* Read header */
    if (fread(header, 1, 32, f) < 32)
    {
        QLOG_ERROR() << "Unable to read patch file's header.";

        return 1;
    }

    /* Check for appropriate magic */
    if (memcmp(header, "BSDIFF40", 8) != 0)
    {
        QLOG_ERROR() << "Unable to determine patch's version / invalid patch";

        return 1;
    }

    /* Read lengths from header */
    bzctrllen=offtin(header+8);
    bzdatalen=offtin(header+16);
    newsize=offtin(header+24);
    if((bzctrllen<0) || (bzdatalen<0) || (newsize<0))
    {
        QLOG_ERROR() << "Unable to determine patch's size / invalid patch";

        return 1;
    }

    /* Close patch file and re-open it via libbzip2 at the right places */
    if (fclose(f))
    {
        QLOG_ERROR() << "Unable to close patch file";

        return 1;
    }
    if ((cpf = fopen(patchFile, "r")) == NULL)
    {
        QLOG_ERROR() << "Unable to open patch file with bzip2";

        return 1;
    }
    if (fseeko(cpf, 32, SEEK_SET))
    {
        QLOG_ERROR() << "Unable to seek to the patch data";

        return 1;
    }
    if ((cpfbz2 = BZ2_bzReadOpen(&cbz2err, cpf, 0, 0, NULL, 0)) == NULL)
    {
        QLOG_ERROR() << "Unable to read with bzip2, bzerror =" << cbz2err;

        return 1;
    }
    if ((dpf = fopen(patchFile, "r")) == NULL)
    {
        QLOG_ERROR() << "Unable to open patch file with bzip2 /2";

        return 1;
    }
    if (fseeko(dpf, 32 + bzctrllen, SEEK_SET))
    {
        QLOG_ERROR() << "Unable to seek to the patch data /2";

        return 1;
    }
    if ((dpfbz2 = BZ2_bzReadOpen(&dbz2err, dpf, 0, 0, NULL, 0)) == NULL)
    {
        QLOG_ERROR() << "Unable to read with bzip2 /2, bzerror =" << dbz2err;

        return 1;
    }
    if ((epf = fopen(patchFile, "r")) == NULL)
    {
        QLOG_ERROR() << "Unable to open patch file with bzip2 /3";

        return 1;
    }
    if (fseeko(epf, 32 + bzctrllen + bzdatalen, SEEK_SET))
    {
        QLOG_ERROR() << "Unable to seek to the patch data /3";

        return 1;
    }
    if ((epfbz2 = BZ2_bzReadOpen(&ebz2err, epf, 0, 0, NULL, 0)) == NULL)
    {
        QLOG_ERROR() << "Unable to read with bzip2 /3, bzerror =" << ebz2err;

        return 1;
    }

    if(((fd=open(oldFile,O_RDONLY,0))<0) || ((oldsize=lseek(fd,0,SEEK_END))==-1) || ((old=(u_char*)malloc(oldsize+1))==NULL) ||
            (lseek(fd,0,SEEK_SET)!=0) || (read(fd,old,oldsize)!=oldsize) || (close(fd)==-1))
    {
        QLOG_ERROR() << "Unable to open the old file";

        return 1;
    }

    if((new_=(u_char*)malloc(newsize+1))==NULL)
    {
        QLOG_ERROR() << "Unable to allocate the new space";

        return 1;
    }

    //start the actual patching
    oldpos=0;newpos=0;
    while(newpos<newsize)
    {
        /* Read control data */
        for(i=0;i<=2;i++) {
            lenread = BZ2_bzRead(&cbz2err, cpfbz2, buf, 8);
            if ((lenread < 8) || ((cbz2err != BZ_OK) && (cbz2err != BZ_STREAM_END)))
            {
                QLOG_ERROR() << "Corrupt patch, unable to read control data";

                return 1;
            }
            ctrl[i]=offtin(buf);
        };

        /* Sanity-check */
        if(newpos+ctrl[0]>newsize)
        {
            QLOG_ERROR() << "Corrupt patch, failed during sanity check 1";

            return 1;
        }

        /* Read diff string */
        lenread = BZ2_bzRead(&dbz2err, dpfbz2, new_ + newpos, ctrl[0]);
        if ((lenread < ctrl[0]) || ((dbz2err != BZ_OK) && (dbz2err != BZ_STREAM_END)))
        {
            QLOG_ERROR() << "Corrupt patch, unable to read diff string";

            return 1;
        }

        /* Add old data to diff string */
        for(i=0;i<ctrl[0];i++)
        {
            if((oldpos+i>=0) && (oldpos+i<oldsize))
            {
                new_[newpos+i]+=old[oldpos+i];
            }
        }

        /* Adjust pointers */
        newpos+=ctrl[0];
        oldpos+=ctrl[0];

        /* Sanity-check */
        if(newpos+ctrl[1]>newsize)
        {
            QLOG_ERROR() << "Corrupt patch, failed during sanity check 2";

            return 1;
        }

        /* Read extra string */
        lenread = BZ2_bzRead(&ebz2err, epfbz2, new_ + newpos, ctrl[1]);
        if ((lenread < ctrl[1]) || ((ebz2err != BZ_OK) && (ebz2err != BZ_STREAM_END)))
        {
            QLOG_ERROR() << "Corrupt patch, unable to read extra string";

            return 1;
        }

        /* Adjust pointers */
        newpos+=ctrl[1];
        oldpos+=ctrl[2];
    };

    /* Clean up the bzip2 reads */
    BZ2_bzReadClose(&cbz2err, cpfbz2);
    BZ2_bzReadClose(&dbz2err, dpfbz2);
    BZ2_bzReadClose(&ebz2err, epfbz2);

    if (fclose(cpf) || fclose(dpf) || fclose(epf))
    {
        QLOG_ERROR() << "Error closing patch files";

        return 1;
    }

    /* Write the new file */
    if(((fd=open(newFile,O_CREAT|O_TRUNC|O_WRONLY,0666))<0) || (write(fd,new_,newsize)!=newsize) || (close(fd)==-1))
    {
        QLOG_ERROR() << "Error writing patched file";

        return 1;
    }

    free(new_);
    free(old);


    return 0;
}


off_t PatchWorker::offtin(u_char *buf)
{
    off_t y;

    y=buf[7]&0x7F;
    y=y*256;y+=buf[6];
    y=y*256;y+=buf[5];
    y=y*256;y+=buf[4];
    y=y*256;y+=buf[3];
    y=y*256;y+=buf[2];
    y=y*256;y+=buf[1];
    y=y*256;y+=buf[0];

    if(buf[7]&0x80) y=-y;

    return y;
}
