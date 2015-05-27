#!/system/bin/sh
# This is "all-in-one" build of mount.exfat:
ln -s mount.exfat dumpexfat
ln -s mount.exfat exfatfsck
ln -s mount.exfat exfatlabel
ln -s mount.exfat mkexfatfs
ln -s mount.exfat mount.exfat-fuse

# and also:
ln -s mount.exfat fsck.exfat
ln -s mount.exfat mkfs.exfat
