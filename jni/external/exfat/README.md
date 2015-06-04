Based on [exfat 1.1.1 r422 sources](https://code.google.com/p/exfat/source/list). Initial Android project and modifications were taken from [OUYA repository](https://github.com/ouya/android_external_exfat).

The changes made are:
* `sync` mount option. The original version keeps cluster map in memory, and flushes it to the disk only at unmount event. This is unreliable for mobile use. Moreover, on my device, Android does not unmout SD card before reboot. This option forces flushing cluster map on `release` events, i.e. on each file close, keeping file system consistent most of the time.
* Original source code uses 64-bit off_t type verision, but Android NDK toolchain and libraries ignore `_FILE_OFFSET_BITS=64` define. Changed to `off64_t`, and also switched to `lseek64`, `pread64`, `pwrite64`.
**Note:** It is not good to typedef `off_t` as 64-bit integer before including other headers, because it has side effect, in particular, it breaks stream I/O functions in Bionic libc.
* Replaced `abort()` with exit`()` on emergency quit, because under Android `abort()` produces crash dump.

This project produces only one binary file: `mount.exfat`. To have the full set of exFAT utilities, and/or for applications that expect other executable names, you have to run all or some of the following commands:

    ln -s mount.exfat mount.exfat-fuse
    ln -s mount.exfat dumpexfat
    ln -s mount.exfat exfatfsck
    ln -s mount.exfat exfatlabel
    ln -s mount.exfat mkexfatfs
    ln -s mount.exfat fsck.exfat
    ln -s mount.exfat mkfs.exfat

My auto-mount script creates all the links, if required.
