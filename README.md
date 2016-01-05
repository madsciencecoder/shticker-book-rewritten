## Shticker Book Rewritten

Custom launcher for the MMORPG Toontown Rewritten.  Named after the in game Schticker book which provides access to various settings and data.  It tries to provide an all in one tool to provide useful tools to make playing easier.

Currently the only supported platform is Linux.  It is designed with being cross-platform in mind but I have not yet had a chance to build it or test for Mac OS X or Windows.  If someone would like to build it for either platform it should just be a matter of changing a few defines in globaldefines.h and configuring the build environment.

### Features

* Automatic downloading and patching of game files
* Linux version will store game files in the user's home under the folder ToontownRewritten.  This is to allow Schticker Book Rewritten to be installed like a normal package in root but still allow it write access to update the game files.
* Built in group tracker via www.toonhq.org
* Built in invasion tracker via www.toonhq.org
* Built in fishing guide via http://siggen.toontown-click.de/fishadvisor/en/ponds.html
* Unlimited number of toons may be launched from the same launcher, no need to open one per toon
* Warns if you close the launcher while a game instance is running as it will cause the game to close if the launcher is closed

#### Planned Features

* Standalone invasion tracker.  More features can be added to the tracker and there is no need to rely on ToonHQ like it is necessary for the groups due to lack of an API.  The notifications also doesn't work 100% with QWebKit.
* Content pack installer.  Currently you must manually place the content pack inside a folder called `resources` in the game's folder.  For Linux this is ToontownRewritten inside the user's home folder.

### How to Compile

This program relies on 2 external libraries: Qt 5 and libbzip2.  QsLog and bsdiff are also used but are embedded into the project.  QsLog is provided as a git submodule so you will need to pull it manually or use git clone --recursive.

#### Linux based distros

##### Ubuntu / Debian

TODO: add dependency packages and link to .deb

##### Fedora/OpenSuse or other RPM based

TODO: add dependency packages and link to .rpm

##### Arch

TODO: add dependency packages and link to AUR PKGBUILD

#### Windows

It should be possible to compile this for Windows.  Qt is very cross platform and all libraries used should compile on Windows.  That being said I have not yet compiled it and have little interest in doing so.  If someone would like help getting it compiled feel free to ask if you run into difficulties.  If there is enough interest on porting it to Windows I might look into it more.  Most, if not all, of the differences Windows would need in the code are already programmed to replace Linux specific code.

#### Mac OS X

Same as Windows, there is no reason it shouldn't compile.  I do not own a Mac or have access to one, however, so I can't really help too much in that regard.  Most of the differences OS X would need in the code are already programmed to replace Linux specific code, however the paths and file names likely would need tweaking in globaldefines.h. 
