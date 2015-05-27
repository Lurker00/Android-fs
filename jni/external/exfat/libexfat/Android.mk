LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE    := libexfat
LOCAL_SRC_FILES := cluster.c io.c log.c lookup.c mount.c node.c time.c utf.c utils.c 

LOCAL_CFLAGS := $(EXFAT_CFLAGS)
LOCAL_C_INCLUDES += $(LOCAL_PATH)
include $(BUILD_STATIC_LIBRARY)
