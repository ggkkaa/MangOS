#include "idt.h"
#include "../../kernel/kernel.h"
#include "print.h"

void create_IDT_entry(uint32_t vector, void* isr, uint8_t flags, struct IDT_entry *IDT) {
    IDT[vector].offset_1 = (uint64_t)isr;
    IDT[vector].offset_2 = ((uint64_t)isr) >> 16;
    IDT[vector].offset_3 = ((uint64_t)isr) >> 32;
    IDT[vector].flags = flags;
    IDT[vector].selector = 0x08;
}

extern void division_exception();
extern void debug_exception();
extern void breakpoint_exception();
extern void overflow_exception();
extern void bound_range_exceeded_exception();
extern void invalid_opcode_exception();
extern void device_not_available_exception();
extern void double_fault_exception();
extern void coprocessor_segment_exception();
extern void invalid_TSS_exception();
extern void segment_not_present_exception();
extern void stack_segment_exception();
extern void general_protection_exception();
extern void page_fault_exception();
extern void floating_point_exception();
extern void alignment_check_exception();
extern void machine_check_exception();
extern void SIMD_floating_point_exception();
extern void virtualization_exception();
extern void control_protection_exception();
extern void hypervisor_injection_exception();
extern void VMM_communication_exception();
extern void security_exception();


struct IDT_entry IDT[256];

void init_IDT() {
    kllog("Initializing the IDT!", 1, 0);
    create_IDT_entry(0, &division_exception, 0x8E, IDT);
    create_IDT_entry(1, &debug_exception, 0x8E, IDT);
    create_IDT_entry(3, &breakpoint_exception, 0x8E, IDT);
    create_IDT_entry(4, &overflow_exception, 0x8E, IDT);
    create_IDT_entry(5, &bound_range_exceeded_exception, 0x8E, IDT);
    create_IDT_entry(6, &invalid_opcode_exception, 0x8E, IDT);
    create_IDT_entry(7, &device_not_available_exception, 0x8E, IDT);
    create_IDT_entry(8, &double_fault_exception, 0x8E, IDT);
    create_IDT_entry(9, &coprocessor_segment_exception, 0x8E, IDT);
    create_IDT_entry(10, &invalid_TSS_exception, 0x8E, IDT);
    create_IDT_entry(11, &segment_not_present_exception, 0x8E, IDT);
    create_IDT_entry(12, &stack_segment_exception, 0x8E, IDT);
    create_IDT_entry(13, &general_protection_exception, 0x8E, IDT);
    create_IDT_entry(14, &page_fault_exception, 0x8E, IDT);
    create_IDT_entry(16, &floating_point_exception, 0x8E, IDT);
    create_IDT_entry(17, &alignment_check_exception, 0x8E, IDT);
    create_IDT_entry(18, &machine_check_exception, 0x8E, IDT);
    create_IDT_entry(19, &SIMD_floating_point_exception, 0x8E, IDT);
    create_IDT_entry(20, &virtualization_exception, 0x8E, IDT);
    create_IDT_entry(21, &control_protection_exception, 0x8E, IDT);
    create_IDT_entry(28, &hypervisor_injection_exception, 0x8E, IDT);
    create_IDT_entry(29, &VMM_communication_exception, 0x8E, IDT);
    create_IDT_entry(30, &security_exception, 0x8E, IDT);
    kllog("IDT entries created!", 1, 0);
    kernel.idtr.size = (sizeof(struct IDT_entry) * 256) - 1;
    kernel.idtr.offset = (uint64_t) IDT;
    asm("lidt %0" : : "m" (kernel.idtr));
    kllog("IDT has been created.", 1, 0);
}