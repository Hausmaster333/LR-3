#include "segment_deque.h"
#include <stdexcept>

template <class T>
void SegmentDeque<T>::sys_push_front(const T& item) {
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
T SegmentDeque<T>::sys_pop_back() {
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
T SegmentDeque<T>::sys_pop_front() {;
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
void SegmentDeque<T>::sys_push_back(const T& item) {
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
T* SegmentDeque<T>::allocate_block() {
    return new T[segment_size];
}   

template <class T>
void SegmentDeque<T>::grow_map_front() {    
    int old_capacity = map_capacity;
    map_capacity = map_capacity * 2;
    block_map->resize(map_capacity);

    for (int index = map_capacity - 1; index >= old_capacity; index--) {
        T* old_block = block_map->get(index - old_capacity);
        block_map->set(index, old_block);
    }

    for (int index = old_capacity - 1; index >= 0; index--) {
        block_map->set(index, nullptr);
    }

    front_block += old_capacity;
    back_block += old_capacity;
}

template <class T>
void SegmentDeque<T>::grow_map_back() {
    int old_capacity = map_capacity;
    map_capacity = map_capacity * 2;
    block_map->resize(map_capacity);

    for (int index = old_capacity; index < map_capacity; index++) {
        block_map->set(index, nullptr);
    }
}

template <class T>
void SegmentDeque<T>::resolve_index(int index, int* block, int* offset) const { 
    int total_offset = front_index + index; // Самый первый индекс + смещение по индексу

    *block = front_block + total_offset / segment_size;
    *offset = total_offset % segment_size;
}

template <class T>
SegmentDeque<T>::SegmentDeque() {
    block_map = new DynamicArray<T*>(4);

    for (int index = 0; index < 4; index++) {
        block_map->set(index, nullptr);
    }

    T* init_block = allocate_block();
    block_map->set(1, init_block); // Ставим начальный блок по центру

    front_block = 1;
    back_block = 1;
    front_index = 4; // Ставим в первом сегменте посередине первый элемент
    back_index = 4;
    count = 0;
    map_capacity = 4;
}

template <class T>
SegmentDeque<T>::SegmentDeque(const T* items, int count) : SegmentDeque() {
    for (int index = 0; index < count; index++) {
        sys_push_back(items[index]);
    }
}

template <class T>
SegmentDeque<T>::SegmentDeque(const SegmentDeque<T>& other) : SegmentDeque() {
    for (int index = 0; index < other.get_count(); index++) {
        T other_elem = other.get(index);

        sys_push_back(other_elem);
    }
}

template <class T>
SegmentDeque<T>* SegmentDeque<T>::push_front(const T& item) {
    SegmentDeque<T>* inst = Instance();

    inst->sys_push_front(item);

    return inst;
}

template <class T>
SegmentDeque<T>* SegmentDeque<T>::push_back(const T& item) {
    SegmentDeque<T>* inst = Instance();

    inst->sys_push_back(item);

    return inst;
}

template <class T>
SegmentDeque<T>* SegmentDeque<T>::pop_front(T* result) {
    SegmentDeque<T>* inst = Instance();

    *result = inst->sys_pop_front();

    return inst;
}

template <class T>
SegmentDeque<T>* SegmentDeque<T>::pop_back(T* result) {
    SegmentDeque<T>* inst = Instance();

    *result = inst->sys_pop_back();

    return inst;
}

template <class T>
SegmentDeque<T>& SegmentDeque<T>::operator=(const SegmentDeque<T>& other) {
    if (this == &other) return *this;

    for (int index = 0; index < this->map_capacity; index++) {
        delete[] block_map->get(index);
    }

    delete block_map;

    block_map = new DynamicArray<T*>(4);

    for (int index = 0; index < 4; index++) {
        block_map->set(index, nullptr);
    }

    T* init_block = allocate_block();
    block_map->set(1, init_block); // Ставим начальный блок по центру

    front_block = 1;
    back_block = 1;
    front_index = 4; // Ставим в первом сегменте посередине первый элемент
    back_index = 4;
    count = 0;
    map_capacity = 4;

    for (int index = 0; index < other.count; index++) {
        sys_push_back(other.get(index));
    }

    return *this; // Возвращаем сам объект
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
    resolve_index(count - 1, &curr_block_index, &offset); // Получаем положение последнего элементам дека

    T* curr_block = block_map->get(curr_block_index);

    return curr_block[offset];
}

template <class T>
const T& SegmentDeque<T>::get(int index) const {
    if (index < 0 || index >= count) throw std::out_of_range("Index out of range");

    int block_index, offset;
    resolve_index(index, &block_index, &offset);

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
    resolve_index(count - 1, &curr_block_index, &offset);

    T* curr_block = block_map->get(curr_block_index);

    return Option<T>::Some(curr_block[offset]);
}

template <class T>
Option<T> SegmentDeque<T>::try_get(int index) const {
    if (index < 0 || index >= count) return Option<T>::None();

    int curr_block_index, offset;
    resolve_index(index, &curr_block_index, &offset);

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
    
    for (int index = start; index <= end; index++) {
        sub->push_back(get(index));
    }

    return sub;
}

template <class T>
Sequence<T>* SegmentDeque<T>::append(const T& item) {
    SegmentDeque<T>* inst = Instance();

    inst->sys_push_back(item);

    return inst;
}

template <class T>
Sequence<T>* SegmentDeque<T>::prepend(const T& item) {
    SegmentDeque<T>* inst = Instance();

    inst->sys_push_front(item);

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
    resolve_index(index, &curr_block_index, &offset);
    
    T* curr_block = inst->block_map->get(curr_block_index);

    for (int index = count - 1; index > index; index--) {
        int write_block, write_offset; // Куда пишем
        resolve_index(index, &write_block, &write_offset);

        int read_block, read_offset; // Откуда пишем
        resolve_index(index - 1, &read_block, &read_offset);

        T* curr_write_block = inst->block_map->get(write_block);
        T* curr_read_block = inst->block_map->get(read_block);

        curr_write_block[write_offset] = curr_read_block[read_offset];                              
    }

    curr_block[offset] = item;

    return inst;
}

template <class T>
Sequence<T>* SegmentDeque<T>::concat(const Sequence<T>* other) {
    if (other == nullptr) {
        throw std::invalid_argument("Cannot concat with nullptr");
    }

    SegmentDeque<T>* concat_deque = EmptyClone();

    for (int index = 0; index < count; index++) {
        T curr_elem = get(index);

        concat_deque->sys_push_back(curr_elem);
    }

    for (int index = 0; index < other->get_count(); index++) {
        T curr_elem = other->get(index);

        concat_deque->sys_push_back(curr_elem);
    }

    return concat_deque;
}

template <class T>
Sequence<T>* SegmentDeque<T>::map(T (*func)(const T& elem)) {
    SegmentDeque<T>* map_deque = EmptyClone();

    for (int index = 0; index < count; index++) {
        T curr_elem = get(index);

        map_deque->sys_push_back(func(curr_elem));
    }

    return map_deque;
}

template <class T>
Sequence<T>* SegmentDeque<T>::where(bool (*predicate)(const T& elem)) {
    SegmentDeque<T>* where_deque = EmptyClone();

    for (int index = 0; index < count; index++) {
        T curr_elem = get(index);

        if (predicate(curr_elem)) {
            where_deque->sys_push_back(curr_elem);
        }
    }

    return where_deque;
}

template <class T>
T SegmentDeque<T>::reduce(T (*func)(const T& first_elem, const T& second_elem), const T& initial_elem) {
    T reduced_elem = initial_elem;

    for (int index = 0; index < count; index++) {
        reduced_elem = func(get(index), reduced_elem);
    }

    return reduced_elem;
}

template <class T>
SegmentDeque<T>::~SegmentDeque() {
    for (int index = 0; index < map_capacity; index++) {
        delete[] block_map->get(index);
    }

    delete block_map;
}

