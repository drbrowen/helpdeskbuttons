#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>


#include "lcdout.h"
#include "restio.h"
#include "dodisplay.h"

#define CONFIGFILE "/etc/helpdeskbuttons.conf"
#define CONFIGSIZE 4095

static int do_exit;

void doexit(int status) {
  do_exit = 1;
}

void wait_for_server(rest_handle *rh,int *hits,short hitct) {
  while(rest_press_button(rh,0,PRESENCE,hits,hitct)) {
    sleep(1);
    if(do_exit == 1) {
      return;
    }
  }

}

int main(int argc,char* argv[])
{
  lcd_device *lcdfd;
  rest_handle *rh;
  unsigned short prevstate,prevpres;
  json_object *jobj;

  struct sigaction act;
  
  int screen_dirty;
  
  int hits[4];

  char *URL;

  int i,j;

  int rest_ok;

  int deskid;

  FILE* config;

  struct stat statconfig;

  char rawconfig[CONFIGSIZE+1];
  
  // Get the LCD up and running.  
  lcdfd = lcd_init();
  if(!lcdfd) {
    exit(1);
  }

  if(!(config=fopen(CONFIGFILE,"r"))) {
    perror("Can't open config file");
    lcd_clear(lcdfd);
    lcd_gotoxy(lcdfd,0,0);
    lcd_puts(lcdfd,"Error opening config");
    exit(1);
  }

  if(stat(CONFIGFILE,&statconfig)) {
    perror("Can't stat config file.");
    lcd_clear(lcdfd);
    lcd_gotoxy(lcdfd,0,0);
    lcd_puts(lcdfd,"Error checking config");
    exit(1);
  }

  if(statconfig.st_size >= CONFIGSIZE) {
    fprintf(stderr,"Config file implausibly large\n");
    lcd_clear(lcdfd);
    lcd_gotoxy(lcdfd,0,0);
    lcd_puts(lcdfd,"Config file too big");
    exit(1);
  }

  fread(rawconfig,1,statconfig.st_size,config);

  rawconfig[CONFIGSIZE] = 0;
  
  jobj = json_tokener_parse(rawconfig);
  
  deskid = 0;
  URL = 0;
  json_object_object_foreach(jobj,key,val) {
    if(strcmp(key,"URL")==0) {
      URL = strdup(json_object_get_string(val));
    }
    if(strcmp(key,"deskid")==0) {
      deskid = json_object_get_int(val);
    }
  }

  if(URL == 0 || deskid == 0) {
    fprintf(stderr,"Missing deskid or URL\n");
    lcd_clear(lcdfd);
    lcd_gotoxy(lcdfd,0,0);
    lcd_puts(lcdfd,"config missing URL or");
    lcd_gotoxy(lcdfd,0,1);
    lcd_puts(lcdfd,"deskid");
    exit(1);
  }

  json_object_put(jobj);

  // Set up for intercepting ctrl-c and such
  do_exit = 0;
  
  act.sa_handler = doexit;
  sigaction(SIGINT,&act,0l);
  sigaction(SIGHUP,&act,0l);
  sigaction(SIGQUIT,&act,0l);
  sigaction(SIGTERM,&act,0l);
 
  
  rh = rest_init(URL,deskid);
  
  lcd_backlight(lcdfd,1);

  do_init_display(lcdfd);

  lcd_force_button_update(lcdfd);
  
  // Check once per second until a connection.
  //while(rest_press_button(rh,0,PRESENCE,hits,presencect(lcdfd))) {
  //  sleep(1);
  //}
  wait_for_server(rh,hits,lcd_presencect(lcdfd));

  prevstate = lcd_buttons(lcdfd);
  prevpres =  lcd_presencect(lcdfd);
  screen_dirty = 1;
  
  while(1)
    {
      if(do_exit) {
	lcd_backlight(lcdfd,0);
    	lcd_clear(lcdfd);
	return(0);
      }      
      
      if(screen_dirty) {
	do_main_display(lcdfd,lcd_presencect(lcdfd),hits);
	screen_dirty = 0;
      }

      usleep(100000);

      i = lcd_check_button_event(lcdfd);

      switch(i) {
      case 0:
	screen_dirty = 1;

	lcd_gotoxy(lcdfd,19,1);
	lcd_puts(lcdfd,"\n\t");
	// New packet from controller
	if(prevstate != lcd_buttons(lcdfd)) {
	  // Button pressed or unpressed
	  for(j=0;j<8;j++) {
	    // Check if a particular button changed to pressed
	    if ( ((prevstate & (1<<j)) != (lcd_buttons(lcdfd) & (1<<j)))
		 && (lcd_buttons(lcdfd) & (1<<j)) ) {
	      // Set up data to send to controller
	      if(j<4) {
		rest_ok = rest_press_button(rh,j,INCREASE,hits,lcd_presencect(lcdfd));
	      } else {
		rest_ok = rest_press_button(rh,j-4,DECREASE,hits,lcd_presencect(lcdfd));
	      }
	      if(rest_ok != 0) {
		do_init_display(lcdfd);
		wait_for_server(rh,hits,lcd_presencect(lcdfd));
	      }
	    }
	  }
	  
	  prevstate = lcd_buttons(lcdfd);
		
	}

	if(prevpres != lcd_presencect(lcdfd)) {
	  prevpres = lcd_presencect(lcdfd);
	  rest_ok = rest_press_button(rh,0,PRESENCE,hits,lcd_presencect(lcdfd));
	  if(rest_ok != 0) {
	    do_init_display(lcdfd);
	    wait_for_server(rh,hits,lcd_presencect(lcdfd));
	  }
	}
	break;

	// No button/presence changes
      case 1:
	break;

      default:
	perror("Read from USB");
	screen_dirty = 1;
	do_init_display(lcdfd);
	wait_for_server(rh,hits,lcd_presencect(lcdfd));
	break;	

      }      
      
    }
  
  return 0;
}
