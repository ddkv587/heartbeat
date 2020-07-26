RM                 := rm -rf
MKDIR              := mkdir -p
COPY               := cp -f
TARGET_ARCH        :=
CFLAGS             := -Wall -c -fmessage-length=0 -std=c++11 -MMD -MP $(PLATFROM_MACRO) -fpermissive -O2
CFLAGS_DEBUG       := -Wall -c -fmessage-length=0 -std=c++11 -MMD -MP $(PLATFROM_MACRO) -fpermissive -O0 -g3
LIBS               := -lm -lpthread -lstdc++
LIBS_DIR           := -L$(SYSROOT)/x86_64/lib -L$(SYSROOT)/x86_64/usr/lib
LIBS_DIR_DEBUG     := $(LIBS_DIR)
APP_NAME           :=
TARGET_MODULE      :=
DEPS_LIBRARIES     :=
