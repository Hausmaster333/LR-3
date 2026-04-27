#include "sorting_station/sorting_station.h"

static int find_type_index(const MutableSegmentedDeque<int>& types, int type) {
    for (int i = 0; i < types.get_count(); i++) {
        if (types.get(i) == type) {
            return i;
        }
    }

    return -1;
}

MutableSegmentedDeque<int> collect_types(const MutableSegmentedDeque<Wagon>& train) {
    MutableSegmentedDeque<int> types;

    for (int i = 0; i < train.get_count(); i++) {
        int type = train.get(i).first();

        if (find_type_index(types, type) == -1) {
            types.push_back(type);
        }
    }

    return types;
}

int count_wagon_types(const MutableSegmentedDeque<Wagon>& train) {
    MutableSegmentedDeque<int> types = collect_types(train);
    return types.get_count();
}

MutableSegmentedDeque<Wagon> sort_train_by_type(const MutableSegmentedDeque<Wagon>& train, MutableSegmentedDeque<StationStep>* steps) {
    if (train.get_count() == 0) return MutableSegmentedDeque<Wagon>();
    
    MutableSegmentedDeque<int> types = collect_types(train);
    int siding_count = types.get_count();
    int direct_type = train.get(0).first();  // тип идущий напрямую

    MutableSegmentedDeque<Wagon>* sidings = new MutableSegmentedDeque<Wagon>[siding_count];

    MutableSegmentedDeque<Wagon> work_train(train);
    MutableSegmentedDeque<Wagon> result;
    Wagon wagon;

    while (work_train.get_count() > 0) {
        work_train.pop_front(&wagon);
        if (steps) steps->push_back({StationAction::PopFromTrain, wagon, -1});

        int type = wagon.first();
        
        if (type == direct_type) {
            // Сразу в результат
            result.push_back(wagon);
            if (steps) steps->push_back({StationAction::PushToResult, wagon, -1});
        } else {
            int siding_index = find_type_index(types, type);
            sidings[siding_index].push_back(wagon);
            if (steps) steps->push_back({StationAction::PushToSiding, wagon, siding_index});
        }
    }

    for (int i = 0; i < siding_count; i++) {
        if (types.get(i) == direct_type) continue;  // пропускаем direct
        
        while (sidings[i].get_count() > 0) {
            sidings[i].pop_front(&wagon);
            result.push_back(wagon);
            if (steps) steps->push_back({StationAction::MoveSidingToResult, wagon, i});
        }
    }

    delete[] sidings;
    return result;
}
