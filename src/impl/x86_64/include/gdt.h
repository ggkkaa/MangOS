struct GDTR {
    uint16_t size;       // GDT size minus 1
    uint64_t address;    // Base address of the GDT
} __attribute__((packed));  

void init_GDT();
void create_system_segment_descriptor(uint64_t *GDT, uint8_t idx, uint64_t base, uint64_t limit, uint64_t access, uint64_t flags);