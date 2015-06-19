Based on [ntfs-3g version 2015.3.14 from Tuxera](http://www.tuxera.com/community/open-source-ntfs-3g/). Initial Android project and "Android glue" was taken from  [OmniROM repository](https://github.com/omnirom/android_external_ntfs-3g).

I've built only `ntfs-3g`, `ntfsfix` and `mkntfs`, but didn't test the latter. Other tools may require similar modifications to build and run. I need only `ntfs-3g` and `ntfsfix` (with -n option only) for my particular tasks.

The changes made are:
* Original source code uses 64-bit off_t type verision, but Android NDK toolchain and libraries ignore `_FILE_OFFSET_BITS=64` and `_LARGEFILE_SOURCE` defines. Changed to `off64_t`, and also switched to `lseek64`, `pread64`, `pwrite64`.
**Note:** It is not good to typedef `off_t` as 64-bit integer before including other headers, because it has side effect, in particular, it breaks stream I/O functions in Bionic libc.
* Bionic headers define `_STRUCT_TIMESPEC`, but neither `st_mtime` nor `__timespec_defined`, as Tuxera expects.
* Bionic headers don't define obsolete `S_IREAD`, `S_IWRITE`, `S_IEXEC`.
* Original sources failed to compile with `IGNORE_MTAB` defined.
* Android NDK x86 libc has missing `ffs()` function, so I had to undefine `HAVE_FFS`.
* Starting from 1.0.4, Android build is defaults to "noatime" option, and it ignores other values in the command line.

**TODO:** Android file managers don't follow symlinks on user storages. I'd try to implement a mount option to resolve symlinks at the driver side, transparently for applications.
