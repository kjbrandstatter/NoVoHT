#include <stdio.h>
#include <cstddef>
#include <cstdlib>
#include <string>
#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include "novoht.h"
#include <sys/time.h>
#define KEY_LEN 32
#define VAL_LEN 128
using namespace std;
struct timeval tp;
NoVoHT *pmap;

double getTime_usec() {
   gettimeofday(&tp, NULL);
   return static_cast<double>(tp.tv_sec) * 1E6+ static_cast<double>(tp.tv_usec);
}
string randomString(int len) {
   string s(len, ' ');
   srand(/*getpid()*/ clock() + getTime_usec());
   static const char alphanum[] = "0123456789"
       "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
       "abcdefghijklmnopqrstuvwxyz";
   for (int i = 0; i < len; ++i) {
      s[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
   }
   return s;
}
struct ipair{
   NoVoHT *map;
   string key;
   string val;
};

void* insert(void* args){
   struct ipair theargs = *((ipair*)args);
      //(theargs.map)->put(theargs.key,theargs.val);
   pmap->put(theargs.key,theargs.val);
}

void* get(void* args){
   struct ipair theargs = *((ipair*)args);
   string* ret = pmap->get(theargs.key);
   if(ret)
      if(ret->compare(theargs.val)){
         cout<< "Found" << endl;
         pthread_exit(NULL);
      }
   cerr << "lookup failure" << endl;
}

void* remove(void* argv){
   struct ipair args = *((ipair*)argv);
   pmap->remove(args.key);
}

int main(int argc, char** argv){
   int size = atoi(argv[1]);
   string* keys = new string[size];
   string* vals = new string[size];
   pthread_t threads[size];
   for (int t = 0; t<size; t++){
      keys[t] = randomString(KEY_LEN);
      vals[t] = randomString(VAL_LEN);
   }
   const char* fname = "";
   if (argc >2) fname = argv[2];
   pmap = new NoVoHT(fname, size, -1);
   for (int i=0; i<size; i++){
      struct ipair args;
      args.map = pmap;
      args.key = keys[i];
      args.val = vals[i];
      int rc;
      rc = pthread_create(&threads[i], NULL, insert, (void *)&args);
      if (rc)
         cout << "Failed: " << rc << endl;
   }
   for (int z =0; z <size; z++){
      pthread_join(threads[z], NULL);
   }
   for (int i=0; i<size; i++){
      struct ipair args;
      args.map = pmap;
      args.key = keys[i];
      args.val = vals[i];
      int rc;
      rc = pthread_create(&threads[i], NULL, get, (void *)&args);
      if (rc)
         cout << "Failed: " << rc << endl;
   }
   for (int z =0; z <size; z++){
      pthread_join(threads[z], NULL);
   }
   for (int i=0; i<size; i++){
      struct ipair args;
      args.map = pmap;
      args.key = keys[i];
      args.val = vals[i];
      int rc;
      rc = pthread_create(&threads[i], NULL, remove, (void *)&args);
      if (rc)
         cout << "Failed: " << rc << endl;
   }
   for (int z =0; z <size; z++){
      pthread_join(threads[z], NULL);
   }
   return 0;
}
