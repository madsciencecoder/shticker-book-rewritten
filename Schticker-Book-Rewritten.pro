#-------------------------------------------------
#
# Project created by QtCreator 2015-12-29T13:25:12
#
#-------------------------------------------------
#   Copyright (c) 2015-2016 Joshua Snyder
#   Distributed under the GNU GPL v3. For full terms see the file LICENSE
#
#   This file is part of Shticker Book Rewritten.
#
#   Shticker Book Rewritten is free software: you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation, either version 3 of the License, or
#   (at your option) any later version.
#
#   Shticker Book Rewritten is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with Shticker Book Rewritten.  If not, see <http://www.gnu.org/licenses/>.

QT       += core gui webkitwidgets network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = shticker-book-rewritten
TEMPLATE = app


SOURCES += main.cpp\
    launcherwindow.cpp \
    updateworker.cpp \
    jsonworker.cpp \
    extractionworker.cpp \
    downloadworker.cpp \
    loginworker.cpp \
    twofactorwindow.cpp \
    patchworker.cpp \
    hashworker.cpp

HEADERS  += launcherwindow.h \
    globaldefines.h \
    updateworker.h \
    jsonworker.h \
    extractionworker.h \
    downloadworker.h \
    loginworker.h \
    twofactorwindow.h \
    patchworker.h \
    hashworker.h

FORMS    += launcherwindow.ui \
    twofactorwindow.ui

RESOURCES += \
    resources.qrc

LIBS += -lbz2

include(libraries/qslog/QsLog.pri)

DISTFILES += LICENSE
unix:!mac {
    isEmpty(PREFIX):PREFIX = /usr
    BINDIR = $$PREFIX/bin
    INSTALLS += target
    target.path = $$BINDIR
    DATADIR = $$PREFIX/share
    PKGDATADIR = $$DATADIR/shticker-book-rewritten
    DEFINES += DATADIR=\\\"$$DATADIR\\\" \
        PKGDATADIR=\\\"$$PKGDATADIR\\\"
    INSTALLS += desktop \
        icon
    desktop.path = $$DATADIR/applications
    desktop.files += shticker-book-rewritten.desktop
    icon.path = $$DATADIR/shticker-book-rewritten/icon
    icon.files += resources/shticker-book-rewritten.png
}
