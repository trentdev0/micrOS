CC := x86_64-elf-gcc
AS := x86_64-elf-as
OBJCOPY := x86_64-elf-objcopy
CFLAGS := -g -pipe -Wall -Wextra -std=gnu11 -ffreestanding -fno-stack-protector -fno-stack-check -fno-lto -fno-pie -fno-pic -m64 -march=x86-64 -mabi=sysv -mno-80387 -mno-mmx -mno-sse -mno-sse2 -mno-red-zone -mcmodel=kernel -I. -MMD
LDFLAGS := -Tlinker.ld -ffreestanding -nostdlib

SRC_DIR := kernel
SRC_C_FILES := $(wildcard $(SRC_DIR)/*.c)
SRC_S_FILES := $(wildcard $(SRC_DIR)/*.S)
OBJ_C_FILES := $(patsubst $(SRC_DIR)/%.c, $(SRC_DIR)/%.o, $(SRC_C_FILES))
OBJ_S_FILES := $(patsubst $(SRC_DIR)/%.S, $(SRC_DIR)/%.o, $(SRC_S_FILES))
OBJ_FILES := $(OBJ_C_FILES) $(OBJ_S_FILES)

3RDPARTY ?= false
ARCH ?= amd64-pc

ifeq ($(3RDPARTY), true)
	3RDPARTY_SRC_C_FILES := $(wildcard $(SRC_DIR)/thirdparty/*.c)
	3RDPARTY_SRC_S_FILES := $(wildcard $(SRC_DIR)/thirdparty/*.S)
	3RDPARTY_OBJ_C_FILES := $(patsubst $(SRC_DIR)/thirdparty/%.c, $(SRC_DIR)/thirdparty/%.o, $(SRC_C_FILES))
	3RDPARTY_OBJ_S_FILES := $(patsubst $(SRC_DIR)/thirdparty/%.S, $(SRC_DIR)/thirdparty/%.o, $(SRC_S_FILES))
	OBJ_FILES += $(3RDPARTY_OBJ_C_FILES) $(3RDPARTY_OBJ_S_FILES)
endif

ifeq ($(ARCH), amd64-pc)
	ARCH_C_FILES := $(wildcard $(SRC_DIR)/arch/amd64-pc/*.c)
	ARCH_S_FILES := $(wildcard $(SRC_DIR)/arch/amd64-pc/*.S)
	ARCH_OBJ_C_FILES := $(patsubst $(SRC_DIR)/arch/amd64-pc/%.c, $(SRC_DIR)/arch/amd64-pc/%.o, $(ARCH_C_FILES))
	ARCH_OBJ_S_FILES := $(patsubst $(SRC_DIR)/arch/amd64-pc/%.S, $(SRC_DIR)/arch/amd64-pc/%.o, $(ARCH_S_FILES))
	OBJ_FILES += $(ARCH_OBJ_C_FILES) $(ARCH_OBJ_S_FILES)
endif

.PHONY: all clean limine cdrom run run-serial debug

all: kernel.elf cdrom

cdrom:
	mkdir -p bootdisk
	cp -v kernel.elf limine.cfg limine/limine.sys limine/limine-cd.bin limine/limine-cd-efi.bin bootdisk/
	xorriso -as mkisofs -b limine-cd.bin --no-emul-boot -boot-load-size 4 -boot-info-table --efi-boot limine-cd-efi.bin -efi-boot-part --efi-boot-image --protective-msdos-label bootdisk -o image.iso
	limine/limine-deploy image.iso

kernel.elf: $(OBJ_FILES)
	$(CC) $(LDFLAGS) $^ -o $@

$(SRC_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(SRC_DIR)/%.o: $(SRC_DIR)/%.S
	$(AS) $< -o $@

ifeq ($(ARCH), amd64-pc)
	$(SRC_DIR)/arch/amd64-pc/%.o: $(SRC_DIR)/arch/amd64-pc/%.c
		$(CC) $(CFLAGS) -c $< -o $@

	$(SRC_DIR)/arch/amd64-pc/%.o: $(SRC_DIR)/arch/amd64-pc/%.S
		$(AS) $< -o $@
endif

ifeq ($(3RDPARTY), true)
	$(SRC_DIR)/thirdparty/%.o: $(SRC_DIR)/thirdparty/%.c
		$(CC) $(CFLAGS) -c $< -o $@

	$(SRC_DIR)/thirdparty/%.o: $(SRC_DIR)/thirdparty/%.S
		$(AS) $< -o $@
endif

limine:
	git clone https://github.com/limine-bootloader/limine.git --branch=v4.x-branch-binary --depth=1
	make -C limine

run:
	qemu-system-x86_64 -cdrom image.iso

debug:
	qemu-system-x86_64 -s -S -cdrom image.iso

run-serial:
	xterm -hold -e "qemu-system-x86_64 -nographic -serial mon:stdio -cdrom image.iso"

clean:
	rm -f kernel/*.o kernel/*.d kernel/arch/amd64-pc/*.o kernel/arch/amd64-pc/*.d kernel/thirdparty/*.o kernel/thirdparty/*.d kernel.elf image.iso
