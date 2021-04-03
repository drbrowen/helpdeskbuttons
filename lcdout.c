#include <termios.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "lcdout.h"

static inline int add_presence(unsigned char presence)
{
  int res;
  int i;

  res = 0;
  for(i=0;i<8;i++)
    {
      res += 1&(presence>>i);
    }

  return res;
}


lcd_device *lcd_init()
{
  int res;
  lcd_device *lcdfd;

  if(!(lcdfd = malloc(sizeof(lcdfd)))) {
    perror("Can't allocate memory for lcd device");
    return NULL;
  }

  res = hid_init();
  if(res != 0) {
    perror("hid_init");
    return NULL;
  }
  
  lcdfd->dev = hid_open(LCD_VID,LCD_PID,NULL);

  if(!(lcdfd->dev)) {
    perror("open hid system");
    return NULL;
  }

  lcdfd->buttonstate = 0;
  
  return lcdfd;
  
}

void lcd_close(lcd_device *lcdfd)
{
  hid_close(lcdfd->dev);

  hid_exit();

  free(lcdfd);

}

int lcd_clear(lcd_device *lcdfd)
{
  int res;
  static unsigned char outbuf[] = {0x02,0x00};

  res = hid_write(lcdfd->dev,outbuf,2);

  if(res == -1) {
    perror("clear LCD screen");
    return 1;
  }

  return res;
  
}


int lcd_puts(lcd_device *lcdfd,char* message)
{
  int res;
  int size;
  int messoffset;
  
  static unsigned char outbuf[8] = {0x01,0x00,0x00,0x00,
				    0x00,0x00,0x00,0x00};;

  size = strlen(message);
  messoffset = 0;
  while(size>7) {
    memcpy(outbuf+1,message+messoffset,7);
    res = hid_write(lcdfd->dev,outbuf,8);
    if(res == -1) {
      perror("lcd_puts can't send characters");
      return 1;
    }
    size -= 7;
    messoffset += 7;
  }

  if(size>0) {
    memcpy(outbuf+1,message+messoffset,size);
    res = hid_write(lcdfd->dev,outbuf,size+1);
    if(res == -1) {
      perror("lcd_puts can't send characters");
      return 1;
    }
  }

  return 0;
  
}

int lcd_gotoxy(lcd_device *lcdfd,unsigned char x, unsigned char y)
{
  static unsigned char outbuf[3] = {0x03,0x00,0x00};
  int res;
  
  x %= 40;
  y %= 2;

  outbuf[1] = x;
  outbuf[2] = y;

  res = hid_write(lcdfd->dev,outbuf,3);

  if(res == -1) {
    perror("gotoxy: can't move");
  }

  return res;
}

int lcd_backlight(lcd_device *lcdfd,unsigned char x)
{
  int res;
  static unsigned char outbuf[2] = {0x04,0x00};

  outbuf[1] = x;

  res = hid_write(lcdfd->dev,outbuf,2);

  if(res == -1) {
    perror("backlight: can't change");
  }

  return res;
}

int lcd_buttonstate(lcd_device *lcdfd)
{
  unsigned char inbuf[3];

  int res;

  res = hid_read_timeout(lcdfd->dev,inbuf,3,100);

  if(res < 0) {
    perror("Can't read packet from usb");
    return res;
  }

  if(res == 3) {
    lcdfd->buttonstate = inbuf[1] << 8;
    lcdfd->buttonstate += inbuf[2];
  }

  return 0;
  
}

int lcd_check_button_event(lcd_device *lcdfd)
{
  unsigned char inbuf[3];

  int res;

  res = hid_read_timeout(lcdfd->dev,inbuf,3,100);

  if(res < 0) {
    perror("Can't read packet from usb");
    return res;
  }

  if(res == 3) {
    lcdfd->buttonstate = inbuf[2] << 8;
    lcdfd->buttonstate += inbuf[1];    
    lcdfd->presence = add_presence(inbuf[2]);

    return 0;
  }

  return 1;
}

int lcd_force_button_update(lcd_device *lcdfd)
{
  int res;
  static unsigned char outbuf[] = {0x05,0x00};

  res = hid_write(lcdfd->dev,outbuf,2);

  if(res == -1) {
    perror("lcd_force_button_update");
    return -1;
  }

  return lcd_check_button_event(lcdfd);

  
}
  
    
