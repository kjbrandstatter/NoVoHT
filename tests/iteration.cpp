/*
 * Copyright (C) 2010-2020 DatasysLab@iit.edu(http://datasys.cs.iit.edu/index.html).
 *      Director: Ioan Raicu(iraicu@cs.iit.edu)
 *	 
 * This file is part of ZHT library(http://datasys.cs.iit.edu/projects/ZHT/index.html).
 *      Ioan Raicu(iraicu@cs.iit.edu),
 *      Tonglin Li(tli13@hawk.iit.edu) with nickname tony,
 *      Xiaobing Zhou(xzhou40@hawk.iit.edu) with nickname xiaobingo.
 * 
 * The ZHT Library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * The ZHT Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with the ZHT Library; if not, write to the 
 * Data-Intensive Distributed Systems Laboratory, 10 W. 31st Street,
 * Stuart Building, Room 003B, Chicago, IL 60616 USA.
 *
 * iteration.cpp
 *
 *  Created on: Aug 12, 2012
 *      Author: tony, xiaobingo
 */

#include "novoht.h"

void test() {

	//	NoVoHT *map = new NoVoHT("hash.table", 1000, 100);
	NoVoHT *map = new NoVoHT("hash.table");
	//	map->put("kevin", "hello");
	map->put("kevin", "overwrite");
	map->put("hello1", "zht1");
	map->put("hello2", "zht2");
	//	printf("The value is %s\n", (map->get("kevin"))->c_str());

	/*	key_iterator ki = map->keyIterator();
	 while (ki.hasNext()) {
	 printf("The next key is %s\n", ki.next().c_str());
	 }

	 val_iterator vi = map->valIterator();
	 while (vi.hasNext()) {
	 printf("The next value is %s\n", vi.next().c_str());
	 }*/

	int i = 0;
	pair_iterator kvi = map->pairIterator();
	while (kvi.hasNext()) {
		kvpair kv = kvi.next();
		printf("The next key is %s\n", kv.key.c_str());
		printf("The next value is %s\n", kv.val.c_str());

		//		kvi.next();
		//map->remove(kv.key);

		//		kvi.remove();
	}

	printf("%s\n", "---------------");

	kvi = map->pairIterator();
	while (kvi.hasNext()) {
		kvpair kv = kvi.next();
		printf("The next key is %s\n", kv.key.c_str());
		printf("The next value is %s\n", kv.val.c_str());
	}

	/*while (ki.hasNext() && vi.hasNext()) {
	 printf("The next key is %s\n", ki.next().c_str());
	 printf("The next value is %s\n", vi.next().c_str());
	 }*/

	//bool b = ki.hasNext();
	/*
	 printf("The next key is %s\n", ki.hasNext() ? ki.next().c_str() : "no key");
	 printf("The next value is %s\n",
	 vi.hasNext() ? vi.next().c_str() : "no value");
	 */
	/*
	 printf("The next key is %s\n", ki.hasNext() ? ki.next().c_str() : "no key");
	 printf("The next value is %s\n",
	 vi.hasNext() ? vi.next().c_str() : "no value");

	 printf("The next key is %s\n", ki.hasNext() ? ki.next().c_str() : "no key");
	 printf("The next value is %s\n",
	 vi.hasNext() ? vi.next().c_str() : "no value");
	 */
}
int main(int argc, char **argv) {

	NoVoHT *map = new NoVoHT("./data/hash.table2");

	map->put("kevin", "overwrite");
	map->put("hello1", "zht1");
	map->put("hello2", "zht2");

	int i = 0;
	pair_iterator kvi = map->pairIterator();
	while (kvi.hasNext()) {
		kvpair kv = kvi.next();
		printf("The next key is %s\n", kv.key.c_str());
		printf("The next value is %s\n", kv.val.c_str());

	}
}

