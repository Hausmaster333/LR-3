#ifndef HANOI_H
#define HANOI_H

#include "hanoi_ring.h"
#include "segment_deque.h"

struct HanoiMove {
    int from_stick_index;
    int to_stick_index;
    int ring_size;
    const char* ring_color;
};

#include "hanoi_vis.h"

static void hanoi_recursive(int n, // Количество колец
                            MutableSegmentedDeque<Ring>& from, int from_id, // Откуда перемещаем
                            MutableSegmentedDeque<Ring>& to, int to_id, // Куда перемещаем
                            MutableSegmentedDeque<Ring>& aux, int aux_id,
                            MutableSegmentedDeque<HanoiMove>& moves)  { // Вспомогательный стержень
    
    Ring elem;

    if (n == 1) {
        from.pop_back(&elem);
        to.push_back(elem);
        moves.push_back(HanoiMove{from_id, to_id, elem.get_size(), elem.get_color()});
        return;
    }

    hanoi_recursive(n - 1, from, from_id, aux, aux_id, to, to_id, moves); // Перемещаем n - 1 колец на вспомогательный стержень

    from.pop_back(&elem); // Самое большое перемещаем
    to.push_back(elem);
    moves.push_back(HanoiMove{from_id, to_id, elem.get_size(), elem.get_color()});

    hanoi_recursive(n - 1, aux, aux_id, to, to_id, from, from_id, moves); // Перемещаем n - 1 колец на целевой стержень
};

static bool ring_compare(const Ring& a, const Ring& b) {
    return a < b;  // Большие кольца первыми
}

void hanoi(MutableSegmentedDeque<Ring>& rings, int start_stick, int target_stick) { // Список предметов в виде дека + номер начального стержня -> создает 3 дека-стержня и кладет кольца на нужный
    if (start_stick < 0 || start_stick > 2 || target_stick < 0 || target_stick > 2 || target_stick == start_stick) {
        throw std::out_of_range("Stick can't be less than 0 and more than 2 and can't be same");
    }
    MutableSegmentedDeque<HanoiMove> moves;

    MutableSegmentedDeque<Ring> sticks[3];
    sticks[start_stick] = rings;

    int aux = 3 - start_stick - target_stick;

    sticks[start_stick].sort(ring_compare);

    hanoi_recursive(sticks[start_stick].get_count(),
                    sticks[start_stick], start_stick,
                    sticks[target_stick], target_stick,
                    sticks[aux], aux,
                    moves);

    generate_hanoi_html(moves, rings, start_stick);
};

#endif