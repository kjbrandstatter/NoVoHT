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
};

template <class type>
class novoht_iterator{
   protected:
      kvpair ** list;
      kvpair * current;
      int index;
      int length;
   public:
      novoht_iterator(kvpair**, int);
      void next();
};

template <class type>
novoht_iterator<type>::novoht_iterator(kvpair** l, int size){
   list = l;
   index = 0;
   length = size;
   while ((current = list[index]) == NULL){
      index++;
      if (index >= length)
         break;
   }
}

template <class type>
void novoht_iterator<type>::next(){
   if (current->next != NULL)
      current = current->next;
   else {
      while ((current = list[++index]) == NULL) {}
   }
}

class key_iterator: public novoht_iterator<string>{
   public:
      string value(void){ return current->key; }
      key_iterator(kvpair** l,int s) : novoht_iterator<string>(l,s) { }
};

class val_iterator: public novoht_iterator<string>{
   public:
      string value(void){ return current->val; }
      val_iterator(kvpair** l,int s) : novoht_iterator<string>(l,s) { }
};

class pair_iterator: public novoht_iterator<kvpair>{
   public:
      kvpair value(void) { return *current; }
      pair_iterator(kvpair** l,int s) : novoht_iterator<kvpair>(l,s) { }
};

class NoVoHT{
   int size;
   kvpair** kvpairs;
   kvpair** oldpairs;
   bool resizing;
   bool map_lock;
   bool write_lock;
   int numEl;
   FILE * dbfile;
   string filename;
   int nRem;
   void resize(int ns);
   int write(kvpair *);
   //void writeFile();
   void readFile();
   int mark(fpos_t);
   int magicNumber;
   float resizeNum;
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
        key_iterator keyIterator();
        val_iterator valIterator();
        pair_iterator pairIterator();
};

unsigned long long hash (string k);

void fsu(kvpair *);

char *readTabString(FILE*, char*);

#endif
