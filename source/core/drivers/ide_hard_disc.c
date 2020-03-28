//CELEROS

uint32_t hard_disc(uint32_t type, uint64_t sector, uint16_t number_of_sectors, uint32_t memory_pointer) {
	uint16_t *buffer = (uint16_t *) memory_pointer;
    uint32_t cycle=0;
	uint32_t offset=0;

    //PIO48
    outb(hard_disc_base+2, (unsigned char)(number_of_sectors >> 8));
    outb(hard_disc_base+3, (unsigned char)(sector >> 24));
    outb(hard_disc_base+4, (unsigned char)(sector >> 32));
    outb(hard_disc_base+5, (unsigned char)(sector >> 40));
    outb(hard_disc_base+2, (unsigned char)number_of_sectors);
    outb(hard_disc_base+3, (unsigned char)sector);
    outb(hard_disc_base+4, (unsigned char)(sector >> 8));
    outb(hard_disc_base+5, (unsigned char)(sector >> 16));
    //type
    if(type==IDE_READ) {
    outb(hard_disc_base+7, 0x24);  //Send command read
    }
    else {
    outb(hard_disc_base+7, 0x34);  //Send command write
    }

    //wait for BSY clear and DRQ set
    for(int i=0; i<1000; i++) {
        inb(hard_disc_base+0x206);  //little wait
        if( (inb(hard_disc_base+0x206) & 0x88)==0x08 ) {  //drq is set
            goto hard_disc_transfer;   
        }    
    }
    for(int i=0; i<1000; i++) {
        wait(1);  //longer wait
        if( (inb(hard_disc_base+0x206) & 0x88)==0x08 ) {  //drq is set
            goto hard_disc_transfer;   
        }    
    }
    //Something is wrong
    if( (inb(hard_disc_base+0x206) & 0x01)==0x01 ) {  //bad block?
		p("Bad block"); 
    }
	p("Something wrong with hard disc transfer");
    return 0;

	hard_disc_transfer:
    if( (inb(hard_disc_base+0x206) & 0x01)==0x01 ) {  //bad block?
        p("Bad block");
    }

	for(int i=0; i<number_of_sectors; i++) {
    	for (int j=0; j<256; j++) {
        	if(type==IDE_READ) {
            	buffer[offset] = inw(hard_disc_base + 0);
        	}
        	else {
            	outw(hard_disc_base + 0, buffer[offset]);
        	}
			offset++;

			//wait
    		for(int i=0; i<1000; i++) {
        		if( (inb(hard_disc_base+0x206) & 0x88)==0x08 ) {
            		break;    
        		}    
    		}
    	}
	}

    return 1;
}
