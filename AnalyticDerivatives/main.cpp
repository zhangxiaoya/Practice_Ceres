#include <iostream>
#include <ceres/ceres.h>

using std::cout;
using std::endl;
using ceres::Problem;
using ceres::CostFunction;
using ceres::SizedCostFunction;
using ceres::Solve;
using ceres::Solver;

class QuadraticCostFunction : public SizedCostFunction<1, 1>
{
public:
    virtual ~QuadraticCostFunction()
    {
    }

    virtual bool Evaluate(double const *const *parameters, double *residuals, double **jacobians) const
    {
        double x = parameters[0][0];
        residuals[0] = 10 - x;
        if(jacobians != NULL && jacobians[0] != NULL)
        {
            jacobians[0][0] = -1;
        }
        return true;
    }
};

int main()
{
    double  x = 0.5;
    const double initialX = x;

    Problem problem;
    CostFunction* costFunction = new QuadraticCostFunction;
    problem.AddResidualBlock(costFunction, NULL, &x);

    Solver::Summary summary;
    Solver::Options options;
    options.minimizer_progress_to_stdout = true;
    Solve(options, &problem,&summary);
    cout << summary.BriefReport() << endl;
    cout << "x: " << initialX << " -> " << x<< endl;
    return 0;
}