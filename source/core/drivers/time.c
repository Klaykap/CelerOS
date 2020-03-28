//LIGHTNINGOS

uint32_t seconds=0;
uint32_t minutes=0;
uint32_t hours=0;
uint32_t weekday=0;
uint32_t monthday=0;
uint32_t month=0;
uint32_t year=0;

void get_time(void) {
    uint32_t cmos_tmp=0;

    //seconds
    outb(0x70, 0x00);
    cmos_tmp = inb(0x71);
    seconds = ( (cmos_tmp & 0x0F) + ((cmos_tmp >> 4) * 10) );

    //minutes
    outb(0x70, 0x02);
    cmos_tmp = inb(0x71);
    minutes = ( (cmos_tmp & 0x0F) + ((cmos_tmp >> 4) * 10) );

    //hours
    outb(0x70, 0x04);
    cmos_tmp = inb(0x71);
    hours = ( (cmos_tmp & 0x0F) + ((cmos_tmp >> 4) * 10) );

    //weekday
    outb(0x70, 0x06);
    cmos_tmp = inb(0x71);
    weekday = ( (cmos_tmp & 0x0F) + ((cmos_tmp >> 4) * 10) );

    //monthday
    outb(0x70, 0x07);
    cmos_tmp = inb(0x71);
    monthday = ( (cmos_tmp & 0x0F) + ((cmos_tmp >> 4) * 10) );

    //month
    outb(0x70, 0x08);
    cmos_tmp = inb(0x71);
    month = ( (cmos_tmp & 0x0F) + ((cmos_tmp >> 4) * 10) );

    //year
    outb(0x70, 0x09);
    cmos_tmp = inb(0x71);
    year = ( (cmos_tmp & 0x0F) + ((cmos_tmp >> 4) * 10) );
}

void set_timer(uint32_t frequency) {
    uint32_t divisor = 1193180 / frequency;

    outb(0x43, 0x36);

    uint8_t l = (uint8_t)(divisor & 0xFF);
    uint8_t h = (uint8_t)( (divisor>>8) & 0xFF );

    outb(0x40, l);
    outb(0x40, h);
}

void wait(uint32_t miliseconds) {
	handler_for_pit=0;
	while(handler_for_pit<miliseconds) { asm("hlt"); }
}
