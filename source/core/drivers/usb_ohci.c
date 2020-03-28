//CELEROS

#define OHCI_REVISION 0x00
#define OHCI_CONTROL 0x04
#define OHCI_COMMAND_STATUS 0x08
#define OHCI_INTERRUPT_STATUS 0x0C
#define OHCI_INTERRUPT_ENABLE 0x10
#define OHCI_HCCA 0x18
#define OHCI_INTERVAL 0x34
#define OHCI_CONTROL_HEAD_ED 0x20
#define OHCI_CONTROL_CURRENT_ED 0x24
#define OHCI_BULK_HEAD_ED 0x28
#define OHCI_BULK_CURRENT_ED 0x2C
#define OHCI_DONE_HEAD 0x30
#define OHCI_PERIODIC_START 0x40
#define OHCI_RH_STATUS 0x50
#define OHCI_PORT_1 0x54
#define OHCI_LEGACY 0x100

#define OHCI_DT_0 0
#define OHCI_DT_1 0x2
#define OHCI_END_TD 0

#define ohci_hcca_mem 0x500000
uint32_t ohci_buffer[1024];

struct ohci_control_ed_t {
	uint32_t control;
	uint32_t tail_td;
	uint32_t head_td;
	uint32_t next_ed;
} ohci_control_ed[10][1024];

struct ohci_bulk_ed_t {
	uint32_t control;
	uint32_t tail_td;
	uint32_t head_td;
	uint32_t next_ed;
} ohci_bulk_ed[10][1024];

struct ohci_td_t {
	uint32_t control;
	uint32_t buffer_first;
	uint32_t next_td;
	uint32_t buffer_last;
} ohci_td[10][1024];

void ohci_run(uint32_t usb) {
	mmio_outl(usb_ports[usb].base, OHCI_CONTROL, (0x100 | 0x80)); //run state and interrupts enable
}

void ohci_stop(uint32_t usb) {
	mmio_outl(usb_ports[usb].base, OHCI_CONTROL, (0x100 | 0xC0)); //suspend state and interrupts enable
}

void ohci_reset(uint32_t usb) {
	uint32_t ohci_interval=0;

	if(usb_ports[usb].base==0) {
		p("This isnt OHCI port");
		return;
	}

	p("OHCI port");
	ph( usb_ports[usb].base );
	p("OHCI revision:");
	ph( mmio_inl(usb_ports[usb].base, OHCI_REVISION) );

	//disabling bios legacy support
	mmio_outl(usb_ports[usb].base, OHCI_LEGACY, 0);

	//save interval
	ohci_interval=mmio_inl(usb_ports[usb].base, OHCI_INTERVAL);
	ph(ohci_interval);

	p("Resetting OHCI...");
	mmio_outl(usb_ports[usb].base, OHCI_COMMAND_STATUS, (mmio_inl(usb_ports[usb].base, OHCI_COMMAND_STATUS) | 0x1) );
	wait(100);
	if( (mmio_inl(usb_ports[usb].base, OHCI_COMMAND_STATUS) & 0x1)==0) {
		p("OHCI port is reset");
	}
	else {
		p("OHCI isnt reset");
	}

	//set interval
	mmio_outl(usb_ports[usb].base, OHCI_INTERVAL, ohci_interval);

	//set some values
	mmio_outl(usb_ports[usb].base, OHCI_HCCA, ohci_hcca_mem);
	mmio_outl(usb_ports[usb].base, OHCI_CONTROL_CURRENT_ED, (uint32_t)&ohci_control_ed[usb][0]);
	mmio_outl(usb_ports[usb].base, OHCI_CONTROL_HEAD_ED, (uint32_t)&ohci_control_ed[usb][0]);
	mmio_outl(usb_ports[usb].base, OHCI_BULK_CURRENT_ED, (uint32_t)&ohci_bulk_ed[usb][0]);
	mmio_outl(usb_ports[usb].base, OHCI_BULK_HEAD_ED, (uint32_t)&ohci_bulk_ed[usb][0]);

	//enable interrupts
	mmio_outl(usb_ports[usb].base, OHCI_INTERRUPT_ENABLE, 0xC0000004);

	//set periodic value
	mmio_outl(usb_ports[usb].base, OHCI_PERIODIC_START, (ohci_interval * 0.9));

	//power ports
	mmio_outl(usb_ports[usb].base, OHCI_RH_STATUS, 0x800);

	//enable OHCI port 1
	mmio_outl(usb_ports[usb].base, OHCI_PORT_1, 0x00000010); //reset
	wait(100);
	mmio_outl(usb_ports[usb].base, OHCI_PORT_1, 0x00100102);
	wait(100);

	if( (mmio_inl(usb_ports[usb].base, OHCI_PORT_1) & 0x09)==0x09 ) {
		p("LOW SPEED PORT");
	}
	else {
		p("FULL SPEED PORT");
	} 

	//enable interrupts and start usb
	mmio_outl(usb_ports[usb].base, OHCI_CONTROL, (0x100 | 0x80));
}

void ohci_set_control_ed(uint32_t usb, uint32_t ed, uint32_t control, uint32_t mem_td_start, uint32_t data_toggle) {
	ohci_control_ed[usb][ed].control=control;
	ohci_control_ed[usb][ed].tail_td=(mem_td_start + 0x20);
	ohci_control_ed[usb][ed].head_td=(mem_td_start | data_toggle);
	ohci_control_ed[usb][ed].next_ed=0;
}

void ohci_set_td(uint32_t usb, uint32_t td, uint32_t next_td, uint32_t control, uint32_t buffer_start, uint32_t buffer_end) {
	ohci_td[usb][td].control=control;
	ohci_td[usb][td].buffer_first=buffer_start;
	ohci_td[usb][td].next_td=next_td;
	ohci_td[usb][td].buffer_last=buffer_end;
}

void ohci_test(uint32_t usb) {
	uint32_t done_head;

	done_head=mmio_inl(usb_ports[usb].base, OHCI_DONE_HEAD);

	ohci_buffer[0]=0x00010500;
	ohci_buffer[1]=0x00000000;

	ohci_set_control_ed(usb, 0, 0x00080000, (uint32_t)&ohci_td[usb][0], 0);

	ohci_set_td(usb, 0, (uint32_t)&ohci_td[1], 0xE2E00000, (uint32_t)&ohci_buffer[0], (uint32_t)&ohci_buffer[2]);
	ohci_set_td(usb, 1, OHCI_END_TD, 0xE3100000, 0, 0);

	wait(1000);

	ph( (uint32_t)&ohci_buffer[0] );

	if( mmio_inl(usb_ports[usb].base, OHCI_DONE_HEAD)!= done_head) {
		p("TRANSFER!");
		ph( mmio_inl(usb_ports[usb].base, OHCI_DONE_HEAD) );
	}

	ph( ohci_control_ed[usb][0].control );
	ph( ohci_td[usb][0].control );
	ph( ohci_td[usb][1].control );
	ph(ohci_buffer[0]);
	ph(ohci_buffer[1]);
	ph(mmio_inl(ohci_hcca_mem, 0));
	ph(mmio_inl(ohci_hcca_mem, 4));
	
}

void ohci_irq(uint32_t usb) {
	if( (mmio_inl(usb_ports[usb].base, OHCI_INTERRUPT_STATUS) & 0x0040) == 0x0040) {
		p("OHCI: RootHubStatusChange");
	}
	if( (mmio_inl(usb_ports[usb].base, OHCI_INTERRUPT_STATUS) & 0x0004) == 0x0004) {
		p("OHCI: StartofFrame");
	}
}
