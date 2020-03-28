//CELEROS

//NAM offsets
#define AC97_NAM_PORT_RESET 0x00
#define AC97_NAM_PORT_MASTER_VOLUME 0x02
#define AC97_NAM_PORT_MONO_VOLUME 0x06
#define AC97_NAM_PORT_PC_BEEP 0x0A
#define AC97_NAM_PORT_PCM_VOLUME 0x18
#define AC97_NAM_PORT_EXT_AUDIO_ID 0x28
#define AC97_NAM_PORT_EXT_AUDIO_STC 0x2A
#define AC97_NAM_PORT_EXT_FRONT_SPLRATE 0x2C
#define AC97_NAM_PORT_EXT_LR_SPLRATE 0x0032

//NABM offsets
#define AC97_NABM_PORT_PISTATUS 0x06
#define AC97_NABM_PORT_PICONTROL 0x0B
#define AC97_NABM_PORT_POBDBAR 0x10
#define AC97_NABM_PORT_POLVI 0x15
#define AC97_NABM_PORT_POSTATUS 0x16
#define AC97_NABM_PORT_MCSTATUS 0x26
#define AC97_NABM_PORT_POCONTROL 0x1B
#define AC97_NABM_PORT_MCCONTROL 0x2B
#define AC97_NABM_PORT_GLB_CTRL_STAT 0x2C

//BUFFERS
struct ac97_buffer_t {
    uint32_t buffer;  //offset to buffer
    uint16_t lenght;  //lenght of buffer
    uint16_t type;  //type of action after play buffer
} ac97_buffer[32];

uint32_t ac97_actual_buffer=0;

void ac97_init(uint16_t volume) {
    //reset
    outw(sound_card.base + AC97_NAM_PORT_RESET, 42);
    outb(sound_card.base1 + AC97_NABM_PORT_GLB_CTRL_STAT, 2);
    wait(10);

    //set volume
    outw(sound_card.base + AC97_NAM_PORT_MASTER_VOLUME, (volume<<8) | volume);
    outw(sound_card.base + AC97_NAM_PORT_MONO_VOLUME, volume);
    outw(sound_card.base + AC97_NAM_PORT_PC_BEEP, volume);
    outw(sound_card.base + AC97_NAM_PORT_PCM_VOLUME, (volume<<8) | volume);
    wait(10); 

    //sample output rate
    outw(sound_card.base + AC97_NAM_PORT_EXT_AUDIO_STC, (inw(sound_card.base + AC97_NAM_PORT_EXT_AUDIO_STC) | 1) );
    wait(10);
    outw(sound_card.base + AC97_NAM_PORT_EXT_FRONT_SPLRATE, 44100);
    outw(sound_card.base + AC97_NAM_PORT_EXT_LR_SPLRATE, 44100);
    wait(10);

	//set first buffer
	pv(inb(sound_card.base1 + AC97_NABM_PORT_POLVI));
    outb(sound_card.base1 + AC97_NABM_PORT_POLVI, 0);
	pv(inb(sound_card.base1 + AC97_NABM_PORT_POLVI));
	ac97_actual_buffer=0;
}

void ac97_play(void) {
    //ac97_buffer[ac97_actual_buffer].buffer=0x300000;
    //ac97_buffer[ac97_actual_buffer].lenght=0xFFFF;
    //ac97_buffer[ac97_actual_buffer].type=0x8000;

    outl(sound_card.base1 + AC97_NABM_PORT_POBDBAR, (uint32_t)&ac97_buffer[ac97_actual_buffer]);
    outb(sound_card.base1 + AC97_NABM_PORT_POLVI, 0);
    outb(sound_card.base1 + AC97_NABM_PORT_POCONTROL, 0x15); //play
}

void ac97_irq(void) {
	p("AC97 IRQ");
    uint32_t pi=0;
    uint32_t po=0;
    uint32_t mc=0;

    pi = (inb(sound_card.base1 + AC97_NABM_PORT_PISTATUS) & 0x1C);
    po = (inb(sound_card.base1 + AC97_NABM_PORT_POSTATUS) & 0x1C);
    mc = (inb(sound_card.base1 + AC97_NABM_PORT_MCSTATUS) & 0x1C);

    outb(sound_card.base1 + AC97_NABM_PORT_PISTATUS, pi);
    outb(sound_card.base1 + AC97_NABM_PORT_POSTATUS, po);
    outb(sound_card.base1 + AC97_NABM_PORT_MCSTATUS, mc);
}
