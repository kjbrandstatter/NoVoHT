#ifndef PHASHMAP_H
#define PHASHMAP_H
#include "novoht.h"
#include <string>
#include <stdio.h>
using namespace std;

struct kvpair{
   struct kvpair * next;
   string key;
   string val;
   //int val;
   fpos_t pos;
   bool diff;
};

struct writeJob{
   pthread_t wjob;
   char * fname;
   writeJob *next;
};

class NoVoHT{
   int size;
   kvpair** kvpairs;
   kvpair** oldpairs;
   bool resizing;
   bool map_lock;
   bool write_lock;
   bool rewriting;
   int numEl;
   FILE * dbfile;
   FILE * swapFile;
   int swapNo;
   string filename;
   int nRem;
   void resize(int ns);
   int write(kvpair *);
   //void writeFile();
   void readFile();
   int mark(fpos_t);
   int magicNumber;
   float resizeNum;
   //writeJob* rewriteQueue;
   void merge();
   pthread_t writeThread;
   void rewriteFile(void*);
   public:
        NoVoHT();
        //NoVoHT(int);
        //NoVoHT(char *);
        NoVoHT(string, int, int);
        NoVoHT(string, int, int, float);
        //NoVoHT(char *, NoVoHT*);
        ~NoVoHT();
        int writeFile();
        int put(string,  string);
        string* get(string);
        int remove(string);
        int getSize() {return numEl;}
        int getCap() {return size;}
        static void* rewriteCaller(void* map){
           printf("thread Called\n");
           ((NoVoHT*)map)->rewriteFile(NULL);
           return NULL;}
};

unsigned long long hash (string k);

void fsu(kvpair *);

char *readTabString(FILE*, char*);
#endif
