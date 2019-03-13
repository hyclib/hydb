
LOCAL_PATH 		:= $(call my-dir)/../..
MY_HEADER_PATH	:= $(LOCAL_PATH)/../../../hy_bin/latest

include $(CLEAR_VARS)

LOCAL_MODULE    		:= hydb
LOCAL_MODULE_FILENAME 	:= libhydb
LOCAL_C_INCLUDES 		:= $(MY_HEADER_PATH)/include\
						   $(LOCAL_PATH)/ext

LOCAL_SRC_FILES := \
source/sqlite/hy_shell.c \
source/sqlite/hy_sqlite3.c \
source/sqlite/sqliteDataBase.cpp \
source/sqlite/sqliteDataSet.cpp \
source/utils/db_stringUtils.cpp \
source/utils/db_thread_linux.cpp \
source/hydb.cpp
 
#LOCAL_SHARED_LIBRARIES  := libhyc

include $(BUILD_SHARED_LIBRARY)
#include $(MY_HEADER_PATH)/libs/android/Android_export_hyc.mk
