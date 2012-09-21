#include <stddef.h>
#include <stdlib.h>
#include <iostream>
#include "novoht.h"
#include <new>

int main (int argc, char ** argv){
   NoVoHT map("append.txt", 1000, -1);
   map.append("two","appone");
   map.append("two", "apptwo");
   map.append("two", "appthree");
   map.append("two", "appthree");
   map.append("two", "appthree");
   map.append("two", "appthree");
   map.append("two", "appthree");
   map.append("two", "appthree");
   map.append("two", "appthree");
   map.append("two", "appthree");
   map.append("two", "applast");
   map.append("three", "appthree");
   map.append("three", "appthree");
   map.append("three", "appthree");
   map.append("three", "appthree");
   map.append("three", "appthree");
   map.append("three", "appthree");
   map.append("three", "appthree");
   map.append("three", "applast");
   cout << map.getSize() << endl;

   return 0;
}
