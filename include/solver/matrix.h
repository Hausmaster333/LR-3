#ifndef SOLVER_MATRIX_H
#define SOLVER_MATRIX_H

#include "solver/vector.h"

class Matrix {
    private:
        MutableArraySequence<Vector> rows_data;
        int row_count;
        int col_count;
    public:
        Matrix();
        Matrix(int rows, int cols);
        Matrix(const double* items, int rows, int cols);

        int get_rows() const;
        int get_cols() const;

        double get(int row, int col) const;
        void set(int row, int col, double value);

        const Vector& get_row(int row) const;
        void set_row(int row, const Vector& value);
        void swap_rows(int first_row, int second_row);
};

#endif
