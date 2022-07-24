#include <stdint.h>
#include <stddef.h>
#include <dev/char/serial.h>
#include <dev/char/console.h>
#include <dev/lapic.h>
#include <lib/elf.h>
#include <lib/print.h>
#include <mm/pmm.h>
#include <mm/slab.h>
#include <mm/vmm.h>
#include <sys/gdt.h>
#include <sys/idt.h>
#include <sys/except.h>
#include <fs/vfs/vfs.h>
#include <limine.h>
#include <fs/initramfs.h>
#include <fs/tmpfs.h>
#include <fs/devtmpfs.h>
#include <sched/sched.h>

// The Limine requests can be placed anywhere, but it is important that
// the compiler does not optimise them away, so, usually, they should
// be made volatile or equivalent.

static volatile struct limine_bootloader_info_request boot_info_request = {
    .id = LIMINE_BOOTLOADER_INFO_REQUEST,
    .revision = 0
};

static void done(void) {
    for (;;) {
        __asm__("hlt");
    }
}

void kmain_thread(void);

// The following will be our kernel's entry point.
void _start(void) {
    serial_init();
    gdt_init();
    idt_init();
    except_init();
    pmm_init();
    slab_init();
    vmm_init();

    kernel_process = ALLOC(struct process);
    kernel_process->mmap_anon_base = 0x80000000000;
    kernel_process->pagemap = vmm_kernel_pagemap;

    cpu_init();

    sched_new_kernel_thread(kmain_thread, NULL, true);
    sched_await();
}

void kmain_thread(void) {
    vfs_init();
    tmpfs_init();
    devtmpfs_init();
    vfs_mount(vfs_root, NULL, "/", "tmpfs");
    vfs_create(vfs_root, "/dev", 0755 | S_IFDIR);
    vfs_mount(vfs_root, NULL, "/dev", "devtmpfs");
    console_init();
    initramfs_init();

    struct pagemap *bash_vm = vmm_new_pagemap();
    struct auxval bash_auxv, ld_auxv;
    const char *ld_path;

    struct vfs_node *bin_bash = vfs_get_node(vfs_root, "/bin/bash", true);
    elf_load(bash_vm, bin_bash->resource, 0x0, &bash_auxv, &ld_path);

    struct vfs_node *ld = vfs_get_node(vfs_root, ld_path, true);
    elf_load(bash_vm, ld->resource, 0x40000000, &ld_auxv, NULL);

    const char *argv[] = {"/bin/bash", "-l", NULL};
    const char *envp[] = {"USER=lyre", "HOME=/", "TERM=linux", NULL};

    struct process *bash_proc = sched_new_process(NULL, bash_vm);
    sched_new_user_thread(bash_proc, (void *)ld_auxv.at_entry,
                          NULL, NULL, argv, envp, &bash_auxv, true);

    print("Hello, %s!\n", "world");

    if (boot_info_request.response) {
        print("Booted by %s %s\n", boot_info_request.response->name,
            boot_info_request.response->version);
    }

    done();
}
