#ifndef PHASHMAP_H
#define PHASHMAP_H
#include "phashmap.h"
#include <string.h>
#include <stdio.h>

struct pair{
   struct pair * next;
   char* key;
   char* val;
   //int val;
   fpos_t pos;
};

class phashmap{
   int size;
   pair** pairs;
   int numEl;
   char * file;
   int nRem;
   void resize(int ns);
   int write(pair *);
   //void writeFile();
   void readFile();
   int mark(fpos_t);
   int magicNumber;
   float resizeNum;
   public:
        phashmap();
        //phashmap(int);
        //phashmap(char *);
        phashmap(char *, int, int);
        phashmap(char *, int, int, float);
        //phashmap(char *, phashmap*);
        ~phashmap();
        int writeFile();
        int put(char*, char*);
        char* get(char*);
        int remove(char*);
        int getSize() {return numEl;}
        int getCap() {return size;}
};

unsigned long long hash (char* k);

void fsu(pair *);
#endif
