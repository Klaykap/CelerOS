//CELEROS GUI

void draw_gui(void) {
	clear_screen(WHITE);

	print("WELCOME TO CELEROS!", LINE(1), COLUMN(1), BLACK, WHITE);

	print("[F1] Developer zone (programs only for developers!)", LINE(3), COLUMN(1), BLACK, WHITE);
}

void draw_dz(void) {
	clear_screen(BLACK);
}

void start_gui(void) {
	draw_gui();

	while(1) {
		wait_for_key();

		if(key==KEY_F1) {
			draw_dz();
			draw_gui();
		}
	}
}
