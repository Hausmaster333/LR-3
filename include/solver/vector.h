#ifndef SOLVER_VECTOR_H
#define SOLVER_VECTOR_H

#include "core/sequence.h"
#include <stdexcept>

class Vector {
    private:
        MutableArraySequence<double> values;
    public:
        Vector();
        Vector(int size);
        Vector(const double* items, int count);

        int get_size() const;
        double get(int index) const;
        void set(int index, double value);
};

#endif
