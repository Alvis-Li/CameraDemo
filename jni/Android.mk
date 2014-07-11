LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := CameraDemo
LOCAL_SRC_FILES := CameraDemo.cpp

include $(BUILD_SHARED_LIBRARY)
