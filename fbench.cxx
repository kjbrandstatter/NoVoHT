#include <iostream>
#include "time.h"
#include <string>
#include "phashmap.h"
#include <cstdlib>
#include <cstddef>
#include <sys/time.h>
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
double testInsert(phashmap &map, string keys[], string vals[], int l){
   clock_t a=clock();
   for (int t = 0; t<l; t++){
      map.put(keys[t], vals[t]);
   }
   clock_t b=clock();
   return diffclock(b,a);;
}

double testGet(phashmap &map, string keys[],  string vals[], int l){
   clock_t a=clock();
   for (int t=0; t<l; t++){
      if (map.get(keys[t])->compare(vals[t]) != 0)
         cerr << "Get Failed" << endl;
   }
   clock_t b=clock();
   return diffclock(b,a);
}

double testRemove(phashmap &map, string keys[], int l){
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
   for (int t=0; t<size; t++){
      keys[t] = randomString(KEY_LEN);
      vals[t] = randomString(VAL_LEN);
   }
   phashmap map ("fbench.data", 1000000, 10000, .7);
   //phashmap map ("", 1000000, 10000, .7);
   //phashmap map ("fbench.data", 1000000, 10000);
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

   return 0;
}
