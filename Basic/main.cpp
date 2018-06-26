#include <iostream>
#include <opencv2/core.hpp>
#include <ceres/ceres.h>
#include <chrono>

using namespace std;

class Curve_fitting_cost
{
public:
    Curve_fitting_cost(double x, double y) : x(x), y(y) {}

    template<typename T>
    bool operator()(const T *const abc, T *residual) const
    {
        residual[0] = T(y) - ceres::exp(abc[0] * T(x) * T(x) + abc[1] * T(x) + abc[2]);
        return true;
    }

    const double x;
    const double y;
};

int main()
{
    double a = 1.0;
    double b = 2.0;
    double c = 1.0;

    int N = 100;
    double w_sigma = 1.0;
    cv::RNG rng;
    double abc[3] = {0.0, 0.0, 0.0};

    vector<double> x_data;
    vector<double> y_data;
    cout << "generating data " << endl;
    for (int i = 0; i < N; ++i)
    {
        double x = i / 100.0;
        x_data.push_back(x);
        y_data.push_back(exp(a * x * x + b * x + c) + rng.gaussian(w_sigma));
        cout << x_data[i] << " " << y_data[i] << endl;
    }
    ceres::Problem problem;
    for (int i = 0; i < N; ++i)
    {
        problem.AddResidualBlock(new ceres::AutoDiffCostFunction<Curve_fitting_cost, 1, 3>(
                new Curve_fitting_cost(x_data[i], y_data[i])
                                 ), nullptr, abc
        );
    }
            ceres::Solver::Options options;
    options.linear_solver_type = ceres::DENSE_QR;
    options.minimizer_progress_to_stdout = true;

    ceres::Solver::Summary summary;
    chrono::steady_clock::time_point t1 = chrono::steady_clock::now();
    ceres::Solve(options, &problem,& summary);
    chrono::steady_clock::time_point t2 = chrono::steady_clock::now();

    chrono::duration<double> time_used = chrono::duration_cast<chrono::duration<double >>(t2 - t1);
    cout << summary.BriefReport() << endl;
    cout << "Use time = " << time_used.count() <<endl;
    cout << "estimated a, b, c = ";
    for ( auto a: abc)
    {
        cout << a << " ";
    }
    cout << endl;
    return 0;
}