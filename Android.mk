LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_CFLAGS += -std=c++11

LOCAL_SHARED_LIBRARIES := \
	liblog \
	libcutils \
	libstlport \
	libInfoCtrlClient \
	libutils \
	libbinder

LOCAL_C_INCLUDES += \
	$(LOCAL_PATH) \
	external/stlport/stlport/ \
	bionic \
	frameworks/base/libs/carinfo_client

LOCAL_SRC_FILES := \
	packet.cpp \
	alg_packet.cpp \
	status_packet.cpp \
	cfg_esfla_packet.cpp \
	cfg_esfalg_packet.cpp \
	cfg_msg_packet.cpp \
	cfg_rst_packet.cpp \
	cfg_esfwt_packet.cpp \
	esf_meas_packet.cpp \
	nav_status_packet.cpp \
	time_helper.cpp \
	mcu_client_helper.cpp \
	gnss_device.cpp

#LOCAL_CFLAGS := \
	-DPLATFORM_SDK_VERSION=$(PLATFORM_SDK_VERSION) \
	-DUNIX_API \
	-DANDROID_BUILD

#LOCAL_STATIC_LIBRARIES += libAgnss

LOCAL_MODULE := libadr

LOCAL_MODULE_TAGS := eng

#LOCAL_PRELINK_MODULE := false

# For multi-arch targets LOCAL_MODULE_RELATIVE_PATH must be used
# to make sure each build has its own destination
#ifeq ($(TARGET_2ND_ARCH),)
#@LOCAL_MODULE_PATH := $(TARGET_OUT_SHARED_LIBRARIES)/hw
#else
#LOCAL_MODULE_RELATIVE_PATH := hw
#endif
include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)

LOCAL_MODULE := calibration
LOCAL_SHARED_LIBRARIES := libadr libstlport
LOCAL_C_INCLUDES +=  external/stlport/stlport \
					 bionic

LOCAL_CFLAGS += -std=c++11
LOCAL_SRC_FILES := adr_auto_cal.cpp
LOCAL_MODULE_TAGS := eng
LOCAL_C_INCLUDES += $(LOCAL_PATH)

include $(BUILD_EXECUTABLE)
