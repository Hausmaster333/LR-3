#include "solver/solver.h"
#include <cmath>
#include <cstdio>
#include <cstring>
#include <stdexcept>

static const double solver_epsilon = 1e-12;

Vector::Vector() {
    values = MutableArraySequence<double>();
}

Vector::Vector(int size) {
    if (size < 0) throw std::out_of_range("Vector size must be >= 0");

    values = MutableArraySequence<double>();
    for (int index = 0; index < size; index++) {
        values.append(0.0);
    }
}

Vector::Vector(const double* items, int count) {
    if (count < 0) throw std::out_of_range("Vector size must be >= 0");

    values = MutableArraySequence<double>();
    for (int index = 0; index < count; index++) {
        values.append(items[index]);
    }
}

int Vector::get_size() const {
    return values.get_count();
}

double Vector::get(int index) const {
    if (index < 0 || index >= values.get_count()) throw std::out_of_range("Vector index out of range");

    return values.get(index);
}

void Vector::set(int index, double value) {
    if (index < 0 || index >= values.get_count()) throw std::out_of_range("Vector index out of range");

    values.set(index, value);
}

Matrix::Matrix(): row_count(0), col_count(0) {
    rows_data = MutableArraySequence<Vector>();
}

Matrix::Matrix(int rows, int cols): row_count(rows), col_count(cols) {
    if (rows < 0 || cols < 0) throw std::out_of_range("Matrix size must be >= 0");

    rows_data = MutableArraySequence<Vector>();
    for (int row = 0; row < rows; row++) {
        rows_data.append(Vector(cols));
    }
}

Matrix::Matrix(const double* items, int rows, int cols): Matrix(rows, cols) {
    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < cols; col++) {
            set(row, col, items[row * cols + col]);
        }
    }
}

int Matrix::get_rows() const {
    return row_count;
}

int Matrix::get_cols() const {
    return col_count;
}

double Matrix::get(int row, int col) const {
    if (row < 0 || row >= row_count || col < 0 || col >= col_count) {
        throw std::out_of_range("Matrix index out of range");
    }

    return rows_data.get(row).get(col);
}

void Matrix::set(int row, int col, double value) {
    if (row < 0 || row >= row_count || col < 0 || col >= col_count) {
        throw std::out_of_range("Matrix index out of range");
    }

    Vector row_value = rows_data.get(row);
    row_value.set(col, value);
    rows_data.set(row, row_value);
}

const Vector& Matrix::get_row(int row) const {
    if (row < 0 || row >= row_count) throw std::out_of_range("Matrix row index out of range");

    return rows_data.get(row);
}

void Matrix::set_row(int row, const Vector& value) {
    if (row < 0 || row >= row_count) throw std::out_of_range("Matrix row index out of range");
    if (value.get_size() != col_count) throw std::out_of_range("Matrix row size mismatch");

    rows_data.set(row, value);
}

void Matrix::swap_rows(int first_row, int second_row) {
    if (first_row < 0 || first_row >= row_count || second_row < 0 || second_row >= row_count) {
        throw std::out_of_range("Matrix row index out of range");
    }

    if (first_row == second_row) return;

    Vector first = rows_data.get(first_row);
    Vector second = rows_data.get(second_row);
    rows_data.set(first_row, second);
    rows_data.set(second_row, first);
}

GaussStep::GaussStep():
    type(GaussStepType::Start),
    pivot_row(-1),
    pivot_col(-1),
    target_row(-1),
    factor(0.0),
    solution_count(0) {

    description[0] = '\0';
}

Solver::Solver() {
}

Matrix Solver::make_augmented(const Matrix& matrix, const Vector& rhs) {
    if (matrix.get_rows() <= 0 || matrix.get_cols() <= 0) throw std::invalid_argument("Matrix must be non-empty");

    if (matrix.get_rows() != matrix.get_cols()) throw std::invalid_argument("Matrix must be square");

    if (rhs.get_size() != matrix.get_rows()) throw std::invalid_argument("Vector size must match matrix size");

    int size = matrix.get_rows();
    Matrix augmented(size, size + 1);

    for (int row = 0; row < size; row++) {
        for (int col = 0; col < size; col++) {
            augmented.set(row, col, matrix.get(row, col));
        }
        augmented.set(row, size, rhs.get(row));
    }

    return augmented;
}

void Solver::append_step(MutableSegmentedDeque<GaussStep>* steps,
                         GaussStepType type,
                         const Matrix& augmented,
                         const Vector& solution,
                         int pivot_row,
                         int pivot_col,
                         int target_row,
                         double factor,
                         int solution_count,
                         const char* description) {

    if (steps == nullptr) return;

    GaussStep step;
    step.type = type;
    step.augmented = augmented;
    step.solution = solution;
    step.pivot_row = pivot_row;
    step.pivot_col = pivot_col;
    step.target_row = target_row;
    step.factor = factor;
    step.solution_count = solution_count;

    std::strncpy(step.description, description, sizeof(step.description) - 1);
    step.description[sizeof(step.description) - 1] = '\0';

    steps->push_back(step);
}

Vector Solver::run_gauss(const Matrix& matrix, const Vector& rhs, GaussMode mode, MutableSegmentedDeque<GaussStep>* steps) {
    Matrix augmented = make_augmented(matrix, rhs);
    int size = matrix.get_rows();
    Vector solution(size);

    append_step(steps, GaussStepType::Start, augmented, solution, -1, -1, -1, 0.0, 0, "Initial augmented matrix");

    for (int pivot_col = 0; pivot_col < size; pivot_col++) {
        int pivot_row = pivot_col;
        double pivot_abs = std::fabs(augmented.get(pivot_col, pivot_col));

        if (mode == GaussMode::PartialPivot) {
            for (int row = pivot_col + 1; row < size; row++) {
                double current_abs = std::fabs(augmented.get(row, pivot_col));
                if (current_abs > pivot_abs) {
                    pivot_abs = current_abs;
                    pivot_row = row;
                }
            }
        }

        char description[128];
        std::snprintf(description, sizeof(description), "Select pivot in column %d: row %d", pivot_col, pivot_row);
        append_step(steps, GaussStepType::SelectPivot, augmented, solution, pivot_row, pivot_col, -1, 0.0, 0, description);

        if (pivot_abs < solver_epsilon) throw std::runtime_error("Gauss failed: zero pivot");

        if (pivot_row != pivot_col) {
            augmented.swap_rows(pivot_row, pivot_col);
            std::snprintf(description, sizeof(description), "Swap R%d and R%d", pivot_col, pivot_row);
            append_step(steps, GaussStepType::SwapRows, augmented, solution, pivot_col, pivot_col, pivot_row, 0.0, 0, description);
        }

        double pivot = augmented.get(pivot_col, pivot_col);

        for (int row = pivot_col + 1; row < size; row++) {
            double factor = augmented.get(row, pivot_col) / pivot;
            if (std::fabs(factor) < solver_epsilon) continue;

            for (int col = pivot_col; col <= size; col++) {
                double value = augmented.get(row, col) - factor * augmented.get(pivot_col, col);
                augmented.set(row, col, value);
            }
            augmented.set(row, pivot_col, 0.0);

            std::snprintf(description, sizeof(description), "R%d = R%d - %.6g * R%d", row, row, factor, pivot_col);
            append_step(steps, GaussStepType::EliminateRow, augmented, solution, pivot_col, pivot_col, row, factor, 0, description);
        }
    }

    for (int row = size - 1; row >= 0; row--) {
        double sum = augmented.get(row, size);

        for (int col = row + 1; col < size; col++) {
            sum -= augmented.get(row, col) * solution.get(col);
        }

        double pivot = augmented.get(row, row);
        if (std::fabs(pivot) < solver_epsilon) {
            throw std::runtime_error("Back substitution failed: zero pivot");
        }

        double value = sum / pivot;
        solution.set(row, value);

        char description[128];
        std::snprintf(description, sizeof(description), "x%d = %.6g", row, value);
        append_step(steps, GaussStepType::BackSubstitution, augmented, solution, row, row, row, 0.0, size - row, description);
    }

    append_step(steps, GaussStepType::Finished, augmented, solution, -1, -1, -1, 0.0, size, "Solution is ready");

    return solution;
}

Vector Solver::solve_gauss_basic(const Matrix& matrix, const Vector& rhs) {
    return run_gauss(matrix, rhs, GaussMode::Basic, nullptr);
}

Vector Solver::solve_gauss_partial_pivot(const Matrix& matrix, const Vector& rhs) {
    return run_gauss(matrix, rhs, GaussMode::PartialPivot, nullptr);
}

MutableSegmentedDeque<GaussStep> Solver::collect_gauss_steps(const Matrix& matrix, const Vector& rhs, GaussMode mode) {
    MutableSegmentedDeque<GaussStep> steps;

    try {
        run_gauss(matrix, rhs, mode, &steps);
    } catch (const std::exception& error) {
        Matrix empty_matrix;
        Vector empty_solution;

        if (steps.get_count() > 0) {
            empty_matrix = steps.get_last().augmented;
            empty_solution = steps.get_last().solution;
        }

        append_step(&steps, GaussStepType::Error, empty_matrix, empty_solution, -1, -1, -1, 0.0, 0, error.what());
    }

    return steps;
}
