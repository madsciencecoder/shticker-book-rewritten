## Shticker Book Rewritten

Custom launcher for the MMORPG Toontown Rewritten.  Named after the in game shticker book which provides access to various settings and data.  It tries to provide an all in one tool to provide useful tools to make playing easier.

It supports all platforms that Toontown Rewritten does.  MacOS users need to manually allow the app to run by opening the Applications folder in Finder and opening it by pressing the command key while opening the app.  This is because Apple forces apps to be signed but charges for the ability to sign them.

### Screenshots

Colors and theme may be different depending on the user's platform and Qt theme.

![](https://raw.githubusercontent.com/madsciencecoder/Shticker-Book-Rewritten/master/pictures/launcher-tab.png)
![](https://raw.githubusercontent.com/madsciencecoder/Shticker-Book-Rewritten/master/pictures/groups-tab.png)
![](https://raw.githubusercontent.com/madsciencecoder/Shticker-Book-Rewritten/master/pictures/invasions-tab.png)
![](https://raw.githubusercontent.com/madsciencecoder/Shticker-Book-Rewritten/master/pictures/fishing-tab.png)
![](https://raw.githubusercontent.com/madsciencecoder/Shticker-Book-Rewritten/master/pictures/about-tab.png)

### Windows Installer

There is now an official installer for Windows.  Please check the releases page to download the most recent version.  Please note that it does not check for updates for itself yet so be sure to either keep an eye out or you can follow the repository for email updates.

### Linux Packages

I have created packages through OpenSuse's OBS for the most popular distributions.  There are currently packages for OpenSuse, Fedora, Debian, and Ubuntu.  If there is a version without a repository please open an issue as I may have forgot to update them.

You can get the packages at https://software.opensuse.org/download.html?project=home%3Amadsciencecoder%3Ashticker-book-rewritten&package=shticker-book-rewritten.  Just click your distribution and follow the instructions for your version.

Note: The packages available have been patched to replace webengine with webkit in the distros that do not provide webengine packages (Debian, Ubuntu, and Fedora).

### Features

* Automatic downloading and patching of game files
* Linux version stores the game files in the user's home under the folder ToontownRewritten.  This is to allow Schticker Book Rewritten to be installed like a normal package in root but still allow it write access to update the game files.
* Built in group tracker via www.toonhq.org
* Built in invasion tracker via www.toonhq.org
* Built in fishing guide via http://siggen.toontown-click.de/fishadvisor/en/ponds.html
* Unlimited number of toons may be launched from the same launcher, no need to open one per toon
* Warns if you close the launcher while a game instance is running as it will cause the game to close if the launcher is closed
* Option to save login details for an unlimited number of accounts
* Option to turn off automatic game file updates on startup to allow you to login faster
* Option to update the game files manually

#### Planned Features

* Standalone invasion tracker.  More features can be added to the tracker and there is no need to rely on ToonHQ like it is necessary for the groups due to lack of an API.  The notifications also don't work 100% with QWebKit.
* Content pack installer.  Currently you must manually place the content pack inside a folder called `resources` in the game's folder.  For Linux this is ToontownRewritten inside the user's home folder.
* Self updating to new versions of the launcher (Windows only since Linux has good package management already)

### How to Compile

This program relies on 2 external libraries: Qt 5 and libbzip2.  Bsdiff is also used but are embedded into the project.

#### Linux based distros

##### Ubuntu / Debian

Required dependency packages: `qt5-qmake build-essential libbz2-dev`.  Qt's webengine is necessary but they do not provide packages.

To build and install:
```
export QT_SELECT=qt5
qmake
make
sudo make install
```

##### OpenSuse

Required dependency packages: `libbz2-devel libqt5-qtwebengine-devel`

To build and install:
```
qmake-qt5
make
sudo make install
````

##### Fedora

Required dependency packages: `qt5-qtbase-devel qt5-qtwebengine-devel gcc-c++ bzip2-devel`.  Qt's webengine is necessary but they do not provide packages until Fedora 24 is released.

To build and install:
```
qmake-qt5
make
sudo make install
````

##### Arch

Required dependency packages: `bzip2 qt5-base qt5-webengine`.

AUR package for the latest git version: https://aur.archlinux.org/packages/shticker-book-rewritten-git/.

#### Windows

The easiest way to compile this on Windows is to download Qt from their website (https://www.qt.io/download-open-source/) and install with at least Qt 5.5 and MinGW.  You will need to manually supply a built bzip2 library for Qt to use which you can download a pre-compiled one from http://sourceforge.net/projects/mingw/files/MinGW/Extension/bzip2/bzip2-1.0.6-4/.  You will need both the dev and dll-2 downloads.  Then copy the files to the Qt folders containing the build libraries.  You will also need openssl DLLs which can be downloaded from http://slproweb.com/products/Win32OpenSSL.html.  To make handling DLLs easier you may wish to add the Qt bin directory to your Windows PATH environment variable.

#### Mac OS X

Please check the releases page to download the most recent version.  You may need to manually allow the app to run by opening the Applications folder in Finder and opening it by pressing the command key while opening the app.  This is because Apple forces apps to be signed but charges for the ability to sign them.  This only needs to be done once, after that you can launch the app like normal.
