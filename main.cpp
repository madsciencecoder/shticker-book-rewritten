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

#include "launcherwindow.h"
#include "libraries/qslog/QsLog.h"
#include "libraries/qslog/QsLogDest.h"
#include "globaldefines.h"

#include <QApplication>
#include <QDir>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //initialize logging
    QsLogging::Logger &logger = QsLogging::Logger::instance();

    //set logging level and file name
    logger.setLoggingLevel(QsLogging::TraceLevel);

    // Create log destinations
    QsLogging::DestinationPtr fileDestination(QsLogging::DestinationFactory::MakeFileDestination(LOG_FILE));
    QsLogging::DestinationPtr debugDestination(QsLogging::DestinationFactory::MakeDebugOutputDestination());

    // set log destinations on the logger
    logger.addDestination(debugDestination);
    logger.addDestination(fileDestination);


    LauncherWindow w;
    w.show();

    return a.exec();
}
