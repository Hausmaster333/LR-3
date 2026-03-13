#ifndef UTILS_H
#define UTILS_H

#include "sequence.h"

template <class T>
Sequence<Sequence<T>*>* split(const Sequence<T>* seq, bool (*predicate)(const T&)) {
    auto* sequences = new MutableArraySequence<Sequence<T>*>();
    auto* current_seq = new MutableArraySequence<T>();

    for (int i = 0; i < seq->get_count(); i++) {
        if (predicate(seq->get(i))) {
            sequences->append(current_seq);
            current_seq = new MutableArraySequence<T>();
        } else {
            current_seq->append(seq->get(i));
        }
    }

    sequences->append(current_seq);

    return sequences;
}

#endif