//CELEROS

//Digital Sound Processor ports
#define DSP_RESET 0x226
#define DSP_READ 0x22A
#define DSP_WRITE 0x22C
#define DSP_BUFFER 0x22A
#define DSP_STATUS 0x22E
#define DSP_INTERRUPT 0x22F
 
//Digital Sound Processor commands
#define DSP_CMD_SAMPLE_RATE 0x41
#define DSP_CMD_TRANSFER_MODE 0xB6
#define DSP_CMD_STOP 0xD5
#define DSP_CMD_VERSION 0xE1
 
//DMA ports
#define DMA_ADDRES 0xC4
#define DMA_COUNT 0xC6
#define DMA_PAGE 0x83
#define DMA_SINGLE_MASK 0xD4
#define DMA_TRANSFER_MODE 0xD6
#define DMA_FLIP_FLOP 0xD8

uint32_t sound_blaster_exist=0;
uint32_t sb16_version_major=0;
uint32_t sb16_version_minor=0;

uint8_t read_dsp(void) {
	inb(DSP_STATUS);
    return inb(DSP_READ);
}

void write_dsp(uint8_t byte) {
    outb(DSP_WRITE, byte);
}
 
void reset_dsp(void) {
          outb(DSP_RESET, 1);
          wait(4); //wait 3  microseconds
          outb(DSP_RESET, 0);
 
          if(read_dsp()==0xAA) { 
                    sound_blaster_exist=1; 
          }
}

void sb16_play(void) {
	//volume
	outb(0x224, 0x22);
	outb(0x225, 0xDD);

	//turn speaker on
	write_dsp(0xD1);

	//set ISA DMA channel 1
	outb(0x0A, 0x05); //disable channel
	outb(0x0C, 0); //flip flop
	outb(0x0B, 0x49); //mode
	outb(0x02, 0x00); //address
	outb(0x02, 0x10); //address
	outb(0x83, 0x01); //page
	outb(0x03, 0xFF); //count 0x0FFF
	outb(0x03, 0x0F); //count 0x0FFF
	outb(0x0A, 0x05); //enable channel

	//set DSP
	write_dsp(0x40); //sample rate command
	write_dsp(0xAA); //sample rate

	write_dsp(0x14); //mode
	write_dsp(0xFF); //lenght
	write_dsp(0x0F); //lenght
}

void sb16_stop(void) {
	write_dsp(DSP_CMD_STOP);
}
 
void sb16_init(void) {
          reset_dsp();
 
          //if DSP doesnt exist
          if(sound_blaster_exist==0) {
                    return;
          }
 
          //get DSP version
          write_dsp(DSP_CMD_VERSION);
          sb16_version_major=read_dsp();
          sb16_version_minor=read_dsp();
	pv(sb16_version_major);
	pv(sb16_version_minor);
 
}
