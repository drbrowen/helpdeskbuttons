#ifndef DODISPLAY_INCLUDE
#define DODISPLAY_INCLUDE

#include "lcdout.h"

void do_main_display(lcd_device *lcdfd,int presence,int hits[4],int deskid);
void do_init_display(lcd_device *lcdfd);
void do_network_activity(lcd_device *lcdfd);

#endif // DODISPLAY_INCLUDE
