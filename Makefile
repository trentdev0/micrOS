CC := x86_64-elf-gcc
AS := x86_64-elf-as
OBJCOPY := x86_64-elf-objcopy
CFLAGS := -g -pipe -Wall -Wextra -std=gnu11 -ffreestanding -fno-stack-protector -fno-stack-check -fno-lto -fno-pie -fno-pic -m64 -march=x86-64 -mabi=sysv -mno-80387 -mno-mmx -mno-sse -mno-sse2 -mno-red-zone -mcmodel=kernel -I. -MMD
LDFLAGS := -ffreestanding -nostdlib

QEMUFLAGS := -cdrom image.iso

KERNEL := kernel
SRC_C_FILES := $(wildcard $(KERNEL)/*.c)
SRC_S_FILES := $(wildcard $(KERNEL)/*.S)
OBJ_C_FILES := $(patsubst $(KERNEL)/%.c, $(KERNEL)/%.o, $(SRC_C_FILES))
OBJ_S_FILES := $(patsubst $(KERNEL)/%.S, $(KERNEL)/%.o, $(SRC_S_FILES))
OBJ_FILES := $(OBJ_C_FILES) $(OBJ_S_FILES)

3RDPARTY ?= true
ARCH ?= amd64-pc

ifeq ($(3RDPARTY), true)
	3RDPARTY_SRC_C_FILES := $(wildcard $(KERNEL)/thirdparty/*.c)
	3RDPARTY_SRC_S_FILES := $(wildcard $(KERNEL)/thirdparty/*.S)
	3RDPARTY_OBJ_C_FILES := $(patsubst $(KERNEL)/thirdparty/%.c, $(KERNEL)/thirdparty/%.o, $(3RDPARTY_SRC_C_FILES))
	3RDPARTY_OBJ_S_FILES := $(patsubst $(KERNEL)/thirdparty/%.S, $(KERNEL)/thirdparty/%.o, $(3RDPARTY_SRC_S_FILES))
	OBJ_FILES += $(3RDPARTY_OBJ_C_FILES) $(3RDPARTY_OBJ_S_FILES)
	CFLAGS += -DTHIRDPARTY
endif

ifeq ($(ARCH), amd64-pc)
	LDFLAGS += -Tkernel/arch/amd64-pc/amd64-pc.ld
	ARCH_C_FILES := $(wildcard $(KERNEL)/arch/amd64-pc/*.c)
	ARCH_S_FILES := $(wildcard $(KERNEL)/arch/amd64-pc/*.S)
	ARCH_OBJ_C_FILES := $(patsubst $(KERNEL)/arch/amd64-pc/%.c, $(KERNEL)/arch/amd64-pc/%.o, $(ARCH_C_FILES))
	ARCH_OBJ_S_FILES := $(patsubst $(KERNEL)/arch/amd64-pc/%.S, $(KERNEL)/arch/amd64-pc/%.o, $(ARCH_S_FILES))
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

$(KERNEL)/%.o: $(KERNEL)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(KERNEL)/%.o: $(KERNEL)/%.S
	$(AS) $< -o $@

ifeq ($(ARCH), amd64-pc)
	$(KERNEL)/arch/amd64-pc/%.o: $(KERNEL)/arch/amd64-pc/%.c
		$(CC) $(CFLAGS) -c $< -o $@

	$(KERNEL)/arch/amd64-pc/%.o: $(KERNEL)/arch/amd64-pc/%.S
		$(AS) $< -o $@
endif

ifeq ($(3RDPARTY), true)
	$(KERNEL)/thirdparty/%.o: $(KERNEL)/thirdparty/%.c
		$(CC) $(CFLAGS) -c $< -o $@

	$(KERNEL)/thirdparty/%.o: $(KERNEL)/thirdparty/%.S
		$(AS) $< -o $@
endif

limine:
	git clone https://github.com/limine-bootloader/limine.git --branch=v4.x-branch-binary --depth=1
	make -C limine

run:
	qemu-system-x86_64 $(QEMUFLAGS)

run-kvm:
	qemu-system-x86_64 $(QEMUFLAGS) --enable-kvm

debug:
	qemu-system-x86_64 -s -S $(QEMUFLAGS)

debug-kvm:
	qemu-system-x86_64 -s -S $(QEMUFLAGS)

clean:
	rm -f kernel/*.o kernel/*.d kernel/arch/amd64-pc/*.o kernel/arch/amd64-pc/*.d kernel/thirdparty/*.o kernel/thirdparty/*.d kernel.elf image.iso