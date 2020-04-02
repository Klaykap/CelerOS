//CELEROS

void wait(uint32_t miliseconds);

#define BIT(x) (1 << x)
#define MEM_UHCI_TD(td) ( (uint32_t)uhci_td + (td * 32) )
#define MEM_UHCI_PACKET(mem) ( (uint32_t)uhci_packet + (mem * 4) )

#define UHCI_COMMAND 0x00
#define UHCI_STATUS 0x02
#define UHCI_INTERRUPT 0x04
#define UHCI_FRAME_NUMBER 0x06
#define UHCI_FRAME_LIST_ADDRESS 0x08
#define UHCI_START_OF_FRAME 0x0C
#define UHCI_SC1 0x10
#define UHCI_SC2 0x12

//TD pointer
#define MEM_TD(usb, td) ( (uint32_t)uhci_td + (usb * 131072) + (td * 32) )
#define END_TD 0x00000001

//TD status
#define STS_LOW_SPEED 0x04000000

//TD header
#define PACKET_IN 0x69
#define PACKET_OUT 0xE1
#define PACKET_SETUP 0x2D
#define PACKET_SOF 0xA5
#define PACKET_ACK 0xD2
#define CONF_ENDPOINT 0
#define DATA_TOGGLE_0 0
#define DATA_TOGGLE_1 1
#define LOW_SPEED_LENGHT 7
#define FULL_SPEED_LENGHT 63
#define ZERO_DEVICE 0

//TD address
#define MEM_PACKET(usb, x) ( (uint32_t)uhci_packet + (usb * 4096) + (x * 4) )

uint32_t uhci_packet[10][1024];
uint32_t uhci_td[10][1024][8];
uint32_t uhci_qh[10][10][2];
uint32_t uhci_frame_mem[10][1024];

uint32_t uhci_in_data=0;
uint32_t uhci_out_data=0;

void write_uhci_cmd(uint32_t usb, uint16_t value) {
    outw(usb_ports[usb].base + UHCI_COMMAND, value);
}

void write_uhci_sts(uint32_t usb, uint16_t value) {
    outw(usb_ports[usb].base + UHCI_STATUS, value);
}

void write_uhci_int(uint32_t usb, uint16_t value) {
    outw(usb_ports[usb].base + UHCI_INTERRUPT, value);
}

void write_uhci_frame_addr(uint32_t usb, uint32_t value) {
    outl(usb_ports[usb].base + UHCI_FRAME_LIST_ADDRESS, value);
}

void write_uhci_frame_num(uint32_t usb, uint16_t value) {
    outw(usb_ports[usb].base + UHCI_FRAME_NUMBER, value);
}

void write_uhci_sofmod(uint32_t usb, uint16_t value) {
    outb(usb_ports[usb].base + UHCI_START_OF_FRAME, value);
}

void write_uhci_sc1(uint32_t usb, uint16_t value) {
    outw(usb_ports[usb].base + UHCI_SC1, value);
}

void write_uhci_sc2(uint32_t usb, uint16_t value) {
    outw(usb_ports[usb].base + UHCI_SC2, value);
}

uint16_t read_uhci_cmd(uint32_t usb) {
    return inw(usb_ports[usb].base + UHCI_COMMAND);
}

uint16_t read_uhci_sts(uint32_t usb) {
    return inw(usb_ports[usb].base + UHCI_STATUS);
}

uint16_t read_uhci_int(uint32_t usb) {
    return inw(usb_ports[usb].base + UHCI_INTERRUPT);
}

uint16_t read_uhci_frame_num(uint32_t usb) {
    return inw(usb_ports[usb].base + UHCI_FRAME_NUMBER);
}

uint32_t read_uhci_frame_addr(uint32_t usb) {
    return inl(usb_ports[usb].base + UHCI_FRAME_LIST_ADDRESS);
}

uint16_t read_uhci_sofmod(uint32_t usb) {
    return inb(usb_ports[usb].base + UHCI_START_OF_FRAME);
}

uint16_t read_uhci_sc1(uint32_t usb) {
    return inw(usb_ports[usb].base + UHCI_SC1);
}

uint16_t read_uhci_sc2(uint32_t usb) {
    return inw(usb_ports[usb].base + UHCI_SC2);
}

void uhci_stop(uint32_t usb) {
	write_uhci_cmd(usb, (read_uhci_cmd(usb) & 0xFFFE));
}

void uhci_run(uint32_t usb) {
	write_uhci_cmd(usb, (read_uhci_cmd(usb) | 0x0081));
}

void uhci_clear_irq(uint32_t usb) {
	write_uhci_sts(usb, (read_uhci_sts(usb) & 0xFFFE));
}

void uhci_disable_interrupts(uint32_t usb) {
	write_uhci_int(usb, 0);
}

void uhci_enable_interrupts(uint32_t usb) {
	write_uhci_int(usb, 0xF);
}

void uhci_disable_controller(uint32_t usb) {
	write_uhci_cmd(usb, 0);
}

void uhci_switch_off_port(uint32_t usb) {
	write_uhci_sc1(usb, 0);
}

void uhci_write_td(uint32_t usb, uint32_t td) {
	p("TD:");
	ph(uhci_td[usb][td][1]);
}

void uhci_debug(uint32_t usb) {
	ph(read_uhci_cmd(usb));
	ph(read_uhci_sts(usb));
	ph(read_uhci_int(usb));
	ph(read_uhci_frame_addr(usb));
	ph(read_uhci_frame_num(usb));
	ph(read_uhci_sofmod(usb));
	ph(read_uhci_sc1(usb));
	ph(read_uhci_sc2(usb));
}

void uhci_reset(uint32_t usb) {
	uint32_t pom=0;

	if(usb_ports[usb].base==0) {
		p("This isnt UHCI port");
		return;
	}

	//clear uhci memory
	for(int i=0; i<1024; i++) {
		uhci_frame_mem[usb][i]=1;

		uhci_td[usb][i][0]=0;
		uhci_td[usb][i][1]=0;
		uhci_td[usb][i][2]=0;
		uhci_td[usb][i][3]=0;
		uhci_td[usb][i][4]=0;
		uhci_td[usb][i][5]=0;
		uhci_td[usb][i][6]=0;
		uhci_td[usb][i][7]=0;
		uhci_td[usb][i][8]=0;

		uhci_packet[usb][i]=0;
	}

	p("UHCI BASE");
	ph(usb_ports[usb].base);

	//stop uhci port
	uhci_stop(usb);

	//enable in pci space
	pci_write(usb_ports[usb].bus, usb_ports[usb].dev, usb_ports[usb].func, 0x04, (pci_get_command(usb_ports[usb].bus, usb_ports[usb].dev, usb_ports[usb].func) | 5) );

	//reset uhci
	//write_uhci_cmd(usb, (read_uhci_cmd(usb) | 0x0004));
	write_uhci_cmd(usb, 0x0004);
	wait(100);
	write_uhci_cmd(usb, (read_uhci_cmd(usb) & 0xFFFB));
	write_uhci_cmd(usb, (0x0002));
	wait(100);
	write_uhci_cmd(usb, (read_uhci_cmd(usb) & 0xFFFD));

	//disabling uhci legacy support
	pci_write(usb_ports[usb].bus, usb_ports[usb].dev, usb_ports[usb].func, 0xC0, 0x8F00);
	wait(2);
	for(int i=0; i<8; i++) {
		outw(usb_ports[usb].base + UHCI_SC1 + (i*2), 0); //disable all ports
	}
	uhci_disable_interrupts(usb);
	uhci_disable_controller(usb);

	//setting base things
	write_uhci_sofmod(usb, 0x40);
	write_uhci_frame_addr(usb, ((uint32_t)&uhci_frame_mem[usb]) );
	write_uhci_frame_num(usb, 0);

	//reset first port
	write_uhci_sc1(usb, BIT(9));
	wait(100);
	write_uhci_sc1(usb, BIT(4));
	wait(100);

	//get speed
	if( (read_uhci_sc1(usb) & 0x0100) == 0) {
		usb_ports[usb].speed=FULL_SPEED;
		p("FULL SPEED PORT");
	}
	else {
		usb_ports[usb].speed=LOW_SPEED;
		p("LOW SPEED PORT");
	}

	//enable first port
	write_uhci_sc1(usb, 0x000E);

	//enable interrupts
    uhci_enable_interrupts(usb);

	//run uhci
	write_uhci_cmd(usb, BIT(0) | BIT(4) | BIT(6) | BIT(7));
	wait(20);
	write_uhci_cmd(usb, BIT(0) | BIT(6) | BIT(7));
	wait(100);

	if( (read_uhci_sts(usb) & BIT(5))==0 ) {
		p("UHCI ports are enabled!!!");
	}
	else {
		p("ERROR! UHCI ports arent enabled!");
		uhci_debug(usb);
	}

	//stop uhci
	uhci_stop(usb);
}

void uhci_make_td(uint32_t usb, uint32_t td, uint32_t pointer, uint32_t type, uint32_t endpoint, uint32_t data_toggle, uint32_t device, uint32_t lenght, uint32_t address) {
	uhci_td[usb][td][0]=pointer;
	if(usb_ports[usb].speed==FULL_SPEED) {
		uhci_td[usb][td][1]=0x00800000;
	}
	else if(usb_ports[usb].speed==LOW_SPEED) {
		uhci_td[usb][td][1]=0x04800000;
	}
	uhci_td[usb][td][2]=(type | (endpoint << 15) | (lenght << 21) | (data_toggle << 19) | (device << 8));
	uhci_td[usb][td][3]=address;
}

void uhci_make_qh(uint32_t usb, uint32_t qh, uint32_t queue_horizontal, uint32_t queue_vertical) {
	uhci_qh[usb][qh][0]=queue_horizontal;
	uhci_qh[usb][qh][1]=queue_vertical;
}

void uhci_pool(uint32_t usb, uint32_t td) {
	handler_for_pit=0;

	while(1) {
		if( (uhci_td[usb][td][1] & 0x00800000)==0 ) {
			return;
		}
		if(handler_for_pit>2000) {
			p("Something wrong with UHCI TD transfer");
			return;
		}
	}
}

void uhci_packet_set_address(uint32_t usb) {
	uhci_stop(usb);

	uhci_frame_mem[usb][0]=MEM_TD(usb, 20);

	uhci_packet[usb][0]=0x00010500;
	uhci_packet[usb][1]=0x00000000;

	uhci_make_td(usb, 20, MEM_TD(usb, 21), PACKET_SETUP, CONF_ENDPOINT, DATA_TOGGLE_0, ZERO_DEVICE, LOW_SPEED_LENGHT, MEM_PACKET(usb, 0));

	uhci_make_td(usb, 21, END_TD, PACKET_IN, CONF_ENDPOINT, DATA_TOGGLE_1, ZERO_DEVICE, LOW_SPEED_LENGHT, MEM_PACKET(usb, 0));

	write_uhci_frame_num(usb, 0);

	uhci_run(usb);
	uhci_pool(usb, 21);
	uhci_stop(usb);

	uhci_write_td(usb, 20);

	for(int i=0; i<2; i++) {
		ph(uhci_packet[usb][i]);
	}

	uhci_frame_mem[usb][0]=1;
}

void uhci_packet_get_configuration(uint32_t usb) {
	uint32_t uhci_endpoint_type=0;
	uint32_t uhci_endpoint_value=0;
	uint8_t *uhci_packet_pointer = (uint8_t *) uhci_packet;
	uint32_t uhci_mem_pointer=0;

	uhci_stop(usb);

	uhci_frame_mem[usb][1]=MEM_TD(usb, 1);

	uhci_packet[usb][0]=0x02000680;
	uhci_packet[usb][1]=0x002E0000;

	//uhci_make_td(usb, 0, MEM_TD(usb, 1), PACKET_SOF, CONF_ENDPOINT, DATA_TOGGLE_0, ZERO_DEVICE, LOW_SPEED_LENGHT, MEM_PACKET(usb, 0));

	uhci_make_td(usb, 1, MEM_TD(usb, 2), PACKET_SETUP, CONF_ENDPOINT, DATA_TOGGLE_0, ZERO_DEVICE, LOW_SPEED_LENGHT, MEM_PACKET(usb, 0));

	uhci_make_td(usb, 2, MEM_TD(usb, 3), PACKET_IN, CONF_ENDPOINT, DATA_TOGGLE_1, ZERO_DEVICE, LOW_SPEED_LENGHT, MEM_PACKET(usb, 3));

	uhci_make_td(usb, 3, MEM_TD(usb, 4), PACKET_IN, CONF_ENDPOINT, DATA_TOGGLE_0, ZERO_DEVICE, LOW_SPEED_LENGHT, MEM_PACKET(usb, 5));

	uhci_make_td(usb, 4, MEM_TD(usb, 5), PACKET_IN, CONF_ENDPOINT, DATA_TOGGLE_1, ZERO_DEVICE, LOW_SPEED_LENGHT, MEM_PACKET(usb, 7));

	uhci_make_td(usb, 5, MEM_TD(usb, 6), PACKET_IN, CONF_ENDPOINT, DATA_TOGGLE_0, ZERO_DEVICE, LOW_SPEED_LENGHT, MEM_PACKET(usb, 9));

	uhci_make_td(usb, 6, MEM_TD(usb, 7), PACKET_IN, CONF_ENDPOINT, DATA_TOGGLE_1, ZERO_DEVICE, LOW_SPEED_LENGHT, MEM_PACKET(usb, 11));

	uhci_make_td(usb, 7, END_TD, PACKET_OUT, CONF_ENDPOINT, DATA_TOGGLE_1, ZERO_DEVICE, LOW_SPEED_LENGHT, MEM_PACKET(usb, 11));

	write_uhci_frame_num(usb, 1);

	uhci_run(usb);
	uhci_pool(usb, 7);
	uhci_stop(usb);

	for(int i=0; i<7; i++) {
		uhci_write_td(usb, i);
	}

	for(int i=3; i<13; i++) {
		ph(uhci_packet[usb][i]);
	}

	//find interface
	uhci_mem_pointer = 12;
	for(int i=0; i<10; i++) {
		if(uhci_packet_pointer[uhci_mem_pointer+1]==0x04) {
			break;
		}
		uhci_mem_pointer = (uhci_mem_pointer + (uhci_packet_pointer[uhci_mem_pointer]) );
	}

	usb_ports[usb].interface = uhci_packet_pointer[uhci_mem_pointer+2];
	usb_ports[usb].class = uhci_packet_pointer[uhci_mem_pointer+5];
	usb_ports[usb].subclass = uhci_packet_pointer[uhci_mem_pointer+6];
	usb_ports[usb].protocol = uhci_packet_pointer[uhci_mem_pointer+7];

	//find endpoint
	uhci_mem_pointer = 12;
	for(int i=0; i<10; i++) {
		if(uhci_packet_pointer[uhci_mem_pointer+1]==0x05) {
			//get endpoint
			uhci_endpoint_type=(uhci_packet_pointer[uhci_mem_pointer+3] & 0x03);
			uhci_endpoint_value=(uhci_packet_pointer[uhci_mem_pointer+3] & 0x07);

			if( uhci_endpoint_type==1 ) {
				p("isonchronous");
				pv(uhci_endpoint_value);
			}
			if( uhci_endpoint_type==2 ) {
				p("bulk");
				usb_ports[usb].endpoint_bulk = uhci_endpoint_value;
				pv(usb_ports[usb].endpoint_bulk);
			}
			if( uhci_endpoint_type==3 ) {
				p("interrupt");
				usb_ports[usb].endpoint_interrupt = uhci_endpoint_value;
				pv(usb_ports[usb].endpoint_interrupt);
			}
		}
		uhci_mem_pointer = (uhci_mem_pointer + (uhci_packet_pointer[uhci_mem_pointer]) );
	}

	if(usb_ports[usb].class==3 && usb_ports[usb].subclass==1 && usb_ports[usb].protocol==1) {
		p("USB KEYBOARD");
		usb_ports[usb].device=DEVICE_USB_KEYBOARD;
		usb_ports[usb].irq_enable=1;
	}

	if(usb_ports[usb].class==3 && usb_ports[usb].subclass==1 && usb_ports[usb].protocol==2) {
		p("USB MOUSE");
		usb_ports[usb].device=DEVICE_USB_MOUSE;
		usb_ports[usb].irq_enable=1;
	}

	uhci_frame_mem[usb][1]=1;
}

void uhci_packet_in(uint32_t usb) {
	uhci_stop(usb);

	uhci_frame_mem[usb][2]=MEM_TD(usb, 10);

	if(uhci_in_data==0) {
		uhci_make_td(usb, 10, END_TD, PACKET_IN, usb_ports[usb].endpoint_interrupt, DATA_TOGGLE_0, 1, FULL_SPEED_LENGHT, MEM_PACKET(usb, 20));
		uhci_in_data=1;
	}
	else {
		uhci_make_td(usb, 10, END_TD, PACKET_IN, usb_ports[usb].endpoint_interrupt, DATA_TOGGLE_1, 1, FULL_SPEED_LENGHT, MEM_PACKET(usb, 20));
		uhci_in_data=0;
	}

	write_uhci_frame_num(usb, 2);

	uhci_run(usb);
	uhci_pool(usb, 10);
	uhci_stop(usb);

	uhci_write_td(usb, 10);

	uhci_frame_mem[usb][2]=1;
}

void uhci_irq(void) {
	p("UHCI IRQ");
	uhci_clear_irq(0);
}
