# mingw32-make для сборки на Windows
# make для сборки на Linux
CC = g++
CFLAGS = -Wall -std=c++17 -Iinclude # Все предупреждения компилятора
GTEST_DIR = googletest/googletest
GTEST_FLAGS = -I$(GTEST_DIR)/include -I$(GTEST_DIR)

program: src/main.cpp src/menu.cpp
	$(CC) $(CFLAGS) src/main.cpp src/menu.cpp src/bit_sequence.cpp -o program

seq_tests: tests/tests.cpp $(GTEST_DIR)/src/gtest-all.cc $(GTEST_DIR)/src/gtest_main.cc
	$(CC) $(CFLAGS) $(GTEST_FLAGS) tests/tests.cpp src/bit_sequence.cpp $(GTEST_DIR)/src/gtest-all.cc $(GTEST_DIR)/src/gtest_main.cc -o seq_tests

deq_tests: tests/deque_tests.cpp $(GTEST_DIR)/src/gtest-all.cc $(GTEST_DIR)/src/gtest_main.cc
	$(CC) $(CFLAGS) $(GTEST_FLAGS) tests/deque_tests.cpp $(GTEST_DIR)/src/gtest-all.cc $(GTEST_DIR)/src/gtest_main.cc -o deq_tests

deq_tests_leak: tests/deque_tests.cpp $(GTEST_DIR)/src/gtest-all.cc $(GTEST_DIR)/src/gtest_main.cc
	$(CC) $(CFLAGS) -g -O0 $(GTEST_FLAGS) tests/deque_tests.cpp $(GTEST_DIR)/src/gtest-all.cc $(GTEST_DIR)/src/gtest_main.cc -o deq_tests_leak
#valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./deq_tests_leak
clean:
	rm *.o program tests