# mingw32-make для сборки на Windows
# make для сборки на Linux
CC = g++
CFLAGS = -Wall -std=c++17 # Все предупреждения компилятора
GTEST_DIR = googletest/googletest
GTEST_FLAGS = -I$(GTEST_DIR)/include -I$(GTEST_DIR)

program: main.cpp menu.cpp
	$(CC) $(CFLAGS) main.cpp menu.cpp bit_sequence.cpp -o program

tests: tests.cpp $(GTEST_DIR)/src/gtest-all.cc $(GTEST_DIR)/src/gtest_main.cc
	$(CC) $(CFLAGS) $(GTEST_FLAGS) tests.cpp bit_sequence.cpp $(GTEST_DIR)/src/gtest-all.cc $(GTEST_DIR)/src/gtest_main.cc -o tests

clean:
	rm *.o program tests