#ifndef BIT_SEQUENCE_H
#define BIT_SEQUENCE_H

#include "sequence.h"
#include "bit.h"

class BitSequence : public Sequence<Bit> {
    private:
        DynamicArray<unsigned char>* data;
        int bit_count;
        mutable Bit cached_bit; // Чтобы могли возвращаться const Bit&, т.к. биты упакованы по 8 бит в байт

        bool get_bit(int index) const;
        void set_bit(int index, bool value);

        static int check_bytes_needed(int n); // Количество байт, необходимое для хранения n бит
    public:
        BitSequence();
        BitSequence(const Bit* items, int count);
        BitSequence(const BitSequence& other);

        const Bit& get_first() const override;
        const Bit& get_last() const override;
        const Bit& get(int index) const override;

        Option<Bit> try_get_first() const override;
        Option<Bit> try_get_last() const override;
        Option<Bit> try_get(int index) const override;

        int get_count() const override;

        Sequence<Bit>* get_sub_sequence(int start, int end) override;

        Sequence<Bit>* append(const Bit& item) override;
        Sequence<Bit>* prepend(const Bit& item) override;
        Sequence<Bit>* insert_at(const Bit& item, int index) override;

        Sequence<Bit>* concat(const Sequence<Bit>* other) override;
        Sequence<Bit>* map(Bit (*func)(const Bit& elem)) override;
        Sequence<Bit>* where(bool (*predicate)(const Bit& elem)) override;
        Bit reduce(Bit (*func)(const Bit& first_elem, const Bit& second_elem), const Bit& initial_elem) override;
        // Sequence<Sequence<Bit>*>* split(bool (*predicate)(const Bit&));

        BitSequence* bit_and(const BitSequence& other) const; // И
        BitSequence* bit_or(const BitSequence& other) const; // ИЛИ
        BitSequence* bit_xor(const BitSequence& other) const; // XOR
        BitSequence* bit_not() const; // НЕ

        class Enumerator : public IEnumerator<Bit> {
        private:
            const BitSequence* bit_seq;
            int index;
            mutable Bit current;
        public:
            Enumerator(const BitSequence* bit_seq) : bit_seq(bit_seq), index(-1), current(false) {}

            bool move_next() override {
                index++;
                return index < bit_seq->bit_count;
            }

            const Bit& get_current() const override {
                current = Bit(bit_seq->get_bit(index));
                return current;
            }

            void reset() override {
                index = -1;
            }
        };

        IEnumerator<Bit>* get_enumerator() const override {
            return new Enumerator(this);
        }

        ~BitSequence() override {
            delete data;
        };
};


#endif