Sayonara


Libraries

    - Qt >= 5.3: Core, Widgets, Network, Xml, Sql, Sqlite http://qt.nokia.com/products/
    - Taglib http://developer.kde.org/~wheeler/taglib.html
    - Gstreamer development files, GStreamer plugins 
    - zlib

1. Linux

	You need 
	* g++ >= 4.8 (important due to C++x11 standard)
	* cmake
	* Qt >= 5.3 + development files 
	* libtaglib
	* Gstreamer 1.0 + development files (libgstreamer1.0, libgstreamer-plugins-base1.0)
        * zlib development files

	= Build =

	* cmake . -DCMAKE_BUILD_TYPE=[Release|Debug|RelWithDebInfo|None] -DCMAKE_INSTALL_PREFIX=/usr (default: Release, /usr/local)
	* make
	* make install (as root)
	* sayonara
	
	1.1 Debian/Ubuntu/Mint:  apt-get install cmake  pkg-config qt5-default qttools5-dev qttools5-dev-tools libqt5core5a libqt5network5 libqt5sql5 libqt5sql5-sqlite libqt5xml5 \
							 libtag1-dev libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev libgstreamer-plugins-good1.0-dev libgstreamer-plugins-bad1.0-dev libsqlite3-0

		optional: gstreamer1.0-plugins-ugly libmp3lame


	1.2 Fedora/Suse: yum install gcc-c++ cmake pkg-config qt5-qtbase-devel qt5-qttools-devel gstreamer1-devel gstreamer1-plugins-base-devel  
		
		optional: gstreamer1-plugins-ugly lame (use rpmforge or rpmfusion)
	

