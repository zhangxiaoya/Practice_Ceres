#include <iostream>
#include <ceres/ceres.h>

using std::cout;
using std::endl;
using ceres::Problem;
using ceres::CostFunction;
using ceres::Solve;
using ceres::Solver;
using std::sqrt;
using ceres::AutoDiffCostFunction;

struct F1
{
    template<typename T>
    bool operator()(const T *const x1, const T *const x2, T *residual) const
    {
        residual[0] = x1[0] + T(10) * x2[0];
        return true;
    }
};

struct F2
{
    template<typename T>
    bool operator()(const T *const x3, const T *const x4, T *residual) const
    {
        residual[0] = sqrt(T(5)) * (x3[0] - x4[0]);
        return true;
    }
};

struct F3
{
    template<typename T>
    bool operator()(const T *const x2, const T *const x3, T *residual) const
    {
        residual[0] = (x2[0] - T(2) * x3[0]) * (x2[0] - T(2) * x3[0]);
        return true;
    }
};

struct F4
{
    template<typename T>
    bool operator()(const T *const x1, const T *const x4, T *residual) const
    {
        residual[0] = sqrt(T(10)) * (x1[0] - x4[0]) * (x1[0] - x4[0]);
        return true;
    }
};

int main()
{
    double x1 = 3.0;
    double x2 = -1.0;
    double x3 = 0.0;
    double x4 = 1.0;

    Problem problem;
    problem.AddResidualBlock(new AutoDiffCostFunction<F1, 1, 1, 1>(new F1), NULL, &x1, &x2);
    problem.AddResidualBlock(new AutoDiffCostFunction<F2, 1, 1, 1>(new F2), NULL, &x3, &x4);
    problem.AddResidualBlock(new AutoDiffCostFunction<F3, 1, 1, 1>(new F3), NULL, &x2, &x3);
    problem.AddResidualBlock(new AutoDiffCostFunction<F4, 1, 1, 1>(new F4), NULL, &x1, &x4);

    Solver::Options options;
    options.minimizer_progress_to_stdout = true;
    Solver::Summary summary;

    Solve(options, &problem, &summary);
    cout << summary.BriefReport() << endl;
    cout << x1 << " " << x2 << " " << x3 << " " << x4 << endl;
    return 0;
}