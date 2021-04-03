#ifndef DODISPLAY_INCLUDE
#define DODISPLAY_INCLUDE

#include "lcdout.h"

void do_main_display(lcd_device *lcdfd,int presence,int hits[4]);
void do_init_display(lcd_device *lcdfd);

#endif // DODISPLAY_INCLUDE
