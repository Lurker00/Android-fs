LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE    := libexfat_label
LOCAL_SRC_FILES := main.c

LOCAL_CFLAGS := $(EXFAT_CFLAGS)
LOCAL_C_INCLUDES += $(LOCAL_PATH)
include $(BUILD_STATIC_LIBRARY)
