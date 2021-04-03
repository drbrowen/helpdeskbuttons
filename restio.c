#include "restio.h"

const static char* rest_button_messages[3] = {"presence","increase","decrease"};

static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
  size_t realsize = size * nmemb;
  struct MemoryStruct *mem = (struct MemoryStruct *)userp;
 
  char *ptr = realloc(mem->memory, mem->size + realsize + 1);
  if(ptr == NULL) {
    /* out of memory! */
    perror("Not enough memory (realloc returned NULL)\n");
    return 0;
  }
 
  mem->memory = ptr;
  memcpy(&(mem->memory[mem->size]), contents, realsize);
  mem->size += realsize;
  mem->memory[mem->size] = 0;
 
  return realsize;
}

rest_handle *rest_init(const char *path,int deskid)
{
  rest_handle* handle;
  if(!(handle = malloc(sizeof(rest_handle)))) {
    return 0;
  }

  handle->path = strdup(path);
  handle->deskid = deskid;
  return handle;
}

void rest_cleanup(rest_handle *handle)
{
  free(handle->path);
  free(handle);
}

int rest_press_button(rest_handle *handle,int which,rest_button_change direction,int *totals,short pcount)
{
  struct MemoryStruct chunk;
  struct curl_slist *headers = NULL;
  

  CURL *curl;
  CURLcode curlres;
  char webbuffer[1024];
  json_object * jobj;
  char ord;

  chunk.memory = malloc(1);
  if(chunk.memory == 0) {
    perror("Can't allocate curl return buffer.");
    exit(1);
  }
  chunk.size = 0;
  
  curl = curl_easy_init();
  if(!curl)
    {
      perror("Can't open curl");
      return -2;
    }

  if(direction != PRESENCE && direction != INCREASE && direction != DECREASE) {
    return -1;
  }
  
  // Prepare to send data to server.
  snprintf(webbuffer,1024,"{\"number\":%d,\"direction\":\"%s\",\"presence\":%d,\"deskid\":%d}",which+1,rest_button_messages[direction],pcount,handle->deskid);

  //printf("Sending %s\n",webbuffer);

  // Really long URL to include hashed auth creds.
  curl_easy_setopt(curl,CURLOPT_URL,handle->path);

  headers = curl_slist_append(headers,"Accept: application/json");
  headers = curl_slist_append(headers,"Content-Type: application/json");
  headers = curl_slist_append(headers,"charset: utf-8");

  curl_easy_setopt(curl,CURLOPT_POSTFIELDS,webbuffer);

  curl_easy_setopt(curl,CURLOPT_HTTPHEADER,headers);

  //curl_easy_setopt(curl,CURLOPT_VERBOSE,1L);

  curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,WriteMemoryCallback);

  curl_easy_setopt(curl,CURLOPT_WRITEDATA,(void *)&chunk);

  curlres = curl_easy_perform(curl);

  curl_easy_cleanup(curl);
    
  if(curlres != CURLE_OK) {
    return -1;
  }

  jobj = json_tokener_parse(chunk.memory);

  ord = 0;
  json_object_object_foreach(jobj,key,val) {
    if(key[0] >= '1' && key[0] <= '4' && key[1] == 0) {
      totals[key[0] - '1'] = json_object_get_int(val);
      ord++;
    }
  }

  json_object_put(jobj);

  if(ord != 0) {
    return 0;
  }

  return -1;
  
}

