/*
 * Copyright 2012 Kevin Brandstatter
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <stddef.h>
#include <stdlib.h>
#include <iostream>
#include "novoht.h"
#include <new>
#include <sys/time.h>

struct timeval tp;
double diffclock(clock_t clock1, clock_t clock2){
   double clock_ts=clock1-clock2;
   double diffms=(clock_ts*1000)/CLOCKS_PER_SEC;
   return diffms;
}
double getTime_usec() {
   gettimeofday(&tp, NULL);
   return static_cast<double>(tp.tv_sec) * 1E6+ static_cast<double>(tp.tv_usec);
}
int main (int argc, char ** argv){
   //NoVoHT map("append.txt", atoi(argv[1])*2, -1);
   NoVoHT *map = new NoVoHT("append.txt", atoi(argv[1])*2,-1);
   double a = getTime_usec();
   for (int i =0; i < atoi(argv[1])/2; i++){
      if (map->append("two","append") < 0)
         cerr << "Append Problem" << endl;
      if (map->append("three","Append") < 0)
         cerr << "Append Problem" << endl;
   }
   double b = getTime_usec();
   cout << "Alternating appends " << (b - a)/1E3 << " milliseconds" << endl;
   map->remove("two");
   map->remove("three");
   double c =getTime_usec();
   for (int i =0; i < atoi(argv[1]); i++){
      if (map->append("two","append") < 0)
         cerr << "Append Problem" << endl;
   }
   double d = getTime_usec();
   cout << "Number of appends " << argv[1] << endl;
   cout << "Alternating appends " << (b - a)/1E3 << " milliseconds" << endl;
   cout << "Consecutive appends " << (d - c)/1E3 << " milliseconds" << endl;

   delete map;
   return 0;
}
