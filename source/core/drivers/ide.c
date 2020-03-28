//CELEROS

#define IDE_READ 1
#define IDE_WRITE 2

#define MASTER 1
#define SLAVE 2

#define HD_MASTER 0x40
#define HD_SLAVE 0x50
#define CDROM_MASTER 0xE0
#define CDROM_SLAVE 0xF0

uint16_t hard_disc_base=0;
uint32_t hard_disc_drive=0;
uint16_t cdrom_base=0;
uint32_t cdrom_drive=0;
uint32_t shared_controller_drive=0;

uint32_t ide_select_drive(uint16_t base, uint16_t drive) {
    outb(base+6, drive); //select drive
	wait(10);

	if( (inb(base+7) & 0x80)!=0x80 ) {  //IDE is not busy, drive is select
    	return 1;    
    }

	//something is wrong, try it repeat
    outb(base+6, drive); //select drive
	wait(10);

	if( (inb(base+7) & 0x80)!=0x80 ) {  //IDE is not busy, drive is select
    	return 1;    
    }

    //With selecting drive is something wrong
    return 0;
}

void init_ide(void) {
	uint8_t first_value=0;
	uint8_t second_value=0;
/*
	//PRIMARY CONTROLLER MASTER
	outb(0x1F6, 0xA0); //select master drive
	wait(5);
	outb(0x1F2, 0);
	outb(0x1F3, 0);
	outb(0x1F4, 0);
	outb(0x1F5, 0);
	outb(0x1F7, 0xEC); //identify command
	wait(5);
	inb(0x1F7);
	first_value=inb(0x1F4);
	second_value=inb(0x1F5);
	//detect what is connected on this controller drive
	if(first_value!=0xFF && second_value!=0xFF) {
		if(first_value==0 && second_value==0) { //PATA
			p("IDE: Hard disc on Primary Master");
			hard_disc_base=0x1F0;
			hard_disc_drive=MASTER;
		}
		else if(first_value==0x14 && second_value==0xEB) { //PATAPI
			p("IDE: CDROM on Primary Master");
			cdrom_base=0x1F0;
			cdrom_drive=MASTER;
		}
		else if(first_value==0x3C && second_value==0xC3) {
			p("IDE: Serial hard disc on Primary Master"); //SATA
			hard_disc_base=0x1F0;
			hard_disc_drive=MASTER;
		}
		else if(first_value==0x69 && second_value==0x96) {
			p("IDE: Serial CDROM on Primary Master"); //SATAPI
			cdrom_base=0x1F0;
			cdrom_drive=MASTER;
		}
	}
	//read returned values from drive
	for(int i=0; i<256; i++) {
		inb(0x1F0);
	}

	//PRIMARY CONTROLLER SLAVE
	outb(0x1F6, 0xB0); //select slave drive
	wait(5);
	outb(0x1F2, 0);
	outb(0x1F3, 0);
	outb(0x1F4, 0);
	outb(0x1F5, 0);
	outb(0x1F7, 0xEC); //identify command
	wait(5);
	inb(0x1F7);
	first_value=inb(0x1F4);
	second_value=inb(0x1F5);
	//detect what is connected on this controller drive
	if(first_value!=0xFF && second_value!=0xFF) {
		if(first_value==0 && second_value==0) { //PATA
			p("IDE: Hard disc on Primary Slave");
			hard_disc_base=0x1F0;
			hard_disc_drive=SLAVE;
		}
		else if(first_value==0x14 && second_value==0xEB) { //PATAPI
			p("IDE: CDROM on Primary Slave");
			cdrom_base=0x1F0;
			cdrom_drive=SLAVE;
		}
		else if(first_value==0x3C && second_value==0xC3) {
			p("IDE: Serial hard disc on Primary Slave"); //SATA
			hard_disc_base=0x1F0;
			hard_disc_drive=SLAVE;
		}
		else if(first_value==0x69 && second_value==0x96) {
			p("IDE: Serial CDROM on Primary Slave"); //SATAPI
			cdrom_base=0x1F0;
			cdrom_drive=SLAVE;
		}
	}
	//read returned values from drive
	for(int i=0; i<256; i++) {
		inb(0x1F0);
	}

	//SECONDARY CONTROLLER MASTER
	outb(0x176, 0xA0); //select master drive
	wait(5);
	outb(0x172, 0);
	outb(0x173, 0);
	outb(0x174, 0);
	outb(0x175, 0);
	outb(0x177, 0xEC); //identify command
	wait(5);
	inb(0x177);
	first_value=inb(0x174);
	second_value=inb(0x175);
	//detect what is connected on this controller drive
	if(first_value!=0xFF && second_value!=0xFF) {
		if(first_value==0 && second_value==0) { //PATA
			p("IDE: Hard disc on Secondary Master");
			hard_disc_base=0x170;
			hard_disc_drive=MASTER;
		}
		else if(first_value==0x14 && second_value==0xEB) { //PATAPI
			p("IDE: CDROM on Secondary Master");
			cdrom_base=0x170;
			cdrom_drive=MASTER;
		}
		else if(first_value==0x3C && second_value==0xC3) {
			p("IDE: Serial hard disc on Secondary Master"); //SATA
			hard_disc_base=0x170;
			hard_disc_drive=MASTER;
		}
		else if(first_value==0x69 && second_value==0x96) {
			p("IDE: Serial CDROM on Secondary Master"); //SATAPI
			cdrom_base=0x170;
			cdrom_drive=MASTER;
		}
	}
	//read returned values from drive
	for(int i=0; i<256; i++) {
		inb(0x170);
	}

	//SECONDARY CONTROLLER SLAVE
	outb(0x176, 0xB0); //select slave drive
	wait(5);
	outb(0x172, 0);
	outb(0x173, 0);
	outb(0x174, 0);
	outb(0x175, 0);
	outb(0x177, 0xEC); //identify command
	wait(5);
	inb(0x177);
	first_value=inb(0x174);
	second_value=inb(0x175);
	//detect what is connected on this controller drive
	if(first_value!=0xFF && second_value!=0xFF) {
		if(first_value==0 && second_value==0) { //PATA
			p("IDE: Hard disc on Secondary Slave");
			hard_disc_base=0x170;
			hard_disc_drive=SLAVE;
		}
		else if(first_value==0x14 && second_value==0xEB) { //PATAPI
			p("IDE: CDROM on Secondary Slave");
			cdrom_base=0x170;
			cdrom_drive=SLAVE;
		}
		else if(first_value==0x3C && second_value==0xC3) {
			p("IDE: Serial hard disc on Secondary Slave"); //SATA
			hard_disc_base=0x170;
			hard_disc_drive=SLAVE;
		}
		else if(first_value==0x69 && second_value==0x96) {
			p("IDE: Serial CDROM on Secondary Slave"); //SATAPI
			cdrom_base=0x170;
			cdrom_drive=SLAVE;
		}
	}
	//read returned values from drive
	for(int i=0; i<256; i++) {
		inb(0x170);
	}

	//THIRD CONTROLLER MASTER
	outb(0x1EE, 0xA0); //select master drive
	wait(5);
	outb(0x1EA, 0);
	outb(0x1EB, 0);
	outb(0x1EC, 0);
	outb(0x1ED, 0);
	outb(0x1EF, 0xEC); //identify command
	wait(5);
	inb(0x1EF);
	first_value=inb(0x1EC);
	second_value=inb(0x1ED);
	//detect what is connected on this controller drive
	if(first_value!=0xFF && second_value!=0xFF) {
		if(first_value==0 && second_value==0) { //PATA
			p("IDE: Hard disc on Third Master");
			hard_disc_base=0x1E8;
			hard_disc_drive=MASTER;
		}
		else if(first_value==0x14 && second_value==0xEB) { //PATAPI
			p("IDE: CDROM on Third Master");
			cdrom_base=0x1E8;
			cdrom_drive=MASTER;
		}
		else if(first_value==0x3C && second_value==0xC3) {
			p("IDE: Serial hard disc on Third Master"); //SATA
			hard_disc_base=0x1E8;
			hard_disc_drive=MASTER;
		}
		else if(first_value==0x69 && second_value==0x96) {
			p("IDE: Serial CDROM on Third Master"); //SATAPI
			cdrom_base=0x1E8;
			cdrom_drive=MASTER;
		}
	}
	//read returned values from drive
	for(int i=0; i<256; i++) {
		inb(0x1E8);
	}

	//THIRD CONTROLLER SLAVE
	outb(0x1EE, 0xB0); //select slave drive
	wait(5);
	outb(0x1EA, 0);
	outb(0x1EB, 0);
	outb(0x1EC, 0);
	outb(0x1ED, 0);
	outb(0x1EF, 0xEC); //identify command
	wait(5);
	inb(0x1EF);
	first_value=inb(0x1EC);
	second_value=inb(0x1ED);
	//detect what is connected on this controller drive
	if(first_value!=0xFF && second_value!=0xFF) {
		if(first_value==0 && second_value==0) { //PATA
			p("IDE: Hard disc on Third Slave");
			hard_disc_base=0x1E8;
			hard_disc_drive=SLAVE;
		}
		else if(first_value==0x14 && second_value==0xEB) { //PATAPI
			p("IDE: CDROM on Third Slave");
			cdrom_base=0x1E8;
			cdrom_drive=SLAVE;
		}
		else if(first_value==0x3C && second_value==0xC3) {
			p("IDE: Serial hard disc on Third Slave"); //SATA
			hard_disc_base=0x1E8;
			hard_disc_drive=SLAVE;
		}
		else if(first_value==0x69 && second_value==0x96) {
			p("IDE: Serial CDROM on Third Slave"); //SATAPI
			cdrom_base=0x1E8;
			cdrom_drive=SLAVE;
		}
	}
	//read returned values from drive
	for(int i=0; i<256; i++) {
		inb(0x1E8);
	}

	//FOURTH CONTROLLER MASTER
	outb(0x16E, 0xA0); //select master drive
	wait(5);
	outb(0x16A, 0);
	outb(0x16B, 0);
	outb(0x16C, 0);
	outb(0x16D, 0);
	outb(0x16F, 0xEC); //identify command
	wait(5);
	inb(0x16F);
	first_value=inb(0x16C);
	second_value=inb(0x16D);
	//detect what is connected on this controller drive
	if(first_value!=0xFF && second_value!=0xFF) {
		if(first_value==0 && second_value==0) { //PATA
			p("IDE: Hard disc on Fourth Master");
			hard_disc_base=0x168;
			hard_disc_drive=MASTER;
		}
		else if(first_value==0x14 && second_value==0xEB) { //PATAPI
			p("IDE: CDROM on Fourth Master");
			cdrom_base=0x168;
			cdrom_drive=MASTER;
		}
		else if(first_value==0x3C && second_value==0xC3) {
			p("IDE: Serial hard disc on Fourth Master"); //SATA
			hard_disc_base=0x168;
			hard_disc_drive=MASTER;
		}
		else if(first_value==0x69 && second_value==0x96) {
			p("IDE: Serial CDROM on Fourth Master"); //SATAPI
			cdrom_base=0x168;
			cdrom_drive=MASTER;
		}
	}
	//read returned values from drive
	for(int i=0; i<256; i++) {
		inb(0x168);
	}

	//FOURTH CONTROLLER SLAVE
	outb(0x16E, 0xB0); //select slave drive
	wait(5);
	outb(0x16A, 0);
	outb(0x16B, 0);
	outb(0x16C, 0);
	outb(0x16D, 0);
	outb(0x16F, 0xEC); //identify command
	wait(5);
	inb(0x16F);
	first_value=inb(0x16C);
	second_value=inb(0x16D);
	//detect what is connected on this controller drive
	if(first_value!=0xFF && second_value!=0xFF) {
		if(first_value==0 && second_value==0) { //PATA
			p("IDE: Hard disc on Fourth Slave");
			hard_disc_base=0x168;
			hard_disc_drive=SLAVE;
		}
		else if(first_value==0x14 && second_value==0xEB) { //PATAPI
			p("IDE: CDROM on Fourth Slave");
			cdrom_base=0x168;
			cdrom_drive=SLAVE;
		}
		else if(first_value==0x3C && second_value==0xC3) {
			p("IDE: Serial hard disc on Fourth Slave"); //SATA
			hard_disc_base=0x168;
			hard_disc_drive=SLAVE;
		}
		else if(first_value==0x69 && second_value==0x96) {
			p("IDE: Serial CDROM on Fourth Slave"); //SATAPI
			cdrom_base=0x168;
			cdrom_drive=SLAVE;
		}
	}
	//read returned values from drive
	for(int i=0; i<256; i++) {
		inb(0x168);
	}
*/
	//select drive
	/*if(hard_disc_base==cdrom_base) {
		p("IDE: SHARED CONTROLLER!");
		if(hard_disc_drive==MASTER) {
			ide_select_drive(hard_disc_base, HD_MASTER);
			shared_controller_drive=MASTER;
		}
		if(hard_disc_drive==SLAVE) {
			ide_select_drive(hard_disc_base, HD_SLAVE);
			shared_controller_drive=SLAVE;
		}
	}
	else {
		if(hard_disc_drive==MASTER) {
			ide_select_drive(hard_disc_base, HD_MASTER);
		}
		if(hard_disc_drive==SLAVE) {
			ide_select_drive(hard_disc_base, HD_SLAVE);
		}

		if(cdrom_drive==MASTER) {
			ide_select_drive(cdrom_base, CDROM_MASTER);
		}
		if(cdrom_drive==SLAVE) {
			ide_select_drive(cdrom_base, CDROM_SLAVE);
		}

		shared_controller_drive=0;
	}*/
	ide_select_drive(0x1F0, HD_MASTER);
	ide_select_drive(0x170, CDROM_MASTER);

	hard_disc_base=0x1F0;
	cdrom_base=0x170;
}
