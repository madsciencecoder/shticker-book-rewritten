## Shticker Book Rewritten

Custom launcher for the MMORPG Toontown Rewritten.  Named after the in game shticker book which provides access to various settings and data.  It tries to provide an all in one tool to provide useful tools to make playing easier.

It is designed with being cross-platform in mind but I have not yet had a chance to build it or test for Mac OS X.  If someone would like to, it should just be a matter of changing a few defines in globaldefines.h and configuring the build environment.

### Windows Installer

There is now an official installer for Windows.  Please check the releases page to download the most recent version.  Please note that it does not check for updates for itself yet so be sure to either keep an eye out or you can follow the repository for email updates.

### Linux Packages

I have created packages through OpenSuse's OBS for the most popular distributions.  There are currently packages for OpenSuse 42.1 and Tumbleweed, Fedora 22 and 23, Debian 8, and Ubuntu 15.04 and 15.10.

You can get the packages at https://software.opensuse.org/download.html?project=home%3Amadsciencecoder%3Ashticker-book-rewritten&package=shticker-book-rewritten.  Just click your distribution and follow the instructions for your version.

### Features

* Automatic downloading and patching of game files
* Linux version will store game files in the user's home under the folder ToontownRewritten.  This is to allow Schticker Book Rewritten to be installed like a normal package in root but still allow it write access to update the game files.
* Built in group tracker via www.toonhq.org
* Built in invasion tracker via www.toonhq.org
* Built in fishing guide via http://siggen.toontown-click.de/fishadvisor/en/ponds.html
* Unlimited number of toons may be launched from the same launcher, no need to open one per toon
* Warns if you close the launcher while a game instance is running as it will cause the game to close if the launcher is closed

#### Planned Features

* Standalone invasion tracker.  More features can be added to the tracker and there is no need to rely on ToonHQ like it is necessary for the groups due to lack of an API.  The notifications also don't work 100% with QWebKit.
* Content pack installer.  Currently you must manually place the content pack inside a folder called `resources` in the game's folder.  For Linux this is ToontownRewritten inside the user's home folder.

### How to Compile

This program relies on 2 external libraries: Qt 5 and libbzip2.  QsLog and bsdiff are also used but are embedded into the project.  QsLog is provided as a git submodule so you will need to pull it manually or use git clone --recursive.

#### Linux based distros

##### Ubuntu / Debian

Required dependency packages: `qt5-qmake libqt5webkit5-dev build-essential libbz2-dev`

To build and install:
```
export QT_SELECT=qt5
qmake
make
sudo make install
```

##### OpenSuse

Required dependency packages: `libbz2-devel libQt5WebKitWidgets-devel`

To build and install:
```
qmake-qt5
make
sudo make install
````

##### Fedora

Required dependency packages: `qt5-qtbase-devel qt5-qtwebkit-devel gcc-c++ bzip2-devel`

To build and install:
```
qmake-qt5
make
sudo make install
````

##### Arch

TODO: add dependency packages and link to AUR PKGBUILD

#### Windows

The easiest way to compile this on Windows is to download Qt from their website (https://www.qt.io/download-open-source/) and install with at least Qt 5.5 and MinGW.  You will need to manually supply a built bzip2 library for Qt to use which you can download a pre-compiled one from http://sourceforge.net/projects/mingw/files/MinGW/Extension/bzip2/bzip2-1.0.6-4/.  You will need both the dev and dll-2 downloads.  Then copy the files to the Qt folders containing the build libraries.  You will also need openssl DLLs which can be downloaded from http://slproweb.com/products/Win32OpenSSL.html.  To make handling DLLs easier you may wish to add the Qt bin directory to your Windows PATH environment variable.

#### Mac OS X

Theoretically it may work.  I do not own a Mac or have access to one, however, so I can't really help too much in that regard.  Most of the differences OS X would need in the code are already programmed to replace Linux specific code, however the paths and file names likely would need tweaking in globaldefines.h. 
