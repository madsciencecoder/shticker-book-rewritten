#ifndef FILELOCATIONCHOOSER_H
#define FILELOCATIONCHOOSER_H

#include <QDialog>
#include <QObject>

namespace Ui {
class FileLocationChooser;
}

class FileLocationChooser : public QDialog
{
    Q_OBJECT

public:
    explicit FileLocationChooser();
    ~FileLocationChooser();

signals:
    void finished();

public slots:

private slots:
    void pathChosen();
    void chooseFile();

private:
    Ui::FileLocationChooser *ui;
};

#endif // FILELOCATIONCHOOSER_H
