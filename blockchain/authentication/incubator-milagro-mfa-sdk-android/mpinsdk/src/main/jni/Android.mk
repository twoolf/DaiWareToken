LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

PATH_TO_CORE	:= ../../../../mpin-sdk-core/src/crypto

LOCAL_MODULE    :=  AndroidMpinSDK
LOCAL_SRC_FILES :=	../../../../mpin-sdk-core/ext/cvshared/cpp/CvString.cpp \
					../../../../mpin-sdk-core/ext/cvshared/cpp/CvXcode.cpp \
					JNICommon.cpp \
					JNIMPinSDK.cpp \
					JNIUser.cpp \
					HTTPConnector.cpp \
					Storage.cpp \
					Context.cpp \
					$(PATH_TO_CORE)/aes.c \
					$(PATH_TO_CORE)/big.c \
					$(PATH_TO_CORE)/clint.h \
					$(PATH_TO_CORE)/DLLDefines.h \
					$(PATH_TO_CORE)/ecp.c \
					$(PATH_TO_CORE)/ecp2.c \
					$(PATH_TO_CORE)/ff.c \
					$(PATH_TO_CORE)/fp.c \
					$(PATH_TO_CORE)/fp12.c \
					$(PATH_TO_CORE)/fp2.c \
					$(PATH_TO_CORE)/fp4.c \
					$(PATH_TO_CORE)/gcm.c \
					$(PATH_TO_CORE)/hash.c \
					$(PATH_TO_CORE)/mpin.c \
					$(PATH_TO_CORE)/mpin.h \
					$(PATH_TO_CORE)/oct.c \
					$(PATH_TO_CORE)/pair.c \
					$(PATH_TO_CORE)/platform.h \
					$(PATH_TO_CORE)/rand.c \
					$(PATH_TO_CORE)/rom.c \
					$(PATH_TO_CORE)/version.c \
					$(PATH_TO_CORE)/version.h \
					\
					$(PATH_TO_CORE)/../mpin_sdk.cpp \
					$(PATH_TO_CORE)/../utils.cpp \
					$(PATH_TO_CORE)/../mpin_crypto_non_tee.cpp

LOCAL_C_INCLUDES += jni/$(PATH_TO_CORE)/../ ../../../mpin-sdk-core/ext/cvshared/cpp/include
				
LOCAL_CFLAGS := -fexceptions -fomit-frame-pointer
LOCAL_LDLIBS := -llog

include $(BUILD_SHARED_LIBRARY)
