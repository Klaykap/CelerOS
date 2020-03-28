//CELEROS

void get_time(void);
void sb16_irq(void);
void ohci_irq(uint32_t usb);
void uhci_irq(void);
void hda_irq(void);
void ac97_irq(void);
void usb_handler_irq(void);
void amd_pcnet_irq(void);

uint32_t handler_time_counter=0;
uint32_t handler_for_pit=0;
uint32_t handler_for_uhci=0;

void isr_handler(void) {
	p("ISR");
}

void irq_handler(uint32_t irq) {
	if(irq_device_map[irq]==OHCI) {
		for(int i=0; i<10; i++) {
			if(usb_ports[i].irq_number==irq) {
				ohci_irq(i);
				return;
			}
		}
	}
	if(irq_device_map[irq]==UHCI) {
		uhci_irq();
		return;
	}
	if(irq_device_map[irq]==INTEL_HD_AUDIO) {
		hda_irq();
		return;
	}
	if(irq_device_map[irq]==SOUND_CARD) {
		ac97_irq();
		return;
	}
	if(irq_device_map[irq]==AMD_PCNET_III) {
		amd_pcnet_irq();
		return;
	}
}

void irq0_handler(void) {
	handler_for_pit++;
	handler_time_counter++;

	if(handler_time_counter>1000) {
		get_time();
		handler_time_counter=0;
	}

    outb(0x20, 0x20); //EOI
}

void irq1_handler(void) {
	keyboard_irq();
    outb(0x20, 0x20); //EOI
}

void irq2_handler(void) {
	irq_handler(2);
    outb(0x20, 0x20); //EOI

}

void irq3_handler(void) {
	irq_handler(3);
    outb(0x20, 0x20); //EOI

}

void irq4_handler(void) {
	irq_handler(4);
    outb(0x20, 0x20); //EOI

}

void irq5_handler(void) {
	irq_handler(5);
    outb(0x20, 0x20); //EOI

}

void irq6_handler(void) {
	irq_handler(6);
    outb(0x20, 0x20); //EOI

}

void irq7_handler(void) {
	irq_handler(7);
    outb(0x20, 0x20); //EOI

}

void irq8_handler(void) {
	irq_handler(8);

    outb(0xA0, 0x20);
    outb(0x20, 0x20); //EOI        
}

void irq9_handler(void) {
	irq_handler(9);
    outb(0xA0, 0x20);
    outb(0x20, 0x20); //EOI

}

void irq10_handler(void) {
	irq_handler(10);

    outb(0xA0, 0x20);
    outb(0x20, 0x20); //EOI

}

void irq11_handler(void) {
	irq_handler(11);

    outb(0xA0, 0x20);
    outb(0x20, 0x20); //EOI
}

void irq12_handler(void) {
	irq_handler(12);
 
    outb(0xA0, 0x20);
    outb(0x20, 0x20); //EOI

}

void irq13_handler(void) {
	irq_handler(13);

    outb(0xA0, 0x20);
    outb(0x20, 0x20); //EOI

}

void irq14_handler(void) {
	inb(0x1F7);
	irq_handler(14);

    outb(0xA0, 0x20);
    outb(0x20, 0x20); //EOI
}

void irq15_handler(void) {
	inb(0x177);
	irq_handler(15);

    outb(0xA0, 0x20);
    outb(0x20, 0x20); //EOI
}
