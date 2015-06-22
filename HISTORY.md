###1.0.4
1. More improvements of exFAT driver toward the robustness.
2. exfatfsck clears "mounted" flag, if no other errors found, and it was run with -f option.

###1.0.3
1. exFAT driver was improved to keep the file system consistent. Slight performance drop can be noticed, but a chance of file system corruption on reboots or power loss has been minimized.
2. Mounting script checks if the device is already mounted, or mount point is already in use, before an attempt to mount.

For No.1, you can read a discussion with Andrew Nayenko (the exFAT author) [here](https://groups.google.com/forum/#!topic/exfat/u5Ldpmy3vkA).

###1.0.2
1. One more `supolicy` rule added to the mounting script. Without it, unmount from Android Settings didn't work.

###1.0.1
1. Mount script uses `supolicy`, if available, for SELinux permissions, with fallback to `setenforce 0`, if `supolicy` is not found.
2. Moved the mounting script into `common` folder of the release package.
3. Removed `mkntfs` from the package.

###1.0.0
First public release.
