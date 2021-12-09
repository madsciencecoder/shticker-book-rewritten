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

#ifndef GLOBALDEFINES_H
#define GLOBALDEFINES_H

#endif // GLOBALDEFINES_H

//Platform Specific Configurations
#ifdef Q_OS_LINUX
#define DEFAULT_PATH QDir::homePath() + QString("/ToontownRewritten")
#define PLATFORM    "linux2"
#define ENGINE_FILENAME QString("./TTREngine")

//for windows support (64bit)
#elif defined(Q_OS_WIN64)
#define DEFAULT_PATH QString(QDir::currentPath()) + QString("/Game-Files")
#define ENGINE_FILENAME QString("TTREngine64.exe")
#define PLATFORM    "win64"

//for windows support (32bit)
#elif defined(Q_OS_WIN)
#define DEFAULT_PATH QString(QDir::currentPath()) + QString("/Game-Files")
#define ENGINE_FILENAME QString("TTREngine.exe")
#define PLATFORM    "win32"

//For OS X support (not used because no mac to compile and test on)
#elif defined(Q_OS_MAC)
#define DEFAULT_PATH QDir::homePath() + QString("/Library/Application Support/Toontown Rewritten")
#define LIBRARY_PATH QString("./Libraries.bundle")
#define ENGINE_FILENAME QString("\"./Toontown Rewritten\"")
#define PLATFORM "darwin"

#else
#error "Unsupported platform."
#endif

//patch manifest URL
#define PATCH_MANIFEST_URL "https://cdn.toontownrewritten.com/content/patchmanifest.txt"

//content distribution URL
#define CDN_URL "http://s3.amazonaws.com/download.toontownrewritten.com/patches/"
