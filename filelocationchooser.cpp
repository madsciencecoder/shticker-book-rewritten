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

#include "filelocationchooser.h"
#include "globaldefines.h"
#include "ui_filelocationchooser.h"

#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QSettings>

FileLocationChooser::FileLocationChooser() : QDialog(), ui(new Ui::FileLocationChooser)
{
    ui->setupUi(this);


    ui->filePathEdit->setText(DEFAULT_PATH);

    connect(ui->browseButton, SIGNAL(clicked()), this, SLOT(chooseFile()));
    connect(ui->saveButton, SIGNAL(clicked()), this, SLOT(pathChosen()));
}

FileLocationChooser::~FileLocationChooser()
{
    delete ui;
}

void FileLocationChooser::pathChosen()
{
    //make sure the user has write permissions
    QFileInfo info(ui->filePathEdit->text());

    if(info.isWritable() && info.isReadable() && info.isExecutable())
    {
        QSettings settings("Shticker-Book-Rewritten", "Shticker-Book-Rewritten");

        settings.beginGroup("FilesPath");
        settings.setValue("path", ui->filePathEdit->text());
        settings.endGroup();

        emit finished();
    }

    else
    {
        QMessageBox::warning(this,
                                  "Invalid Folder",
                                  "The folder you have chosen is invalid.  Please make sure your user has read, write, and execute permissions.",
                                   QMessageBox::Ok);
    }
}

void FileLocationChooser::chooseFile()
{
    QString path = QFileDialog::getExistingDirectory(0, "Game file location", QDir::homePath());

    ui->filePathEdit->setText(path);
}
