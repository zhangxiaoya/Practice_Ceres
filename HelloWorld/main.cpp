#include <iostream>
#include <ceres/ceres.h>

struct CostFunctor
{
    template<typename T>
    bool operator()(const T *const x, T *residual) const
    {
        residual[0] = T(10.0) - x[0];
        return true;
    }
};

int main()
{
    double initialX = 5.0;
    double x = initialX;

    ceres::Problem problem;
    ceres::CostFunction *costFunction = new ceres::AutoDiffCostFunction<CostFunctor, 1, 1>(new CostFunctor);
    problem.AddResidualBlock(costFunction, nullptr, &x);

    ceres::Solver::Options options;
    options.linear_solver_type = ceres::DENSE_QR;
    options.minimizer_progress_to_stdout = true;
    ceres::Solver::Summary summary;
    ceres::Solve(options, &problem, &summary);

    std::cout << summary.BriefReport() << std::endl;
    std::cout << "X = " << initialX << "->" << x << "\n";
    return 0;
}