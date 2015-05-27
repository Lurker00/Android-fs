LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := probe
LOCAL_SRC_FILES := probe.c devmapper.c \
	blkid/cache.c \
	blkid/dev.c \
	blkid/devname.c \
	blkid/devno.c \
	blkid/getsize.c \
	blkid/llseek.c \
	blkid/probe.c \
	blkid/read.c \
	blkid/resolve.c \
	blkid/save.c \
	blkid/tag.c \
	uuid/isnull.c \
	uuid/unpack.c \
	uuid/unparse.c

LOCAL_CFLAGS := -DHAVE_UNISTD_H \
                -DHAVE_ERRNO_H \
                -DHAVE_SYS_STAT_H \
                -DHAVE_SYS_TYPES_H \
                -DHAVE_SYS_PRCTL_H \
                -DHAVE_LSEEK64 \
                -DHAVE_LSEEK64_PROTOTYPE \
                -DHAVE_INTTYPES_H

LOCAL_SHARED_LIBRARIES += libc
LOCAL_LDLIBS := -llog

include $(BUILD_EXECUTABLE)
