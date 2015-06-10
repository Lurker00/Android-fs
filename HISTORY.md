###1.0.2
1. One more `supolicy` rule added to the mounting script. Without it, unmount from Android Settings didn't work.

###1.0.1
1. Mount script uses `supolicy`, if available, for SELinux permissions, with fallback to `setenforce 0`, if `supolicy` is not found.
2. Moved the mounting script into `common` folder of the release package.
3. Removed `mkntfs` from the package.

###1.0.0
First public release.
