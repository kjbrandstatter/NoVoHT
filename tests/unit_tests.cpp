#include <novoht.h>
#include <iostream>
typedef struct  {
   bool success;
   string message;
} status;
int test_write(){
   return -1;
}
int test_writeAppend(){
   return -1;
}
int test_readFile(){
   return -1;
}
int test_put(){
   return -1;
}
int test_get(){
   return -1;
}
int test_append(){
   return -1;
}
int test_remove(){
   return -1;
}
int test_writeFile(){
   return -1;
}
int test_rewriteFile(){
   return -1;
}

status test_overwrite_get(){
   NoVoHT *map = new NoVoHT("get.nht", 1000, -1);
   status return_stat;
   map->put("testkey", "testval");
   string * meow = map->get("testkey");
   if (meow->compare(string("testval")) != 0) {
      return_stat.success = false;
      return_stat.message = string("Get operation returned wrong value");
      return return_stat;
   }
   *meow = string("newval");
   meow = map->get("testkey");
   if (meow->compare(string("testval")) != 0) {
      return_stat.success = false;
      return_stat.message = string("Get value overwritten");
      return return_stat;
   }
   return_stat.success = true;
   return_stat.message = string("Overwrite get Test Passed");
   return return_stat;
}

int main(){
   status test_stat = test_overwrite_get();
   cout << test_stat.message << endl;
}
