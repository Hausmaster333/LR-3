#include "utils.h"
#include "sequence.h"
#include "bit_sequence.h"
#include <gtest/gtest.h>

int square(const int& x) {
    return x * x;
}

bool is_positive(const int& x) {
    return x > 0;
}

int sum(const int& a, const int& b) {
    return a + b;
}

bool is_zero(const int& x) {
    return x == 0;
}

bool is_zero_bit(const Bit& b) {
    return !b.get();
}

TEST(DynamicArrayTest, Constructors) {
    DynamicArray<int> empty_array; // DynamicArray()
    EXPECT_EQ(empty_array.get_size(), 0);

    DynamicArray<int> array(5); // DynamicArray(int size)
    EXPECT_EQ(array.get_size(), 5);

    int items[] = {1, 2, 3};
    DynamicArray<int> arr_from_items(items, 3); // DynamicArray(const T* items, int count)
    EXPECT_EQ(arr_from_items.get(0), 1);
    EXPECT_EQ(arr_from_items.get(1), 2);
    EXPECT_EQ(arr_from_items.get(2), 3);
    EXPECT_EQ(arr_from_items.get_size(), 3);

    DynamicArray<int> other(3);
    other.set(0, 1);
    other.set(1, 2);
    DynamicArray<int> copy_from_other(other); // DynamicArray(const DynamicArray<T>& other)
    EXPECT_EQ(copy_from_other.get(0), 1);
    EXPECT_EQ(copy_from_other.get(1), 2);
    EXPECT_EQ(copy_from_other.get_size(), 3);
}

TEST(DynamicArrayTest, GetAndSet) {
    DynamicArray<int> arr(5);
    arr.set(0, 42); // set(index, value)
    EXPECT_EQ(arr.get(0), 42); // get(index)
    EXPECT_EQ(arr.get_size(), 5); // get_size()

    arr.resize(10); // resize()
    EXPECT_EQ(arr.get_size(), 10);
}

TEST(DynamicArrayTest, OutOfRange) {
    DynamicArray<int> arr(5);
    EXPECT_THROW(arr.get(-1), std::out_of_range); 
    EXPECT_THROW(arr.get(10), std::out_of_range);
}

// =========================================================

TEST(LinkedListTest, Constructors) {
    LinkedList<int> empty_list; // LinkedList()
    EXPECT_EQ(empty_list.get_length(), 0);

    int items[] = {1, 5, 4};
    LinkedList<int> list_from_items(items, 3); // LinkedList(const T* items, int count)
    EXPECT_EQ(list_from_items.get_length(), 3);
    EXPECT_EQ(list_from_items.get_first(), 1);
    EXPECT_EQ(list_from_items.get_last(), 4);
    EXPECT_EQ(list_from_items.get(1), 5);

    LinkedList<int> list_from_other(list_from_items); // LinkedList(const LinkedList<T>& other)
    EXPECT_EQ(list_from_other.get_length(), 3);
    EXPECT_EQ(list_from_other.get_first(), 1);
    EXPECT_EQ(list_from_other.get_last(), 4);
    EXPECT_EQ(list_from_items.get(1), 5);
}

TEST(LinkedListTest, GetAndSet) {
    LinkedList<int> list;
    list.append(4); // append()
    list.append(5);
    list.prepend(8); // prepend()
    list.prepend(-4);
    list.insert_at(8, 3); // insert_at()

    EXPECT_EQ(list.get_length(), 5); // get_length()
    EXPECT_EQ(list.get_first(), -4); // get_first()
    EXPECT_EQ(list.get_last(), 5); // get_last()
    EXPECT_EQ(list.get(3), 8); // get()
}

TEST(LinkedListTest, OutOfRange) {
    LinkedList<int> list;
    EXPECT_THROW(list.get_first(), std::out_of_range);
    EXPECT_THROW(list.get(0), std::out_of_range);
}

TEST(LinkedListTest, Concat) {
    LinkedList<int> first_list;
    first_list.append(4);
    first_list.append(10);

    LinkedList<int> second_list;
    second_list.append(-3);
    second_list.append(7);

    LinkedList<int>* concat_list = first_list.concat(&second_list); // concat
    EXPECT_EQ(concat_list->get_length(), 4);
    EXPECT_EQ(concat_list->get_first(), 4);
    EXPECT_EQ(concat_list->get_last(), 7);
    
    delete concat_list;
}

TEST(LinkedListTest, GetSubList) {
    int items[] = {1, 2, 3, 4, 5};
    LinkedList<int> list(items, 5);

    LinkedList<int>* sub_list = list.get_sub_list(1, 3); // get_sub_list
    EXPECT_EQ(sub_list->get_length(), 3);    
    EXPECT_EQ(sub_list->get_first(), 2);
    EXPECT_EQ(sub_list->get_last(), 4);

    delete sub_list;
}

TEST(LinkedListTest, Iterator) {
    int items[] = {1, 2, 3};
    LinkedList<int> list(items, 3);

    int sum = 0;
    IEnumerator<int>* iter = list.get_enumerator();
    while(iter->move_next()) {
        sum += iter->get_current();
    }
    delete iter;

    EXPECT_EQ(sum, 6);
}

// =======================

TEST(MutableArraySequenceTest, Constructors) { // Тестируем на Mutable варианте
    MutableArraySequence<int> empty_arr; // ArraySequence();
    EXPECT_EQ(empty_arr.get_count(), 0);
    
    int items[] = {1, 2, 3, 4};
    MutableArraySequence<int> arr(items, 4); // ArraySequence(const T* items, int count)
    EXPECT_EQ(arr.get_count(), 4);
    EXPECT_EQ(arr.get_first(), 1);
    EXPECT_EQ(arr.get_last(), 4);
    EXPECT_EQ(arr.get(2), 3);

    DynamicArray<int> dyn_arr(items, 4);
    MutableArraySequence<int> arr_from_dyn_arr(dyn_arr); // ArraySequence(const DynamicArray<T>& other)
    EXPECT_EQ(arr_from_dyn_arr.get_count(), 4);
    EXPECT_EQ(arr_from_dyn_arr.get_first(), 1);
    EXPECT_EQ(arr_from_dyn_arr.get_last(), 4);
    EXPECT_EQ(arr_from_dyn_arr.get(2), 3);
    
    MutableArraySequence<int> arr_from_other(arr); // ArraySequence(const ArraySequence<T>& other)
    EXPECT_EQ(arr_from_other.get_count(), 4);
    EXPECT_EQ(arr_from_other.get_first(), 1);
    EXPECT_EQ(arr_from_other.get_last(), 4);
    EXPECT_EQ(arr_from_other.get(2), 3);
}

TEST(MutableArraySequenceTest, GetAndSet) {
    MutableArraySequence<int> arr;
    arr.append(5); // append()
    arr.append(10);
    arr.prepend(6); // prepend()
    arr.prepend(-10);
    arr.insert_at(0, 3); // insert_at()

    EXPECT_EQ(arr.get_count(), 5); // get_count()
    EXPECT_EQ(arr.get_first(), -10); // get_first()
    EXPECT_EQ(arr.get_last(), 10); // get_last()
    EXPECT_EQ(arr.get(3), 0); // get()
}

TEST(MutableArraySequenceTest, GetSubSequence) {
    int items[] = {1, 2, 3, 4, 5, 6, 7};
    MutableArraySequence<int> arr(items, 7);

    Sequence<int>* sub_arr = arr.get_sub_sequence(2, 5); // get_sub_sequence
    EXPECT_EQ(sub_arr->get_count(), 4);
    EXPECT_EQ(sub_arr->get_first(), 3);
    EXPECT_EQ(sub_arr->get_last(), 6);
    EXPECT_EQ(sub_arr->get(2), 5);

    delete sub_arr;
}

TEST(MutableArraySequenceTest, Concat) {
    int pos_items[] = {0, 2, 3, 4, 5};
    int neg_items[] = {-6, -7, -8, -9};

    MutableArraySequence<int> first_arr(pos_items, 5);
    MutableArraySequence<int> second_arr(neg_items, 4);

    Sequence<int>* concat_arr = first_arr.concat(&second_arr); // concat
    EXPECT_EQ(concat_arr->get_count(), 9);
    EXPECT_EQ(concat_arr->get_first(), 0);
    EXPECT_EQ(concat_arr->get_last(), -9);
    EXPECT_EQ(concat_arr->get(6), -7);

    delete concat_arr;
}

TEST(MutableArraySequenceTest, Map) {
    int items[] = {1, -10, 3, 4, -5};
    MutableArraySequence<int> arr(items, 5);

    Sequence<int>* mapped_arr = arr.map(square);
    EXPECT_EQ(mapped_arr->get_first(), 1);
    EXPECT_EQ(mapped_arr->get(1), 100);
    EXPECT_EQ(mapped_arr->get(2), 9);
    EXPECT_EQ(mapped_arr->get_last(), 25);

    delete mapped_arr;
}

TEST(MutableArraySequenceTest, Where) {
    int items[] = {1, -10, 3, 4, -5};
    MutableArraySequence<int> arr(items, 5);

    Sequence<int>* where_arr = arr.where(is_positive);
    EXPECT_EQ(where_arr->get_first(), 1);
    EXPECT_EQ(where_arr->get(1), 3);
    EXPECT_EQ(where_arr->get_last(), 4);

    delete where_arr;
}

TEST(MutableArraySequenceTest, Reduce) {
    int items[] = {1, 2, 3, 4, 5};
    MutableArraySequence<int> arr(items, 5);

    int reduced = arr.reduce(sum, 0);
    EXPECT_EQ(reduced, 15);
}

// =======================

TEST(ImmutableArraySequenceTest, Constructors) { // Тестируем на Immutable варианте
    ImmutableArraySequence<int> empty_arr; // ArraySequence();
    EXPECT_EQ(empty_arr.get_count(), 0);
    
    int items[] = {1, 2, 3, 4};
    ImmutableArraySequence<int> arr(items, 4); // ArraySequence(const T* items, int count)
    EXPECT_EQ(arr.get_count(), 4);
    EXPECT_EQ(arr.get_first(), 1);
    EXPECT_EQ(arr.get_last(), 4);
    EXPECT_EQ(arr.get(2), 3);

    DynamicArray<int> dyn_arr(items, 4);
    ImmutableArraySequence<int> arr_from_dyn_arr(dyn_arr); // ArraySequence(const DynamicArray<T>& other)
    EXPECT_EQ(arr_from_dyn_arr.get_count(), 4);
    EXPECT_EQ(arr_from_dyn_arr.get_first(), 1);
    EXPECT_EQ(arr_from_dyn_arr.get_last(), 4);
    EXPECT_EQ(arr_from_dyn_arr.get(2), 3);
    
    ImmutableArraySequence<int> arr_from_other(arr); // ArraySequence(const ArraySequence<T>& other)
    EXPECT_EQ(arr_from_other.get_count(), 4);
    EXPECT_EQ(arr_from_other.get_first(), 1);
    EXPECT_EQ(arr_from_other.get_last(), 4);
    EXPECT_EQ(arr_from_other.get(2), 3);
}

TEST(ImmutableArraySequenceTest, GetAndSet) {
    int items[] = {1, 2, 3};
    ImmutableArraySequence<int> arr_1(items, 3);

    Sequence<int>* arr_2 = arr_1.append(4); // append()
    EXPECT_EQ(arr_1.get_count(), 3); // Размер arr не меняется
    EXPECT_EQ(arr_2->get_count(), 4); // Добавляется в копию
    EXPECT_EQ(arr_2->get_last(), 4);

    Sequence<int>* arr_3 = arr_1.prepend(10); // prepend()
    EXPECT_EQ(arr_1.get_count(), 3);
    EXPECT_EQ(arr_3->get_count(), 4);
    EXPECT_EQ(arr_3->get_first(), 10);
    
    Sequence<int>* arr_4 = arr_1.insert_at(0, 3);
    EXPECT_EQ(arr_1.get_count(), 3);
    EXPECT_EQ(arr_4->get_count(), 4);
    EXPECT_EQ(arr_4->get(3), 0);
    
    delete arr_2;
    delete arr_3;
    delete arr_4;
}

TEST(ImmutableArraySequenceTest, GetSubSequence) {
    int items[] = {1, 2, 3, 4, 5, 6, 7};
    ImmutableArraySequence<int> arr(items, 7);

    Sequence<int>* sub_arr = arr.get_sub_sequence(2, 5); // get_sub_sequence
    EXPECT_EQ(sub_arr->get_count(), 4);
    EXPECT_EQ(sub_arr->get_first(), 3);
    EXPECT_EQ(sub_arr->get_last(), 6);
    EXPECT_EQ(sub_arr->get(2), 5);

    delete sub_arr;
}

TEST(ImmutableArraySequenceTest, Concat) {
    int pos_items[] = {0, 2, 3, 4, 5};
    int neg_items[] = {-6, -7, -8, -9};

    ImmutableArraySequence<int> first_arr(pos_items, 5);
    ImmutableArraySequence<int> second_arr(neg_items, 4);

    Sequence<int>* concat_arr = first_arr.concat(&second_arr); // concat
    EXPECT_EQ(concat_arr->get_count(), 9);
    EXPECT_EQ(concat_arr->get_first(), 0);
    EXPECT_EQ(concat_arr->get_last(), -9);
    EXPECT_EQ(concat_arr->get(6), -7);

    delete concat_arr;
}

TEST(ImmutableArraySequenceTest, Map) {
    int items[] = {1, -10, 3, 4, -5};
    ImmutableArraySequence<int> arr(items, 5);

    Sequence<int>* mapped_arr = arr.map(square);
    EXPECT_EQ(mapped_arr->get_first(), 1);
    EXPECT_EQ(mapped_arr->get(1), 100);
    EXPECT_EQ(mapped_arr->get(2), 9);
    EXPECT_EQ(mapped_arr->get_last(), 25);

    delete mapped_arr;
}

TEST(ImmutableArraySequenceTest, Where) {
    int items[] = {1, -10, 3, 4, -5};
    ImmutableArraySequence<int> arr(items, 5);

    Sequence<int>* where_arr = arr.where(is_positive);
    EXPECT_EQ(where_arr->get_first(), 1);
    EXPECT_EQ(where_arr->get(1), 3);
    EXPECT_EQ(where_arr->get_last(), 4);

    delete where_arr;
}

TEST(ImmutableArraySequenceTest, Reduce) {
    int items[] = {1, 2, 3, 4, 5};
    ImmutableArraySequence<int> arr(items, 5);

    int reduced = arr.reduce(sum, 0);
    EXPECT_EQ(reduced, 15);
}

// =======================

TEST(MutableListSequenceTest, Constructors) { // Тестируем на Mutable варианте
    MutableListSequence<int> empty_list; // ListSequence()
    EXPECT_EQ(empty_list.get_count(), 0);
    
    int items[] = {1, 2, 3, 4};
    MutableListSequence<int> list(items, 4); // ListSequence(const T* items, int count)
    EXPECT_EQ(list.get_count(), 4);
    EXPECT_EQ(list.get_first(), 1);
    EXPECT_EQ(list.get_last(), 4);
    EXPECT_EQ(list.get(2), 3);

    LinkedList<int> link_list(items, 4);
    MutableListSequence<int> list_from_link_list(link_list); // ListSequence(const LinkedList<T>& other)
    EXPECT_EQ(list_from_link_list.get_count(), 4);
    EXPECT_EQ(list_from_link_list.get_first(), 1);
    EXPECT_EQ(list_from_link_list.get_last(), 4);
    EXPECT_EQ(list_from_link_list.get(2), 3);
    
    MutableListSequence<int> list_from_other(list_from_link_list); // ListSequence(const ArraySequence<T>& other)
    EXPECT_EQ(list_from_other.get_count(), 4);
    EXPECT_EQ(list_from_other.get_first(), 1);
    EXPECT_EQ(list_from_other.get_last(), 4);
    EXPECT_EQ(list_from_other.get(2), 3);
}

TEST(MutableListSequenceTest, GetAndSet) {
    MutableListSequence<int> list;
    list.append(5); // append()
    list.append(10);
    list.prepend(6); // prepend()
    list.prepend(-10);
    list.insert_at(0, 3); // insert_at()

    EXPECT_EQ(list.get_count(), 5); // get_count()
    EXPECT_EQ(list.get_first(), -10); // get_first()
    EXPECT_EQ(list.get_last(), 10); // get_last()
    EXPECT_EQ(list.get(3), 0); // get()
}

TEST(MutableListSequenceTest, GetSubSequence) {
    int items[] = {1, 2, 3, 4, 5, 6, 7};
    MutableListSequence<int> list(items, 7);

    Sequence<int>* sub_list = list.get_sub_sequence(2, 5); // get_sub_sequence
    EXPECT_EQ(sub_list->get_count(), 4);
    EXPECT_EQ(sub_list->get_first(), 3);
    EXPECT_EQ(sub_list->get_last(), 6);
    EXPECT_EQ(sub_list->get(2), 5);

    delete sub_list;
}

TEST(MutableListSequenceTest, Concat) {
    int pos_items[] = {0, 2, 3, 4, 5};
    int neg_items[] = {-6, -7, -8, -9};

    MutableListSequence<int> first_list(pos_items, 5);
    MutableListSequence<int> second_list(neg_items, 4);

    Sequence<int>* concat_list = first_list.concat(&second_list); // concat
    EXPECT_EQ(concat_list->get_count(), 9);
    EXPECT_EQ(concat_list->get_first(), 0);
    EXPECT_EQ(concat_list->get_last(), -9);
    EXPECT_EQ(concat_list->get(6), -7);

    delete concat_list;
}

TEST(MutableListSequenceTest, Map) {
    int items[] = {1, -10, 3, 4, -5};
    MutableListSequence<int> list(items, 5);

    Sequence<int>* mapped_list = list.map(square);
    EXPECT_EQ(mapped_list->get_first(), 1);
    EXPECT_EQ(mapped_list->get(1), 100);
    EXPECT_EQ(mapped_list->get(2), 9);
    EXPECT_EQ(mapped_list->get_last(), 25);

    delete mapped_list;
}

TEST(MutableListSequenceTest, Where) {
    int items[] = {1, -10, 3, 4, -5};
    MutableListSequence<int> list(items, 5);

    Sequence<int>* where_list = list.where(is_positive);
    EXPECT_EQ(where_list->get_first(), 1);
    EXPECT_EQ(where_list->get(1), 3);
    EXPECT_EQ(where_list->get_last(), 4);

    delete where_list;
}

TEST(MutableListSequenceTest, Reduce) {
    int items[] = {1, 2, 3, 4, 5};
    MutableListSequence<int> list(items, 5);

    int reduced = list.reduce(sum, 0);
    EXPECT_EQ(reduced, 15);
}

// =======================

TEST(ImmutableListSequenceTest, Constructors) { // Тестируем на Immutable варианте
    ImmutableListSequence<int> empty_list; // ListSequence();
    EXPECT_EQ(empty_list.get_count(), 0);
    
    int items[] = {1, 2, 3, 4};
    ImmutableListSequence<int> list(items, 4); // ListSequence(const T* items, int count)
    EXPECT_EQ(list.get_count(), 4);
    EXPECT_EQ(list.get_first(), 1);
    EXPECT_EQ(list.get_last(), 4);
    EXPECT_EQ(list.get(2), 3);

    LinkedList<int> link_list(items, 4);
    ImmutableListSequence<int> list_from_link_list(link_list); // ListSequence(const LinkedList<T>& other)
    EXPECT_EQ(list_from_link_list.get_count(), 4);
    EXPECT_EQ(list_from_link_list.get_first(), 1);
    EXPECT_EQ(list_from_link_list.get_last(), 4);
    EXPECT_EQ(list_from_link_list.get(2), 3);
    
    ImmutableListSequence<int> list_from_other(list); // ListSequence(const ListSequence<T>& other)
    EXPECT_EQ(list_from_other.get_count(), 4);
    EXPECT_EQ(list_from_other.get_first(), 1);
    EXPECT_EQ(list_from_other.get_last(), 4);
    EXPECT_EQ(list_from_other.get(2), 3);
}

TEST(ImmutableListSequenceTest, GetAndSet) {
    int items[] = {1, 2, 3};
    ImmutableListSequence<int> list_1(items, 3);

    Sequence<int>* list_2 = list_1.append(4); // append()
    EXPECT_EQ(list_1.get_count(), 3); // Размер arr не меняется
    EXPECT_EQ(list_2->get_count(), 4); // Добавляется в копию
    EXPECT_EQ(list_2->get_last(), 4);

    Sequence<int>* list_3 = list_1.prepend(10); // prepend()
    EXPECT_EQ(list_1.get_count(), 3);
    EXPECT_EQ(list_3->get_count(), 4);
    EXPECT_EQ(list_3->get_first(), 10);
    
    Sequence<int>* list_4 = list_1.insert_at(0, 3);
    EXPECT_EQ(list_1.get_count(), 3);
    EXPECT_EQ(list_4->get_count(), 4);
    EXPECT_EQ(list_4->get(3), 0);
    
    delete list_2;
    delete list_3;
    delete list_4;
}

TEST(ImmutableListSequenceTest, GetSubSequence) {
    int items[] = {1, 2, 3, 4, 5, 6, 7};
    ImmutableListSequence<int> list(items, 7);

    Sequence<int>* sub_list = list.get_sub_sequence(2, 5); // get_sub_sequence
    EXPECT_EQ(sub_list->get_count(), 4);
    EXPECT_EQ(sub_list->get_first(), 3);
    EXPECT_EQ(sub_list->get_last(), 6);
    EXPECT_EQ(sub_list->get(2), 5);

    delete sub_list;
}

TEST(ImmutableListSequenceTest, Concat) {
    int pos_items[] = {0, 2, 3, 4, 5};
    int neg_items[] = {-6, -7, -8, -9};

    ImmutableListSequence<int> first_list(pos_items, 5);
    ImmutableListSequence<int> second_list(neg_items, 4);

    Sequence<int>* concat_list = first_list.concat(&second_list); // concat
    EXPECT_EQ(concat_list->get_count(), 9);
    EXPECT_EQ(concat_list->get_first(), 0);
    EXPECT_EQ(concat_list->get_last(), -9);
    EXPECT_EQ(concat_list->get(6), -7);

    delete concat_list;
}

TEST(ImmutableListSequenceTest, Map) {
    int items[] = {1, -10, 3, 4, -5};
    ImmutableListSequence<int> list(items, 5);

    Sequence<int>* mapped_list = list.map(square);
    EXPECT_EQ(mapped_list->get_first(), 1);
    EXPECT_EQ(mapped_list->get(1), 100);
    EXPECT_EQ(mapped_list->get(2), 9);
    EXPECT_EQ(mapped_list->get_last(), 25);

    delete mapped_list;
}

TEST(ImmutableListSequenceTest, Where) {
    int items[] = {1, -10, 3, 4, -5};
    ImmutableListSequence<int> list(items, 5);

    Sequence<int>* where_list = list.where(is_positive);
    EXPECT_EQ(where_list->get_first(), 1);
    EXPECT_EQ(where_list->get(1), 3);
    EXPECT_EQ(where_list->get_last(), 4);

    delete where_list;
}

TEST(ImmutableListSequenceTest, Reduce) {
    int items[] = {1, 2, 3, 4, 5};
    ImmutableListSequence<int> list(items, 5);

    int reduced = list.reduce(sum, 0);
    EXPECT_EQ(reduced, 15);
}

// =======================

TEST(BitTest, Operations) {
    Bit a(1), b(0), c(42);
    EXPECT_EQ(c.get(), true);
    EXPECT_EQ((a & b).get(), false);
    EXPECT_EQ((a | b).get(), true);
    EXPECT_EQ((a ^ b).get(), true);
    EXPECT_EQ((~a).get(), false);
    EXPECT_EQ((~b).get(), true);
}

TEST(BitSequenceTest, ConstructFromArray) {
    Bit items[] = {Bit(1), Bit(0), Bit(1), Bit(1)};
    BitSequence seq(items, 4);

    EXPECT_EQ(seq.get_count(), 4);
    EXPECT_EQ(seq.get(0), Bit(1));
    EXPECT_EQ(seq.get(1), Bit(0));
    EXPECT_EQ(seq.get(2), Bit(1));
    EXPECT_EQ(seq.get(3), Bit(1));
}

TEST(BitSequenceTest, BitAnd) {
    BitSequence first;
    BitSequence second;

    first.append(Bit(1));
    first.append(Bit(0));
    first.append(Bit(1));

    second.append(Bit(1));
    second.append(Bit(1));
    second.append(Bit(0));

    BitSequence* seq_and = first.bit_and(second);
    EXPECT_EQ(seq_and->get(0), Bit(1));
    EXPECT_EQ(seq_and->get(1), Bit(0));
    EXPECT_EQ(seq_and->get(2), Bit(0));

    delete seq_and;
}

TEST(BitSequenceTest, BitOr) {
    BitSequence first;
    BitSequence second;

    first.append(Bit(1));
    first.append(Bit(0));
    first.append(Bit(0));

    second.append(Bit(1));
    second.append(Bit(1));
    second.append(Bit(0));

    BitSequence* seq_or = first.bit_or(second);
    EXPECT_EQ(seq_or->get(0), Bit(1));
    EXPECT_EQ(seq_or->get(1), Bit(1));
    EXPECT_EQ(seq_or->get(2), Bit(0));

    delete seq_or;
}

TEST(BitSequenceTest, BitXOR) {
    BitSequence first;
    BitSequence second;

    first.append(Bit(1));
    first.append(Bit(0));
    first.append(Bit(0));

    second.append(Bit(1));
    second.append(Bit(1));
    second.append(Bit(0));

    BitSequence* seq_xor = first.bit_xor(second);
    EXPECT_EQ(seq_xor->get(0), Bit(0));
    EXPECT_EQ(seq_xor->get(1), Bit(1));
    EXPECT_EQ(seq_xor->get(2), Bit(0));

    delete seq_xor;
}

TEST(BitSequenceTest, BitNot) {
    BitSequence first;

    first.append(Bit(1));
    first.append(Bit(0));
    first.append(Bit(0));

    BitSequence* seq_not = first.bit_not();
    EXPECT_EQ(seq_not->get(0), Bit(0));
    EXPECT_EQ(seq_not->get(1), Bit(1));
    EXPECT_EQ(seq_not->get(2), Bit(1));

    delete seq_not;
}

// =======================

TEST(OptionTest, Some) {
    Option<int> opt = Option<int>::Some(42);
    EXPECT_TRUE(opt.has_value());
    EXPECT_EQ(opt.get_value(), 42);
}

TEST(OptionTest, None) {
    Option<int> opt = Option<int>::None();
    EXPECT_FALSE(opt.has_value());
    EXPECT_THROW(opt.get_value(), std::runtime_error);
}

TEST(OptionTest, TryGetArraySequence) {
    MutableArraySequence<int> seq;

    EXPECT_FALSE(seq.try_get_first().has_value());
    EXPECT_FALSE(seq.try_get_last().has_value());
    EXPECT_FALSE(seq.try_get(0).has_value());

    seq.append(10);
    seq.append(20);

    Option<int> first = seq.try_get_first();
    EXPECT_TRUE(first.has_value());
    EXPECT_EQ(first.get_value(), 10);

    Option<int> last = seq.try_get_last();
    EXPECT_TRUE(last.has_value());
    EXPECT_EQ(last.get_value(), 20);

    Option<int> elem = seq.try_get(1);
    EXPECT_TRUE(elem.has_value());
    EXPECT_EQ(elem.get_value(), 20);

    EXPECT_FALSE(seq.try_get(-1).has_value());
    EXPECT_FALSE(seq.try_get(5).has_value());
}

TEST(OptionTest, TryGetListSequence) {
    MutableArraySequence<int> seq;

    EXPECT_FALSE(seq.try_get_first().has_value());
    EXPECT_FALSE(seq.try_get_last().has_value());

    seq.append(10);
    seq.append(20);

    EXPECT_TRUE(seq.try_get_first().has_value());
    EXPECT_EQ(seq.try_get_first().get_value(), 10);
    EXPECT_EQ(seq.try_get(1).get_value(), 20);
}

// =======================

TEST(ArrayOfArraysTest, CopyConstructor) {
    DynamicArray<int> row(2);
    row.set(0, 5); row.set(1, 6);

    DynamicArray<DynamicArray<int>> original(1);
    original.set(0, row);

    DynamicArray<DynamicArray<int>> copy(original);
    EXPECT_EQ(copy.get(0).get(0), 5);
    EXPECT_EQ(copy.get(0).get(1), 6);
}

TEST(ArrayOfArraysTest, CreateAndAccess) {
    DynamicArray<int> row_1(3);
    row_1.set(0, 1); 
    row_1.set(1, 2); 
    row_1.set(2, 3);

    DynamicArray<int> row_2(2);
    row_2.set(0, 10); 
    row_2.set(1, 20);

    DynamicArray<DynamicArray<int>> matrix(2);
    matrix.set(0, row_1);
    matrix.set(1, row_2);

    EXPECT_EQ(matrix.get(0).get(0), 1);
    EXPECT_EQ(matrix.get(0).get(2), 3);
    EXPECT_EQ(matrix.get(1).get(0), 10);
    EXPECT_EQ(matrix.get(1).get(1), 20);
    EXPECT_EQ(matrix.get_size(), 2);
}

TEST(ArrayOfArraysTest, IndependentCopy) {
    DynamicArray<int> row(2);
    row.set(0, 1); row.set(1, 2);

    DynamicArray<DynamicArray<int>> original(1);
    original.set(0, row);

    DynamicArray<DynamicArray<int>> copy(original);

    DynamicArray<int> newRow(2); // меняем оригинал
    newRow.set(0, 99); newRow.set(1, 100);
    original.set(0, newRow);
    
    EXPECT_EQ(copy.get(0).get(0), 1); // копия не изменилась
    EXPECT_EQ(copy.get(0).get(1), 2);
}

TEST(ListOfArraysTest, CopyConstructor) {
    LinkedList<DynamicArray<int>> original;

    DynamicArray<int> row(2);
    row.set(0, 7); row.set(1, 8);
    original.append(row);

    LinkedList<DynamicArray<int>> copy(original);
    EXPECT_EQ(copy.get(0).get(0), 7);
    EXPECT_EQ(copy.get(0).get(1), 8);
}

TEST(ListOfArraysTest, AppendAndAccess) {
    LinkedList<DynamicArray<int>> list;

    DynamicArray<int> row_1(3);
    row_1.set(0, 1);
    row_1.set(1, 2);
    row_1.set(2, 3);

    DynamicArray<int> row_2(2);
    row_2.set(0, 10); 
    row_2.set(1, 20);

    list.append(row_1);
    list.append(row_2);

    EXPECT_EQ(list.get_length(), 2);
    EXPECT_EQ(list.get(0).get(0), 1);
    EXPECT_EQ(list.get(0).get(2), 3);
    EXPECT_EQ(list.get(1).get(0), 10);
}

TEST(ListOfArraysTest, PrependAndAccess) {
    LinkedList<DynamicArray<int>> list;

    DynamicArray<int> row_1(2);
    row_1.set(0, 1); 
    row_1.set(1, 2);

    DynamicArray<int> row_2(2);
    row_2.set(0, 3); 
    row_2.set(1, 4);

    list.append(row_1);
    list.prepend(row_2);

    EXPECT_EQ(list.get_first().get(0), 3);
    EXPECT_EQ(list.get_last().get(0), 1);
}

TEST(ListOfAяrraysTest, IndependentCopy) {
    LinkedList<DynamicArray<int>> original;

    DynamicArray<int> row(2);
    row.set(0, 1); row.set(1, 2);
    original.append(row);

    LinkedList<DynamicArray<int>> copy(original);

    DynamicArray<int> new_row(2); // меняем оригинал
    new_row.set(0, 99); new_row.set(1, 100);

    EXPECT_EQ(copy.get(0).get(0), 1); // копия не изменилась
}

// =======================

TEST(SplitTest, MutableArrayBasic) {
    int items[] = {1, 2, 0, 3, 4, 0, 5};
    Sequence<int>* seq = new MutableArraySequence<int>(items, 7);

    auto* split_seq = split(seq, is_zero);

    EXPECT_EQ(split_seq->get_count(), 3);
    EXPECT_EQ(split_seq->get(0)->get_count(), 2);
    EXPECT_EQ(split_seq->get(0)->get(0), 1);
    EXPECT_EQ(split_seq->get(0)->get(1), 2);
    EXPECT_EQ(split_seq->get(1)->get_count(), 2);
    EXPECT_EQ(split_seq->get(1)->get(0), 3);
    EXPECT_EQ(split_seq->get(1)->get(1), 4);
    EXPECT_EQ(split_seq->get(2)->get_count(), 1);
    EXPECT_EQ(split_seq->get(2)->get(0), 5);

    for (int i = 0; i < split_seq->get_count(); i++) {
        delete split_seq->get(i);
    }

    delete split_seq;
    delete seq;
}

TEST(SplitTest, MutableListBasic) {
    int items[] = {1, 2, 0, 3, 4, 0, 5};
    Sequence<int>* seq = new MutableListSequence<int>(items, 7);

    auto* split_seq = split(seq, is_zero);

    EXPECT_EQ(split_seq->get_count(), 3);
    EXPECT_EQ(split_seq->get(0)->get_count(), 2);
    EXPECT_EQ(split_seq->get(1)->get_count(), 2);
    EXPECT_EQ(split_seq->get(2)->get_count(), 1);

    for (int i = 0; i < split_seq->get_count(); i++) {
        delete split_seq->get(i);
    }

    delete split_seq;
    delete seq;
}

TEST(SplitTest, DelimiterAtStart) {
    int items[] = {0, 1, 2, 3};
    Sequence<int>* seq = new MutableArraySequence<int>(items, 4);

    auto* split_seq = split(seq, is_zero);

    EXPECT_EQ(split_seq->get_count(), 2);
    EXPECT_EQ(split_seq->get(0)->get_count(), 0); // пустой фрагмент до разделителя
    EXPECT_EQ(split_seq->get(1)->get_count(), 3);

    for (int i = 0; i < split_seq->get_count(); i++) {
        delete split_seq->get(i);
    }

    delete split_seq;
    delete seq;
}

TEST(SplitTest, DelimiterAtEnd) {
    int items[] = {1, 2, 3, 0};
    Sequence<int>* seq = new MutableArraySequence<int>(items, 4);

    auto* split_seq = split(seq, is_zero);

    EXPECT_EQ(split_seq->get_count(), 2);
    EXPECT_EQ(split_seq->get(0)->get_count(), 3);
    EXPECT_EQ(split_seq->get(1)->get_count(), 0); // пустой фрагмент после разделителя

    for (int i = 0; i < split_seq->get_count(); i++) {
        delete split_seq->get(i);
    }

    delete split_seq;
    delete seq;
}

TEST(SplitTest, NoDelimiter) {
    int items[] = {1, 2, 3};
    Sequence<int>* seq = new MutableArraySequence<int>(items, 3);

    auto* split_seq = split(seq, is_zero);

    EXPECT_EQ(split_seq->get_count(), 1);
    EXPECT_EQ(split_seq->get(0)->get_count(), 3);

    for (int i = 0; i < split_seq->get_count(); i++) {
        delete split_seq->get(i);
    }

    delete split_seq;
    delete seq;
}

TEST(SplitTest, EmptySequence) {
    Sequence<int>* seq = new MutableArraySequence<int>();

    auto* split_seq = split(seq, is_zero);

    EXPECT_EQ(split_seq->get_count(), 1);
    EXPECT_EQ(split_seq->get(0)->get_count(), 0);

    for (int i = 0; i < split_seq->get_count(); i++) {
        delete split_seq->get(i);
    }

    delete split_seq;
    delete seq;
}

TEST(SplitTest, BitSequenceSplit) {
    BitSequence seq;
    seq.append(Bit(1));
    seq.append(Bit(0));
    seq.append(Bit(1));
    seq.append(Bit(1));
    seq.append(Bit(0));
    seq.append(Bit(1));

    auto* split_seq = split(&seq, is_zero_bit); // разделитель — 0

    EXPECT_EQ(split_seq->get_count(), 3);
    EXPECT_EQ(split_seq->get(0)->get_count(), 1);
    EXPECT_EQ(split_seq->get(1)->get_count(), 2);
    EXPECT_EQ(split_seq->get(2)->get_count(), 1);

    for (int i = 0; i < split_seq->get_count(); i++) {
        delete split_seq->get(i);
    }

    delete split_seq;
}

// =======================

TEST(SliceTest, MutableArrayBasic) {
    int items[] = {1, 2, 3, 4, 5};
    Sequence<int>* seq = new MutableArraySequence<int>(items, 5);

    int rep_items[] = {9, 10};
    Sequence<int>* replacement = new MutableArraySequence<int>(rep_items, 2);

    Sequence<int>* sliced_seq = seq->slice(1, 2, replacement);

    EXPECT_EQ(sliced_seq->get_count(), 5);
    EXPECT_EQ(sliced_seq->get(0), 1);
    EXPECT_EQ(sliced_seq->get(1), 9);
    EXPECT_EQ(sliced_seq->get(2), 10);
    EXPECT_EQ(sliced_seq->get(3), 4);
    EXPECT_EQ(sliced_seq->get(4), 5);

    delete sliced_seq;
    delete replacement;
    delete seq;
}

TEST(SliceTest, MutableListBasic) {
    int items[] = {1, 2, 3, 4, 5};
    Sequence<int>* seq = new MutableListSequence<int>(items, 5);

    int rep_items[] = {9, 10};
    Sequence<int>* replacement = new MutableListSequence<int>(rep_items, 2);

    Sequence<int>* sliced_seq = seq->slice(1, 2, replacement);

    EXPECT_EQ(sliced_seq->get_count(), 5);
    EXPECT_EQ(sliced_seq->get(0), 1);
    EXPECT_EQ(sliced_seq->get(1), 9);
    EXPECT_EQ(sliced_seq->get(2), 10);
    EXPECT_EQ(sliced_seq->get(3), 4);
    EXPECT_EQ(sliced_seq->get(4), 5);

    delete sliced_seq;
    delete replacement;
    delete seq;
}

TEST(SliceTest, DeleteWithoutReplacement) {
    int items[] = {1, 2, 3, 4, 5};
    Sequence<int>* seq = new MutableArraySequence<int>(items, 5);

    Sequence<int>* sliced_seq = seq->slice(1, 2);

    EXPECT_EQ(sliced_seq->get_count(), 3);
    EXPECT_EQ(sliced_seq->get(0), 1);
    EXPECT_EQ(sliced_seq->get(1), 4);
    EXPECT_EQ(sliced_seq->get(2), 5);

    delete sliced_seq;
    delete seq;
}

TEST(SliceTest, NegativeIndex) {
    int items[] = {1, 2, 3, 4, 5};
    Sequence<int>* seq = new MutableArraySequence<int>(items, 5);

    Sequence<int>* sliced_seq = seq->slice(-2, 1); // index = 3, удаляем 1 элемент

    EXPECT_EQ(sliced_seq->get_count(), 4);
    EXPECT_EQ(sliced_seq->get(0), 1);
    EXPECT_EQ(sliced_seq->get(1), 2);
    EXPECT_EQ(sliced_seq->get(2), 3);
    EXPECT_EQ(sliced_seq->get(3), 5);

    delete sliced_seq;
    delete seq;
}

TEST(SliceTest, NegativeIndexWithReplacement) {
    int items[] = {1, 2, 3, 4, 5};
    Sequence<int>* seq = new MutableArraySequence<int>(items, 5);

    int rep_items[] = {99};
    Sequence<int>* replacement = new MutableArraySequence<int>(rep_items, 1);

    Sequence<int>* sliced_seq = seq->slice(-3, 2, replacement); // index = 2, удаляем 2

    EXPECT_EQ(sliced_seq->get_count(), 4);
    EXPECT_EQ(sliced_seq->get(0), 1);
    EXPECT_EQ(sliced_seq->get(1), 2);
    EXPECT_EQ(sliced_seq->get(2), 99);
    EXPECT_EQ(sliced_seq->get(3), 5);

    delete sliced_seq;
    delete replacement;
    delete seq;
}

TEST(SliceTest, IndexOutOfRange) {
    int items[] = {1, 2, 3};
    Sequence<int>* seq = new MutableArraySequence<int>(items, 3);

    EXPECT_THROW(seq->slice(5, 1), std::out_of_range);
    EXPECT_THROW(seq->slice(-4, 1), std::out_of_range);

    delete seq;
}

TEST(SliceTest, DeleteMoreThanAvailable) {
    int items[] = {1, 2, 3, 4, 5};
    Sequence<int>* seq = new MutableArraySequence<int>(items, 5);

    Sequence<int>* sliced_seq = seq->slice(3, 100); // просит 100, но от позиции 3 только 2

    EXPECT_EQ(sliced_seq->get_count(), 3);
    EXPECT_EQ(sliced_seq->get(0), 1);
    EXPECT_EQ(sliced_seq->get(1), 2);
    EXPECT_EQ(sliced_seq->get(2), 3);

    delete sliced_seq;
    delete seq;
}

TEST(SliceTest, BitSequenceSlice) {
    BitSequence seq;
    seq.append(Bit(1));
    seq.append(Bit(0));
    seq.append(Bit(1));
    seq.append(Bit(0));
    seq.append(Bit(1));

    Sequence<Bit>* sliced_seq = seq.slice(1, 2); // удаляем 2 бита с позиции 1

    EXPECT_EQ(sliced_seq->get_count(), 3);
    EXPECT_EQ(sliced_seq->get(0), Bit(1));
    EXPECT_EQ(sliced_seq->get(1), Bit(0));
    EXPECT_EQ(sliced_seq->get(2), Bit(1));

    delete sliced_seq;
}

// =======================

TEST(SequencePolymorhTest, ArrayAndList) {
    Sequence<int>* arr = new MutableArraySequence<int>();
    Sequence<int>* list = new MutableListSequence<int>();

    arr->append(1);
    list->append(1);

    EXPECT_EQ(arr->get(0), list->get(0));

    delete arr;
    delete list;
}

// =========================