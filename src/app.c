/*
 * License for the Ledger Nano S Binary Keyboard Demo project, originally found
 * here: https://github.com/parkerhoyes/nanos-app-binarykbdemo
 *
 * Copyright (C) 2016 Parker Hoyes <contact@parkerhoyes.com>
 *
 * This software is provided "as-is", without any express or implied warranty.
 * In no event will the authors be held liable for any damages arising from the
 * use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not claim
 *    that you wrote the original software. If you use this software in a
 *    product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 */

#include "app.h"

#include <stdbool.h>
#include <stddef.h>

#include "os.h"
#include "os_io_seproxyhal.h"

#include "bui.h"
#include "bui_bkb.h"

static bui_bitmap_128x32_t app_disp_buffer;
static int8_t app_disp_progress;
static bui_bkb_bkb_t app_bkb;
static char app_bkb_type_buff[25];

void app_init() {
	// Set a ticker interval of 40 ms
	G_io_seproxyhal_spi_buffer[0] = SEPROXYHAL_TAG_SET_TICKER_INTERVAL;
	G_io_seproxyhal_spi_buffer[1] = 0;
	G_io_seproxyhal_spi_buffer[2] = 2;
	G_io_seproxyhal_spi_buffer[3] = 0;
	G_io_seproxyhal_spi_buffer[4] = 40;
	io_seproxyhal_spi_send(G_io_seproxyhal_spi_buffer, 5);

	app_disp_progress = -1;
	bui_bkb_init(&app_bkb, bui_bkb_layout_standard, sizeof(bui_bkb_layout_standard), app_bkb_type_buff, 0,
			sizeof(app_bkb_type_buff), true);
	app_display();
}

void app_display() {
	bui_fill(&app_disp_buffer, false);
	app_draw();
	if (app_disp_progress == -1)
		app_disp_progress = bui_display(&app_disp_buffer, 0);
	else
		app_disp_progress = 0;
}

void app_draw() {
	bui_bkb_draw(&app_bkb, &app_disp_buffer);
}

void app_event_button_push(unsigned int button_mask, unsigned int button_mask_counter) {
	switch (button_mask) {
	case BUTTON_EVT_RELEASED | BUTTON_LEFT | BUTTON_RIGHT:
		os_sched_exit(0); // Go back to the dashboard
		break;
	case BUTTON_EVT_RELEASED | BUTTON_LEFT:
		bui_bkb_choose(&app_bkb, BUI_DIR_LEFT);
		app_display();
		break;
	case BUTTON_EVT_RELEASED | BUTTON_RIGHT:
		bui_bkb_choose(&app_bkb, BUI_DIR_RIGHT);
		app_display();
		break;
	}
}

void app_event_ticker() {
	if (bui_bkb_tick(&app_bkb))
		app_display();
}

void app_event_display_processed() {
	if (app_disp_progress != -1)
		app_disp_progress = bui_display(&app_disp_buffer, app_disp_progress);
}
