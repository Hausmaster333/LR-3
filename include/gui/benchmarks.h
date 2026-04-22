#ifndef BENCHMARKS_H
#define BENCHMARKS_H

#include "deque/segment_deque.h"
#include "imgui.h"
#include <chrono>
#include <random>

template <class U>
size_t measure_deque_memory(const SegmentDeque<U>& deque) {
    size_t size = sizeof(SegmentDeque<U>);
    size += deque.map_capacity * sizeof(U*);
    
    int allocated = 0;
    for (int i = 0; i < deque.map_capacity; i++) {
        if (deque.block_map.get(i) != nullptr) allocated++;
    }
    size += allocated * 8 * sizeof(U);  // 8 = segment_size
    
    return size;
};

template <class U>
size_t measure_array_seq_memory(const ArraySequence<U>& dyn_array) {
    size_t size = sizeof(ArraySequence<U>);
    size += dyn_array.array.get_size() * sizeof(U); // данные внутри массива
    return size;
};

template <class U>
size_t measure_list_seq_memory(const ListSequence<U>& linked_list) {
    size_t size = sizeof(ListSequence<U>);
    
    // Каждый узел: данные T + указатель next
    int count = linked_list.list.get_length();
    size += count * linked_list.list.get_node_size();
    
    return size;
};

void run_push_front_benchmark(double* deque_times, double* array_times, double* list_times) {
    int sizes[] = {1000, 2000, 5000, 10000, 20000, 50000, 100000};
    int n_sizes = 7;

    for (int size = 0; size < n_sizes; size++) {
        int n = sizes[size];

        MutableSegmentedDeque<int> deque;
        auto start_deque = std::chrono::high_resolution_clock::now();

        for (int i = 0; i < n; i++) {
            deque.push_front(i);
        }

        auto finish_deque = std::chrono::high_resolution_clock::now();
        deque_times[size] = std::chrono::duration<double, std::milli>(finish_deque - start_deque).count();
        
        MutableArraySequence<int> array;
        auto start_array = std::chrono::high_resolution_clock::now();

        for (int i = 0; i < n; i++) {
            array.prepend(i);
        }

        auto finish_array = std::chrono::high_resolution_clock::now();
        array_times[size] = std::chrono::duration<double, std::milli>(finish_array - start_array).count();

        MutableListSequence<int> list;
        auto start_list = std::chrono::high_resolution_clock::now();

        for (int i = 0; i < n; i++) {
            list.prepend(i);
        }

        auto finish_list = std::chrono::high_resolution_clock::now();
        list_times[size] = std::chrono::duration<double, std::milli>(finish_list - start_list).count();
    }
};

void run_get_benchmark(double* deque_times, double* array_times, double* list_times) {
    int sizes[] = {1000, 2000, 5000, 10000, 20000, 50000, 100000};
    const int ACCESSES = 10000; // сколько случайных get'ов делать на каждом размере
    
    std::mt19937 rng(42); // генератор с фиксированным seed для воспроизводимости
 
    for (int s = 0; s < 7; s++) {
        int n = sizes[s];
        std::uniform_int_distribution<int> dist(0, n - 1);
 
        // --- Подготовка: заполняем структуры (это НЕ замеряем) ---
        MutableSegmentedDeque<int> deque;
        MutableArraySequence<int> array;
        MutableListSequence<int> list;
        for (int i = 0; i < n; i++) {
            deque.push_back(i);
            array.append(i);
            list.append(i);
        }
 
        // --- Генерируем случайные индексы заранее, чтобы все структуры обращались к одинаковым ---
        int* indices = new int[ACCESSES];
        for (int i = 0; i < ACCESSES; i++) {
            indices[i] = dist(rng);
        }
 
        // --- Замер SegmentedDeque ---
        auto t1 = std::chrono::high_resolution_clock::now();
        volatile int sum1 = 0; // volatile чтобы компилятор не выкинул цикл как "бесполезный"
        for (int i = 0; i < ACCESSES; i++) {
            sum1 += deque.get(indices[i]);
        }
        auto t2 = std::chrono::high_resolution_clock::now();
        deque_times[s] = std::chrono::duration<double, std::milli>(t2 - t1).count();
 
        // --- Замер ArraySequence ---
        t1 = std::chrono::high_resolution_clock::now();
        volatile int sum2 = 0;
        for (int i = 0; i < ACCESSES; i++) {
            sum2 += array.get(indices[i]);
        }
        t2 = std::chrono::high_resolution_clock::now();
        array_times[s] = std::chrono::duration<double, std::milli>(t2 - t1).count();
 
        // --- Замер ListSequence ---
        t1 = std::chrono::high_resolution_clock::now();
        volatile int sum3 = 0;
        for (int i = 0; i < ACCESSES; i++) {
            sum3 += list.get(indices[i]);
        }
        t2 = std::chrono::high_resolution_clock::now();
        list_times[s] = std::chrono::duration<double, std::milli>(t2 - t1).count();
 
        delete[] indices;
    }
}

void run_memory_benchmark(double* deque_mem, double* array_mem, double* list_mem) {
    int sizes[] = {1000, 2000, 5000, 10000, 20000, 50000, 100000};
    
    for (int s = 0; s < 7; s++) {
        int n = sizes[s];

        // Создаём дек, заполняем, измеряем
        MutableSegmentedDeque<int> deque;
        for (int i = 0; i < n; i++) deque.push_back(i);
        deque_mem[s] = (double)measure_deque_memory(deque) / 1024.0; // в КБ

        // ArraySequence
        MutableArraySequence<int> array;
        for (int i = 0; i < n; i++) array.append(i);
        array_mem[s] = (double)measure_array_seq_memory(array) / 1024.0;

        // ListSequence
        MutableListSequence<int> list;
        for (int i = 0; i < n; i++) list.append(i);
        list_mem[s] = (double)measure_list_seq_memory(list) / 1024.0;
    }
}

void run_benchmarks(double* deque_push_front_times, double* array_push_front_times, double* list_push_front_times,
                    double* deque_get_times, double* array_get_times, double* list_get_times,
                    double* deque_mem, double* array_mem, double* list_mem) {

    run_push_front_benchmark(deque_push_front_times, array_push_front_times, list_push_front_times);
    run_get_benchmark(deque_get_times, array_get_times, list_get_times);
    run_memory_benchmark(deque_mem, array_mem, list_mem);
};

#endif