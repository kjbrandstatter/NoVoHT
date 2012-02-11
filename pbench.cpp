#include <stdio.h>
#include <cstddef>
#include <cstdlib>
#include <string>
#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include "novoht.h"
#include <sys/time.h>
#include <sys/shm.h>
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
   int shmid;
   string key;
   string val;
};

void* insert(void* args){
   struct ipair theargs = *((ipair*)args);
   NoVoHT *map = (NoVoHT*)shmat(theargs.shmid,0,0);
      //(theargs.map)->put(theargs.key,theargs.val);
   map->put(theargs.key,theargs.val);
   pthread_exit(NULL);
}

void* get(void* args){
   struct ipair theargs = *((ipair*)args);
   NoVoHT *map = (NoVoHT*)shmat(theargs.shmid,0,0);
   string* ret = map->get(theargs.key);
   if(ret)
      if(ret->compare(theargs.val)){
         cout<< "Found" << endl;
         pthread_exit(NULL);
      }
      else{ cerr << "No match" << endl;}
   else
        cerr << "lookup failure" << endl;
   pthread_exit(NULL);
}

void* remove(void* argv){
   struct ipair args = *((ipair*)argv);
   NoVoHT *map = (NoVoHT*)shmat(args.shmid,0,0);
   map->remove(args.key);
   pthread_exit(NULL);
}

int main(int argc, char** argv){
   int shmid = shmget(1000, 10*sizeof(NoVoHT), IPC_CREAT);
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
   pmap = (NoVoHT*)shmat(shmid,0,0);
   pmap = new NoVoHT(fname, size, -1);
   for (int i=0; i<size; i++){
      struct ipair args;
      args.shmid = shmid;
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
      args.shmid = shmid;
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
      args.shmid = shmid;
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
   delete [] keys;
   delete [] vals;
   delete pmap;
   return 0;
}
