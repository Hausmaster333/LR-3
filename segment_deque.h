#ifndef SEGMENT_DEQUE_H
#define SEGMENT_DEQUE_H

#include "sequence.h"
#include <stdexcept>

static const int SEGMENT_SIZE = 8;

template <class T>
class SegmentDeque: public Sequence<T> {
    protected:
        DynamicArray<T*>* block_map;
        int map_capacity;
        int front_block;
        int front_index;
        int back_block;
        int back_index;
        int count;

        T* allocate_block();
        void grow_map_front();
        void grow_map_back();

        void resolve_index(int index, int& block, int& offset) const;
    public:

        SegmentDeque();
        SegmentDeque(const T* items, int count);
        SegmentDeque(const SegmentDeque<T>& other);

        virtual SegmentDeque<T>* Instance() = 0;
        virtual SegmentDeque<T>* EmptyClone() = 0;

        SegmentDeque<T>& operator=(const SegmentDeque<T>& other);

        const T& get_first() const override;
        const T& get_last() const override;
        const T& get(int index) const override;

        Option<T> try_get_first() const override;
        Option<T> try_get_last() const override;
        Option<T> try_get(int index) const override;

        int get_count() const override;

        Sequence<T>* get_sub_sequence(const T& item) override;
        
        Sequence<T>* append(const T& item) override;
        Sequence<T>* prepend(const T& item) override;
        Sequence<T>* insert_at(const T& item, int index) const;
        
        Sequence<T>* concat(const Sequence<T>* other) override;
        Sequence<T>* map(T (*func)(const T& elem)) override;
        Sequence<T>* where(bool (*predicate)(const T& elem)) override;
        T reduce(T (*func)(const T& first_elem, const T& second_elem), const T& initial_elem) override;

        void push_front(const T& elem);
        void push_back(const T& elem);
        T pop_front();
        T pop_back();

        void sort(bool (*compare)(const T& a, const T& b) = nullptr);
        SegmentDeque<T>* merge(const SegmentDeque<T>* other, bool (*compare)(const T& a, const T& b) = nullptr);
        int find_sub_sequence(const Sequence<T>* sub_seq) const;

        class Enumerator: public IEnumerator<T> {
            private:
                const SegmentDeque<T>* deque;
                int index;
            public:
                Enumerator(const SegmentDeque<T>* deque) : deque(deque), index(-1) {}

                bool move_next() override {
                    index++;
                    return index < deque->count;
                }

                const T& get_current() const override {
                    return deque->get(index);
                }

                void reset() override {
                    index = -1;
                }
        };

        IEnumerator<T>* get_enumerator() const override {
            return new Enumerator(this);
        }

        ~SegmentDeque() override;
};

template <class T>
class MutableSegmentedDeque : public SegmentDeque<T> {
    protected:
        SegmentDeque<T>* Instance() override {
            return this;
        }
        
        SegmentDeque<T>* EmptyClone() override {
            return new MutableSegmentedDeque<T>();
        }
    public:
        MutableSegmentedDeque() : SegmentDeque<T>() {}
        MutableSegmentedDeque(const T* items, int count) : SegmentDeque<T>(items, count) {}
        MutableSegmentedDeque(const SegmentDeque<T>& other) : SegmentDeque<T>(other) {}
};

template <class T>
class ImmutableSegmentedDeque : public SegmentDeque<T> {
    protected:
        SegmentDeque<T>* Instance() override {
            return new ImmutableSegmentedDeque<T>(*this);
        }
        
        SegmentDeque<T>* EmptyClone() override {
            return new ImmutableSegmentedDeque<T>();
        }
    public:
        ImmutableSegmentedDeque() : SegmentDeque<T>() {}
        ImmutableSegmentedDeque(const T* items, int count) : SegmentDeque<T>(items, count) {}
        ImmutableSegmentedDeque(const SegmentDeque<T>& other) : SegmentDeque<T>(other) {}
};

#include "segment_deque.tpp"

#endif