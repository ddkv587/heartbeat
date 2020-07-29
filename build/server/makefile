include $(CLEAR_VARS)

TARGET_MODULE  := server

CPP_INCLUDE    := -I$(SOURCE_ROOT) \
                  -I$(SYSROOT)/usr/include
CPP_SRCDIR     := $(SOURCE_ROOT)

#CPP_SRC        := $(shell find $(CPP_SRCDIR) -name "*.cpp")
#C_SRC          := $(shell find $(CPP_SRCDIR) -name "*.c")
CPP_SRC        := $(shell find $(CPP_SRCDIR) -name "server.cpp")

LIBS           += -lssl -lcrypto
LIBS_DIR       += -L$(ROOT_PATH)/target/$(PLATFROM)/$(PROJECT_NAME)/Release
LIBS_DIR_DEBUG += -L$(ROOT_PATH)/target/$(PLATFROM)/$(PROJECT_NAME)/Debug

include $(BUILD_EXECUTABLE)
