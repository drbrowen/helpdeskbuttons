#ifndef RESTIO_INCLUDE
#define RESTIO_INCLUDE

#include <curl/curl.h>
#include <json-c/json.h>
#include <string.h>

typedef struct {
  char* path;
  int deskid;
} rest_handle;

struct MemoryStruct {
  char *memory;
  size_t size;
};

typedef enum {
	      PRESENCE=0,
	      INCREASE=1,
	      DECREASE=2
} rest_button_change;


rest_handle *rest_init(const char *path,int deskid);
void rest_cleanup(rest_handle *handle);
int rest_press_button(rest_handle *handle,int which,rest_button_change direction,int *totals,short presence);


#endif //RESTIO_INCLUDE
