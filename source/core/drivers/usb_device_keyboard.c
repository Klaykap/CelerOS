//CELEROS

struct usb_keyboards_t {
	uint32_t modifier;
	uint32_t key;
	uint32_t counter;
} usb_keyboards[10];

uint8_t usb_keyboard_convert[5][256];

void usb_keyboard_handler(uint32_t usb) {
	if(usb_ports[usb].type==TYPE_UHCI) {
		usb_ports[usb].irq_enable=0;
		uhci_packet_in(usb);
	}
	else {
		return;
	}

	if( (uhci_packet[usb][20] & 0xFF)!=usb_keyboards[usb].modifier ) {
		usb_keyboards[usb].modifier=(uhci_packet[usb][20] & 0xFF);
	}

	if( ((uhci_packet[usb][20] >> 16) & 0xFF)!=usb_keyboards[usb].key ) {
		usb_keyboards[usb].counter=0;
		usb_keyboards[usb].key=((uhci_packet[usb][20] >> 16) & 0xFF);
		//here is handle for keyboard
		handler_for_wait=1;
		key_ascii=usb_keyboard_convert[usb_keyboards[usb].modifier][usb_keyboards[usb].key];
	}
	else { //is press too same key
		if(usb_keyboards[usb].counter<10) {
			usb_keyboards[usb].counter++;
		}
		else {
			//here is handle for keyboard
			handler_for_wait=1;
			key_ascii=usb_keyboard_convert[usb_keyboards[usb].modifier][usb_keyboards[usb].key];
		}
	}

	usb_ports[usb].irq_enable=1;
}

void usb_keyboard_set_sk(void) {

}
