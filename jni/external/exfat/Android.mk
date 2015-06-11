EXFAT_ROOT := $(call my-dir)
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

EXFAT_CFLAGS := -Wall -O2 -std=c99 \
                -D__GLIBC__ \
                -D_FILE_OFFSET_BITS=64 \
                -DALWAYS_FLUSH_CMAP=1 \
                -DUSE_TRANSITIONAL_LFS=1 \
                -I$(EXFAT_ROOT)/libexfat \
                -I$(EXFAT_ROOT)/../fuse/include

LOCAL_MODULE := mount.exfat
LOCAL_SRC_FILES := main.c
LOCAL_STATIC_LIBRARIES += libexfat_mount libexfat_fsck libexfat_mkfs libexfat_dump libexfat_label
LOCAL_STATIC_LIBRARIES += libexfat libfuse
include $(BUILD_EXECUTABLE)

include $(EXFAT_ROOT)/libexfat/Android.mk
include $(EXFAT_ROOT)/fuse/Android.mk
include $(EXFAT_ROOT)/mkfs/Android.mk
include $(EXFAT_ROOT)/fsck/Android.mk
include $(EXFAT_ROOT)/dump/Android.mk
include $(EXFAT_ROOT)/label/Android.mk
