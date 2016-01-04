LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE    := skia_android
ifeq ($(TARGET_ARCH_ABI),armeabi-v7a)
    LOCAL_SRC_FILES := libs/arm/libskia_android.a
else ifeq ($(TARGET_ARCH_ABI),armeabi)
    LOCAL_SRC_FILES := libs/arm/libskia_android.a
else ifeq ($(TARGET_ARCH_ABI),x86)
    LOCAL_SRC_FILES := libs/x86/libskia_android.a
endif
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := NativeScript
ifeq ($(TARGET_ARCH_ABI),armeabi-v7a)
    LOCAL_SRC_FILES := libs/arm/libNativeScript.so
else ifeq ($(TARGET_ARCH_ABI),armeabi)
    LOCAL_SRC_FILES := libs/arm/libNativeScript.so
else ifeq ($(TARGET_ARCH_ABI),x86)
    LOCAL_SRC_FILES := libs/x86/libNativeScript.so
endif
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)

LOCAL_CPPFLAGS += -std=c++11 -Wall -Wextra -Wno-unused-parameter
LOCAL_MODULE    := canvas
LOCAL_SRC_FILES := CanvasRenderingContext2D.cpp V8GlobalHelpers.cpp \
				CanvasRenderingContextGL.cpp NSMain.cpp

LOCAL_C_INCLUDES := $(LOCAL_PATH)/include $(LOCAL_PATH)/include/skia/core $(LOCAL_PATH)/include/skia/config
LOCAL_LDLIBS    := -llog -landroid -lz -lEGL -lGLESv2 -ljnigraphics
LOCAL_STATIC_LIBRARIES := skia_android cpufeatures
LOCAL_SHARED_LIBRARIES := NativeScript

include $(BUILD_SHARED_LIBRARY)

$(call import-module,android/cpufeatures)
