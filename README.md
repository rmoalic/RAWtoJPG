RAWtoJPG
============

This is a windows shell extension to (batch) convert RAW files to JPG.

Uses the [Windows store raw extensions](https://apps.microsoft.com/store/detail/extension-dimage-raw/9NCTDW2W1BH8?hl=fr-fr&gl=FR) (internaly [libraw](https://www.libraw.org/about)) or any [WIC codec](https://www.photools.com/1167/wic-support-codec-availability/). With default settings.

To get better result, use a proper raw developer like [darktable](https://www.darktable.org/) or [lightroom](https://www.adobe.com/fr/products/photoshop-lightroom.html).

![Screenshot](screen.png)
![Screenshot](screen2.png)


Build
-----

* Install MSVC
* Start MSVC console
* Run ```build_msvc.bat``` from the MSVC console

Package/install
-------

* Install WiX
* Run ```package.bat``` to create the msi file
* Run ```msiexec /i rawtojpg-setup.msi``` to install

* Right click on a raw file en click ```Convert to JPG``` (ctrl + click to replace all)

* Run ```msiexec /x rawtojpg-setup.msi``` to uninstall

* (while making changes, you might need to restart ```explorer.exe``` to unlock ```RAWtoJPG.dll```)

TODO
----
* Try concurrent convertion
* Encoder Options config ([WIC](https://docs.microsoft.com/en-us/windows/win32/wic/-wic-creating-encoder#encoder-options-usage))
* Better file association (try to find why PerceivedType image does not work)
* Dll not unloading (ref counting issue ?)