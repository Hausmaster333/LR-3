# mingw32-make для сборки на Windows
# make для сборки на Linux
CC = g++
CFLAGS = -Wall -std=c++17 # Все предупреждения компилятора
GTEST_DIR = googletest/googletest
GTEST_FLAGS = -I$(GTEST_DIR)/include -I$(GTEST_DIR)

program: main.cpp menu.cpp
	$(CC) $(CFLAGS) main.cpp menu.cpp bit_sequence.cpp -o program

seq_tests: tests.cpp $(GTEST_DIR)/src/gtest-all.cc $(GTEST_DIR)/src/gtest_main.cc
	$(CC) $(CFLAGS) $(GTEST_FLAGS) tests.cpp bit_sequence.cpp $(GTEST_DIR)/src/gtest-all.cc $(GTEST_DIR)/src/gtest_main.cc -o tests

deq_tests: tests.cpp $(GTEST_DIR)/src/gtest-all.cc $(GTEST_DIR)/src/gtest_main.cc
	$(CC) $(CFLAGS) $(GTEST_FLAGS) deque_tests.cpp $(GTEST_DIR)/src/gtest-all.cc $(GTEST_DIR)/src/gtest_main.cc -o deq_tests

deq_tests_leak: tests.cpp $(GTEST_DIR)/src/gtest-all.cc $(GTEST_DIR)/src/gtest_main.cc
	$(CC) $(CFLAGS) -g -O0 $(GTEST_FLAGS) deque_tests.cpp $(GTEST_DIR)/src/gtest-all.cc $(GTEST_DIR)/src/gtest_main.cc -o deq_tests_leak
#valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./deq_tests_leak
clean:
	rm *.o program tests