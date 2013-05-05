noinst_PROGRAMS = genbench unit_tests
noinst_HEADERS = novoht-wrapper.h hashmap-definition.h
genbench_SOURCES = genbench.cpp ../src/novoht.cpp
genbench_CPPFLAGS = -I../src
genbench_LDFLAGS = -pthread
unit_tests_SOURCES = unit_tests.cpp ../src/novoht.cpp
unit_tests_CPPFLAGS = -I../src
unit_tests_LDFLAGS = -pthread
