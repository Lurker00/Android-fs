# Copyright (C) 2007 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

LOCAL_PATH := $(call my-dir)

LOCAL_STATIC_LIBRARIES := libfuse

include \
	$(LOCAL_PATH)/probe/Android.mk \
	$(LOCAL_PATH)/external/fuse/Android.mk \
	$(LOCAL_PATH)/external/exfat/Android.mk \
	$(LOCAL_PATH)/external/ntfs-3g/Android.mk
