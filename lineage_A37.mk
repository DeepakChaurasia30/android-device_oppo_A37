# Copyright (C) 2014 The CyanogenMod Project
# Copyright (C) 2018 The LineageOS Project
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

$(call inherit-product, device/oppo/A37/full_A37.mk)

# Inherit some common LineageOS stuff.
$(call inherit-product, vendor/lineage/config/common_mini_phone.mk)

# Must define platform variant before including any common things
TARGET_BOARD_PLATFORM_VARIANT := msm8916

# Vendor security patch level
PRODUCT_PROPERTY_OVERRIDES += \
    ro.lineage.build.vendor_security_patch=2016-09-01

PRODUCT_NAME := lineage_A37
BOARD_VENDOR := oppo
PRODUCT_DEVICE := A37

PRODUCT_GMS_CLIENTID_BASE := android-oppo

PRODUCT_BUILD_PROP_OVERRIDES += \
    PRIVATE_BUILD_DESC="msm8916_64-user 5.1.1 LMY47V eng.root.20190711.032745 release-keys" \
    PRODUCT_NAME=A37 \
    TARGET_DEVICE=A37

BUILD_FINGERPRINT := OPPO/A37fw/A37f:5.1.1/LMY47V/1519717163:user/release-keys
