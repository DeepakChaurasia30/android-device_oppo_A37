# Copyright (C) 2015 The CyanogenMod Project
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

include $(CLEAR_VARS)
LOCAL_SRC_FILES := boringssl/p_dec.c boringssl/p_open.c boringssl/cipher.c \
    boringssl/e_des.c boringssl/cleanup.c boringssl/ctrl.c
    
LOCAL_CFLAGS += -std=c99
LOCAL_C_INCLUDES := boringssl
LOCAL_SHARED_LIBRARIES := libcrypto
LOCAL_MODULE := libshims_boringssl
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_VENDOR_MODULE := true
include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_SRC_FILES := ims/MediaBuffer.c
LOCAL_SHARED_LIBRARIES := libstagefright_foundation
LOCAL_MODULE := libshims_ims
LOCAL_MODULE_TAGS := optional
include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_SRC_FILES := camera_shim.c
LOCAL_SHARED_LIBRARIES := libutils libgui liblog
LOCAL_MODULE := libcamera_shim
LOCAL_MODULE_TAGS := optional
LOCAL_VENDOR_MODULE := true
include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_SRC_FILES := \
    atomic.cpp \
    android/sensor.cpp \
    gui/SensorManager.cpp \
    MediaCodec.cpp \
    AudioSource.cpp \
    MetaData.cpp \
    camera_parameters/CameraParameters.cpp \
    justshoot_shim.cpp

LOCAL_C_INCLUDES := gui
LOCAL_SHARED_LIBRARIES := libsensor libutils liblog libbinder \
                          libandroid libui libstagefright libmedia

LOCAL_MODULE := libshim_camera
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_VENDOR_MODULE := true
include $(BUILD_SHARED_LIBRARY)
