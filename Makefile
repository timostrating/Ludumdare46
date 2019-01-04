PATH := $(DEVKITARM)/bin:$(PATH)

#  Project settings

NAME       := GBAProject
SOURCE_DIR := source
LIB_DIR    := lib
TOOLS_DIR  := tools
BUILD_DIR  := build
SPECS      := -specs=gba.specs

# Compilation settings

CROSS	?= arm-none-eabi-
AS	:= $(CROSS)as
CC	:= $(CROSS)gcc
LD	:= $(CROSS)gcc
OBJCOPY	:= $(CROSS)objcopy

ARCH	:= -mthumb-interwork -mthumb

INCFLAGS := -I$(LIB_DIR)/libtonc/include -I$(LIB_DIR)/minunit
LIBFLAGS := -L$(LIB_DIR)/libtonc/lib -ltonc
ASFLAGS	:= -mthumb-interwork
CFLAGS	:= $(ARCH) -O2 -Wall -fno-strict-aliasing $(INCFLAGS) $(LIBFLAGS)
LDFLAGS	:= $(ARCH) $(SPECS) $(LIBFLAGS)

# Find and predetermine all relevant source files

APP_MAIN_SOURCE := $(shell find $(SOURCE_DIR)/app -name '*main.c')
APP_MAIN_OBJECT:= $(APP_MAIN_SOURCE:%.c=%.o)
APP_SOURCES   := $(shell find $(SOURCE_DIR)/app -name '*.c' ! -name "*main.c"  ! -name "*.test.c")
APP_OBJECTS   := $(APP_SOURCES:%.c=%.o)

TEST_MAIN_SOURCE := $(SOURCE_DIR)/app/main.test.c
TEST_MAIN_OBJECT:= $(TEST_MAIN_SOURCE:%.c=%.o)
TEST_SOURCES   := $(shell find $(SOURCE_DIR)/app -name '*.test.c' ! -name "*main.test.c")
TEST_OBJECTS   := $(TEST_SOURCES:%.c=%.o)

# Build commands and dependencies

build : $(NAME).elf

test : $(NAME)-test.elf

$(NAME).elf : $(APP_OBJECTS) $(APP_MAIN_OBJECT)
	$(LD) $^ $(LDFLAGS) -o $@

$(NAME)-test.elf : $(APP_OBJECTS) $(TEST_OBJECTS) $(TEST_MAIN_OBJECT)
	$(LD) $^ $(LDFLAGS) -o $@

$(APP_OBJECTS) : %.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(APP_MAIN_OBJECT) : $(APP_MAIN_SOURCE)
	$(CC) $(CFLAGS) -c $< -o $@

$(TEST_OBJECTS) : %.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(TEST_MAIN_OBJECT) : $(TEST_MAIN_SOURCE)
	$(CC) $(CFLAGS) -c $< -o $@

$(TEST_MAIN_SOURCE) : $(TEST_OBJECTS)
	$(TOOLS_DIR)/minunit_test_builder/build_main_test_file.sh $(SOURCE_DIR)/app

libtonc.a :

clean :
	@rm -fv *.gba
	@rm -fv *.elf
	@rm -fv *.sav
	@rm -fv *.gbfs
	@rm -rf $(APP_OBJECTS) $(TEST_OBJECTS)
	@rm -rf $(APP_MAIN_OBJECT) $(TEST_MAIN_OBJECT)
	@rm -rf $(TEST_MAIN_SOURCE)
