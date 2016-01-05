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
#define FILES_PATH  QDir::homePath() + "/ToontownRewritten/"
#define PLATFORM    "linux2"
#define ENGINE_FILENAME "TTREngine"

//for windows support (not used yet because I don't used Windows, included in case someone would like to compile it)
#elif defined(Q_OS_WIN)
#define FILES_PATH  ""
#define ENGINE_FILENAME "TTREngine.exe"
#define PLATFORM    "win32"

//For OS X support (not used because no mac to compile and test on)
#elif defined(Q_OS_MAC)
#define FILES_PATH  ""
#define ENGINE_FILENAME ""
#define PLATFORM    "darwin"

#else
#error "Unsupported platform."
#endif

//patch manifest URL
#define PATCH_MANIFEST_URL "https://cdn.toontownrewritten.com/content/patchmanifest.txt"

//content distribution URL
#define CDN_URL "https://cdn.toontownrewritten.com/content/"

//cache directory for web apps and temporary downloads of updated files
#define CACHE_DIR   FILES_PATH + "cache/"
