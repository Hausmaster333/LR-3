#ifndef UTILS_H
#define UTILS_H

#include "core/sequence.h"
#include "tuples.h"

template <class T>
Sequence<Sequence<T>*>* split(const Sequence<T>* seq, bool (*predicate)(const T&)) {
    auto* sequences = new MutableArraySequence<Sequence<T>*>();
    auto* current_seq = new MutableArraySequence<T>();

    EnumeratorWrapper<T> iter(seq->get_enumerator());
    while (iter.move_next()) {
        if (predicate(iter.get_current())) {
            sequences->append(current_seq);
            current_seq = new MutableArraySequence<T>();
        } else {
            current_seq->append(iter.get_current());
        }
    }

    sequences->append(current_seq);

    return sequences;
}

template <class T1, class T2>
Sequence<Pair<T1, T2>>* zip(const Sequence<T1>* a, const Sequence<T2>* b) {
    if (a == nullptr || b == nullptr) {
        throw std::invalid_argument("Cannot zip with nullptr");
    }

    auto* result = new MutableArraySequence<Pair<T1, T2>>();

    EnumeratorWrapper<T1> a_iter(a->get_enumerator());
    EnumeratorWrapper<T2> b_iter(b->get_enumerator());

    while (a_iter.move_next() && b_iter.move_next()) {
        result->append(Pair<T1, T2>(a_iter.get_current(), b_iter.get_current()));
    }

    return result;
}

template <class T1, class T2>
Pair<Sequence<T1>*, Sequence<T2>*> unzip(const Sequence<Pair<T1, T2>>* seq) {
    if (seq == nullptr) {
        throw std::invalid_argument("Cannot unzip nullptr");
    }

    auto* first_seq = new MutableArraySequence<T1>();
    auto* second_seq = new MutableArraySequence<T2>();

    EnumeratorWrapper<Pair<T1, T2>> iter(seq->get_enumerator());
    while (iter.move_next()) {
        const Pair<T1, T2>& p = iter.get_current();

        first_seq->append(p.first());
        second_seq->append(p.second());
    }

    return Pair<Sequence<T1>*, Sequence<T2>*>(first_seq, second_seq);
}

template <class T1, class T2, class T3>
Sequence<Triple<T1, T2, T3>>* zip3(const Sequence<T1>* a, const Sequence<T2>* b, const Sequence<T3>* c) {
    if (a == nullptr || b == nullptr || c == nullptr) {
        throw std::invalid_argument("Cannot zip3 with nullptr");
    }

    auto* result = new MutableArraySequence<Triple<T1, T2, T3>>();

    EnumeratorWrapper<T1> a_iter(a->get_enumerator());
    EnumeratorWrapper<T2> b_iter(b->get_enumerator());
    EnumeratorWrapper<T3> c_iter(c->get_enumerator());

    while (a_iter.move_next() && b_iter.move_next() && c_iter.move_next()) {
        result->append(Triple<T1, T2, T3>(a_iter.get_current(), b_iter.get_current(), c_iter.get_current()));
    }

    return result;
}

template <class T1, class T2, class T3>
Triple<Sequence<T1>*, Sequence<T2>*, Sequence<T3>*> unzip3(const Sequence<Triple<T1, T2, T3>>* seq) {
    if (seq == nullptr) {
        throw std::invalid_argument("Cannot unzip3 nullptr");
    }

    auto* first_seq = new MutableArraySequence<T1>();
    auto* second_seq = new MutableArraySequence<T2>();
    auto* third_seq = new MutableArraySequence<T3>();

    EnumeratorWrapper<Triple<T1, T2, T3>> iter(seq->get_enumerator());
    while (iter.move_next()) {
        const Triple<T1, T2, T3>& t = iter.get_current();

        first_seq->append(t.first());
        second_seq->append(t.second());
        third_seq->append(t.third());
    }

    return Triple<Sequence<T1>*, Sequence<T2>*, Sequence<T3>*>(first_seq, second_seq, third_seq);
}

#endif