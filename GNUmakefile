MAKEFLAGS += -rR
.SUFFIXES:

override OUTPUT := mangOS

CC := cc

CFLAGS := -g -O2 -pipe

CPPFLAGS :=

NASMFLAGS := -F dwarf -g

override CC_IS_CLANG := $(shell ! $(CC) --version 2>/dev/null | grep 'clang' > /dev/null 2>&1; echo $$?)

ifeq ($(CC_IS_CLANG),1)
	override CC +=
		-target x86_64-unknown-none
endif

override CFLAGS += \
	-Wall \
	-Wextra \
	-std=gnu11 \
	-ffreestanding \
	-fno-stack-protector \
	-fno-stack-check \
	-fno-PIC \
	-m64 \
	-march=x86-64 \
	-mno-80387 \
	-mno-mmx \
	-mno-sse \
	-mno-sse2 \
	-mno-red-zone \
	-mcmodel=kernel

override CPPFLAGS := \
	-I src \
	-I src/impl/kernel/modules/utilities/include \
	-I src/impl/x86_64/include \
	$(CPPFLAGS) \
	-DLIMINE_API_REVISION=3 \
	-MMD \
	-MP

override NASMFLAGS += \
	-Wall \
	-f elf64

override LDFLAGS += \
	-Wl,-m,elf_x86_64 \
	-Wl,--build-id=none \
	-nostdlib \
	-static \
	-z max-page-size=0x1000 \
	-T targets/x86_64/linker.ld

override SRCFILES := $(shell cd src && find -L * -type f | LC_ALL=C sort)
override CFILES := $(filter %.c,$(SRCFILES))
override ASFILES := $(filter %.S,$(SRCFILES))
override NASMFILES := $(filter %.asm,$(SRCFILES))
override OBJ := $(addprefix obj/,$(CFILES:.c=.c.o) $(ASFILES:.S=.S.o) $(NASMFILES:.asm=.asm.o))
override HEADER_DEPS := $(addprefix obj/,$(CFILES:.c=.c.d) $(ASFILES:.S=.S.d))

.PHONY: all
all: build/$(OUTPUT)

-include $(HEADER_DEPS)

build/$(OUTPUT): GNUmakefile targets/x86_64/linker.ld $(OBJ)
	@mkdir -p "$$(dirname $@)"
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJ) -o $@

obj/%.c.o: src/%.c GNUmakefile
	mkdir -p "$$(dirname $@)"
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

obj/%.asm.o: src/%.asm GNUmakefile
	mkdir -p "$$(dirname $@)"
	nasm $(NASMFLAGS) $< -o $@


.PHONY: iso
iso: build/$(OUTPUT)
	cp -v  build/mangOS targets/x86_64/iso/boot

	xorriso -as mkisofs \
	 -R -r -J \
	 -b boot/limine/limine-bios-cd.bin \
        -no-emul-boot \
	-boot-load-size 4 \
	-boot-info-table -hfsplus \
        -apm-block-size 2048 \
	--efi-boot boot/limine/limine-uefi-cd.bin \
        -efi-boot-part \
	--efi-boot-image \
	--protective-msdos-label \
        ./targets/x86_64/iso/ -o image.iso

.PHONY: clean
clean:
	rm -rf bin obj