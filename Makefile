CC = arm-none-eabi-gcc
AS = arm-none-eabi-as
LD = arm-none-eabi-ld
OBJCOPY = arm-none-eabi-objcopy

#CFLAGS = -mcpu=arm1176jzf-s -ffreestanding -fno-pic -fno-stack-protector -O1

CFLAGS += -mcpu=arm968e-s
CFLAGS += -march=armv5te
CFLAGS += -marm 
CFLAGS += -msoft-float
CFLAGS += -ffreestanding -nostdlib

LDFLAGS = -T kernel.ld

BUILD_DIR = build

TARGET = $(BUILD_DIR)/kernel.elf
BINARY = $(BUILD_DIR)/kernel.bin
PATCHED = "QI\#FC\#1C.bin"
DEST = /mnt/c/Users/Kerem/Downloads


C_SOURCES = $(wildcard *.c)
S_SOURCES = $(wildcard *.s)

C_OBJS = $(patsubst %.c,$(BUILD_DIR)/%.o,$(C_SOURCES))
S_OBJS = $(patsubst %.s,$(BUILD_DIR)/%.o,$(S_SOURCES))

OBJS = $(C_OBJS) $(S_OBJS)

all: $(BINARY) $(PATCHED)
	

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BINARY): $(TARGET)
	$(OBJCOPY) -O binary --gap-fill=0x00 $(TARGET) $(BINARY)

$(TARGET): $(OBJS)
	$(LD) $(LDFLAGS) -o $(TARGET) $(OBJS)

# C -> build/*.o
$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# ASM -> build/*.o
$(BUILD_DIR)/%.o: %.s | $(BUILD_DIR)
	$(AS) -mcpu=arm968e-s $< -o $@


$(PATCHED): $(BINARY)
	python3 patch.py $(PATCHED) $(BINARY) $(PATCHED)  
#   cp $(PATCHED) $(DEST)/
#   cp $(BINARY) $(DEST)/
	
clean:
	rm -rf $(BUILD_DIR)