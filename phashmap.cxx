#include <stddef.h>
#include <stdlib.h>
#include <new>
#include <string>
#include <locale>
#include <stdio.h>
//#include <openssl/md5.h>
#include "phashmap.h"

phashmap::phashmap(){
   pairs = new pair*[1000];
   size = 1000;
   numEl = 0;
   file = NULL;
   magicNumber = 1000;
   resizeNum = -1;
}

/*
phashmap::phashmap(int s){
   pairs = new pair*[s];
   size = s;
   numEl=0;
   file = NULL;
}

phashmap::phashmap(char * f){
   pairs = new pair*[1000];
   size = 1000;
   numEl=0;
   file = f;
   readFile();
}
*/
phashmap::phashmap(char * f,int s, int m){
   pairs = new pair*[s];
   magicNumber = m;
   nRem = 0;
   resizeNum = 0;
   size = s;
   numEl=0;
   file = f;
   readFile();
}
phashmap::phashmap(char * f,int s, int m, float r){
   pairs = new pair*[s];
   magicNumber = m;
   nRem = 0;
   resizeNum = r;
   size = s;
   numEl=0;
   file = f;
   readFile();
}
/*
phashmap::phashmap(char * f, phashmap *map){
   pairs = new pair*[1000];
   size = 1000;
   numEl=0;
   file = f;
   readFile();
}*/
phashmap::~phashmap(){
   writeFile();
   for (int i = 0; i < size; i++){
      fsu(pairs[i]);
   }
}

//0 success, -1 no insert, -2 no write
int phashmap::put(char* k, char* v){
   if (numEl >= size*resizeNum) {
      if (resizeNum !=0){
         resize(size*2);
      }
   }
   int slot;
   slot = hash(k)%size;
   if (slot < 0)
      slot = 0-slot;
   pair *cur = pairs[slot];
   pair *add = new pair;
   add->key = k;
   add->val = v;
   add->next = NULL;
   if (cur == NULL){
      pairs[slot] = add;
      numEl++;
      return write(add);
   }
   while (cur->next != NULL){
      if (strcmp(k, cur->key) == 0) {return -1;}
      cur = cur->next;
   }
   if (strcmp(k, cur->key) == 0) {return -1;}
   cur->next = add;
   numEl++;
   return write(add);
}

char* phashmap::get(char* k){
   int loc = hash(k)%size;
   if (loc < 0)
      loc = 0-loc;
   pair *cur = pairs[loc];
   while (cur != NULL){
      if (strcmp(cur->key, k) == 0) return cur->val;
      cur = cur->next;
   }
   return NULL;
}

//return 0 for success, -1 fail to remove, -2+ write failure
int phashmap::remove(char* k){
   int ret =0;
   int loc = hash(k)%size;
   pair *cur = pairs[loc];
   if (cur == NULL) return ret-1;       //not found
   if (strcmp(cur->key,k) ==0) {
      fpos_t toRem = pairs[loc]->pos;
      pairs[loc] = cur->next;
      numEl--;
      ret+=mark(toRem);
      free(cur->key);
      free(cur->val);
      delete cur;
      nRem++;
      if (nRem == magicNumber) ret+=writeFile(); //write and save write success
      return ret;
   }
   while(cur != NULL){
      if (cur->next == NULL) return ret-1;
      else if (strcmp(cur->next->key,k)==0){
         pair *r = cur->next;
         cur->next = r->next;
         ret+=mark(r->pos);              //mark and sace status code
         free(r->key);
         free(r->val);
         delete r;
         numEl--;
         nRem++;
         if (nRem == magicNumber) ret+=writeFile();              //mark and sace status code
         return ret;
      }
      cur = cur->next;
   }
   return ret-1;        //not found
}

//return 0 if success -2 if failed
//write hashmap to file
int phashmap::writeFile(){
   int ret =0;
   FILE * out = fopen(file, "w");
   if (!out)return -2;
   for (int i=0; i<size;i++){
      pair *cur = pairs[i];
      while (cur != NULL){
         if(cur->key != NULL && cur->val)
                fprintf(out, "%s\t%s\n", cur->key, cur->val);
         cur = cur->next;
      }
   }
   fclose(out);
   return ret;
}

//success 0 fail -2
//resize the hashmap's base size
void phashmap::resize(int ns){
   int olds = size;
   size = ns;
   pair** old = pairs;
   pairs = new pair*[ns];
   numEl = 0;
   for (int i=0; i<olds;i++){
      pair *cur = old[i];
      while (cur != NULL){
         put(cur->key, cur->val);
         pair *last = cur;
         cur = cur->next;
         delete last;
      }
   }
   delete [] old;
}

//success 0 fail -2
//write pair to file
int phashmap::write(pair * p){
   if(!file) return -2;
   FILE * data = fopen(file, "a");
   if (!data) return -2;
   fgetpos(data, &(p->pos));
   fprintf(data, "%s\t%s\n", p->key, p->val);
   fclose(data);
   return 0;
}

//success 0 fail -2
//mark line in file for deletion
int phashmap::mark(fpos_t position){
   if(!file) return -2;
   FILE * data = fopen(file, "r+");
   if (!data) return -2;
   fsetpos(data, &position);
   fputc((int) '~', data);
   fclose(data);
   return 0;
}

void phashmap::readFile(){
   if(!file) return;
   FILE * data = fopen(file, "r+");
   if (!data) return;
   char s[300];
   while(fscanf(data, "%s", s) != EOF){
      char *key = (char *)calloc(300, sizeof(char));
      strcpy(key, s);
      char* val = (char *)calloc(300,sizeof(char));
      fscanf(data,"%s",s);
      strcpy(val,s);
      if (key[0] != '~'){
         put(key,val);
      }
   }
   fclose(data);
   writeFile();
}

unsigned long long hash(char* k){ //FNV hash
   unsigned long long x = 14695981039346656037ULL;
   while (*k){
      x = x ^(*k++);
      x = x * 1099511628211;
   }
   return (x);
}

void fsu(pair* p){
   if(p == NULL) return;
   fsu(p->next);
   delete p;
}
