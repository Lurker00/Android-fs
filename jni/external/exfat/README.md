Based on [exfat 1.1.1 r422 sources](https://code.google.com/p/exfat/source/list). Initial Android project and modifications were taken from [OUYA repository](https://github.com/ouya/android_external_exfat).

The changes made are:
* Several patches to keep the file system consistent as much as possible. The original version keeps cluster map in memory, and flushes it to the disk only at unmount event. Also, it updates the file system information about changed files only on file close. This is unreliable for mobile use, especially when SD card is heavily used by applications. Moreover, modern Android does not unmout SD card (mounted not by `vold`) before reboot or power off. You can read a discussion with Andrew Nayenko (the exFAT author) [here](https://groups.google.com/forum/#!topic/exfat/u5Ldpmy3vkA).
* Original source code uses 64-bit off_t type version, but Android NDK toolchain and libraries ignore `_FILE_OFFSET_BITS=64` define. Changed to `off64_t`, and also switched to `lseek64`, `pread64`, `pwrite64`.
**Note:** It is not good to typedef `off_t` as 64-bit integer before including other headers, because it has side effect, in particular, it breaks stream I/O functions in Bionic libc.
* Replaced `abort()` with exit`()` on emergency quit, because under Android `abort()` produces crash dump.
* Starting from 1.0.4, Android build is default to "`noatime`" option, and ignores other values ("`atime`", "`relatime`") provided from the command line.

This project produces only one binary file: `mount.exfat`. To have the full set of exFAT utilities, and/or for applications that expect other executable names, you have to run all or some of the following commands:

    ln -s mount.exfat mount.exfat-fuse
    ln -s mount.exfat dumpexfat
    ln -s mount.exfat exfatfsck
    ln -s mount.exfat exfatlabel
    ln -s mount.exfat mkexfatfs
    ln -s mount.exfat fsck.exfat
    ln -s mount.exfat mkfs.exfat

My auto-mount script creates all the links, if required.

**Note regarding the file system consistency**
All the changes to the functionality, compared to the original code, is active only if the volume was mounted with `sync` option. This `sync` option _does not_ sync every write with the underlying device, it only flushes file system changes earlier and more often, that the original version (or if mounted without `sync` option).

Starting from [1.0.3](https://github.com/Lurker00/Android-fs/releases), this is the default behaviour for my Android builds: no `sync` option is required, and this safer behaviour can't be disabled under Android. 

Starting from [1.0.4](https://github.com/Lurker00/Android-fs/releases), it marks the volume as opened only before making any changes to the file system, and unmarks it as soon as the files being written are closed, and changes are flushed to the device. Short operations (create/rename/delete files and directories) are finished with `fsync` to the device. Files under 32 MB are flushed and `fsync`ed on number of occupied clusters growth. For files bigger than 32 MB `fsync` is not used until closing, for the sake of performance.
