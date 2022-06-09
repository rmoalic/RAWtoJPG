RAWtoJPG
============

This is a windows shell extension to (batch) convert RAW files to JPG.

![Screenshot](screen.png)
![Screenshot](screen2.png)


Build
-----

* Install MSVC
* Start MSVC console
* Run ```build_msvc.bat``` from the MSVC console
* Run ```register.bat``` to install  the extension

* Right click on a raw file en click ```Convert to JPG```

* (while making changes, you will need to restart ```explorer.exe``` to unlock ```RAWtoJPG.dll```)

* Run ```unregister.bat``` to uninstall  the extension

TODO
----
* Create UI thread (explorer window is unresponsive during convertion)
* Try concurrent convertion
* Replace All function
* Installer
* Better file association (try to find why PerceivedType image does not work)