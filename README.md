# Demonstration of zxnext_layer2

The **zxnext_layer2_demo** project contains an example program demonstrating
how to use the [zxnext_layer2](https://github.com/stefanbylund/zxnext_layer2) C
API for using the layer 2 screen of the Sinclair ZX Spectrum Next.

This program is also used for regression testing of
[zxnext_layer2](https://github.com/stefanbylund/zxnext_layer2). The program
contains a set of screens. The first set of screens tests all drawing functions.
The second set of screens are the same as the first set but all drawing is done
on a layer 2 off-screen buffer. The third set of screens calls all drawing
functions repeatedly to give a visual indication of their performance. The
fourth set of screens tests horizontal and vertical hardware scrolling of a
single screen and between multiple screens. Switch between the screens by
pressing any key.

## How to Use

1. On Windows, you need [MinGW](http://www.mingw.org/),
[UnxUtils](https://sourceforge.net/projects/unxutils/) or
[Cygwin](https://www.cygwin.com/) for the basic Unix commands. Add the chosen
set of Unix commands to your path.

2. Install the latest version of [z88dk](https://github.com/z88dk/z88dk) and the
[ZEsarUX](https://sourceforge.net/projects/zesarux/) or
[CSpect](https://dailly.blogspot.se/) emulator.

3. Download [zxnext_layer2](https://github.com/stefanbylund/zxnext_layer2/blob/master/build/zxnext_layer2.zip)
and unpack it in a suitable place.

4. Download the zxnext_layer2_demo repository either as a ZIP archive using the
"Clone or download" button at the top of this page or with Git using the
following command:

> git clone https://github.com/stefanbylund/zxnext_layer2_demo.git

5. Make sure the zxnext_layer2 and zxnext_layer2_demo directories are located in
a common parent directory.

6. Go to the zxnext_layer2_demo directory and enter the following command to
build a TAP file or an SNA file if BUILD_SNA=true is specified:

> make all [BUILD_SNA=true]

7. For the ZEsarUX emulator, copy all *.nxi files from \<zxnext_layer2_demo_folder\>/resources
to \<zesarux\>/extras/media/spectrum/esxdos_handler/0.8.6_TBBLUE. For the CSpect
emulator, pass the option -mmc=\<zxnext_layer2_demo_folder\>/resources/.

8. Run the zxnext_layer2_demo/bin/\<compiler-flavour\>/zxnext_layer2_demo.tap
file in the ZEsarUX emulator or the
zxnext_layer2_demo/bin/\<compiler-flavour\>/zxnext_layer2_demo.sna file in the
CSpect emulator.

**Note:** Since the Sinclair ZX Spectrum Next is still under development and all
tools need to catch up with the latest specs, it is important to use the latest
version of zxnext_layer2, z88dk and ZEsarUX or CSpect.

**Tip:** If you don't care for makefiles or don't want to install
MinGW/UnxUtils/Cygwin on Windows, it is quite easy to build zxnext_layer2_demo
manually by extracting the z88dk command-line invocation in the Makefile.

**Tip:** To start the ZEsarUX emulator directly in Sinclair ZX Spectrum Next
mode, start it with the following options:

> zesarux --machine tbblue --enable-mmc --enable-divmmc-ports --mmc-file extras/media/disk_images/tbblue.mmc
    --enable-esxdos-handler --esxdos-root-dir extras/media/spectrum/esxdos_handler/0.8.6_TBBLUE --quickexit

**Tip:** To start the CSpect emulator directly in Sinclair ZX Spectrum Next
mode, start it with the options below. Note that the -mmc path must end with a
/ or \ character!

> CSpect -zxnext -s7 -mmc=\<zxnext_layer2_demo_folder\>/resources/ zxnext_layer2_demo.sna

## License

This software is licensed under the terms of the MIT license.
