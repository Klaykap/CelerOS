//CELEROS

#define LEFT_BUTTON 1
#define RIGHT_BUTTON 2
#define MIDDLE_BUTTON 3

struct usb_mouses_t {
	uint32_t button_packet;
	uint32_t button;
	uint32_t x_movement;
	uint32_t y_movement;
} usb_mouses[10];

void usb_mouse_handler(uint32_t usb) {
	if(usb_ports[usb].type==TYPE_UHCI) {
		usb_ports[usb].irq_enable=0;
		uhci_packet_in(usb);
	}
	else {
		return;
	}

	if( (uhci_packet[usb][20] & 0xFF)!=usb_mouses[usb].button_packet ) {
		usb_mouses[usb].button_packet=(uhci_packet[usb][20] & 0xFF);

		if(usb_mouses[usb].button_packet & 0x1) {
			usb_mouses[usb].button=LEFT_BUTTON;
		}
		if(usb_mouses[usb].button_packet & 0x2) {
			usb_mouses[usb].button=RIGHT_BUTTON;
		}
		if(usb_mouses[usb].button_packet & 0x4) {
			usb_mouses[usb].button=MIDDLE_BUTTON;
		}
	}

	if( ((uhci_packet[usb][20] >> 8) & 0xFF)!=usb_mouses[usb].x_movement) {
		usb_mouses[usb].x_movement=((uhci_packet[usb][20] >> 8) & 0xFF);
	}

	if( ((uhci_packet[usb][20] >> 16) & 0xFF)!=usb_mouses[usb].y_movement) {
		usb_mouses[usb].y_movement=((uhci_packet[usb][20] >> 16) & 0xFF);
	}

	if(usb_mouses[usb].button!=0) {
		pv(usb_mouses[usb].button);
	}

	usb_ports[usb].irq_enable=1;
}
