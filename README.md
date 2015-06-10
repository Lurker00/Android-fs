#Android fuse drivers for exFAT and NTFS

**DISCLAIMER:** The source code was taken from open source projects, and only altered for Android compatibility. Expect the same bugs and misbehavior found in the original versions! **No warranty and no support at all: use it at your own risk and responsibility!**

The primary goal of this project was to build the drivers for Lenovo Yoga Tablet 2 8'' 830L (firmware upgraded to 830LC) tablet, which utilizes Intel Atom (x86) CPU and runs Android 4.4.2. There are some ARM builds of these drivers over there, but I found nothing for x86. Also, I failed to find a ready-to-build project with up-to-date source code.

I spent (hope - not wasted!) some time to commit it step by step, starting from original sources, with all further add-ons and modifications commented, for easy repeat for more recent sources, or to merge in an update.

I provide a CWM/TWRP installable zip package, with binaries for `armeabi`, `armeabi-v7a`, `arm64-v8a`, `x86`, `x86_64` and a mounting script, in the [Releases](https://github.com/Lurker00/Android-fs/releases). **Root and SuperSU are required for the mounting script to work correctly!** For more information, please read ["How to mount?"](#how-to-mount) chapter below.

##History

###1.0.2
1. One more `supolicy` rule added to the mounting script. Without it, unmount from Android Settings didn't work.

###1.0.1
1. Mount script uses `supolicy`, if available, for SELinux permissions, with fallback to `setenforce 0`, if `supolicy` is not found.
2. Moved the mounting script into `common` folder of the release package.
3. Removed `mkntfs` from the package.

###1.0.0
First public release.

##How to mount?
The primary goal is auto-mount of the microSD card on Android boot, with full functionality. Unmount can be done from the Android Settings. Re-mount after the boot is possible, but currently is a bit tricky (see below).

Currently, to install all this stuff, you need to root your device. If you have done, you already have SuperSU installed, and, as a result, `/system/etc/install-recovery-2.sh` (if exists) is executed on every boot by SuperSU. I've provided one in the `mount` directory of the project, and in the [Release](https://github.com/Lurker00/Android-fs/releases) packages. The script checks if the file system is supported, then tries to mount the external microSD. It appends a log file at `/data/local/tmp/exfat_mount.log`. You may disable it by setting the `logfile` to `/dev/null` in the script (it is done in the installation zip). The script expect the following executable binaries with permissions to run at `/system/xbin`:
* `probe` (to detect the file system type)
* `mount.exfat`
* `ntfs-3g`
* `ntfsfix`

I hope other commands it uses are available in the shell of your device.

The script creates the links for exFAT tools, if required. It checks the file system before mounting, and mounts it read-only if it has found errors. It does not fix the errors found: please use Windows or your OS tools to correct the file system errors.

**NOTE:** If there is no `/system/xbin/supolicy` (provided by SuperUS), the script sets SELinux to Permissive mode! Instead, the mount would be read-only. If you know how to overcome it, please let me know.

The `/system/etc/install-recovery-2.sh` script can be executed manually, from a terminal, e.g. to re-mount without system restart. For this particular purpose, please run it with any parameter, e.g.

    /system/etc/install-recovery-2.sh mount

(`mount` is just for example: any parameter!). Then, the script uses `debuggerd` replacement technology to mount from `init` process.

Of course, I've checked all above, and it works just fine on my particular device, though it does not mean it will work for you!

##How to build
Build process of this project is rather straightforward:

1. Install Android NDK for your platform (I use Windows 7 64-bit/Cygwin, NDK r10b).
2. Checkout or download and unpack the source code.
3. Run `ndk-build` in the root directory of the source code tree.

I tried to build for `mips` as well and faced a compile error for `mips64`. It can be worked around with easy, but I never net a `mips`-based Andriod device, so I don't bother.

##Notes on exFAT driver build
Please read [here](https://github.com/Lurker00/Android-fs/blob/master/jni/external/exfat/README.md).

##Notes on NTFS driver build
Please read [here](https://github.com/Lurker00/Android-fs/blob/master/jni/ntfs-3g/README.md)
