#include <stddef.h>
#include <stdlib.h>
#include <new>
#include <string>
#include <string.h>
#include <locale>
#include <stdio.h>
#include <iostream>
#include <pthread.h>
#include "novoht.h"

NoVoHT::NoVoHT(){
   kvpairs = new kvpair*[1000];
   size = 1000;
   numEl = 0;
   magicNumber = 1000;
   resizeNum = -1;
   resizing=false;
   map_lock=false;
   write_lock=false;
   resizing = false;
   rewriting = false;
   oldpairs = NULL;
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

NoVoHT::NoVoHT(const string&f) {
	size = 1000;
	kvpairs = new kvpair*[size];
	for (int x = 0; x < size; x++) {
		kvpairs[x] = NULL;
	}
	resizing = false;
	map_lock = false;
	write_lock = false;
	rewriting = false;
	magicNumber = 1;
	nRem = 0;
	resizeNum = 0;
	numEl = 0;
	filename = f;
	dbfile = fopen(f.c_str(), "r+");
	if (!dbfile)
		dbfile = fopen(f.c_str(), "w+");
	readFile();
	oldpairs = NULL;
}

NoVoHT::NoVoHT(const string& f, const int& s, const int& m) {
	kvpairs = new kvpair*[s];
	for (int x = 0; x < s; x++) {
		kvpairs[x] = NULL;
	}
	resizing = false;
	map_lock = false;
	write_lock = false;
        rewriting = false;
	magicNumber = m;
	nRem = 0;
	resizeNum = 0;
	size = s;
	numEl = 0;
	filename = f;
	dbfile = fopen(f.c_str(), "r+");
	if (!dbfile)
		dbfile = fopen(f.c_str(), "w+");
        //setbuf(dbfile, NULL);
	//readFile();
	oldpairs = NULL;
}

NoVoHT::NoVoHT(const string& f, const int& s, const int& m, const float& r) {
	kvpairs = new kvpair*[s];
	for (int x = 0; x < s; x++) {
		kvpairs[x] = NULL;
	}
	resizing = false;
	map_lock = false;
	write_lock = false;
        rewriting = false;
	magicNumber = m;
	nRem = 0;
	resizeNum = r;
	size = s;
	numEl = 0;
	filename = f;
	dbfile = fopen(f.c_str(), "r+");
	if (!dbfile)
		dbfile = fopen(f.c_str(), "w+");
	readFile();
	oldpairs = NULL;
}
/*
 NoVoHT::NoVoHT(char * f, NoVoHT *map){
 kvpairs = new kvpair*[1000];
 size = 1000;
 numEl=0;
 file = f;
 readFile();
 }*/

//0 success, -1 no insert, -2 no write
int NoVoHT::put(string k, string v) {
	//while(resizing || map_lock){ /* Wait till done */}
	while (map_lock) {
	}
	map_lock = true;
	if (numEl >= size * resizeNum) {
		if (resizeNum != 0) {
			resize(size * 2);
		}
	}
	int slot;
	slot = hash(k) % size;
	kvpair *cur = kvpairs[slot];
	kvpair *add = new kvpair;
	add->key = k;
	add->val = v;
	add->next = NULL;
        add->positions = NULL;
	if (cur == NULL) {
		kvpairs[slot] = add;
		numEl++;
		map_lock = false;
		return write(add);
	}
	while (cur->next != NULL) {
		if (k.compare(cur->key) == 0) {
			cur->val = v;
			mark(cur->positions);
			delete add;
			map_lock = false;
			return write(cur);
		}
		cur = cur->next;
	}
	if (k.compare(cur->key) == 0) {
		cur->val = v;
		mark(cur->positions);
		delete add;
		map_lock = false;
		return write(cur);
	}
	cur->next = add;
	numEl++;
	map_lock = false;
	return write(add);
}

NoVoHT::~NoVoHT(){
   if (dbfile){
      //writeFile();
      pthread_join(writeThread, NULL);
      fclose(dbfile);
   }
   for (int i = 0; i < size; i++){
      fsu(kvpairs[i]);
   }
   delete [] kvpairs;
}

string* NoVoHT::get(string k) {
	while (map_lock) { /* Wait till done */
	}
	int loc = hash(k) % size;
	kvpair *cur = kvpairs[loc];
	while (cur != NULL && !k.empty()) {
		if (k.compare(cur->key) == 0)
			return &(cur->val);
		cur = cur->next;
	}
	return NULL;
}

//return 0 for success, -1 fail to remove, -2+ write failure
int NoVoHT::remove(string k){
   while(map_lock){ /* Wait till done */}
   int ret =0;
   int loc = hash(k)%size;
   kvpair *cur = kvpairs[loc];
   if (cur == NULL) return ret-1;       //not found
   if (k.compare(cur->key) ==0) {
      //fpos_t toRem = kvpairs[loc]->pos;
      fpos_list * toRem = kvpairs[loc]->positions;
      kvpairs[loc] = cur->next;
      numEl--;
      ret= rewriting ? logrm(k, toRem) +ret : ret + mark(toRem);
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
      //   fpos_t toRem = r->pos;
         struct fpos_list * toRem = kvpairs[loc]->positions;
         ret= rewriting ? logrm(k, toRem) +ret : ret + mark(toRem);
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

// Test
int NoVoHT::append(string k, string aval){
   while(map_lock) { /* Wait for it... */ }
   int ret = 0;
   int loc = hash(k)%size;
   kvpair* cur = kvpairs[loc];
   while (cur != NULL){
      if (k.compare(cur->key) == 0){
         cur->val += ":" + aval;
         return writeAppend(cur, aval) + ret;
      }
      cur = cur->next;
   }
   kvpair* add = new kvpair;
   add->key = k;
   add->val = aval;
   add->next = kvpairs[loc];
   add->positions = NULL;
   kvpairs[loc] = add;
   numEl++;
   map_lock = false;
   return write(add);
}

//return 0 if success -2 if failed
//write hashmap to file
//// Test
int NoVoHT::writeFile(){
   while (write_lock){}
   if (!dbfile)return (filename.compare("") == 0 ? 0 : -2);
   if (rewriting) {
      //pthread_join(writeThread, NULL);
      nRem = 0;
      return 0;
   }
   rewriting = true;
   swapFile = dbfile;
   dbfile = fopen(".novoht.swp", "w+");
   nRem = 0;
   int rc = pthread_create(&writeThread, NULL, rewriteCaller, this);
   if (rc){
      printf("Thread not created");
      fclose(dbfile);
      dbfile = swapFile;
      rewriting = false;
   }
   return rc;
}

// Fix
void NoVoHT::rewriteFile(void *args){
   //write_lock=true;
   rewind(swapFile);
   ftruncate(fileno(swapFile), 0);
   for (int i=0; i<size;i++){
      kvpair *cur = kvpairs[i];
      while (cur != NULL){
         if(!cur->key.empty() && !cur->val.empty() && !cur->diff){
            destroyFposList(cur->positions);
            cur->positions = new fpos_list;
            cur->positions->next = NULL;
                fgetpos(swapFile, &(cur->positions->pos));
                fprintf(swapFile, "%s\t%s\t", cur->key.c_str(), cur->val.c_str());
         }
         cur = cur->next;
      }
   }
   merge();
   pthread_exit(NULL);
}

// Fix
void NoVoHT::merge(){
   while(write_lock){}
   write_lock=true;
   char buf[300];
   char sec[300];
   rewind(dbfile);
   while (readTabString(dbfile,buf) != NULL){
      if(buf[0] == '~'){
         readTabString(dbfile, sec);
         char * pos;
         pos = strtok(sec, ",");
         while (pos != NULL) {
            fseek(swapFile, (off_t) atoi(pos), SEEK_SET);
            char test[300];
            readTabString(swapFile,test);
            if (strcmp(test,(buf+1)) == 0){
               fseek(swapFile, (off_t) atoi(pos), SEEK_SET);
               fputc('~',swapFile);
            }
            pos = strtok(NULL, ",");
         }
      }
      else{
         while (map_lock) {}
         map_lock = true;
         fseek(swapFile, 0, SEEK_END);
         string s(buf);
         kvpair* p = kvpairs[hash(s)%size];
         while (p != NULL){
            if (p->key.compare(s) == 0){
               destroyFposList(p->positions);
               p->positions = new fpos_list;
               p->positions->next = NULL;
               fgetpos(swapFile, &(p->positions->pos));
               fprintf(swapFile, "%s\t%s\t", p->key.c_str(), p->val.c_str());
               p->diff = false;
               break;
            }
            else
               p = p->next;
         }
         map_lock = false;
      }
   }
   FILE *tmp = dbfile;
   dbfile = swapFile;
   fclose(tmp);
   remove(".novoht.swp");
   rewriting = false;
   write_lock = false;
}

//success 0 fail -2
//resize the hashmap's base size
void NoVoHT::resize(int ns) {
	resizing = true;
	int olds = size;
	size = ns;
	oldpairs = kvpairs;
	kvpairs = new kvpair*[ns];
	for (int z = 0; z < ns; z++) {
		kvpairs[z] = NULL;
	}
	numEl = 0;
	for (int i = 0; i < olds; i++) {
		kvpair *cur = oldpairs[i];
		while (cur != NULL) {
			int pos = hash(cur->key) % size;
			kvpair * tmp = kvpairs[pos];
			kvpairs[pos] = cur;
			cur = cur->next;
			kvpairs[pos]->next = tmp;
		}
	}
	delete[] oldpairs;
	resizing = false;
}

//success 0 fail -2
//write kvpair to file
//Test
int NoVoHT::write(kvpair * p) {
	while (write_lock) {
	}
	if (!dbfile)
		return (filename.compare("") == 0 ? 0 : -2);
	write_lock = true;
	fseek(dbfile, 0, SEEK_END);
        if (p->positions != NULL) destroyFposList(p->positions);
        p->positions = new fpos_list;
        p->positions->next = NULL;
	fgetpos(dbfile, &(p->positions->pos));
	fprintf(dbfile, "%s\t%s\t", p->key.c_str(), p->val.c_str());
        fflush(dbfile);
	write_lock = false;
	return 0;
}

//Test
int NoVoHT::writeAppend(kvpair *p, string appendString){
	while (write_lock) {
	}
	if (!dbfile)
		return (filename.compare("") == 0 ? 0 : -2);
	write_lock = true;
	fseek(dbfile, 0, SEEK_END);
        fpos_list * new_pos = new fpos_list;
        new_pos->next = p->positions;
        p->positions = new_pos;
	fgetpos(dbfile, &(new_pos->pos));
	fprintf(dbfile, "%s\t%s\t", p->key.c_str(), appendString.c_str());
        fflush(dbfile);
	write_lock = false;
	return 0;
}

//Test
int NoVoHT::logrm(string key, fpos_list * plist){
   while (write_lock){}
   write_lock = true;
   fseek(dbfile, 0, SEEK_END);
   fputc('~', dbfile);
   fprintf(dbfile, "%s\t", key.c_str());
   while (plist != NULL){
      fprintf(dbfile, "%ld,", plist->pos.__pos );
      plist = plist -> next;
   }
   fprintf(dbfile, "\t");
   write_lock = false;
   return 0;
}

//success 0 fail -2
//mark line in file for deletion
// Test
int NoVoHT::mark(fpos_list * plist) {
	while (write_lock) {
	}
	if (!dbfile)
		return (filename.compare("") == 0 ? 0 : -2);
	write_lock = true;
        while (plist != NULL){
           fsetpos(dbfile, &(plist->pos));
           fputc((int) '~', dbfile);
           plist = plist -> next;
        }
	write_lock = false;
	return 0;
}
char *readTabString(FILE *file, char *buffer) {
	int n = 0;
	char t;
	while ((t = fgetc(file)) != EOF && n < 300) {
		if (t == '\t') {
			buffer[n] = '\0';
			return buffer;
		}
		buffer[n] = t;
		n++;
	}
	buffer[n] = '\0';
	return (n == 0 ? NULL : buffer);
}

// rewrite this shit
void NoVoHT::readFile() {
	if (!dbfile)
		return;
	char s[300];
	char v[300];
	while (readTabString(dbfile, s) != NULL) {
		string key(s);
		if (readTabString(dbfile, v) == NULL)
			break;
		string val(v);
		if (key[0] != '~') {
			put(key, val);
		}
	}
	writeFile();
}

unsigned long long hash(string k) { //FNV hash
#if SIZE_OF_LONG_LONG_INT==8
#define FNV_PRIME 14695981039346656037
#define FNV_OFFSET 1099511628211
#else //SIZE_OF_LONG_LONG_INT == 4
#define FNV_PRIME 16777619
#define FNV_OFFSET 2166136261
#endif
	unsigned long long x = FNV_PRIME;
	for (unsigned int y = 0; y < k.length(); y++) {
		x = x ^ (k[y]);
		x = x * FNV_OFFSET;
	}
	return (x);
}

void fsu(kvpair* p) {
	if (p != NULL) {
		fsu(p->next);
		delete p;
	}
}

void destroyFposList(fpos_list * list){
   if (list == NULL) return;
   destroyFposList(list->next);
   delete list;
   list = NULL;
}

void delete_kvpair(kvpair * redshirt){
   destroyFposList(redshirt->positions);
   delete redshirt;
   redshirt = NULL;
}

bool NoVoHT::isRewriting() const {

	return rewriting;
}

int NoVoHT::flushDbfile() {

	return fflush(dbfile);
}

key_iterator NoVoHT::keyIterator() {
	return key_iterator(kvpairs, size, this);
}
val_iterator NoVoHT::valIterator() {
	return val_iterator(kvpairs, size, this);
}
pair_iterator NoVoHT::pairIterator() {
	return pair_iterator(kvpairs, size, this);
}
