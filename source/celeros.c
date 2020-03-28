//CelerOS

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned uint32_t;
typedef unsigned long long uint64_t;

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile ( "inb %1, %0"
                   : "=a"(ret)
                   : "Nd"(port) );
    return ret;
}

static inline uint16_t inw(uint16_t port) {
    uint16_t ret;
    asm volatile ( "inw %1, %0"
                   : "=a"(ret)
                   : "Nd"(port) );
    return ret;
}

static inline uint32_t inl(uint16_t port) {
    uint32_t ret;
    asm volatile ( "inl %1, %0"
                   : "=a"(ret)
                   : "Nd"(port) );
    return ret;
}

static inline void outb(uint16_t port, uint8_t val) {
    asm volatile ( "outb %0, %1" : : "a"(val), "Nd"(port) );
}

static inline void outw(uint16_t port, uint16_t val) {
    asm volatile ( "outw %0, %1" : : "a"(val), "Nd"(port) );
}

static inline void outl(uint16_t port, uint32_t val) {
    asm volatile ( "outl %0, %1" : : "a"(val), "Nd"(port) );
}

uint8_t mmio_inb(uint32_t base, uint32_t offset) {
	uint8_t *memory = (uint8_t *) (base+offset);

	return memory[0];
}

uint16_t mmio_inw(uint32_t base, uint32_t offset) {
	uint16_t *memory = (uint16_t *) (base+offset);

	return memory[0];
}

uint32_t mmio_inl(uint32_t base, uint32_t offset) {
	uint32_t *memory = (uint32_t *) (base+offset);

	return memory[0];
}

uint8_t mmio_outb(uint32_t base, uint32_t offset, uint8_t value) {
	uint8_t *memory = (uint8_t *) (base+offset);

	memory[0]=value;
}

uint16_t mmio_outw(uint32_t base, uint32_t offset, uint16_t value) {
	uint16_t *memory = (uint16_t *) (base+offset);

	memory[0]=value;
}

uint32_t mmio_outl(uint32_t base, uint32_t offset, uint32_t value) {
	uint32_t *memory = (uint32_t *) (base+offset);

	memory[0]=value;
}

void wait(uint32_t miliseconds);

//drivers
#include "core/drivers/font.c"
#include "core/drivers/vesa.c"
#include "core/drivers/vbe3.c"

#include "core/drivers/acpi.c"

#include "core/drivers/pci.c"

#include "core/drivers/ps2_keyboard.c"

#include "core/drivers/ide.c"
#include "core/drivers/ide_hard_disc.c"

#include "core/drivers/filesystem_fat32.c"

#include "core/drivers/handlers.c"

#include "core/drivers/usb.c"
#include "core/drivers/usb_ohci.c"
#include "core/drivers/usb_uhci.c"
#include "core/drivers/usb_ehci.c"
#include "core/drivers/usb_device_keyboard.c"
#include "core/drivers/usb_device_mouse.c"

#include "core/drivers/idt.c"

#include "core/drivers/time.c"

#include "core/drivers/sound_pc_speaker.c"
#include "core/drivers/sound_sb16.c"
#include "core/drivers/sound_ac97.c"
#include "core/drivers/sound_intel_hd_audio.c"

#include "core/drivers/internet_card_amd_pcnet.c"

#include "core/drivers/internet_protocol_arp.c"

//programs

//gui
#include "core/gui.c"
 
void celeros_core(void) {
	read_vesa_info();
	clear_screen(WHITE);

	print("CELEROS - operation system written by Klaykap - version 0.0.1 T", LINE(1), COLUMN(3), BLACK, WHITE);
	print("github.com/Klaykap/CelerOS - everybody is welcome!", LINE(3), COLUMN(3), BLACK, WHITE);

	debug_line = 5;

	p("  > celeros core is starting");
	p("  > scanning ACPI...");
	//acpi();
	p("  > scanning PCI...");
	usb_pointer=0;
	ide_pointer=0;
	for(int i=0; i<16; i++) {
		irq_device_map[i]=0;
	}
	pci_scan_all();
	p("  > setting variabiles...");
	key=0;
	key_ascii=0;
	shift=0;
	capslock=0;
	handler_for_wait=0;
	handler_time_counter=0;
	handler_for_uhci=0;
	uhci_in_data=0;
	uhci_out_data=0;
	p("  > setting interrupts...");
	set_idt();
	p("  > set PIT timer for 1024 hz rate...");
	set_timer(1024);
	p("  > initalite IDE interface...");
	init_ide();
	p("  > initalize FAT32 filesystem...");
	fat_select_device(FAT_HD);
	fat_read_info();
	//fat_test();
	p("  > turn pc speaker off...");
	turn_pc_speaker_off();
	//p("  > resetting sound card Intel HD Audio...");
	//hda_init();
	//hda_debug();
	//p("  > resetting USB ports...");
	//usb_reset_all();
	//ohci_reset(0);
	//ohci_test(0);
	//ohci_reset(0);
	//uhci_packet_set_address(0);
	//uhci_packet_get_configuration(0);
	p("  > setting keyboard SK...");
	//usb_keyboard_set_sk();
	ps2_keyboard_set_sk();
	//p("  > initalize internet card...");
	//amd_pcnet_init();
	//send_arp();
	p("  > initalize sound card...");
	sb16_init();
	sb16_play();

	p("  > CelerOS core is running");
	p("  PRESS ANY KEY TO START GUI");

	//while(1) {
	//	wait(50);
	//	usb_handle_irq();
	//}

	//wait_for_key();
	//start_gui();
}
