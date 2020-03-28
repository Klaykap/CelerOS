//CELEROS

#define HDA_CORB_MEM 0x01000000
#define HDA_RIRB_MEM 0x02000000
#define HDA_DMA_MEM 0x03000000

#define CORB_ENTRY_2 0x0001
#define CORB_ENTRY_16 0x0002
#define CORB_ENTRY_256 0x0004

#define RIRB_ENTRY_2 0x0001
#define RIRB_ENTRY_16 0x0002
#define RIRB_ENTRY_256 0x0004

uint32_t corb_mem[256];
uint32_t rirb_mem[256];

struct hda_buffers_t {
	uint64_t address;
	uint32_t lenght;
	uint32_t interrupt;
} hda_buffers[256];

void corb_write_command(uint32_t pointer, uint32_t cad, uint32_t nid, uint32_t verb, uint32_t command) {
	uint32_t *corb = (uint32_t *) HDA_CORB_MEM;

	corb[pointer]=( (cad << 28) | (nid << 20) | (verb << 8) | (command) );
}

void hda_init(void) {
	if(hda.base==0) {
		p("This computer doesnt have HDA sound card");
		return;
	}

	p("INITALIZING INTEL HD AUDIO");

	p("base memory:");
	ph(hda.base);

	p("major version:");
	pv( mmio_inb(hda.base, 0x03) );
	p("minor version:");
	pv( mmio_inb(hda.base, 0x02) );

	p("RESETING...");
	mmio_outb(hda.base, 0x4C, 0x00);  //stop DMA CORB engine
	mmio_outb(hda.base, 0x5C, 0x00);  //stop DMA RIRB engine

	pv( mmio_inl(hda.base, 0x08) );  //print actual status

	//reseting
	if( (mmio_inl(hda.base, 0x08) & 0x0001)==0x0000 ) {
		mmio_outl(hda.base, 0x08, 0x0001);
	}
	wait(500);
	if( (mmio_inl(hda.base, 0x08) & 0x0001)==0x0001 ) {
		p("Reset is done!");
	}
	pv( mmio_inl(hda.base, 0x08) );

	p("DISABLING IRQ...");
	mmio_outl(hda.base, 0x20, 0x0000);

	p("SETTING CORB AND RIRB MEMORY...");
	ph( mmio_inl(HDA_CORB_MEM, 0) );
	ph( mmio_inl(HDA_RIRB_MEM, 0) );
	ph( mmio_inl(HDA_DMA_MEM, 0) );

	//CORB
	mmio_outb(hda.base, 0x4C, 0x00);  //stop DMA engine

	mmio_outl(hda.base, 0x40, HDA_CORB_MEM); //corb memory
	mmio_outl(hda.base, 0x44, 0); //corb memory

	mmio_outw(hda.base, 0x4A, 0x8000); //first part of reset corb
	wait(500);
	if( mmio_inw(hda.base, 0x4A)==0x8000 ) { //is first reset right?
		mmio_outw(hda.base, 0x4A, 0x0000); //second part of reset corb
		wait(500);
		if( mmio_inw(hda.base, 0x4A)==0x0000 ) { //is reset complete?
			p("Resetting of CORB is done!");
		}
		else {
			p("ERROR by second resetting CORB!");
			ph( mmio_inw(hda.base, 0x4A) );
		}
	}
	else {
		p("ERROR by first resetting CORB!");
		ph( mmio_inw(hda.base, 0x4A) );
	}
	//get number of commands in memory
	ph( mmio_inb(hda.base, 0x4E) );
	if( (mmio_inb(hda.base, 0x4E) >> 4)==0x01 ) {
		p("DMA CORB support 2 command in memory");
		hda.corb_lenght=CORB_ENTRY_2;
		mmio_outb(hda.base, 0x4E, 0x10);
		mmio_outw(hda.base, 0x48, 0x0002); 	//set corb write pointer 
	}
	else if( (mmio_inb(hda.base, 0x4E) >> 4)==0x02 ) {
		p("DMA CORB support 16 command in memory");
		hda.corb_lenght=CORB_ENTRY_16;
		mmio_outb(hda.base, 0x4E, 0x21);
		mmio_outw(hda.base, 0x48, 0x0010); 	//set corb write pointer 
	}
	else if( (mmio_inb(hda.base, 0x4E) >> 4)==0x04 ) {
		p("DMA CORB support 256 command in memory");
		hda.corb_lenght=CORB_ENTRY_256;
		mmio_outb(hda.base, 0x4E, 0x42);
		mmio_outw(hda.base, 0x48, 0x00FF); 	//set corb write pointer 
	}

	//RIRB
	mmio_outb(hda.base, 0x5C, 0x00);  //stop DMA engine

	mmio_outl(hda.base, 0x50, HDA_RIRB_MEM); //rirb memory
	mmio_outl(hda.base, 0x54, 0); //rirb memory

	//reset rirb write pointer
	mmio_outw(hda.base, 0x58, 0x8000);
	wait(500);

	//get rirb lenght
	if( (mmio_inb(hda.base, 0x5E) >> 4)==0x0001 ) {
		p("RIRB DMA support 2 entries in memory");
		hda.rirb_lenght=RIRB_ENTRY_2;
		mmio_outb(hda.base, 0x5E, 0x10);
	}
	else if( (mmio_inb(hda.base, 0x5E) >> 4)==0x0002 ) {
		p("RIRB DMA support 16 entries in memory");
		hda.rirb_lenght=RIRB_ENTRY_16;
		mmio_outb(hda.base, 0x5E, 0x21);
	}
	else if( (mmio_inb(hda.base, 0x5E) >> 4)==0x0004 ) {
		p("RIRB DMA support 256 entries in memory");
		hda.rirb_lenght=RIRB_ENTRY_256;
		mmio_outb(hda.base, 0x5E, 0x42);
	}

	//DMA
	mmio_outl(hda.base, 0x70, HDA_DMA_MEM); //dma memory
	mmio_outl(hda.base, 0x74, 0); //dma memory
}

void hda_irq(void) {
	p("HDA IRQ");
}

void hda_debug(void) {
	uint32_t *rirb = (uint32_t *) HDA_RIRB_MEM;
	corb_write_command(0, 0, 0, 0xF01, 0);

	mmio_outb(hda.base, 0x4C, 0x02);  //start CORB DMA engine
	mmio_outb(hda.base, 0x5C, 0x02);  //start RIRB DMA engine

	wait(200);

	ph(rirb[0]);
}
