PATH := $(DEVKITARM)/bin:$(PATH)

#  Project settings

NAME       := GBAProject
SOURCE_DIR := source
LIB_DIR    := lib
TOOLS_DIR  := tools
BUILD_DIR  := build
SPECS      := -specs=gba.specs

# Find all additional build tools

BUILD_MAIN_TEST_FILE := $(TOOLS_DIR)/minunit_test_builder/build_main_test_file.sh
TILED2GBA_DIR := $(TOOLS_DIR)/tiled2gba
TILED2GBA_CONVERTER_DIR := $(TILED2GBA_DIR)/converter
TILED2GBA_CONVERTER := $(TILED2GBA_CONVERTER_DIR)/build/Tiled2GBA
TILED2GBA_PLAYER_DIR := $(TILED2GBA_DIR)/player
TILED2GBA_PLAYER_LIB := $(TILED2GBA_PLAYER_DIR)/lib/gbaMap

MIDI2GBA_DIR := $(TOOLS_DIR)/midi2gba
MIDI2GBA_CONVERTER_DIR := $(MIDI2GBA_DIR)/converter
MIDI2GBA_CONVERTER := $(MIDI2GBA_CONVERTER_DIR)/build/MIDI2GBA
MIDI2GBA_PLAYER_DIR := $(MIDI2GBA_DIR)/player
MIDI2GBA_PLAYER_LIB := $(MIDI2GBA_PLAYER_DIR)/lib/gbaAudio

# Compilation settings

CROSS	?= arm-none-eabi-
AS	:= $(CROSS)as
CC	:= $(CROSS)gcc
LD	:= $(CROSS)gcc
OBJCOPY	:= $(CROSS)objcopy

ARCH	:= -mthumb-interwork -mthumb

INCFLAGS := -I$(LIB_DIR)/libtonc/include -I$(LIB_DIR)/minunit -I$(MIDI2GBA_PLAYER_LIB)/include -I$(TILED2GBA_PLAYER_LIB)/include -I$(LIB_DIR)/gbfs/include
LIBFLAGS := -L$(LIB_DIR)/libtonc/lib -ltonc -L$(MIDI2GBA_PLAYER_LIB)/lib -lgbaAudio -L$(TILED2GBA_PLAYER_LIB)/lib -lgbaMap -L$(LIB_DIR)/gbfs/lib -lgbfs
ASFLAGS	:= -mthumb-interwork
CFLAGS	:= $(ARCH) -O2 -Wall -fno-strict-aliasing $(INCFLAGS) $(LIBFLAGS)
LDFLAGS	:= $(ARCH) $(SPECS) $(LIBFLAGS)

.PHONY : build clean

# Find and predetermine all relevant source files

APP_MAIN_SOURCE := $(shell find $(SOURCE_DIR)/app -name '*main.c')
APP_MAIN_OBJECT := $(APP_MAIN_SOURCE:%.c=%.o)
APP_SOURCES     := $(shell find $(SOURCE_DIR)/app -name '*.c' ! -name "*main.c"  ! -name "*.test.c")
APP_OBJECTS     := $(APP_SOURCES:%.c=%.o)

TEST_MAIN_SOURCE := $(SOURCE_DIR)/app/main.test.c
TEST_MAIN_OBJECT := $(TEST_MAIN_SOURCE:%.c=%.o)
TEST_SOURCES     := $(shell find $(SOURCE_DIR)/app -name '*.test.c' ! -name "*main.test.c")
TEST_OBJECTS     := $(TEST_SOURCES:%.c=%.o)

GBFS_OUT     := content.gbfs
MAP_ASSETS   := $(shell find $(SOURCE_DIR)/assets -name '*.tmx')
MAP_BINARIES := $(MAP_ASSETS:%.tmx=%.bin)

MIDI_ASSETS   := $(shell find $(SOURCE_DIR)/assets -name '*.mid')
MIDI_BINARIES := $(MIDI_ASSETS:%.mid=%.bin)

# Build commands and dependencies

build : libs $(NAME).gba

test : libs $(NAME)-test.gba

libs: $(MIDI2GBA_PLAYER_LIB) $(TILED2GBA_PLAYER_LIB) $(LIB_DIR)/libtonc $(LIB_DIR)/gbfs

$(NAME).gba : $(NAME)-no_content.gba $(GBFS_OUT)
	cat $^ > $@

$(NAME)-test.gba : $(NAME)-test.elf
	$(OBJCOPY) -v -O binary $< $@
	-@gbafix $@ -t$(NAME)
	padbin 256 $@ 

$(NAME)-no_content.gba : $(NAME).elf
	$(OBJCOPY) -v -O binary $< $@
	-@gbafix $@ -t$(NAME)
	padbin 256 $@ 

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

$(TEST_MAIN_SOURCE) : $(TEST_OBJECTS) $(BUILD_MAIN_TEST_FILE)
	$(BUILD_MAIN_TEST_FILE) $(SOURCE_DIR)/app

$(GBFS_OUT) : $(MAP_BINARIES) $(MIDI_BINARIES)
	gbfs $@ $^

$(MAP_BINARIES) : %.bin : %.tmx $(TILED2GBA_CONVERTER)
	$(TILED2GBA_CONVERTER) $< $@ --binary

$(TILED2GBA_CONVERTER):
	mkdir $(TILED2GBA_CONVERTER_DIR)/build
	cd $(TILED2GBA_CONVERTER_DIR)/build && cmake ..
	cd $(TILED2GBA_CONVERTER_DIR)/build && make

$(TILED2GBA_PLAYER_LIB): $(TILED2GBA_PLAYER_LIB)/lib

$(TILED2GBA_PLAYER_LIB)/lib:
	cd $(TILED2GBA_PLAYER_LIB) && make

$(MIDI_BINARIES): %.bin : %.mid $(MIDI2GBA_CONVERTER)
	$(MIDI2GBA_CONVERTER) $< $@

$(MIDI2GBA_CONVERTER):
	mkdir $(MIDI2GBA_CONVERTER_DIR)/build
	cd $(MIDI2GBA_CONVERTER_DIR)/build && cmake ..
	cd $(MIDI2GBA_CONVERTER_DIR)/build && make

$(MIDI2GBA_PLAYER_LIB): $(MIDI2GBA_PLAYER_LIB)/lib

$(MIDI2GBA_PLAYER_LIB)/lib:
	cd $(MIDI2GBA_PLAYER_LIB) && make

$(LIB_DIR)/libtonc: $(LIB_DIR)/libtonc/lib

$(LIB_DIR)/libtonc/lib:
	cd $(LIB_DIR)/libtonc && make

$(LIB_DIR)/gbfs: $(LIB_DIR)/gbfs/lib

$(LIB_DIR)/gbfs/lib:
	cd $(LIB_DIR)/gbfs && make

clean :
	@rm -fv *.gba
	@rm -fv *.elf
	@rm -fv *.sav
	@rm -fv *.gbfs
	@rm -fv $(MAP_BINARIES) $(MIDI_BINARIES)
	@rm -rf $(APP_OBJECTS) $(TEST_OBJECTS)
	@rm -rf $(APP_MAIN_OBJECT) $(TEST_MAIN_OBJECT)
	@rm -rf $(TEST_MAIN_SOURCE)
