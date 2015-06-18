Here you can create a zip file to install the drivers via recovery console:

1. Build the project.
2. Copy per-platform directories from `libs` to this place.
3. Copy `mount/install-recovery-2.sh` into `common` directory.
4. Zip the content of this directory (excluding this file).
5. Sign the resulting ZIP archive.

**Note:** The installation script uses `unzip` from recovery's `busibox`, and, with most of ZIP file signers, `unzip` fails with the message

    unzip: zip flags 1 and 8 are not supported

You can't see the message itself in the recovery console, but, instead,

    ! Error: unzip failed!

from my script. The solution I've found is to sign using [ZipSigner](https://play.google.com/store/apps/details?id=kellinwood.zipsigner2) on an Android device.
