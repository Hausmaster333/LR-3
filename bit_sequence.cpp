#include "bit_sequence.h"
#include <stdexcept>

int BitSequence::check_bytes_needed(int n) {
    if (n <= 0) return 0;
    return (n + 7) / 8;
}

bool BitSequence::get_bit(int index) const {
    unsigned char byte = data->get(index / 8); // Определяем, в каком байте лежит нужный бит
    // В битах нумерование справа налево
    return (byte >> (index % 8)) & 1; // Определяем позицию внутри этого байта (0–7) через index % 8 и сдвигаем бит вправо, чтобы бит оказался на позиции 0, после чего выполняем побитовое И с 1
}

void BitSequence::set_bit(int index, bool value) {
    int byte_index = index / 8;
    int bit_index = index % 8;
    unsigned char byte = data->get(byte_index);

    if (value) { // Если 1
        byte |= (1 << bit_index); // Сдвигом получаем маску и используем ИЛИ, чтобы поставить 1 в нужный бит
    } else { // Если 0
        byte &= ~(1 << bit_index); // Сдвигаем + инвертируем и используем И, чтобы оставить 1 только там, где обе 1, а в остальных будет 0
    }

    data->set(byte_index, byte);
}

BitSequence::BitSequence() : bit_count(0), cached_bit(false) {
    data = new DynamicArray<unsigned char>(0);
}

BitSequence::BitSequence(const Bit* items, int count) : bit_count(count), cached_bit(false) {
    if (count < 0) throw std::invalid_argument("Count cannot be negative");

    int bytes = check_bytes_needed(count);
    data = new DynamicArray<unsigned char>(bytes);

    for (int i = 0; i < bytes; i++) { // Зануляем все байты
        data->set(i, 0);
    }

    for (int i = 0; i < count; i++) { // Заполняем байты битами
        set_bit(i, items[i].get());
    }
}

BitSequence::BitSequence(const BitSequence& other) : bit_count(other.bit_count), cached_bit(false) {
    data = new DynamicArray<unsigned char>(*other.data);
}

const Bit& BitSequence::get_first() const {
    if (bit_count == 0) throw std::out_of_range("Sequence is empty");

    cached_bit = Bit(get_bit(0));

    return cached_bit;
}

const Bit& BitSequence::get_last() const {
    if (bit_count == 0) throw std::out_of_range("Sequence is empty");

    cached_bit = Bit(get_bit(bit_count - 1));

    return cached_bit;
}

const Bit& BitSequence::get(int index) const {
    if (index < 0 || index >= bit_count) throw std::out_of_range("Index out of range");

    cached_bit = Bit(get_bit(index));

    return cached_bit;
}

Option<Bit> BitSequence::try_get_first() const {
    if (bit_count == 0) return Option<Bit>();

    return Option<Bit>(Bit(get_bit(0)));
}

Option<Bit> BitSequence::try_get_last() const {
    if (bit_count == 0) return Option<Bit>();

    return Option<Bit>(Bit(get_bit(bit_count - 1)));
}

Option<Bit> BitSequence::try_get(int index) const {
    if (index < 0 || index >= bit_count) return Option<Bit>();

    return Option<Bit>(Bit(get_bit(index)));
}

int BitSequence::get_count() const {
    return bit_count;
}

Sequence<Bit>* BitSequence::get_sub_sequence(int start, int end) {
    if (start < 0 || end < 0 || start >= bit_count || end >= bit_count || start > end) {
        throw std::out_of_range("Index out of range");
    }

    int new_count = end - start + 1;

    BitSequence* result = new BitSequence();
    result->data->resize(check_bytes_needed(new_count));

    for (int i = 0; i < check_bytes_needed(new_count); i++) {
        result->data->set(i, 0);
    }

    result->bit_count = new_count;

    for (int i = 0; i < new_count; i++) {
        result->set_bit(i, get_bit(start + i));
    }

    return result;
}

Sequence<Bit>* BitSequence::append(const Bit& item) {
    int new_count = bit_count + 1;
    int new_bytes = check_bytes_needed(new_count);

    if (new_bytes > data->get_size()) {
        data->resize(new_bytes);
        data->set(new_bytes - 1, 0);
    }

    bit_count = new_count;
    set_bit(bit_count - 1, item.get());

    return this;
}

Sequence<Bit>* BitSequence::prepend(const Bit& item) {
    DynamicArray<unsigned char>* old_data = new DynamicArray<unsigned char>(*data);
    int old_count = bit_count;

    int new_count = old_count + 1;
    int new_bytes = check_bytes_needed(new_count);
    data->resize(new_bytes);

    for (int i = 0; i < new_bytes; i++) {
        data->set(i, 0);
    }

    bit_count = new_count;

    set_bit(0, item.get());
    for (int i = 0; i < old_count; i++) {
        unsigned char old_byte = old_data->get(i / 8);
        bool val = (old_byte >> (i % 8)) & 1;
        set_bit(i + 1, val);
    }
    delete old_data;

    return this;
}

Sequence<Bit>* BitSequence::insert_at(const Bit& item, int index) {
    if (index < 0 || index > bit_count) throw std::out_of_range("Index out of range");

    DynamicArray<unsigned char>* old_data = new DynamicArray<unsigned char>(*data);
    int old_count = bit_count;
    
    int new_count = old_count + 1;
    int new_bytes = check_bytes_needed(new_count);
    data->resize(new_bytes);

    for (int i = 0; i < new_bytes; i++) {
        data->set(i, 0);
    }

    bit_count = new_count;

    for (int i = 0; i < index; i++) {
        unsigned char old_byte = old_data->get(i / 8);
        bool val = (old_byte >> (i % 8)) & 1;
        set_bit(i, val);
    }

    set_bit(index, item.get());

    for (int i = index; i < old_count; i++) {
        unsigned char old_byte = old_data->get(i / 8);
        bool val = (old_byte >> (i % 8)) & 1;
        set_bit(i + 1, val);
    }

    delete old_data;

    return this;
}

Sequence<Bit>* BitSequence::concat(const Sequence<Bit>* other) {
    for (int i = 0; i < other->get_count(); i++) {
        append(other->get(i));
    }

    return this;
}

Sequence<Bit>* BitSequence::map(Bit (*func)(const Bit& elem)) {
    BitSequence* mapped_bit = new BitSequence();

    for (int i = 0; i < bit_count; i++) {
        Bit current_elem(get_bit(i));
        mapped_bit->append(func(current_elem));
    }

    return mapped_bit;
}

Sequence<Bit>* BitSequence::where(bool (*predicate)(const Bit& elem)) {
    BitSequence* where_bit = new BitSequence();

    for (int i = 0; i < bit_count; i++) {
        Bit current_elem(get_bit(i));

        if (predicate(current_elem)) {
            where_bit->append(current_elem);
        }
    }

    return where_bit;
}

Bit BitSequence::reduce(Bit (*func)(const Bit& first_elem, const Bit& second_elem), const Bit& initial_elem) {
    Bit reduced_elem = initial_elem;

    for (int i = 0; i < bit_count; i++) {
        Bit current_elem(get_bit(i));
        reduced_elem = func(reduced_elem, current_elem);
    }

    return reduced_elem;
}

// Sequence<Sequence<Bit>*>* BitSequence::split(bool (*predicate)(const Bit&)) {
//     auto* sequences = new MutableArraySequence<Sequence<Bit>*>();
//     BitSequence* current_seq = new BitSequence();

//     for (int i = 0; i < bit_count; i++) {
//         Bit b(get_bit(i));
//         if (predicate(b)) {
//             sequences->append(current_seq);
//             current_seq = new BitSequence();
//         } else {
//             current_seq->append(b);
//         }
//     }
//     sequences->append(current_seq);

//     return sequences;
// }

BitSequence* BitSequence::bit_and(const BitSequence& other) const {
    int min_count = (bit_count < other.bit_count) ? bit_count : other.bit_count;
    int min_bytes = check_bytes_needed(min_count);

    BitSequence* and_bit_seq = new BitSequence();
    and_bit_seq->data->resize(min_bytes);
    and_bit_seq->bit_count = min_count;

    for (int i = 0; i < min_bytes; i++) {
        and_bit_seq->data->set(i, data->get(i) & other.data->get(i));
    }

    return and_bit_seq;
}

BitSequence* BitSequence::bit_or(const BitSequence& other) const {
    int max_count = (bit_count > other.bit_count) ? bit_count : other.bit_count;
    int max_bytes = check_bytes_needed(max_count);

    BitSequence* or_bit_seq = new BitSequence();
    or_bit_seq->data->resize(max_bytes);
    or_bit_seq->bit_count = max_count;

    for (int i = 0; i < max_bytes; i++) {
        unsigned char a = (i < check_bytes_needed(bit_count)) ? data->get(i) : 0;
        unsigned char b = (i < check_bytes_needed(other.bit_count)) ? other.data->get(i) : 0;

        or_bit_seq->data->set(i, a | b);
    }

    return or_bit_seq;
}

BitSequence* BitSequence::bit_xor(const BitSequence& other) const {
    int max_count = (bit_count > other.bit_count) ? bit_count : other.bit_count;
    int max_bytes = check_bytes_needed(max_count);

    BitSequence* xor_bit_seq = new BitSequence();
    xor_bit_seq->data->resize(max_bytes);
    xor_bit_seq->bit_count = max_count;

    for (int i = 0; i < max_bytes; i++) {
        unsigned char a = (i < check_bytes_needed(bit_count)) ? data->get(i) : 0;
        unsigned char b = (i < check_bytes_needed(other.bit_count)) ? other.data->get(i) : 0;

        xor_bit_seq->data->set(i, a ^ b);
    }

    return xor_bit_seq;
}

BitSequence* BitSequence::bit_not() const {
    int bytes = check_bytes_needed(bit_count);

    BitSequence* not_bit_seq = new BitSequence();
    not_bit_seq->data->resize(bytes);
    not_bit_seq->bit_count = bit_count;

    for (int i = 0; i < bytes; i++) {
        not_bit_seq->data->set(i, ~data->get(i));
    }

    return not_bit_seq;
}
