LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    		:= hydb
LOCAL_SRC_FILES 		:= libhydb.so
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/../..

include $(PREBUILT_SHARED_LIBRARY)
