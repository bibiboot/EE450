#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libmemcached-1.0/memcached.h>

//void set(char *key, char *value);
//char *get(char *key);
/*
int main(int argc, char *argv[])
{
  set("key", "Danish");
  char *data = get("key");

  printf("%s\n", data);

}
*/


void set(char *key, char *value){
  memcached_server_st *servers = NULL;
  memcached_st *memc;
  memcached_return rc;
  memc= memcached_create(NULL);
  servers= memcached_server_list_append(servers, "localhost", 11211, &rc);
  rc= memcached_server_push(memc, servers);
  rc= memcached_set(memc, key, strlen(key), value, strlen(value), (time_t)0, (uint32_t)0);
  if (rc == MEMCACHED_SUCCESS)
    fprintf(stderr,"Key stored successfully\n");
  else
    fprintf(stderr,"Couldn't store key: %s\n",memcached_strerror(memc, rc));
}

char *get(char *key){

  memcached_server_st *servers = NULL;
  memcached_st *memc;
  memcached_return rc;
  memc= memcached_create(NULL);
  servers= memcached_server_list_append(servers, "localhost", 11211, &rc);
  rc= memcached_server_push(memc, servers);
  memcached_return rcg;
  size_t length = 1000;
  char *data = memcached_get(memc, key, strlen(key), &length, (uint32_t)0, &rcg);
  return data;
}
