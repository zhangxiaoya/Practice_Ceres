#include <iostream>
#include <ceres/ceres.h>

using std::cout;
using std::cin;
using std::endl;
using ceres::NumericDiffCostFunction;
using ceres::Problem;
using ceres::CENTRAL;
using ceres::CostFunction;
using ceres::Solver;
using ceres::Solve;

class CostFunctor
{
public:
    template <typename T>
    bool operator()(const T *const x, T* residual) const {
        residual[0] = 10.0 - x[0];
        return true;
    }
};

int main()
{
    double initialX = 0.5;
    double x = initialX;

    Problem problem;
    CostFunction* costFunction = new NumericDiffCostFunction<CostFunctor, CENTRAL, 1,1>(new CostFunctor);
    problem.AddResidualBlock(costFunction, NULL, &x);

    Solver::Options options;
    options.minimizer_progress_to_stdout = true;
    Solver::Summary summary;
    Solve(options,&problem, &summary);
    cout <<summary.BriefReport()<< endl;
    cout << "x : " << initialX << " -> " << x<< endl;
    return 0;
}