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

#ifndef TWOFACTORWINDOW_H
#define TWOFACTORWINDOW_H

#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QDialog>
#include <QObject>

namespace Ui {
class TwoFactorWindow;
}

class TwoFactorWindow : public QDialog
{
    Q_OBJECT

public:
    explicit TwoFactorWindow(QString);
    ~TwoFactorWindow();

signals:
    void sendToken(QString);

protected slots:
    void cancel();
    void tokenReceived();

private:
    QLineEdit *codeBox;
    QPushButton *cancelButton;
    QPushButton *okButton;
    QLabel *twoFactorMessage;

    Ui::TwoFactorWindow *ui;
};

#endif // TWOFACTORWINDOW_H
