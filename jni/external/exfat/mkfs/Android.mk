LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE    := libexfat_mkfs
LOCAL_SRC_FILES := cbm.c fat.c main.c mkexfat.c rootdir.c uct.c uctc.c vbr.c

LOCAL_CFLAGS := $(EXFAT_CFLAGS)
LOCAL_C_INCLUDES += $(LOCAL_PATH)
include $(BUILD_STATIC_LIBRARY)
