#Android fuse drivers for exFAT and NTFS

**DISCLAIMER:** The source code was taken from open source projects, and only altered for Android compatibility. Expect the same bugs and misbehavior found in the original versions :P **No warranty and no support at all: use it at your own risk and responsibility!**

The primary goal of this project was to build the drivers for Lenovo Yoga Tablet 2 8'' 830L (firmware upgraded to 830LC) tablet, which utilizes Intel Atom (x86) CPU and runs Android 4.4.2. There are some ARM builds of these drivers over there, but I found nothing for x86. Also, I failed to find a ready-to-build project with up-to-date source code.
 
I spent (hope - not wasted!) some time to commit it step by step, starting from original sources, with all further add-ons and modifications commented, for easy repeat from more recent sources, or to merge in an update.

##How to build
Buil process of this project is rather straightforward:

1. Install Android NDK for your platform (I use Windows 7 64-bit/Cygwin, NDK r10b).
2. Checkout or download and unpack the source code.
3. Run `ndk-build` in the root directory of the source code tree.

##Notes on exFAT driver build
Based on [exfat 1.1.1 r422 sources](https://code.google.com/p/exfat/source/list).
This project produces only one binary file: `mount.exfat`. To have the full set of exFAT utilities, and/or for applications that expect other executable names, you have to run all or some of the following commands:

    ln -s mount.exfat mount.exfat-fuse
    ln -s mount.exfat dumpexfat
    ln -s mount.exfat exfatfsck
    ln -s mount.exfat exfatlabel
    ln -s mount.exfat mkexfatfs
    ln -s mount.exfat fsck.exfat
    ln -s mount.exfat mkfs.exfat

##Notes on NTFS driver build.
Based on [ntfs-3g version 2015.3.14 from Tuxera](http://www.tuxera.com/community/open-source-ntfs-3g/). I've built only `ntfs-3g`, `ntfsfix` and `mkntfs`, but didn't test the latter. Other tools may require similar modifications to build and run. I need only `ntfs-3g` and `ntfsfix` (with -n option only) for my particular tasks.

##How to mount?
The primary goal is auto-mount of the microSD card on Android boot, with full functionality. Unmount and re-mount after the boot is possible, but currently is a bit tricky.

Probably, it is possible to create a CWM Recovery zip archive to avoid manual installation, but I've yet to learn how to do it. Currently, to install all this stuff, you need to root your device. If you have done, you already have SuperSU installed, and, as a result, `/system/etc/install-recovery-2.sh` (if exists) is executed on every boot by SuperSU. I've provided one in the `mount` directory of the project. It checks if the file system is supported, then tries to mount the external microSD. It appends a log file at `/data/local/tmp/exfat_mount.log`. You may disable it by setting the `logfile` to `/dev/null` in the script. The script expect the following executable binaries with permissions to run at `/system/xbin`:
* `probe` (to detect the file system type)
* `mount.exfat`
* `ntfs-3g`
* `ntfsfix`
 
I hope other commands it uses are available in the shell of your device.

**NOTE:** the script sets SELinux to Permissive mode! Instead, the mount would be read-only. If you know how to overcome it, please let me know.

The `/system/etc/install-recovery-2.sh` script can be executed manually, from a terminal, e.g. to re-mount without system restart. For this particular purpose, please run it with any parameter, e.g.
    /system/etc/install-recovery-2.sh mount
(`mount` is just for example: any parameter!). Then, the script uses `debuggerd` replacement technology to mount from `init` process.

Of course, I've checked all above, and it works just fine on my particular device, though it does not mean it will work for you!
