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

#include <unistd.h>
#include <stdlib.h>
#include <new>
#include <string>
#include <string.h>
#include <locale>
#include <stdio.h>
#include <iostream>
#include <pthread.h>
#include "novoht.h"
#define DEBUG	TRUE

NoVoHT::NoVoHT(){
   initialize("", 1000, -1, 0);
}

NoVoHT::NoVoHT(const string& f) {
   initialize(f, 1000, -1, 0);
}

NoVoHT::NoVoHT(const string& f, const int& s, const int& m) {
   initialize(f, s, m, 0);
}

// NoVoHT( Filename, size, magic number, resize threashold )
NoVoHT::NoVoHT(const string& f, const int& s, const int& m, const float& r) {
   initialize(f, s, m, r);
}

void NoVoHT::initialize(const string& fname, const int& initSize,
        const int& gcnum, const float& resizeRatio){
	kvpairs = new kvpair*[initSize];
	for (int x = 0; x < initSize; x++) {
		kvpairs[x] = NULL;
	}
        sem_init(&map_lock, 0, 1);
        sem_init(&write_lock, 0, 1);
	resizing = false;
   rewriting = false;
	nRem = 0;
	resizeNum = resizeRatio;
	size = initSize;
	numEl = 0;
	filename = fname;
	oldpairs = NULL;
	readFile();
	magicNumber = gcnum;
}

//0 success, -1 no insert, -2 no write
int NoVoHT::put(string k, string v) {
   sem_wait(&map_lock);
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
		int rc = write(add);
		if (rc){
			delete add;
			sem_post(&map_lock);
			return -1;
		}
		kvpairs[slot] = add;
		numEl++;
      sem_post(&map_lock);
		return 0;
	}
	while (cur->next != NULL) {
		if (k.compare(cur->key) == 0) {
			delete add;
			string oldv = cur->val;
			cur->val = v;
			int rc = write(cur);
			if (rc) {
				cur->val = oldv;
				sem_post(&map_lock);
				return -1;
			}
			mark(cur->positions);
         sem_post(&map_lock);
			return 0;
		}
		cur = cur->next;
	}
	if (k.compare(cur->key) == 0) {
		delete add;
		string oldv = cur->val;
		cur->val = v;
		int rc = write(cur);
		if (rc) {
			cur->val = oldv;
			sem_post(&map_lock);
			return -1;
		}
		mark(cur->positions);
      sem_post(&map_lock);
		return 0;
	}
	int rc = write(add);
	if (rc){
		delete add;
		sem_post(&map_lock);
		return -1;
	}
	cur->next = add;
	numEl++;
   sem_post(&map_lock);
	return 0;
}

NoVoHT::~NoVoHT(){
   if (dbfile){
      writeFile();
      if (writeThread)
         pthread_join(writeThread, NULL);
      fclose(dbfile);
   }
   for (int i = 0; i < size; i++){
      fsu(kvpairs[i]);
   }
   delete [] kvpairs;
}

string* NoVoHT::get(string k) {
   sem_wait(&map_lock);
   sem_post(&map_lock);
	int loc = hash(k) % size;
	kvpair *cur = kvpairs[loc];
	while (cur != NULL && !k.empty()) {
		if (k.compare(cur->key) == 0)
			return new string((cur->val).c_str());
		cur = cur->next;
	}
	return NULL;
}

//return 0 for success, -1 fail to remove, -2 not found
int NoVoHT::remove(string k){
   sem_wait(&map_lock);
   int loc = hash(k)%size;
   kvpair *cur = kvpairs[loc];
   if (cur == NULL) {
      sem_post(&map_lock);
      return -2;       //not found
   }
   if (k.compare(cur->key) ==0) {
		int ret;
      fpos_list * toRem = kvpairs[loc]->positions;
      kvpairs[loc] = cur->next;
      numEl--;
      ret= rewriting ? logrm(k, toRem) : mark(toRem);
		if (ret) {
			sem_post(&map_lock);
			return -1;
		}
      delete cur;
      nRem++;
      if (nRem >= magicNumber) writeFile(); //write and save write success
      sem_post(&map_lock);
      return 0;
   }
   while(cur != NULL){
      if (cur->next == NULL){
         sem_post(&map_lock);
         return -2;
      }

      else if (k.compare(cur->next->key)==0){
			int ret;
         kvpair *r = cur->next;
         cur->next = r->next;
      //   fpos_t toRem = r->pos;
         struct fpos_list * toRem = r->positions;
         ret= rewriting ? logrm(k, toRem) : mark(toRem);
			if (ret) {
				sem_post(&map_lock);
				return -1;
			}
         delete r;
         numEl--;
         nRem++;
         if (nRem >= magicNumber) writeFile();              //mark and sace status code
         sem_post(&map_lock);
         return 0;
      }
      cur = cur->next;
   }
   sem_post(&map_lock);
   return -2;        //not found
}

// Test
int NoVoHT::append(string k, string aval){
   //while(map_lock) { /* Wait for it... */ }
   sem_wait(&map_lock);
   int ret = 0;
   int loc = hash(k)%size;
   kvpair* cur = kvpairs[loc];
   while (cur != NULL){
      if (k.compare(cur->key) == 0){
			int rc;
			string oldv = cur->val;
         cur->val += aval;
			rc = writeAppend(cur,aval);
			if (rc) {
				cur->val = oldv;
				sem_post(&map_lock);
				return -1;
			}
         sem_post(&map_lock);
         return 0;
      }
      cur = cur->next;
   }
   kvpair* add = new kvpair;
   add->key = k;
   add->val = aval;
   add->next = kvpairs[loc];
   add->positions = NULL;
	int rc;
	rc = write(add);
	if (rc){
		delete add;
		sem_post(&map_lock);
		return -1;
	}
   kvpairs[loc] = add;
   numEl++;
   //map_lock = false;
   sem_post(&map_lock);
   return 0;
}

int NoVoHT::writeFileFG() {
   int ret = writeFile();
   pthread_join(writeThread, NULL);
   return ret;
}

//return 0 if success -2 if failed
//write hashmap to file
//// Test
int NoVoHT::writeFile(){
   //while (write_lock){}
   if (!dbfile) {
      return (filename.compare("") == 0 ? 0 : -2);
   }
   if (rewriting) {
      //pthread_join(writeThread, NULL);
      nRem = 0;
      return 0;
   }
   sem_wait(&write_lock);
   rewriting = true;
   //swapFile = dbfile;
   fclose(dbfile);
   swapFile = fopen(".novoht.swp", "w");
   dbfile = fopen(".novoht.mrg", "w");
   nRem = 0;
   int rc = pthread_create(&writeThread, NULL, rewriteCaller, this);
   if (rc){
      printf("Thread not created");
		fclose(dbfile);
		dbfile = swapFile;
		rewriting = false;
   }
   sem_post(&write_lock);
   return rc;
}

// Fix
void NoVoHT::rewriteFile(void *args){
   //write_lock=true;
   for (int i=0; i<size;i++){
      kvpair *cur = kvpairs[i];
      while (cur != NULL){
         if(!cur->diff){
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

/*
 * swapFile -> .novoht.swp (fname if recovering)
 * dbfile -> .novoht.mrg
 */

void NoVoHT::merge(){
   char buf[300];
   char sec[300];
	sem_wait(&map_lock);
   sem_wait(&write_lock);
   fflush(dbfile);
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
         //while (map_lock) {}
         //map_lock = true;
         //sem_wait(&map_lock);
         fseek(swapFile, 0, SEEK_END);
         string s(buf);
         readTabString(dbfile,sec);
         kvpair* p = kvpairs[hash(s)%size];
         while (p != NULL){
            if (p->key.compare(s) == 0){
               destroyFposList(p->positions);
               p->positions = new fpos_list;
               p->positions->next = NULL;
               fgetpos(swapFile, &(p->positions->pos));
               fprintf(swapFile, "%s\t%s\t", p->key.c_str(), p->val.c_str());
               printf("%s\t%s\t", p->key.c_str(), p->val.c_str());
               p->diff = false;
               break;
            }
            else
               p = p->next;
         }
         //map_lock = false;
         //sem_post(&map_lock);
      }
   }
	fclose(dbfile);
   fclose(swapFile);
   rename(".novoht.swp", filename.c_str());
   dbfile = fopen(filename.c_str(), "r+");
   //remove(".novoht.mrg");
	int rmrc = unlink(".novoht.mrg");
   if (rmrc) {
      perror("Error deleting merge file");
   }
   rewriting = false;
	sem_post(&map_lock);
   sem_post(&write_lock);
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
	//while (write_lock) {
        //int semv;
        //sem_getvalue(&map_lock, &semv);
        //printf("released = %d\n", semv);
	if (!dbfile)
		return (filename.compare("") == 0 ? 0 : -2);
	//write_lock = true;
        sem_wait(&write_lock);
	fseek(dbfile, 0, SEEK_END);
        if (p->positions != NULL) destroyFposList(p->positions);
        p->positions = new fpos_list;
        p->positions->next = NULL;
	fgetpos(dbfile, &(p->positions->pos));
	fprintf(dbfile, "%s\t%s\t", p->key.c_str(), p->val.c_str());
   fflush(dbfile);
	//write_lock = false;
        sem_post(&write_lock);
	return 0;
}

//Test
int NoVoHT::writeAppend(kvpair *p, string appendString){
	//while (write_lock) {
	//}
	if (!dbfile)
		return (filename.compare("") == 0 ? 0 : -2);
	//write_lock = true;
        sem_wait(&write_lock);
	fseek(dbfile, 0, SEEK_END);
        fpos_list * new_pos = new fpos_list;
        new_pos->next = p->positions;
        p->positions = new_pos;
	fgetpos(dbfile, &(new_pos->pos));
	fprintf(dbfile, "%s\t%s\t", p->key.c_str(), appendString.c_str());
        fflush(dbfile);
	//write_lock = false;
        sem_post(&write_lock);
	return 0;
}

//Test
int NoVoHT::logrm(string key, fpos_list * plist){
   //while (write_lock){}
   //write_lock = true;
   sem_wait(&write_lock);
   fseek(dbfile, 0, SEEK_END);
   fputc('~', dbfile);
   fprintf(dbfile, "%s\t", key.c_str());
   while (plist != NULL){
      fprintf(dbfile, "%ld,", plist->pos.__pos );
      plist = plist -> next;
   }
   fprintf(dbfile, "\t");
   fflush(dbfile);
   //write_lock = false;
   sem_post(&write_lock);
   return 0;
}

//success 0 fail -2
//mark line in file for deletion
// Test
int NoVoHT::mark(fpos_list * plist) {
	//while (write_lock) {
	if (!dbfile)
		return (filename.compare("") == 0 ? 0 : -2);
	//write_lock = true;
        sem_wait(&write_lock);
        while (plist != NULL){
           fsetpos(dbfile, &(plist->pos));
           fputc((int) '~', dbfile);
           plist = plist -> next;
        }
        fflush(dbfile);
	//write_lock = false;
        sem_post(&write_lock);
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
   //if ((dbfile = fopen(".nddovoht.mrg", "r")) == 0){
   if (filename.empty()) return;
   rename(filename.c_str(), ".novoht.swp");
   dbfile = fopen(filename.c_str(), "w+");
   setvbuf(dbfile,NULL, _IONBF, 0);
   if ((swapFile = fopen(".novoht.swp", "r")) == NULL)
      return;
   rewriting = false;
	char s[300];
	char v[300];
	while (readTabString(swapFile, s) != NULL) {
		string key(s);
		if (readTabString(swapFile, v) == NULL)
			break;
		string val(v);
		if (key[0] != '~') {
			put(key, val);
		}
	}
   fclose(swapFile);
   if (unlink(".novoht.swp")) perror("Error deleting swap file");
   //FILE * mergeFile;
   if ((swapFile = fopen(".novoht.mrg", "r")) != NULL){ // Corrupted close, merging changes
      char buf[300];
      char sec[300];
      while (readTabString(swapFile,buf) != NULL){
         if(buf[0] == '~'){
            remove(buf+1);
            readTabString(swapFile, sec);
         }
         else{
            readTabString(swapFile,sec);
            put(string(buf), string(sec));
         }
      }
      int rmrc = unlink(".novoht.mrg");
      if (rmrc) {
         perror("Error deleting merge file");
      }
   }
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
