#include <iostream>
#include "time.h"
#include <string>
#include "novoht.h"
#include <cstdlib>
#include <cstddef>
#include <sys/time.h>
//#include "meta.pb.h"
#define KEY_LEN 32
#define VAL_LEN 128
using namespace std;
struct timeval tp;

double diffclock(clock_t clock1, clock_t clock2){
   double clock_ts=clock1-clock2;
   double diffms=(clock_ts*1000)/CLOCKS_PER_SEC;
   return diffms;
}

double getTime_usec() {
   gettimeofday(&tp, NULL);
   return static_cast<double>(tp.tv_sec) * 1E6
      + static_cast<double>(tp.tv_usec);
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
double testInsert(NoVoHT &map, string keys[], string vals[], int l){
   clock_t a=clock();
   for (int t = 0; t<l; t++){
      map.put(keys[t], vals[t]);
   }
   clock_t b=clock();
   return diffclock(b,a);;
}

double testGet(NoVoHT &map, string keys[],  string vals[], int l){
   clock_t a=clock();
   for (int t=0; t<l; t++){
      if (map.get(keys[t])->compare(vals[t]) != 0)
         cerr << "Get Failed" << endl;
   }
   clock_t b=clock();
   return diffclock(b,a);
}

double testRemove(NoVoHT &map, string keys[], int l){
   clock_t a=clock();
   for (int t=0; t<l; t++){
      map.remove(keys[t]);
   }
   clock_t b=clock();
   return diffclock(b,a);
}
int main(int argc, char *argv[]){
   cout << "\nInitializing key-value pairs for testing\n" << endl;
   int size = atoi(argv[1]);
   string* keys = new string[size];
   string* vals = new string[size];
   /*
   for (int t=0; t<size; t++){
      Package package, package_ret;
      string key = randomString(25);//as key
      package.set_virtualpath(key); 
      package.set_isdir(true);
      package.set_replicano(5); 
      package.set_operation(3); 
      package.set_realfullpath("Some-Real-longer-longer-and-longer-Paths--------");
      package.add_listitem("item-----1");
      package.add_listitem("item-----2");
      package.add_listitem("item-----3");
      package.add_listitem("item-----4");
      package.add_listitem("item-----5");
      string value = package.SerializeAsString();
      keys[t] = key;
      vals[t] = value;
   }*/
   for (int t=0; t<size; t++){
      keys[t] = randomString(KEY_LEN);
      vals[t] = randomString(VAL_LEN);
   }
   //NoVoHT map ("fbench.data", 1000000, 10000);
   NoVoHT map ("fbench.data", 10000000, -1);
   //NoVoHT map ("", 1000000, 10000, .7);
   //NoVoHT map ("", 1000000, -1);
   //NoVoHT map ("/dev/shm/fbench.data", 1000000, -1);
   double ins, ret, rem;
   cout << "Testing Insertion: Inserting " << size << " elements" << endl;
   ins = testInsert(map, keys,vals,size);
   cout << "Testing Retrieval: Retrieving " << size << " elements" << endl;
   ret = testGet(map,keys,vals,size);
   cout << "Testing Removal:   Removing " << size << " elements" << endl;
   rem = testRemove(map,keys,size);
   cout << "\nInsertion done in " << ins << " milliseconds" << endl;
   cout << "Retrieval done in " << ret << " milliseconds" << endl;
   cout << "Removal done in " << rem << " milliseconds" << endl;

   delete [] keys;
   delete [] vals;

   return 0;
}
