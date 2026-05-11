#ifndef SOLVER_H
#define SOLVER_H

#include "deque/segment_deque.h"
#include "solver/matrix.h"
#include "solver/vector.h"

enum class GaussMode {
    Basic,
    PartialPivot
};

enum class GaussStepType {
    Start,
    SelectPivot,
    SwapRows,
    EliminateRow,
    BackSubstitution,
    Finished,
    Error
};

struct GaussStep {
    GaussStepType type;
    Matrix augmented;
    Vector solution;
    int pivot_row;
    int pivot_col;
    int target_row;
    double factor;
    int solution_count;
    char description[128];

    GaussStep();
};

class Solver {
    private:
        Solver();

        static Matrix make_augmented(const Matrix& matrix, const Vector& rhs);
        static Vector run_gauss(const Matrix& matrix,
                                const Vector& rhs,
                                GaussMode mode,
                                MutableSegmentedDeque<GaussStep>* steps);

        static void append_step(MutableSegmentedDeque<GaussStep>* steps,
                                GaussStepType type,
                                const Matrix& augmented,
                                const Vector& solution,
                                int pivot_row,
                                int pivot_col,
                                int target_row,
                                double factor,
                                int solution_count,
                                const char* description);
    public:
        Solver(const Solver&) = delete;
        Solver& operator=(const Solver&) = delete;

        static Vector solve_gauss_basic(const Matrix& matrix, const Vector& rhs);
        static Vector solve_gauss_partial_pivot(const Matrix& matrix, const Vector& rhs);
        static MutableSegmentedDeque<GaussStep> collect_gauss_steps(const Matrix& matrix,
                                                                    const Vector& rhs,
                                                                    GaussMode mode);
};

#endif
