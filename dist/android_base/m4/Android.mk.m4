LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

# Do not change this value, at least for now : SDL2 expects a libmain.so.
LOCAL_MODULE := main

LOCAL_CFLAGS := GAME_CFLAGS

# Uncomment this to prevent THUMB compilation.
# LOCAL_ARM_MODE := arm

SDL_PATH := ../SDL
LOCAL_C_INCLUDES := $(LOCAL_PATH)/$(SDL_PATH)/include \
                    $(LOCAL_PATH)/../include \
                    $(LOCAL_PATH)/../include/contrib

# Add your application source files here...
FILE_LIST := GAME_SOURCE_FILES
LOCAL_SRC_FILES := $(FILE_LIST:$(LOCAL_PATH)/%=%) $(SDL_PATH)/src/main/android/SDL_android_main.c

LOCAL_SHARED_LIBRARIES := SDL2
LOCAL_STATIC_LIBRARIES := cpufeatures

LOCAL_LDLIBS := -lGLESv1_CM -lGLESv2 -llog

include $(BUILD_SHARED_LIBRARY)

$(call import-module,android/cpufeatures)
