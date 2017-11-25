Demonstration of zxnext_layer2
==============================

The zxnext_layer2_demo is a set of example programs demonstrating how to use the
zxnext_layer2 (https://github.com/stefanbylund/zxnext_layer2) C layer 2 graphics
library for the Sinclair ZX Spectrum Next. These program are also used for
regression testing of the zxnext_layer2 project.

There are four demo programs where each demo program consists of a set of
screens, switch between the screens by pressing any key.

* zxnext_draw_demo: This program contains three sets of screens. The first set
tests all layer 2 drawing functions using the main layer 2 screen. The two other
sets are the same as the first set but all drawing is done on the shadow layer 2
screen and on a layer 2 off-screen buffer copied to the main layer 2 screen.

* zxnext_perf_demo: This program calls all layer 2 drawing functions repeatedly
to give a visual indication of their performance.

* zxnext_scroll_demo: This program demonstrates horizontal, vertical and
diagonal hardware scrolling of a single screen and between multiple screens.

* zxnext_misc_demo: This program tests layer priorities and drawing on the main
and shadow layer 2 screens paged to the top 16K RAM instead of the bottom 16K.

The zxnext_layer2_demo project is available on GitHub at:
https://github.com/stefanbylund/zxnext_layer2_demo

Run in CSpect emulator
----------------------

1. The absolute path to the zxnext_layer2_demo folder (i.e. this folder) is
   denoted as <zxnext_layer2_demo>.

2. Install the latest version of the CSpect emulator from https://dailly.blogspot.se/.

3. Run the programs in the CSpect emulator:

> CSpect.exe -zxnext -mmc=<zxnext_layer2_demo>/ <zxnext_layer2_demo>/<program>.sna

Note that the -mmc path must end with a / or \ character!

Run in ZEsarUX emulator
-----------------------

1. The absolute path to the zxnext_layer2_demo folder (i.e. this folder) is
   denoted as <zxnext_layer2_demo>.

2. Install the latest version of the ZEsarUX emulator from https://sourceforge.net/projects/zesarux/.

3. Copy all *.nxi files to <zesarux>/extras/media/spectrum/esxdos_handler/0.8.6_TBBLUE.

4. Run the programs in the ZEsarUX emulator:

> zesarux.exe --machine tbblue --enable-mmc --enable-divmmc-ports --mmc-file tbblue.mmc
  --enable-esxdos-handler --esxdos-root-dir extras/media/spectrum/esxdos_handler/0.8.6_TBBLUE --quickexit
  --tape <zxnext_layer2_demo>/<program>.tap
