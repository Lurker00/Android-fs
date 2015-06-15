APP_ABI := x86 x86_64 armeabi armeabi-v7a arm64-v8a
#APP_ABI := all
APP_PLATFORM := android-17

APP_CFLAGS += -fno-stack-protector

#NDK_TOOLCHAIN_VERSION=clang

ifeq ($(NDK_TOOLCHAIN_VERSION),clang)
	APP_CFLAGS += -mno-unaligned-access
	APP_CFLAGS += -Wno-typedef-redefinition
else
endif
