include $(CLEAR_VARS)

TARGET_MODULE  := default_library

CPP_INCLUDE    := -I$(SOURCE_ROOT)/src \
                  -I$(SYSROOT)/usr/include
CPP_SRCDIR     := $(SOURCE_ROOT)/src

CPP_SRC        := $(shell find $(CPP_SRCDIR) -name "*.cpp")
C_SRC          := $(shell find $(CPP_SRCDIR) -name "*.c")

LIBS           += 
LIBS_DIR       += -L$(ROOT_PATH)/target/$(PLATFROM)/$(PROJECT_NAME)/Release
LIBS_DIR_DEBUG += -L$(ROOT_PATH)/target/$(PLATFROM)/$(PROJECT_NAME)/Debug

include $(BUILD_SHARED_LIBRARY)