//CELEROS

#define LINE(x) (x * 8)
#define COLUMN(x) (x * 8)

#define WHITE 0xFFFF
#define BLACK 0x0000

uint32_t vesa_lfb=0;
uint16_t vesa_x=0;
uint16_t vesa_y=0;
uint16_t vesa_bpp=0;

uint32_t debug_line=0;

uint8_t vesa_text_mode_chars[7500];
uint16_t vesa_text_mode_backgrounds[7500];
uint16_t vesa_text_mode_colours[7500];

void read_vesa_info(void) {
    uint32_t *vesablockd = (uint32_t *) 0x1000;  //position of Vesa Mode Info Block loaded by bootloader dword read
    uint16_t *vesablockw = (uint16_t *) 0x1000;  //position of Vesa Mode Info Block loaded by bootloader word read
    uint8_t *vesablockb = (uint8_t *) 0x1000;  //position of Vesa Mode Info Block loaded by bootloader byte read

    vesa_lfb=vesablockd[10];  //Linear Frame Buffer position

	vesa_x=vesablockw[9];
    vesa_y=vesablockw[10];
    vesa_bpp=vesablockb[23];
}

void pixel(uint32_t line, uint32_t column, uint16_t colour) {
    uint16_t *vidmem = (uint16_t *) (vesa_lfb);  //start of frame buffer
    uint32_t i = ((line*vesa_x)+column);
    vidmem[i]=colour;
}

uint16_t read_pixel(uint32_t line, uint32_t column) {
    uint16_t *vidmem = (uint16_t *) (vesa_lfb);  //start of frame buffer
    uint32_t i = ((line*vesa_x)+column);
    return (vidmem[i]);
}

void clear_screen(uint16_t colour) {
    uint16_t *vidmem = (uint16_t *) (vesa_lfb);  //start of frame buffer, for high speed
	uint32_t lenght=0;

	lenght=(vesa_x * vesa_y);
    for(int i=0; i<lenght; i++) {
        vidmem[i]=colour;
    }
}

void print_char(uint8_t char_for_print, uint32_t line, uint32_t column, uint16_t colour, uint16_t background) {
	uint32_t char_position=0;

	char_position = (char_for_print * 8);

	vesa_text_mode_chars[((line * 100) + column)]=char_for_print;
	vesa_text_mode_colours[((line * 100) + column)]=colour;
	vesa_text_mode_backgrounds[((line * 100) + column)]=background;

	for(int font_line=0; font_line<8; font_line++) {  //write char lines
            //pixel 0
            if( (vesa_font[char_position+font_line] & 0x80) == 0x80) {
                pixel( (line+font_line), column, colour);
            }
			else {
                pixel( (line+font_line), column, background);
			}

            //pixel 1
            if( (vesa_font[char_position+font_line] & 0x40) == 0x40) {
                pixel( (line+font_line), (column+1), colour);
            }
			else {
                pixel( (line+font_line), (column+1), background);
			}

            //pixel 2
            if( (vesa_font[char_position+font_line] & 0x20) == 0x20) {
                pixel( (line+font_line), (column+2), colour);
            }
			else {
                pixel( (line+font_line), (column+2), background);
			}

            //pixel 3
            if( (vesa_font[char_position+font_line] & 0x10) == 0x10) {
                pixel( (line+font_line), (column+3), colour);
            } 
			else {
                pixel( (line+font_line), (column+3), background);
			}

            //pixel 4
            if( (vesa_font[char_position+font_line] & 0x08) == 0x08) {
                pixel( (line+font_line), (column+4), colour);
            } 
			else {
                pixel( (line+font_line), (column+4), background);
			}

            //pixel 5
            if( (vesa_font[char_position+font_line] & 0x04) == 0x04) {
                pixel( (line+font_line), (column+5), colour);
            }
			else {
                pixel( (line+font_line), (column+5), background);
			}

            //pixel 6
            if( (vesa_font[char_position+font_line] & 0x02) == 0x02) {
                pixel( (line+font_line), (column+6), colour);
            }
			else {
                pixel( (line+font_line), (column+6), background);
			}

            //pixel 7
            if( (vesa_font[char_position+font_line] & 0x01) == 0x01) {
                pixel( (line+font_line), (column+7), colour);
            }
			else {
                pixel( (line+font_line), (column+7), background);
			}
	}
}

void print(uint8_t *message, uint32_t line, uint32_t column, uint16_t colour, uint16_t background) {
    uint32_t char_for_print=0;
    uint32_t position=0;

	position=0;

    while(*message!=0) {
        char_for_print=*message;  //get char from message
		print_char(char_for_print, line, COLUMN(position), colour, background);
        message++;
		position++;
    }

}

void print_var(uint64_t n, uint32_t line, uint32_t column, uint16_t colour, uint16_t background) {
    if (n==0) {
        print("0", line, column, colour, background);
        return;
    }

    long acc = n;
    char c[32];
    long i = 0;
    while (acc > 0) {
        c[i] = '0' + acc%10;
        acc /= 10;
        i++;
    }
    c[i] = 0;

    char c2[32];
    c2[i--] = 0;
    long j = 0;
    while(i >= 0)
    {
        c2[i--] = c[j++];
    }

    print(c2, line, column, colour, background);

}

void print_hex(uint32_t n, uint32_t line, uint32_t column, uint16_t colour, uint16_t background){
    uint8_t c[10];
    uint8_t digit=0;

    c[0]=('0');
    c[1]=('x');

    for (int i=0; i<8; i++) {
        digit = (n >> (32 - (i * 4) - 4)) & 0xF;
        if (digit < 0xA) {
            c[i+2]=('0' + digit);
        }
        else {
            c[i+2]=('A' + (digit - 0xA));
        }
    }
    c[10]=0;  //end of string

    print(c, line, column, colour, background);
}

//drawing functions
void draw_straight_line(uint16_t line, uint16_t column, uint16_t lenght, uint16_t colour) {
    for(int i=0; i<lenght; i++) {
        pixel(line, column, colour);
        column++;   //next column
    }
}

void draw_straight_column(uint16_t line, uint16_t column, uint16_t height, uint16_t colour) {
    for(int i=0; i<height; i++) {
        pixel(line, column, colour);
        line++;    //next line
    }
}

void draw_square(uint16_t line, uint16_t column, uint16_t lenght, uint16_t height, uint16_t colour) {
    for(int i=0; i<height; i++) {
        for(int j=0; j<lenght; j++) { 
            pixel( (line+i), (column+j), colour);
        }
    }
}

void draw_empty_square(uint16_t line, uint16_t column, uint16_t lenght, uint16_t height, uint16_t colour) {
	draw_straight_line(line, column, lenght, colour);
	draw_straight_line( (line+height), column, lenght, colour);
	draw_straight_column(line, column, height, colour);
	draw_straight_column(line, (column+lenght), height, colour);
	pixel((line+height), (column+lenght), colour);
}

//scan functions
void scan_monitor(uint16_t line, uint16_t column, uint16_t lenght, uint16_t height, uint32_t memory_position) {
    uint16_t *memory = (uint16_t *) (memory_position);
	uint32_t offset=0;

	for(uint32_t i=0; i<height; i++) {
		for(uint32_t j=0; j<lenght; j++) {
			memory[offset]=read_pixel( (line+i), (column+j) );
			offset++;
		}
	}
}

void draw_monitor(uint16_t line, uint16_t column, uint16_t lenght, uint16_t height, uint32_t memory_position) {
	uint16_t *memory = (uint16_t *) (memory_position);
	uint32_t offset=0;

	for(uint32_t i=0; i<height; i++) {
		for(uint32_t j=0; j<lenght; j++) {
			pixel( (line+i), (column+j), memory[offset]);
			offset++;
		}
	}
}

//textmode
void set_pixel_colour(uint32_t line, uint32_t column, uint16_t colour, uint16_t background) {
	vesa_text_mode_colours[((line * 100) + column)]=colour;
	vesa_text_mode_backgrounds[((line * 100) + column)]=background;

	print_char(vesa_text_mode_chars[((line * 100) + column)], line, column, colour, background);

}

void set_pixel_text(uint8_t text, uint32_t line, uint32_t column) {
	vesa_text_mode_chars[((line * 100) + column)]=text;

	print_char(text, line, column, vesa_text_mode_colours[((line * 100) + column)], vesa_text_mode_backgrounds[((line * 100) + column)]);

}

#define GET_PIXEL_COLOUR(line, column) (vesa_text_mode_colours[((line * 100) + column)])
#define GET_PIXEL_TEXT(line, column) (vesa_text_mode_chars[((line * 100) + column)])

//debug
void p(uint8_t *message) {
    if(debug_line>=75) {
        debug_line=0;
    }

    print(message, LINE(debug_line), 0, BLACK, WHITE);

    debug_line++;
}

void pv(uint64_t value) {
    if(debug_line>=75) {
        debug_line=0;
    }

    print_var(value, LINE(debug_line), 0, BLACK, WHITE);

    debug_line++;
}

void ph(uint32_t value) {
    if(debug_line>=75) {
        debug_line=0;
    }

    print_hex(value, LINE(debug_line), 0, BLACK, WHITE);

    debug_line++;
}
