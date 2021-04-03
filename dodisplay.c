#include "dodisplay.h"
#include <stdio.h>


// Mostly just a list of things to do in order.

void do_main_display(lcd_device *lcdfd,int presence,int hits[4])
{
  char buf[64];
  lcd_clear(lcdfd);
  lcd_gotoxy(lcdfd,0,0);

  snprintf(buf,64,"Wht Grn Ylo Red  Here:%2d",presence);
  lcd_puts(lcdfd,buf);

  lcd_gotoxy(lcdfd,0,1);

  snprintf(buf,64,"%3d %3d %3d %3d",
	   hits[0],
	   hits[1],
	   hits[2],
	   hits[3]);

  lcd_puts(lcdfd,buf);
  
}


void do_init_display(lcd_device *lcdfd)
{
  lcd_clear(lcdfd);
  lcd_gotoxy(lcdfd,0,0);
  lcd_puts(lcdfd,"Help Desk Stats for ALL!");

  // Read initial button and presence state.
  lcd_force_button_update(lcdfd); 
  lcd_gotoxy(lcdfd,0,1);
  lcd_puts(lcdfd,"Waiting for server...");

}
