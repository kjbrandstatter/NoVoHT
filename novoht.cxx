#include <stddef.h>
#include <stdlib.h>
#include <new>
#include <string>
#include <locale>
#include <stdio.h>
#include <iostream>
#include "novoht.h"

NoVoHT::NoVoHT(){
   kvpairs = new kvpair*[1000];
   size = 1000;
   numEl = 0;
   magicNumber = 1000;
   resizeNum = -1;
}

/*
NoVoHT::NoVoHT(int s){
   kvpairs = new kvpair*[s];
   size = s;
   numEl=0;
   file = NULL;
}

NoVoHT::NoVoHT(char * f){
   kvpairs = new kvpair*[1000];
   size = 1000;
   numEl=0;
   file = f;
   readFile();
}
*/
NoVoHT::NoVoHT(string f,int s, int m){
   kvpairs = new kvpair*[s];
   magicNumber = m;
   nRem = 0;
   resizeNum = 0;
   size = s;
   numEl=0;
   filename=f;
   dbfile = fopen(f.c_str(), "r+");
   if (!dbfile) dbfile = fopen(f.c_str(), "w+");
   readFile();
}
NoVoHT::NoVoHT(string f,int s, int m, float r){
   kvpairs = new kvpair*[s];
   magicNumber = m;
   nRem = 0;
   resizeNum = r;
   size = s;
   numEl=0;
   filename=f;
   dbfile = fopen(f.c_str(), "r+");
   if (!dbfile) dbfile = fopen(f.c_str(), "w+");
   readFile();
}
/*
NoVoHT::NoVoHT(char * f, NoVoHT *map){
   kvpairs = new kvpair*[1000];
   size = 1000;
   numEl=0;
   file = f;
   readFile();
}*/
NoVoHT::~NoVoHT(){
   if (dbfile){
      writeFile();
      fclose(dbfile);
   }
   for (int i = 0; i < size; i++){
      fsu(kvpairs[i]);
   }
   delete [] kvpairs;
}

//0 success, -1 no insert, -2 no write
int NoVoHT::put(string k, string v){
   if (numEl >= size*resizeNum) {
      if (resizeNum !=0){
         resize(size*2);
      }
   }
   long slot;
   slot = hash(k)%size;
   kvpair *cur = kvpairs[slot];
   kvpair *add = new kvpair;
   add->key = k;
   add->val = v;
   add->next = NULL;
   if (cur == NULL){
      kvpairs[slot] = add;
      numEl++;
      return write(add);
   }
   while (cur->next != NULL){
      if (k.compare(cur->key) == 0) {delete add; return -1;}
      cur = cur->next;
   }
   if (k.compare(cur->key) == 0) {delete add; return -1;}
   cur->next = add;
   numEl++;
   return write(add);
}

string* NoVoHT::get(string k){
   int loc = hash(k)%size;
   kvpair *cur = kvpairs[loc];
   while (cur != NULL && !k.empty()){
      if (k.compare(cur->key) == 0) return &(cur->val);
      cur = cur->next;
   }
   return NULL;
}

//return 0 for success, -1 fail to remove, -2+ write failure
int NoVoHT::remove(string k){
   int ret =0;
   int loc = hash(k)%size;
   kvpair *cur = kvpairs[loc];
   if (cur == NULL) return ret-1;       //not found
   if (k.compare(cur->key) ==0) {
      fpos_t toRem = kvpairs[loc]->pos;
      kvpairs[loc] = cur->next;
      numEl--;
      ret+=mark(toRem);
      delete cur;
      nRem++;
      if (nRem == magicNumber) ret+=writeFile(); //write and save write success
      return ret;
   }
   while(cur != NULL){
      if (cur->next == NULL) return ret-1;
      else if (k.compare(cur->next->key)==0){
         kvpair *r = cur->next;
         cur->next = r->next;
         ret+=mark(r->pos);              //mark and sace status code
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
int NoVoHT::writeFile(){
   int ret =0;
   if (!dbfile)return -2;
   //fclose(dbfile);
   //dbfile = fopen(filename.c_str(), "w+");
   //dbfile = freopen(filename.c_str(), "w+", dbfile);
   fseek(dbfile, 0,SEEK_CUR);
   for (int i=0; i<size;i++){
      kvpair *cur = kvpairs[i];
      while (cur != NULL){
         if(!cur->key.empty() && !cur->val.empty()){
                fgetpos(dbfile, &(cur->pos));
                fprintf(dbfile, "%s\t%s\n", cur->key.c_str(), cur->val.c_str());
         }
         cur = cur->next;
      }
   }
   ftruncate(fileno(dbfile), (off_t)SEEK_CUR-SEEK_SET);
   //fclose(out);
   return ret;
}

//success 0 fail -2
//resize the hashmap's base size
void NoVoHT::resize(int ns){
   int olds = size;
   size = ns;
   kvpair** old = kvpairs;
   kvpairs = new kvpair*[ns];
   numEl = 0;
   for (int i=0; i<olds;i++){
      kvpair *cur = old[i];
      while (cur != NULL){
         put(cur->key, cur->val);
         kvpair *last = cur;
         cur = cur->next;
         delete last;
      }
   }
   delete [] old;
}

//success 0 fail -2
//write kvpair to file
int NoVoHT::write(kvpair * p){
   //FILE * data = fopen(file.c_str(), "a");
   if (!dbfile) return -2;
   fseek(dbfile, 0, SEEK_END);
   fgetpos(dbfile, &(p->pos));
   fprintf(dbfile, "%s\t%s\n", p->key.c_str(), p->val.c_str());
   //fclose(data);
   return 0;
}

//success 0 fail -2
//mark line in file for deletion
int NoVoHT::mark(fpos_t position){
   if(!dbfile) return -2;
   //FILE * data = fopen(file.c_str(), "r+");
   fsetpos(dbfile, &position);
   fputc((int) '~', dbfile);
   //fclose(data);
   return 0;
}

void NoVoHT::readFile(){
   if(!dbfile) return;
   //FILE * data = fopen(file.c_str(), "r+");
   char s[300];
   while(fscanf(dbfile, "%s", s) != EOF){
      string key(s);
      fscanf(dbfile,"%s",s);
      string val(s);
      if (key[0] != '~'){
         put(key,val);
      }
   }
   //fclose(data);
   writeFile();
}

unsigned long long hash(string k){ //FNV hash
#if SIZE_OF_LONG_LONG_INT==8
#define FNV_PRIME 14695981039346656037
#define FNV_OFFSET 1099511628211
#else //SIZE_OF_LONG_LONG_INT == 4
#define FNV_PRIME 16777619
#define FNV_OFFSET 2166136261
#endif
   unsigned long long x = FNV_PRIME;
   for (unsigned int y=0;y<k.length();y++){
      x = x ^ (k[y]);
      x = x * FNV_OFFSET;
   }
   return (x);
}

void fsu(kvpair* p){
   if(p != NULL){
      fsu(p->next);
      delete p;
   }
}
