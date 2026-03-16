#include "segment_deque.h"
#include <stdexcept>

template <class T>
T* SegmentDeque<T>::allocate_block() {
    return new T[segment_size];
}   

template <class T>
void SegmentDeque<T>::grow_map_front() {    
    int old_capacity = map_capacity;
    map_capacity = map_capacity * 2;
    block_map->resize(map_capacity);

    for (int i = map_capacity - 1; i >= old_capacity; i--) {
        T* old_block = block_map->get(i - old_capacity);
        block_map->set(i, old_block);
    }

    for (int i = old_capacity - 1; i >= 0; i--) {
        block_map->set(i, nullptr);
    }

    front_block += old_capacity;
    back_block += old_capacity;
}

template <class T>
void SegmentDeque<T>::grow_map_back() {
    int old_capacity = map_capacity;
    map_capacity = map_capacity * 2;
    block_map->resize(map_capacity);

    for (int i = old_capacity; i < map_capacity; i++) {
        block_map->set(i, nullptr);
    }
}

template <class T>
void SegmentDeque<T>::resolve_index(int index, int& block, int& offset) const { 
    int total_offset = front_index + index; // Самый первый индекс + смещение по индексу

    block = front_block + total_offset / segment_size;
    offset = total_offset % segment_size;
}

template <class T>
SegmentDeque<T>::SegmentDeque() {
    block_map = new DynamicArray<T*>(4);

    for (int i = 0; i < 4; i++) {
        block_map->set(i, nullptr);
    }

    T* init_block = allocate_block();

    block_map->set(1, init_block); // Ставим начальный блок по центру

    front_block = 1;
    back_block = 1;
    front_index = 4;
    back_index = 4;
    count = 0;
    map_capacity = 4;
}

template <class T>
void SegmentDeque<T>::push_back(const T& item) {
    T* curr_block = block_map->get(back_block);
    curr_block[back_index] = item;
    back_index++;

    if (back_index == segment_size) {
        back_block++;
        back_index = 0;

        if (back_block >= map_capacity) {
            grow_map_back();
        }

        if (block_map->get(back_block) == nullptr) {
            block_map->set(back_block, allocate_block());
        }
    }

    count++;
}

template <class T>
void SegmentDeque<T>::push_front(const T& item) {
    front_index--;

    if (front_index < 0) {
        front_block--;
        front_index = segment_size - 1;

        if (front_block < 0) {
            grow_map_front();
        }

        if (block_map->get(front_block) == nullptr) {
            block_map->set(front_block, allocate_block());
        }
    }  

    T* curr_block = block_map->get(front_block);
    curr_block[front_index] = item;

    count++;
}

template <class T>
T SegmentDeque<T>::pop_back() {
    if (count == 0) throw std::out_of_range("Deque is empty");

    back_index--;

    if (back_index < 0) {
        back_block--;
        back_index = segment_size - 1;
    }

    T* curr_block = block_map->get(back_block);
    T curr_elem = curr_block[back_index];

    count--;

    return curr_elem;
}

template <class T>
T SegmentDeque<T>::pop_front() {;
    if (count == 0) throw std::out_of_range("Deque is empty");

    T* curr_block = block_map->get(front_block);
    T curr_elem = curr_block[front_index];

    front_index++;

    if (front_index >= segment_size) {
        front_block++;
        front_index = 0;
    }

    count--;

    return curr_elem;
}

template <class T>
const T& SegmentDeque<T>::get_first() const {
    if (count == 0) throw std::out_of_range("Deque is empty");

    T* curr_block = block_map->get(front_block);

    return curr_block[front_index];
}

template <class T>
const T& SegmentDeque<T>::get_last() const {
    if (count == 0) throw std::out_of_range("Deque is empty");
    
    int curr_block_index, offset;
    resolve_index(count - 1, curr_block_index, offset); // Получаем положение последнего элементам дека

    T* curr_block = block_map->get(curr_block_index);

    return curr_block[offset];
}

template <class T>
const T& SegmentDeque<T>::get(int index) const {
    if (index < 0 || index >= count) throw std::out_of_range("Index out of range");

    int block_index, offset;
    resolve_index(index, block_index, offset);

    T* curr_block = block_map->get(block_index);

    return curr_block[offset];
}

template <class T>
Option<T> SegmentDeque<T>::try_get_first() const {
    if (count == 0) return Option<T>::None();

    T* curr_block = block_map->get(front_block);

    return Option<T>::Some(curr_block[front_index]);
}

template <class T>
Option<T> SegmentDeque<T>::try_get_last() const {
    if (count == 0) return Option<T>::None();

    int curr_block_index, offset;
    resolve_index(count - 1, curr_block_index, offset);

    T* curr_block = block_map->get(curr_block_index);

    return Option<T>::Some(curr_block[offset]);
}

template <class T>
Option<T> SegmentDeque<T>::try_get(int index) const {
    if (index < 0 || index >= count) return Option<T>::None();

    int curr_block_index, offset;
    resolve_index(index, curr_block_index, offset);

    T* curr_block = block_map->get(curr_block_index);

    return Option<T>::Some(curr_block[offset]);
}

template <class T>
int SegmentDeque<T>::get_count() const {
    return count;
}

template <class T>
Sequence<T>* SegmentDeque<T>::get_sub_sequence(int start, int end) {
    if (count == 0) throw std::out_of_range("Deque is empty\n");

    if (start < 0 || end < 0 || start >= count || end >= count || start > end) throw std::out_of_range("Index out of range");

    SegmentDeque<T>* sub = EmptyClone();
    
    for (int i = start; i <= end; i++) {
        sub->push_back(get(i));
    }

    return sub;
}

template <class T>
Sequence<T>* SegmentDeque<T>::append(const T& item) {
    SegmentDeque<T>* inst = Instance();

    inst->push_back(item);

    return inst;
}

template <class T>
Sequence<T>* SegmentDeque<T>::prepend(const T& item) {
    SegmentDeque<T>* inst = Instance();

    inst->push_front(item);

    return inst;
}

template <class T>
Sequence<T>* SegmentDeque<T>::insert_at(const T& item, int index) {
    SegmentDeque<T>* inst = Instance();

    if (index < 0 || index > inst->count) {
        throw std::out_of_range("Index out of range");
    }

    inst->push_back(get(count - 1)); // Добавляем пустой элемент в конец через T() либо копируем последний, для своих классов нужен конструктор по умолчанию

    int curr_block_index, offset;
    resolve_index(index, curr_block_index, offset);
    
    T* curr_block = inst->block_map->get(curr_block_index);

    for (int i = count - 1; i > index; i--) {
        int write_block, write_offset; // Куда пишем
        resolve_index(i, write_block, write_offset);

        int read_block, read_offset; // Откуда пишем
        resolve_index(i - 1, read_block, read_offset);

        T* curr_write_block = inst->block_map->get(write_block);
        T* curr_read_block = inst->block_map->get(read_block);

        curr_write_block[write_offset] = curr_read_block[read_offset];                              
    }

    curr_block[offset] = item;

    return inst;
}
