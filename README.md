#Android fuse drivers for exFAT and NTFS

**DISCLAIMER:** The source code was taken from open source projects, and only altered for Android compatibility. Expect the same bugs and misbehavior found in the original versions :P **No warranty and no support at all: use it at your own risk and responsibility!**

The primary goal of this project is to build the drivers for Lenovo Yoga Tablet 2 8'' 830L (firmware upgraded to 830LC) tablet, which utilizes Intel Atom (x86) CPU. There are ARM builds of these drivers over there, but I found nothing for x86. Also, I failed to find a ready-to-build project with up-to-date source code.

This project is rather straightforward:

1. Install Android NDK for your platform (I used Windows/Cygwin).
2. Checkout or download and unpack source code.
3. Run `ndk-build` in the root directory of the source code tree.
 
I spent (hope - not wasted!) some time to commit it step by step, starting from original sources, with all further add-ons and modifications commented, for easy repeat from more recent sources, or to merge in an update.

##Notes on exFAT driver build

This project produces only one binary file: `mount.exfat`. To have the full set of exFAT utilities, and/or for applications that expect other executable names, you have to run all or some of the following commands:

    ln -s mount.exfat dumpexfat
    ln -s mount.exfat exfatfsck
    ln -s mount.exfat exfatlabel
    ln -s mount.exfat mkexfatfs
    ln -s mount.exfat mount.exfat-fuse
    ln -s mount.exfat fsck.exfat
    ln -s mount.exfat mkfs.exfat
