#include "sequence.h"
#include "dynamic_array.h"
#include "linked_list.h"
#include <stdexcept>

template <class T>
Sequence<T>* Sequence<T>::slice(int index, int count, const Sequence<T>* replace_seq) {
    int len = get_count();

    if (index < 0) {
        index = len + index;
    }

    if (index < 0 || index >= len) {
        throw std::out_of_range("Slice index out of range");
    }

    if (count > len - index) {
        count = len - index;
    }

    auto* sliced_seq = new MutableArraySequence<T>();

    for (int i = 0; i < index; i++) {
        sliced_seq->append(get(i));
    }

    if (replace_seq != nullptr) {
        for (int i = 0; i < replace_seq->get_count(); i++) {
            sliced_seq->append(replace_seq->get(i));
        }
    }

    for (int i = index + count; i < len; i++) {
        sliced_seq->append(get(i));
    }

    return sliced_seq;
}

template <class T>
ArraySequence<T>::ArraySequence() : count(0) {
    array = new DynamicArray<T>();
}

template <class T>
ArraySequence<T>::ArraySequence(const T* items, int count) : count(count) {
    array = new DynamicArray<T>(items, count);
}

template <class T>
ArraySequence<T>::ArraySequence(const DynamicArray<T>& other) : count(other.get_size()) {
    array = new DynamicArray<T>(other);
}

template <class T>
ArraySequence<T>::ArraySequence(const ArraySequence<T>& other) : count(other.count) {
    array = new DynamicArray<T>(*other.array);
}

template <class T>
const T& ArraySequence<T>::get_first() const {
    if (count == 0) throw std::out_of_range("Sequence is empty");

    return array->get(0);
}

template <class T>
const T& ArraySequence<T>::get_last() const {
    if (count == 0) throw std::out_of_range("Sequence is empty");

    int last_index = get_count() - 1;
    return array->get(last_index);
}

template <class T>
const T& ArraySequence<T>::get(int index) const {
    if (index < 0 || index >= count) throw std::out_of_range("Sequence is empty");

    return array->get(index);
}

template <class T>
Option<T> ArraySequence<T>::try_get_first() const {
    if (count == 0) return Option<T>::None();

    return Option<T>::Some(array->get(0));
}

template <class T>
Option<T> ArraySequence<T>::try_get_last() const {
    if (count == 0) return Option<T>::None();

    int last_index = get_count() - 1;
    return Option<T>::Some(array->get(last_index));
}

template <class T>
Option<T> ArraySequence<T>::try_get(int index) const {
    if (index < 0 || index >= count) return Option<T>::None();

    return Option<T>::Some(array->get(index));
}

template <class T>
int ArraySequence<T>::get_count() const {
    return count;
}

template <class T>
Sequence<T>* ArraySequence<T>::get_sub_sequence(int start, int end) {
    if (count == 0) throw std::out_of_range("Sequence is empty\n");

    if (start < 0 || end < 0 || start >= count || end >= count || start > end) throw std::out_of_range("Index out of range");

    int new_count = end - start + 1;
    T* items = new T[new_count];

    for (int i = 0; i < new_count; i++) {
        items[i] = array->get(start + i);
    }

    ArraySequence<T>* sub_array = EmptyClone();
    delete sub_array->array;
    sub_array->array = new DynamicArray<T>(items, new_count);
    sub_array->count = new_count;

    delete[] items;

    return sub_array;
}

template <class T>
Sequence<T>* ArraySequence<T>::append(const T& item) {
    ArraySequence<T>* inst = Instance();

    int size = inst->array->get_size();

    if (inst->count >= size) {
        int new_size = (size == 0) ? 4 : size * 2;
        inst->array->resize(new_size);
    }

    inst->array->set(inst->count, item);
    inst->count++;

    return inst;
}

template <class T>
Sequence<T>* ArraySequence<T>::prepend(const T& item) {
    ArraySequence<T>* inst = Instance();

    int size = inst->array->get_size();

    if (inst->count >= size) {
        int new_size = (size == 0) ? 4 : size * 2;
        inst->array->resize(new_size);
    }

    for (int i = inst->count; i > 0; i--) {
        inst->array->set(i, inst->array->get(i - 1));
    }

    inst->array->set(0, item);
    inst->count++;
    return inst;
}

template <class T>
Sequence<T>* ArraySequence<T>::insert_at(const T& item, int index) {
    ArraySequence<T>* inst = Instance();

    if (index < 0 || index > inst->count) {
        throw std::out_of_range("Index out of range");
    }

    int size = inst->array->get_size();

    if (inst->count >= size) {
        int new_size = (size == 0) ? 4 : size * 2;
        inst->array->resize(new_size);
    }

    for (int i = inst->count; i > index; i--) {
        inst->array->set(i, inst->array->get(i - 1));
    }

    inst->array->set(index, item);
    inst->count++;
    return inst;
}

template <class T>
Sequence<T>* ArraySequence<T>::concat(const Sequence<T>* other) {
    if (other == nullptr) {
        throw std::invalid_argument("Cannot concat with nullptr\n");
    }

    ArraySequence<T>* concat_arr = EmptyClone();
    concat_arr->array->resize(count + other->get_count());
    concat_arr->count = count + other->get_count();

    for (int this_index = 0; this_index < count; this_index++) {
        concat_arr->array->set(this_index, get(this_index));
    }

    for (int other_index = 0; other_index < other->get_count(); other_index++) {
        concat_arr->array->set(count + other_index, other->get(other_index));
    }

    return concat_arr;
}

template <class T>
Sequence<T>* ArraySequence<T>::map(T (*func)(const T& elem)) {
    ArraySequence<T>* mapped_arr = EmptyClone();
    mapped_arr->array->resize(count);
    mapped_arr->count = count;

    for (int index = 0; index < count; index++) {
        mapped_arr->array->set(index, func(get(index)));
    }

    return mapped_arr;
}

template <class T>
Sequence<T>* ArraySequence<T>::where(bool (*predicate)(const T& elem)) {
    ArraySequence<T>* where_arr = EmptyClone();
    where_arr->array->resize(count);

    int new_count = 0;
    for (int i = 0; i < count; i++) {
        T current_elem = get(i);

        if (predicate(current_elem)) {
            where_arr->array->set(new_count, current_elem);
            new_count++;
        }        
    }

    where_arr->array->resize(new_count);
    where_arr->count = new_count;

    return where_arr;
}

template <class T>
T ArraySequence<T>::reduce(T (*func)(const T& first_elem, const T& second_elem), const T& initial_elem) {
    T reduced_elem = initial_elem;

    for (int i = 0; i < get_count(); i++) {
        reduced_elem = func(get(i), reduced_elem);
    }

    return reduced_elem;
}

// template <class T>
// Sequence<Sequence<T>*>* ArraySequence<T>::split(bool (*predicate)(const T&)) {
//     auto* sequences = new MutableArraySequence<Sequence<T>*>();
//     Sequence<T>* current_seq = EmptyClone();

//     for (int i = 0; i < count; i++) {
//         T current_elem = get(i);

//         if (predicate(current_elem)) {
//             sequences->append(current_seq);
//             current_seq = EmptyClone();
//         } else {
//             current_seq->append(current_elem);
//         }
//     }
//     sequences->append(current_seq);

//     return sequences;
// }

// ==================================================

template <class T>
ListSequence<T>::ListSequence() {
    list = new LinkedList<T>();
}

template <class T>
ListSequence<T>::ListSequence(const T* items, int count) {
    list = new LinkedList<T>(items, count);
}

template <class T>
ListSequence<T>::ListSequence(const LinkedList<T>& other) {
    list = new LinkedList<T>(other);
}

template <class T>
ListSequence<T>::ListSequence(const ListSequence<T>& other) {
    list = new LinkedList<T>(*other.list);
}

template <class T>
const T& ListSequence<T>::get_first() const {
    if (list->get_length() == 0) throw std::out_of_range("Sequence is empty");
    
    return list->get_first();
}

template <class T>
const T& ListSequence<T>::get_last() const {
    if (list->get_length() == 0) throw std::out_of_range("Sequence is empty");

    return list->get_last();
}

template <class T>
const T& ListSequence<T>::get(int index) const {
    if (index < 0 || index >= list->get_length()) throw std::out_of_range("Sequence is empty");

    return list->get(index);
}

template <class T>
Option<T> ListSequence<T>::try_get_first() const {
    if (list->get_length() == 0) return Option<T>::None();

    return Option<T>::Some(list->get_first());
}

template <class T>
Option<T> ListSequence<T>::try_get_last() const {
    if (list->get_length() == 0) return Option<T>::None();

    return Option<T>::Some(list->get_last());
}

template <class T>
Option<T> ListSequence<T>::try_get(int index) const {
    if (index < 0 || index >= list->get_length()) return Option<T>::None();

    return Option<T>::Some(list->get(index));
}

template <class T>
int ListSequence<T>::get_count() const {
    return list->get_length();
}

template <class T>
Sequence<T>* ListSequence<T>::get_sub_sequence(int start, int end) {
    if (start < 0 || end < 0 || start >= get_count() || end >= get_count() || start > end) {
        throw std::out_of_range("Index out of range");
    }

    ListSequence<T>* sub_list = EmptyClone();
    LinkedList<T>* sub_list_inside = this->list->get_sub_list(start, end);

    delete sub_list->list;
    sub_list->list = sub_list_inside;

    return sub_list;
}

template <class T>
Sequence<T>* ListSequence<T>::append(const T& item) {
    ListSequence<T>* inst = Instance();

    inst->list->append(item);
    return inst;
}

template <class T>
Sequence<T>* ListSequence<T>::prepend(const T& item) {
    ListSequence<T>* inst = Instance();

    inst->list->prepend(item);
    return inst;
}

template <class T>
Sequence<T>* ListSequence<T>::insert_at(const T& item, int index) {
    ListSequence<T>* inst = Instance();

    inst->list->insert_at(item, index);
    return inst;
}

template <class T>
Sequence<T>* ListSequence<T>::concat(const Sequence<T>* other) {
    if (other == nullptr) {
        throw std::invalid_argument("Cannot concat with nullptr\n");
    }
    
    ListSequence<T>* concat_list = EmptyClone();

    IEnumerator<T>* this_iter = get_enumerator();
    while(this_iter->move_next()) {
        concat_list->list->append(this_iter->get_current());
    }
    delete this_iter;

    IEnumerator<T>* other_iter = other->get_enumerator();
    while(other_iter->move_next()) {
        concat_list->list->append(other_iter->get_current());
    }
    delete other_iter;

    return concat_list;
}

template <class T>
Sequence<T>* ListSequence<T>::map(T (*func)(const T& elem)) {
    ListSequence<T>* mapped_list = EmptyClone();

    IEnumerator<T>* iter = get_enumerator();
    while (iter->move_next()) {
        T current_mapped_elem = func(iter->get_current());
        mapped_list->list->append(current_mapped_elem);
    }
    delete iter;

    return mapped_list;
}

template <class T>
Sequence<T>* ListSequence<T>::where(bool (*predicate)(const T& elem)) {
    ListSequence<T>* where_list = EmptyClone();

    IEnumerator<T>* iter = get_enumerator();
    while(iter->move_next()) {
        T current_elem = iter->get_current();

        if (predicate(current_elem)) {
            where_list->list->append(current_elem);
        }
    }
    delete iter;

    return where_list;
}

template <class T>
T ListSequence<T>::reduce(T (*func)(const T& first_elem, const T& second_elem), const T& initial_elem) {
    T reduced_elem = initial_elem;

    IEnumerator<T>* iter = get_enumerator();
    while (iter->move_next()) {
        T current_elem = iter->get_current();
        reduced_elem = func(current_elem, reduced_elem);
    }
    delete iter;

    return reduced_elem;
}

// template <class T>
// Sequence<Sequence<T>*>* ListSequence<T>::split(bool (*predicate)(const T&)) {
//     auto* sequences = new MutableListSequence<Sequence<T>*>();
//     Sequence<T>* current_seq = EmptyClone();

//     IEnumerator<T>* iter = get_enumerator();
//     while (iter->move_next()) {
//         T current_elem = iter->get_current();

//         if (predicate(current_elem)) {
//             sequences->append(current_seq);
//             current_seq = EmptyClone();
//         } else {
//             current_seq->append(current_elem)
//         }
//     }
//     delete iter;

//     sequences->append(current_seq);

//     return sequences;
// }
