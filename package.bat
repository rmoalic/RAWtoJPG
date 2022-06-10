@echo off

echo For WIX version 3.11.2.4516

candle setup.wxs
light setup.wixobj -out rawtojpg-setup.msi