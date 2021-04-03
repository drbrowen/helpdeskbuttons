#ifndef LCDOUT_INCLUDE
#define LCDOUT_INCLUDE
#include <hidapi/hidapi.h>

#define LCD_VID 0x1929
#define LCD_PID 0x050c

#define LCD_EP_OUT 1
#define LCD_EP_IN  2

typedef struct {
  hid_device *dev;
  unsigned short buttonstate;
  unsigned short presence;
} lcd_device;

#define lcd_buttons(x)      (x->buttonstate)
#define lcd_presencect(x)   (x->presence)


lcd_device *lcd_init();
int  lcd_clear(lcd_device *lcdfd);
int  lcd_puts(lcd_device *lcdfd,char* message);
int  lcd_gotoxy(lcd_device *lcdfd,unsigned char x,unsigned char y);
int  lcd_backlight(lcd_device *lcdfd,unsigned char on);
void lcd_close(lcd_device *dev);
int lcd_buttonstate(lcd_device *lcdfd);
int lcd_check_button_event(lcd_device *lcdfd);
int lcd_force_button_update(lcd_device *lcdfd);

#endif   //LCDOUT_INCLUDE
