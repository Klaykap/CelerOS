//CELEROS

#define PCNET_EEPROM0 0x00
#define PCNET_EEPROM2 0x02
#define PCNET_EEPROM4 0x04
#define PCNET_RDP 0x10
#define PCNET_RAP 0x14
#define PCNET_RESET 0x18
#define PCNET_BDP 0x1C

/*struct icard_pcnet_init_block_t {
	uint8_t mode[1];
	uint8_t rlen;
	uint8_t tlen;
	uint8_t mac[5];
	uint16_t reserved;
	uint64_t logical_address;
	uint32_t receive_descriptor;
	uint32_t transmit_descriptor;
} icard_pcnet_init_block; */

uint32_t icard_pcnet_init_block[6];

/*struct icard_pcnet_transmit_descriptor_t {
	uint32_t address;
	uint32_t flags;
	uint32_t flags2;
	uint32_t free;
} icard_pcnet_transmit_descriptor[512];*/

uint32_t icard_pcnet_transmit_descriptor[8][4];

/*struct icard_pcnet_receive_descriptor_t {
	uint32_t address;
	uint32_t flags;
	uint32_t flags2;
	uint32_t free;
} icard_pcnet_receive_descriptor[512];*/

uint32_t icard_pcnet_receive_descriptor[8][4];

uint8_t amd_pcnet_buffer_transmit[8][2048];
uint8_t amd_pcnet_buffer_receive[8][2048];

uint32_t amd_pcnet_receive_packet_number=0;
uint32_t amd_pcnet_transmit_packet_number=0;

void amd_pcnet_write_rap(uint32_t val) {
	outl(icard.base + PCNET_RAP, val);
}

uint32_t amd_pcnet_read_csr(uint32_t offset) {
	outl(icard.base + PCNET_RAP, offset);
	return inl(icard.base + PCNET_RDP);
}

void amd_pcnet_write_csr(uint32_t offset, uint32_t val) {
	outl(icard.base + PCNET_RAP, offset);
	outl(icard.base + PCNET_RDP, val);
}

uint32_t amd_pcnet_read_bcr(uint32_t offset) {
	outl(icard.base + PCNET_RAP, offset);
	return inl(icard.base + PCNET_BDP);
}

void amd_pcnet_write_bcr(uint32_t offset, uint32_t val) {
	outl(icard.base + PCNET_RAP, offset);
	outl(icard.base + PCNET_BDP, val);
}

void amd_pcnet_init(void) {
	p("AMD PC NET");
	ph(icard.base);

	//get MAC address
	icard.mac[0]=(inw(icard.base + PCNET_EEPROM0) & 0xFF);
	icard.mac[1]=(inw(icard.base + PCNET_EEPROM0) >> 8);
	icard.mac[2]=(inw(icard.base + PCNET_EEPROM2) & 0xFF);
	icard.mac[3]=(inw(icard.base + PCNET_EEPROM2) >> 8);
	icard.mac[4]=(inw(icard.base + PCNET_EEPROM4) & 0xFF);
	icard.mac[5]=(inw(icard.base + PCNET_EEPROM4) >> 8);

	//set IP address
	icard.ip[0]=192;
	icard.ip[1]=168;
	icard.ip[2]=56;
	icard.ip[3]=1;

	//reset
	inw(icard.base + 0x14);
	outw(icard.base + 0x14, 0);
	wait(50);
	outw(icard.base + 0x12, 20);
	outw(icard.base + 0x16, 0x0102);

	//set init block
	icard_pcnet_init_block[0]=0x30300000;
	icard_pcnet_init_block[1]= ( (icard.mac[3] << 24) | (icard.mac[2] << 16) | (icard.mac[1] << 8) | (icard.mac[0]) );
	icard_pcnet_init_block[2]= ( (icard.mac[5] << 8) |(icard.mac[4]) );
	icard_pcnet_init_block[3]=0;
	icard_pcnet_init_block[4]=0;
	icard_pcnet_init_block[5]=(uint32_t)icard_pcnet_receive_descriptor;
	icard_pcnet_init_block[6]=(uint32_t)icard_pcnet_transmit_descriptor;

	//set descriptors
	for(int i=0; i<8; i++) {
		icard_pcnet_transmit_descriptor[i][0]=( (uint32_t)amd_pcnet_buffer_transmit + (i * 2048) );
		icard_pcnet_transmit_descriptor[i][1]=0x8000F000;
		icard_pcnet_transmit_descriptor[i][2]=0;
		icard_pcnet_transmit_descriptor[i][3]=0;
		icard_pcnet_receive_descriptor[i][0]=( (uint32_t)amd_pcnet_buffer_receive + (i * 2048) );
		icard_pcnet_receive_descriptor[i][1]=0x8000F7FF;
		icard_pcnet_receive_descriptor[i][2]=0;
		icard_pcnet_receive_descriptor[i][3]=0;
	}

	//send init block
	amd_pcnet_write_csr(1, ( ((uint32_t)icard_pcnet_init_block) & 0x0000FFFF) );
	amd_pcnet_write_csr(2, ( ((uint32_t)icard_pcnet_init_block) >> 16) );

	//set interrupts
	amd_pcnet_write_csr(3, 0);

	//enable editing packets to 64 bytes
	amd_pcnet_write_csr(4, (1 << 11));

	//init card
	amd_pcnet_write_csr(0, 0x41);
	wait(5);

	//start card
	amd_pcnet_write_csr(0, 0x42);

	p("MAC");
	ph( icard.mac[0] );
	ph( icard.mac[1] );
	ph( icard.mac[2] );
	ph( icard.mac[3] );
	ph( icard.mac[4] );
	ph( icard.mac[5] );
}

void amd_pcnet_transmit_package(uint32_t address, uint16_t lenght) {
	if(lenght>1518) {
		lenght=1518;
	}

	icard_pcnet_transmit_descriptor[amd_pcnet_transmit_packet_number][0]=address;
	icard_pcnet_transmit_descriptor[amd_pcnet_transmit_packet_number][2]=0;
	icard_pcnet_transmit_descriptor[amd_pcnet_transmit_packet_number][1]=(0x8300F000 | ((-lenght) & 0xFFF) );

	//send packet
	amd_pcnet_write_csr(0, 0x0048);

	//set to next 
	amd_pcnet_transmit_packet_number++;
	if(amd_pcnet_transmit_packet_number==8) {
		amd_pcnet_transmit_packet_number=0;
	}
}

void amd_pcnet_receive_package(void) {
	int size=0;

	while( (icard_pcnet_receive_descriptor[amd_pcnet_transmit_packet_number][1] & 0x80000000) == 0) {
		if( (icard_pcnet_receive_descriptor[amd_pcnet_transmit_packet_number][1] & 0x40000000) == 0) { //no error
			if( (icard_pcnet_receive_descriptor[amd_pcnet_transmit_packet_number][1] & 0x03000000) == 0x03000000) {
				size=(icard_pcnet_receive_descriptor[amd_pcnet_transmit_packet_number][2] & 0xFFF);
				//we have packet
			}
		}

		icard_pcnet_receive_descriptor[amd_pcnet_transmit_packet_number][1]=0x8000F7FF;
		icard_pcnet_receive_descriptor[amd_pcnet_transmit_packet_number][2]=0;

		//set to next 
		amd_pcnet_receive_packet_number++;
		if(amd_pcnet_receive_packet_number==8) {
			amd_pcnet_receive_packet_number=0;
		}

	}
}

void amd_pcnet_irq(void) {
	p("AMD IRQ");
	ph(amd_pcnet_read_csr(0));

	if( (amd_pcnet_read_csr(0) & 0x400) == 0x400) {
		p("Dosiel balicek");
	}
	if( (amd_pcnet_read_csr(0) & 0x200) == 0x200) {
		p("Prenos hotovy");
	}

	amd_pcnet_write_csr(0, amd_pcnet_read_csr(0));
}
