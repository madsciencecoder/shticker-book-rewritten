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
