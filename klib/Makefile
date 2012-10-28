###############################################################################
#	makefile
#	 by Alex Chadwick
#
#	A makefile script for generation of raspberry pi kernel images.
###############################################################################

# The toolchain to use. arm-none-eabi works, but there does exist 
# arm-bcm2708-linux-gnueabi.
ARMGNU ?= /opt/local/bin/arm-none-eabi

# The intermediate directory for compiled object files.
BUILD = ../build/

# The directory in which source files are stored.
SOURCE = ./

# The name of the output file to generate.
TARGET = $(BUILD)libklib.a

# The names of all object files that must be generated. Deduced from the 
# assembly code files in source.
ASM_OBJECTS := $(patsubst $(SOURCE)%.s,$(BUILD)%.o,$(wildcard $(SOURCE)*.s))
C_OBJECTS   := $(patsubst $(SOURCE)%.c,$(BUILD)%.o,$(wildcard $(SOURCE)*.c))
OBJECTS     := $(ASM_OBJECTS) $(C_OBJECTS)

# Rule to make everything.
all: $(TARGET)

# Rule to remake everything. Does not include clean.
rebuild: all

# Rule to make the image file.
$(TARGET) : $(OBJECTS)
	$(ARMGNU)-ar -r $(TARGET) $(OBJECTS)

# Rule to make the object files.
$(BUILD)%.o: $(SOURCE)%.s
	mkdir -p $(BUILD)
	$(ARMGNU)-as -I $(SOURCE) $< -o $@
	
$(BUILD)%.o : $(SOURCE)%.c
	mkdir -p $(BUILD)
	$(ARMGNU)-gcc -std=c99 -c -o $@ $?


# Rule to clean files.
clean : 
	-rm -f $(BUILD)*.o 
	-rm -f $(BUILD)output.elf
	-rm -f $(TARGET)
	-rm -f $(LIST)
	-rm -f $(MAP)
