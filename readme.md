vtex_c2tga
=======

Converts Compiled Valve Texture (.vtex_c) files to Targa (.tga) files.

### Notes

 * Only handles vtex files encoded in DXT1, DXT5/DXT5_nm, and RGBA as those are the only formats used by Dota2 so far.

### Dependencies
 
 * [ImageMagick] (http://www.imagemagick.org/) For reading DXT1/DXT5/RGBA images and writing .tga files.

### Compiling

$g++ -std=c++11 vtex_c2tga.cpp \`Magick++-config --cppflags --cxxflags --ldflags --libs\` -o vtex_c2tga.exe

### Usage

$vtex_c2tga.exe "/path/to/.vtex_c" "/path/to/output/.tga"

### License

MIT License. See LICENSE file for further details.