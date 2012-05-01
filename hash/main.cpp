//#include <cmath>
#include <math.h>
#include <stdio.h>
#include "hash-functions.h"
#include <unistd.h>
#include <string.h>
#include <string>
#include <stdlib.h>
#include "time.h"
#include <sys/time.h>

using namespace std;

struct timeval tp;

double getTime_usec(){
   gettimeofday(&tp, NULL);
   return static_cast<double>(tp.tv_sec)*1E6
        + static_cast<double>(tp.tv_usec);
}

string randomString(int len) {
   string s(len, ' ');
   srand(getpid() +clock() + getTime_usec());
   static const char alphanum[] = "0123456789"
        "abcdefghijklmnopqrstuvwxyz"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
   for (int i =0; i < len; ++i){
      s[i] = alphanum[rand() % (sizeof(alphanum) -1)];
   }
   return s;
}

unsigned long hash1(const char* k){
   unsigned long x = 0;
   while (*k){
      x= (x<<4)+ *k++;
      unsigned long g = x & 0xF00000000L;
      if (g) x ^= g >> 24;
      x &= ~g;
   }
   return x;
}

double diffclock(clock_t clock1, clock_t clock2){
	double clock_ts=clock1-clock2;
	double diffms=(clock_ts*10)/CLOCKS_PER_SEC;
	return diffms;
}
void testbasic(string testvals[], int buckets[], int nv, int nb){
   printf("Testing basic hash\n");
   int collisions = 0;
   clock_t beg=clock();
   for (int y = 0; y < nv; y++){
      unsigned long x = hash1(testvals[y].c_str());
      if(buckets[x%nb] > 0)
         collisions++;
      buckets[x%nb]++;
   }
   clock_t end=clock();
   int max =buckets[0], min = buckets[0];
   double variance = 0, avgload = double(nb)/double(nb);
   buckets[0] = 0;
   for (int i = 1; i < nb; i++){
      if (buckets[i] > max) max = buckets[i];
      if (buckets[i] < min) min = buckets[i];
      //variance += pow(double(buckets[i])-avgload, 2);
      double tmp = double(buckets[i])-avgload;
      double t2 = tmp*tmp;
      variance += t2;
      buckets[i]= 0;
   }
   printf("Time to hash: %lfms\n", diffclock(end,beg));
   printf("collisions = %d\n", collisions);
   printf("Bucket max: %d\n", max);
   variance /= double(nb);
   printf("Bucket min: %d\n", min);
   printf("variance = %f\n", variance);
   double stdev = sqrt(variance);
   printf("stnd dev: %lf\n\n", stdev);
}

void testsfh(string testvals[], int buckets[], int nv, int nb){
   printf("Testing SuperFastHash\n");
   struct SuperFastHash hash;
   int collisions = 0;
   clock_t beg=clock();
   for (int y = 0; y < nv; y++){
      unsigned int x = hash(testvals[y].c_str());
      if(buckets[x%nb] > 0)
         collisions++;
      buckets[x%nb]++;
   }
   clock_t end=clock();
   int max =buckets[0], min = buckets[0];
   double variance = 0, avgload = double(nb)/double(nb);
   buckets[0] = 0;
   for (int i = 1; i < nb; i++){
      if (buckets[i] > max) max = buckets[i];
      if (buckets[i] < min) min = buckets[i];
      //variance += pow(double(buckets[i])-avgload, 2);
      double tmp = double(buckets[i])-avgload;
      double t2 = tmp*tmp;
      variance += t2;
      buckets[i]= 0;
   }
   printf("Time to hash: %lfms\n", diffclock(end,beg));
   printf("collisions = %d\n", collisions);
   printf("Bucket max: %d\n", max);
   variance /= double(nb);
   printf("Bucket min: %d\n", min);
   printf("variance = %f\n", variance);
   double stdev = sqrt(variance);
   printf("stnd dev: %lf\n\n", stdev);
}
void testfnv(string testvals[], int buckets[], int nv, int nb){
   printf("Testing FNVHash\n");
   struct FNVHash hash;
   int collisions = 0;
   clock_t beg=clock();
   for (int y = 0; y < nv; y++){
      unsigned int x = hash(testvals[y].c_str());
      if(buckets[x%nb] > 0)
         collisions++;
      buckets[x%nb]++;
   }
   clock_t end=clock();
   int max =buckets[0], min = buckets[0];
   double variance = 0, avgload = double(nb)/double(nb);
   buckets[0] = 0;
   for (int i = 1; i < nb; i++){
      if (buckets[i] > max) max = buckets[i];
      if (buckets[i] < min) min = buckets[i];
      //variance += pow(double(buckets[i])-avgload, 2);
      double tmp = double(buckets[i])-avgload;
      double t2 = tmp*tmp;
      variance += t2;
      buckets[i]= 0;
   }
   printf("Time to hash: %lfms\n", diffclock(end,beg));
   printf("collisions = %d\n", collisions);
   printf("Bucket max: %d\n", max);
   variance /= double(nb);
   printf("Bucket min: %d\n", min);
   printf("variance = %f\n", variance);
   double stdev = sqrt(variance);
   printf("stnd dev: %lf\n\n", stdev);
}
void testcrc(string testvals[], int buckets[], int nv, int nb){
   printf("Testing CRC32\n");
   struct CRC32 hash;
   int collisions = 0;
   clock_t beg=clock();
   for (int y = 0; y < nv; y++){
      unsigned int x = hash(testvals[y].c_str());
      if(buckets[x%nb] > 0)
         collisions++;
      buckets[x%nb]++;
   }
   clock_t end=clock();
   int max =buckets[0], min = buckets[0];
   double variance = 0, avgload = double(nb)/double(nb);
   buckets[0] = 0;
   for (int i = 1; i < nb; i++){
      if (buckets[i] > max) max = buckets[i];
      if (buckets[i] < min) min = buckets[i];
      //variance += pow(double(buckets[i])-avgload, 2);
      double tmp = double(buckets[i])-avgload;
      double t2 = tmp*tmp;
      variance += t2;
      buckets[i]= 0;
   }
   printf("Time to hash: %lfms\n", diffclock(end,beg));
   printf("collisions = %d\n", collisions);
   printf("Bucket max: %d\n", max);
   variance /= double(nb);
   printf("Bucket min: %d\n", min);
   printf("variance = %f\n", variance);
   double stdev = sqrt(variance);
   printf("stnd dev: %lf\n\n", stdev);
}
void testsdbm(string testvals[], int buckets[], int nv, int nb){
   printf("Testing SDBM\n");
   struct SDBM hash;
   int collisions = 0;
   clock_t beg=clock();
   for (int y = 0; y < nv; y++){
      unsigned int x = hash(testvals[y].c_str());
      if(buckets[x%nb] > 0)
         collisions++;
      buckets[x%nb]++;
   }
   clock_t end=clock();
   int max =buckets[0], min = buckets[0];
   double variance = 0, avgload = double(nb)/double(nb);
   buckets[0] = 0;
   for (int i = 1; i < nb; i++){
      if (buckets[i] > max) max = buckets[i];
      if (buckets[i] < min) min = buckets[i];
      //variance += pow(double(buckets[i])-avgload, 2);
      double tmp = double(buckets[i])-avgload;
      double t2 = tmp*tmp;
      variance += t2;
      buckets[i]= 0;
   }
   printf("Time to hash: %lfms\n", diffclock(end,beg));
   printf("collisions = %d\n", collisions);
   printf("Bucket max: %d\n", max);
   variance /= double(nb);
   printf("Bucket min: %d\n", min);
   printf("variance = %f\n", variance);
   double stdev = sqrt(variance);
   printf("stnd dev: %lf\n\n", stdev);
}
void testbjh(string testvals[], int buckets[], int nv, int nb){
   printf("Testing BobJenkinsHash\n");
   struct BobJenkinsHash hash;
   int collisions = 0;
   clock_t beg=clock();
   for (int y = 0; y < nv; y++){
      unsigned int x = hash(testvals[y].c_str());
      if(buckets[x%nb] > 0)
         collisions++;
      buckets[x%nb]++;
   }
   clock_t end=clock();
   int max =buckets[0], min = buckets[0];
   double variance = 0, avgload = double(nb)/double(nb);
   buckets[0] = 0;
   for (int i = 1; i < nb; i++){
      if (buckets[i] > max) max = buckets[i];
      if (buckets[i] < min) min = buckets[i];
      //variance += pow(double(buckets[i])-avgload, 2);
      double tmp = double(buckets[i])-avgload;
      double t2 = tmp*tmp;
      variance += t2;
      buckets[i]= 0;
   }
   printf("Time to hash: %lfms\n", diffclock(end,beg));
   printf("collisions = %d\n", collisions);
   printf("Bucket max: %d\n", max);
   variance /= double(nb);
   printf("Bucket min: %d\n", min);
   printf("variance = %f\n", variance);
   double stdev = sqrt(variance);
   printf("stnd dev: %lf\n\n", stdev);
}
void testoat(string testvals[], int buckets[], int nv, int nb){
   printf("Testing OneAtATimeHash\n");
   struct OneAtATimeHash hash;
   int collisions = 0;
   clock_t beg=clock();
   for (int y = 0; y < nv; y++){
      unsigned int x = hash(testvals[y].c_str());
      if(buckets[x%nb] > 0)
         collisions++;
      buckets[x%nb]++;
   }
   clock_t end=clock();
   int max =buckets[0], min = buckets[0];
   double variance = 0, avgload = double(nb)/double(nb);
   buckets[0] = 0;
   for (int i = 1; i < nb; i++){
      if (buckets[i] > max) max = buckets[i];
      if (buckets[i] < min) min = buckets[i];
      variance += pow(double(buckets[i])-avgload, 2);
      //double tmp = double(buckets[i])-avgload;
      //double t2 = tmp*tmp;
      //variance += t2;
      buckets[i]= 0;
   }
   printf("Time to hash: %lfms\n", diffclock(end,beg));
   printf("collisions = %d\n", collisions);
   printf("Bucket max: %d\n", max);
   variance /= double(nb);
   printf("Bucket min: %d\n", min);
   printf("variance = %f\n", variance);
   double stdev = sqrt(variance);
   printf("stnd dev: %lf\n\n", stdev);
}
/*
void testsha(string * testvals, int * buckets, int nv, int nb){
   printf("Testing OneAtATimeHash\n");
   clock_t beg=clock();
   for (int y = 0; y < nv; y++){
      int x = SHA1(testvals[y].c_str());
      buckets[x%nb]++;
   }
   clock_t end=clock();
   int max =buckets[0], min = buckets[0];
   buckets[0] = 0;
   for (int i = 1; i < nb; i++){
      if (buckets[i] > max) max = buckets[i];
      if (buckets[i] < min) min = buckets[i];
      buckets[i]= 0;
   }
   printf("Time to hash: %fms\n", diffclock(end,beg));
   printf("Bucket max: %d\n", max);
   printf("Bucket min: %d\n\n", min);
}
*/

int main(int argc, char** argv){
   int numvals = atoi(argv[1]);
   int numslots = numvals;
   int keylength = atoi(argv[2]);
   printf("Hashing %d %d byte strings\n", numvals, keylength);
   int *buckets = new int[numslots];
   for (int i = 0; i < numslots; i++){
      buckets[i]=0;
   }
   string * testvals = new string[numvals];
   for (int y = 0; y < numvals; y++){
      testvals[y] = randomString(keylength);
   }
   testbasic(testvals, buckets, numvals, numslots);
   testsfh(testvals, buckets, numvals, numslots);
   testfnv(testvals, buckets, numvals, numslots);
   testcrc(testvals, buckets, numvals, numslots);
   testbjh(testvals, buckets, numvals, numslots);
   testoat(testvals, buckets, numvals, numslots);
   testsdbm(testvals, buckets, numvals, numslots);
   delete [] buckets;
   delete [] testvals;
   return 0;
}
