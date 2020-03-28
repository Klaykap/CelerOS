//CELEROS

struct arp_t {
	uint8_t preamble[7];
	uint8_t mac_receive[5];
	uint8_t mac_our[5];
	uint16_t type_of_data;

    uint16_t htype;
    uint16_t ptype;
    uint8_t  hlen;
    uint8_t  plen;
    uint16_t opcode;
    uint8_t  our_mac[5];
    uint8_t  our_ip[3];
    uint8_t  his_mac[5];
    uint8_t  his_ip[3];

	uint8_t pad[17];

	uint32_t crc;
} arp;

void send_arp(void) {
	p("ARP");

	arp.preamble[0]=0x55;
	arp.preamble[1]=0x55;
	arp.preamble[2]=0x55;
	arp.preamble[3]=0x55;
	arp.preamble[4]=0x55;
	arp.preamble[5]=0x55;
	arp.preamble[6]=0x55;
	arp.preamble[7]=0xD5;

	arp.mac_receive[0]=icard.mac[0];
	arp.mac_receive[1]=icard.mac[1];
	arp.mac_receive[2]=icard.mac[2];
	arp.mac_receive[3]=icard.mac[3];
	arp.mac_receive[4]=icard.mac[4];
	arp.mac_receive[5]=icard.mac[5];

	arp.mac_our[0]=icard.mac[0];
	arp.mac_our[1]=icard.mac[1];
	arp.mac_our[2]=icard.mac[2];
	arp.mac_our[3]=icard.mac[3];
	arp.mac_our[4]=icard.mac[4];
	arp.mac_our[5]=icard.mac[5];

	arp.type_of_data=0x0806;

	arp.htype=0x1;
	arp.ptype=0x0800;
	arp.hlen=6;
	arp.plen=4;
	arp.opcode=0x0001;

	arp.our_mac[0]=icard.mac[0];
	arp.our_mac[1]=icard.mac[1];
	arp.our_mac[2]=icard.mac[2];
	arp.our_mac[3]=icard.mac[3];
	arp.our_mac[4]=icard.mac[4];
	arp.our_mac[5]=icard.mac[5];
	arp.our_ip[0]=icard.ip[0];
	arp.our_ip[1]=icard.ip[1];
	arp.our_ip[2]=icard.ip[2];
	arp.our_ip[3]=icard.ip[3];

	arp.his_mac[0]=0xFF;
	arp.his_mac[1]=0xFF;
	arp.his_mac[2]=0xFF;
	arp.his_mac[3]=0xFF;
	arp.his_mac[4]=0xFF;
	arp.his_mac[5]=0xFF;
	arp.his_ip[0]=192;
	arp.his_ip[1]=168;
	arp.his_ip[2]=56;
	arp.his_ip[3]=0xFF;

	for(int i=0; i<18; i++) {
		arp.pad[i]=0;
	}

	arp.crc=0;

	//send
	amd_pcnet_transmit_package( (uint32_t)&arp, sizeof(arp));
}
