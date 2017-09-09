Demonstration of zxnext_layer2
==============================

The zxnext_layer2_demo is an example program demonstrating how to use the
zxnext_layer2 (https://github.com/stefanbylund/zxnext_layer2) C API for the
layer 2 screen of the Sinclair ZX Spectrum Next. This program is also used for
regression testing of the zxnext_layer2 project.

The program consists of 32 different screens. Switch between the screens by
pressing any key. The first set of screens tests all drawing functions. The
second set of screens are the same as the first set but all drawing is done on a
layer 2 off-screen buffer. The third set of screens calls all drawing functions
repeatedly to give a visual indication of their performance. The fourth set of
screens tests horizontal and vertical hardware scrolling of a single screen and
between multiple screens.

The zxnext_layer2_demo project is available on GitHub at:
https://github.com/stefanbylund/zxnext_layer2_demo

Run in CSpect emulator
----------------------

1. The absolute path to the zxnext_layer2_demo folder (i.e. this folder) is
   denoted as <zxnext_layer2_demo>.

2. Install the latest version of the CSpect emulator from https://dailly.blogspot.se/.

3. Run the zxnext_layer2_demo.sna file in the CSpect emulator:

> CSpect.exe -zxnext -mmc=<zxnext_layer2_demo>/ <zxnext_layer2_demo>/zxnext_layer2_demo.sna

Note that the -mmc path must end with a / or \ character!

Run in ZEsarUX emulator
-----------------------

1. The absolute path to the zxnext_layer2_demo folder (i.e. this folder) is
   denoted as <zxnext_layer2_demo>.

2. Install the latest version of the ZEsarUX emulator from https://sourceforge.net/projects/zesarux/.

3. Copy all *.nxi files to <zesarux>/extras/media/spectrum/esxdos_handler/0.8.6_TBBLUE.

4. Run the zxnext_layer2_demo.tap file in the ZEsarUX emulator:

> zesarux.exe --machine tbblue --enable-mmc --enable-divmmc-ports --mmc-file extras/media/disk_images/tbblue.mmc
  --enable-esxdos-handler --esxdos-root-dir extras/media/spectrum/esxdos_handler/0.8.6_TBBLUE --quickexit
  --tape <zxnext_layer2_demo>/zxnext_layer2_demo.tap