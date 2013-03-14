/*
 * Copyright 2013 Kevin Brandstatter
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
#ifndef NOVOHT_WRAPPER
#define NOVOHT_WRAPPER

#include "hashmap-definition.h"
#include "novoht.h"
#include <string>
using namespace std;

class NoVoHTwrapper : public HashMap {
	NoVoHT *map;
	public:
		NoVoHTwrapper(const char*, int);
      NoVoHTwrapper(const char*, int, int);
//		NoVoHTwrapper() : HashMap() {}
		virtual bool put(string, string);
		virtual string* get(string);
		virtual bool remove(string);
		virtual ~NoVoHTwrapper(void);

};
//NoVoHTwrapper::NoVoHTwrapper() : HashMap::HashMap() {
//	map = new NoVoHT("fdata", 10000, -1);
//}
NoVoHTwrapper::NoVoHTwrapper(const char* fname, int size) : HashMap() {
	map = new NoVoHT(fname, size, -1);
}

NoVoHTwrapper::NoVoHTwrapper(const char* fname, int size, int magic) : HashMap() {
	map = new NoVoHT(fname, size, magic);
}

bool NoVoHTwrapper::put(string key, string val){
	return map->put(key,val) == 0;
}

bool NoVoHTwrapper::remove(string victim){
	return map->remove(victim) == 0;
}

string* NoVoHTwrapper::get(string key){
	return map->get(key);
}

NoVoHTwrapper::~NoVoHTwrapper(){
	delete map;
}

#endif	
