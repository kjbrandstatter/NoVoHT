#include <iostream>
#include "time.h"
#include <string>
//#include "phashmap.h"
//#include <kchashdb.h>
#include <hash_map>
#include <cstdlib>
#include <cstddef>
#include <sys/time.h>
#define KEY_LEN 32
#define VAL_LEN 128
using namespace std;
using namespace stdext;
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
/*
double testInsert(HashDB map, string keys[], string vals[], int l){
   clock_t a=clock();
   for (int t = 0; t<l; t++){
      map.set(keys[t], vals[t]);
   }
   clock_t b=clock();
   return diffclock(b,a);;
}

double testGet(HashDB map, string keys[],  string vals[], int l){
   clock_t a=clock();
   for (int t=0; t<l; t++){
      string value;
      map.get(keys[t], &value);
      if (value.compare(vals[t]) != 0)
         cerr << "Get Failed" << endl;
   }
   clock_t b=clock();
   return diffclock(b,a);
}

double testRemove(HashDB map, string keys[], int l){
   clock_t a=clock();
   for (int t=0; t<l; t++){
      map.remove(keys[t]);
   }
   clock_t b=clock();
   return diffclock(b,a);
}
*/
int main(int argc, char *argv[]){
//   kyotocabinet::HashDB map;
   hash_map<string,string> map;
   cout << "\nInitializing key-value pairs for testing\n" << endl;
   int size = atoi(argv[1]);
   string* keys = new string[size];
   string* vals = new string[size];
   for (int t=0; t<size; t++){
      keys[t] = randomString(KEY_LEN);
      vals[t] = randomString(VAL_LEN);
   }
   //phashmap map ("fbench.data", 1000000, 10000, .7);
   //phashmap map ("", 1000000, 10000, .7);
   //phashmap map ("fbench.data", 1000000, 10000);
   double ins, ret, rem;
   cout << "Testing Insertion: Inserting " << size << " elements" << endl;
   clock_t a=clock();
   for (int t = 0; t<size; t++){
      //map.insert(keys[t], vals[t]);
      map[keys[t]] = vals[t];
   }
   clock_t b=clock();
   ins = diffclock(b,a);;
   cout << "Testing Retrieval: Retrieving " << size << " elements" << endl;
   a=clock();
   for (int t=0; t<size; t++){
      if (map[keys[t]].compare(vals[t]) != 0)
         cerr << "Get Failed" << endl;
   }
   b=clock();
   ret =  diffclock(b,a);
   cout << "Testing Removal:   Removing " << size << " elements" << endl;
   a=clock();
   for (int t=0; t<size; t++){
      map[keys[t]] = "";
   }
   b=clock();
   rem = diffclock(b,a);
   cout << "\nInsertion done in " << ins << " milliseconds" << endl;
   cout << "Retrieval done in " << ret << " milliseconds" << endl;
   cout << "Removal done in " << rem << " milliseconds" << endl;

   return 0;
}
