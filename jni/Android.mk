LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)
LOCAL_SRC_FILES := MemDump.c
LOCAL_MODULE := MemDump
include $(BUILD_EXECUTABLE)
