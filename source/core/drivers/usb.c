//CELEROS

//functions
void ohci_reset(uint32_t usb);

void wait(uint32_t miliseconds);

void uhci_packet_in(uint32_t usb);
void uhci_packet_out(uint32_t usb);

void usb_mouse_handler(uint32_t usb);
void usb_keyboard_handler(uint32_t usb);

//device
#define DEVICE_USB_KEYBOARD 1
#define DEVICE_USB_MOUSE 2
#define DEVICE_USB_FLASH 3

//speed
#define LOW_SPEED 1
#define FULL_SPEED 2
#define HIGH_SPEED 3

void usb_handler_irq(void) {
	for(int i=0; i<10; i++) {
		if(usb_ports[i].irq_enable==1) {
			if(usb_ports[i].device==DEVICE_USB_MOUSE) {
				usb_mouse_handler(i);
			}
			if(usb_ports[i].device==DEVICE_USB_KEYBOARD) {
				usb_keyboard_handler(i);
			}
		}
	}
}

void usb_reset_all(void) {
	for(int i=0; i<10; i++) {
		if(usb_ports[i].type==TYPE_OHCI) {
			ohci_reset(i);
		}
	}
}
