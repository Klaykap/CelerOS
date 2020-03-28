//CELEROS

uint16_t bios[32000];

void vbe3(void) {
	extern int call();

	uint16_t *bios_mem = (uint16_t *) 0xC0000;
	uint8_t *bios8 = (uint8_t *) 0xC0000;
	uint16_t *func = (uint16_t *) 0x2000;

	if(func[1]==0xC000) {
		p("VBE protected mode is here");
	}

	ph(func[0]);
	ph(func[1]);
	ph(func[2]);
	ph(func[3]);

	for(int i=0; i<32000; i++) {
		bios[i]=bios_mem[i];
	}

	for(int i=0; i<32000; i++) {
		if(bios8[i]=='P' && bios8[(i+1)]=='M' && bios8[(i+2)]=='I' && bios8[(i+3)]=='D') {
			p("VBE3");
			for(int j=0; j<20; j++) {
				ph(bios8[i+j]); 
			}
			//set values
			bios8[9] = 0x00;
			bios8[10] = 0x30;
			bios[19] = 1; //protected mode

			//call pm init vesa function
			call(0xC0000 + ((bios8[5] << 8) && bios8[6]));
		}
	}
}
