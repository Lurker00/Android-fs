#!/system/bin/sh

logfile=/data/local/tmp/exfat_mount.log
#logfile=/sdcard/exfat_mount.log

echo '******' $(date) '******' >> $logfile
echo 'id: ' $(id) >> $logfile 2>&1

if [ x$SECONDARY_STORAGE == x ]; then
	echo "Error: SECONDARY_STORAGE is not set. Don\'t know where to mount!" >> $logfile
	exit 1
fi

binpath=/system/xbin

probe=$binpath/probe
if [ ! -e $probe ]; then
	echo "Error: $probe not found!" >> $logfile
	exit 1
fi

device=/dev/block/mmcblk1p1
echo "Starting for $device" >> $logfile

FS=($($probe $device))
echo "$device file system:" ${FS[0]} >> $logfile

# ***** Setup tools for different file systems:
if [ ${FS[0]} == "exFAT" ]; then
	echo "$device is exFAT" >> $logfile

	mount=$binpath/mount.exfat
	if [ -f $mount ]; then
		links=( "mkfs.exfat" "fsck.exfat" "dumpexfat" "exfatfsck" "exfatlabel" "mkexfatfs" "mount.exfat-fuse" )
		for l in "${links[@]}"
		do
			if [ ! -e $binpath/$l ]; then
	 	 		mount -o remount,rw /
				ln -s $mount $binpath/$l
				echo "Created symlink $binpath/$l" >> $logfile
			fi
		done
	fi

	fsck=$binpath/exfatfsck
	fsck_ops=

	options_ro="ro,uid=1023,gid=1023,umask=0000"
	options_rw="rw,uid=1023,gid=1023,umask=0000,noatime,sync"
elif [ ${FS[0]} == "NTFS" ]; then
	echo "$device is NTFS" >> $logfile
	mount=$binpath/ntfs-3g
	fsck=$binpath/ntfsfix
	fsck_ops='-n'
	options_ro="ro,uid=1023,gid=1023,umask=0000"
	options_rw="rw,uid=1023,gid=1023,umask=0000,noatime"
else
	echo "Error: $device is not supported!" >> $logfile
	exit 1
fi

# ***** Check if we have required tools.
if [ ! -e $mount ]; then
	echo "Error: $mount not found!" >> $logfile
	exit 1
fi
if [ ! -e $fsck ]; then
	echo "Error: $fsck not found!" >> $logfile
	exit 1
fi

# ***** Mount for rw only if the file system is not damaged.
mnt_cmd="$mount -o $options_rw $device /mnt/media_rw/sdcard1"

$fsck $fsck_ops $device >> $logfile 2>&1
if [ $? != 0 ]; then
	mnt_cmd="$mount -o $options_ro $device /mnt/media_rw/sdcard1"
fi
echo "$mnt_cmd" >> $logfile

if [ -e /sys/fs/selinux/enforce ]; then
	if [ -e /system/xbin/supolicy ]; then
# ***** Set permissions for fuse. Instead, our mounts would be RO for others.
		echo "Using supolicy to provide permissions to fuse." >> $logfile
		/system/xbin/supolicy --live "allow sdcardd unlabeled dir { append create execute write relabelfrom link unlink ioctl getattr setattr read rename lock mounton quotaon swapon rmdir audit_access remove_name add_name reparent execmod search open }"
		/system/xbin/supolicy --live "allow sdcardd unlabeled file { append create write relabelfrom link unlink ioctl getattr setattr read rename lock mounton quotaon swapon audit_access open }"
		/system/xbin/supolicy --live "allow unlabeled unlabeled filesystem associate"
	else
# ***** Turn SELinux to Permissive. Instead, our mounts would be RO for others.
		echo "Using setenforce 0 to provide permissions to fuse." >> $logfile
		setenforce 0
	fi
fi

# ***** Just in case, and it would work only on boot.
chown media_rw:media_rw $SECONDARY_STORAGE > /dev/null 2>&1
chmod 777 $SECONDARY_STORAGE               > /dev/null 2>&1

# ***** Try to mount:
if [ 0 != 0 ]; then
	$mount -o $options_rw $device $SECONDARY_STORAGE
	echo "Mounted directly" >> $logfile
else
	if [ x$1 != x ]; then
	 	mount -o remount,rw /system
		rm -f                              /system/bin/debuggerd.mine
		echo '#!/system/bin/sh'          > /system/bin/debuggerd.mine
		echo "$mnt_cmd"                  >> /system/bin/debuggerd.mine
		echo 'start fuse_sdcard1'       >> /system/bin/debuggerd.mine
		chmod 777                          /system/bin/debuggerd.mine
		stop  debuggerd
		mv /system/bin/debuggerd      /system/bin/debuggerd.its
		mv /system/bin/debuggerd.mine /system/bin/debuggerd
		start debuggerd
		sleep 2
		stop  debuggerd
		mv /system/bin/debuggerd      /system/bin/debuggerd.mine
		mv /system/bin/debuggerd.its  /system/bin/debuggerd
		start debuggerd
	 	mount -o remount,ro /system
		vdc volume mount $SECONDARY_STORAGE
		echo "Mounted via debuggerd" >> $logfile
	else
		$mnt_cmd >> $logfile 2>&1
		if [ $? == 0 ]; then
			start fuse_sdcard1 >> $logfile 2>&1
			vdc volume mount $SECONDARY_STORAGE >> $logfile 2>&1
			echo "Mounted for fuse" >> $logfile
		else
			echo "Failed to mount for fuse ($?)" >> $logfile
		fi
	fi
fi
