# Copyright (C) 2015-2017 The CyanogenMod Project
# Copyright (C) 2017, The LineageOS Project
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

# Inherit from those products. Most specific first.
$(call inherit-product, $(SRC_TARGET_DIR)/product/generic.mk)
$(call inherit-product, device/oppo/a37/device.mk)
$(call inherit-product, vendor/lineage/config/common_mini_go_phone.mk)

# GMS GO (1GB)
#WITH_GMS := true
#GMS_MAKEFILE := gms_go.mk

#$(call inherit-product, vendor/lineage/config/partner_gms.mk)

# Must define platform variant before including any common things
TARGET_BOARD_PLATFORM_VARIANT := msm8916

# Assert
TARGET_OTA_ASSERT_DEVICE := a37f,A37f,A37fw,a37fw,msm8916,msm8939

TARGET_VENDOR := Oppo
PRODUCT_DEVICE := a37
PRODUCT_NAME := lineage_a37_gms_go
BOARD_VENDOR := Oppo
PRODUCT_BRAND := Oppo
PRODUCT_MODEL := A37
PRODUCT_MANUFACTURER := Oppo

# Device Identifiers
PRODUCT_BUILD_PROP_OVERRIDES += \
     PRODUCT_MODEL="Oppo A37" \
     PRODUCT_NAME="A37" \
     PRODUCT_DEVICE="A37"


# Build fingerprint
PRODUCT_BUILD_PROP_OVERRIDES += \
    PRIVATE_BUILD_DESC="msm8916_64-user 5.1.1 LMY47V eng.root.20190711.032745 release-keys"

BUILD_FINGERPRINT := OPPO/A37fw/A37f:5.1.1/LMY47V/1519717163:user/release-keys

# GMS
PRODUCT_GMS_CLIENTID_BASE := android-oppo

# DocumentsUI overlay
PRODUCT_PACKAGES += \
    DocumentsUIOverlay

# Trust
PRODUCT_PACKAGES += \
    vendor.lineage.trust@1.0-service

# APN(s)
PRODUCT_COPY_FILES += vendor/lineage/prebuilt/common/etc/apns-conf.xml:system/etc/apns-conf.xml
