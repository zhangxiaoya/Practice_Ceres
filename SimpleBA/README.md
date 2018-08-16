# Bundle Adjustment

开发Ceres库的一个主要原因是需要解决大规模的Bundle Adjustment问题,关于大规模Bundle Adjustment问题的介绍在在MVG那本书中有介绍,另外,还有一篇文章,叫做 [Bundle Adjustment — A Modern Synthesis](https://hal.inria.fr/inria-00548290/document).

给定一些图像特征的位置以及这些图像特征的关联信息,Bundle Adjustment的目的是找到这些特征点的3D点位置和相机参数,这些位置和参数信息满足最小化重投影误差. 这些优化的问题通常构建一个非线性最小二乘问题,这个最小二乘问题的误差是一个观测到的图像特征位置与对应的3D点位置在图像上重投影的误差的L2笵数的平方, Ceres提供丰富的支持用来解决Bundle Adjustment问题.

## Bundle Adjustment数据集

有一个公开的数据集叫做BAL,是Bundle Adjustment in the Large的简写.地址在[Bundle Adjustment in the Large](http://grail.cs.washington.edu/projects/bal/)

## 定义Cost function
第一步,通常是顶一个函数对象,用来计算重投影误差或者残差,这个函数对象与例子中的指数曲线拟合类似,每一个函数对象的实例都对应与一个图像观测.

在BAL数据库中问题,定义残差依赖于一个三维的点坐标和一个9个维度的相机参数(内参+外参).其中9个相机参数包含旋转向量(3个变量),平移向量(3个变量),一个焦距和两个畸变扰动,这些参数的详细信息在BAL的[HomePage](http://grail.cs.washington.edu/projects/bal/)和[Bundler Homepage](http://www.cs.cornell.edu/~snavely/bundler/)

下面是定义残差函数对象
```
struct SnavelyProjectError
{
    SnavelyProjectError(double observed_x, double observed_y):
        observed_x(observed_x),
        observed_y(observed_y){}
    template<typename T>
    bool operator()(const T* const camera,, const T* const point) const
    {
        T p[3];
        ceres::AngleAxisRotatePoint(camera, point, p);
        p[0] += camera[3];
        p[1] += camera[4];
        p[2] += camera[5];

        T xp = - p[0] / p[2];
        T yp = - p[1] / p[2];

        const T& l1 = camera[7];
        const T& l2 = camera[8];
        T r2 = xp * xp + yp * yp;
        T distortion = T(1.0) + r2 * (l1 + l2 * l2);

        const T& focal = camera[6];
        T prediced_x = fotal * distortion * xp;
        T prediced_y = fotal * distortion * yp;

        residual[0] = prediced_x - T(observed_x);
        residual[1] = prediced_y - T(observed_y);
        return true;
    }

    double observed_x;
    double observed_y
};
```
这个残差函数对象的定义明显与之前的例子不同,之前的例子都是一些无用的简单的计算(对应的最小二乘问题都可以用解析解的方式获取),这里的残差函数对象计算很复杂,包含真实问题的重投影误差计算的详细过程.

同时,计算这么一个残差函数的分析雅克比矩阵很困难,但是在ceres张的自动查分求导接口让求解雅克比矩阵变得更简单,关于计算旋转矩阵的函数在ceres中也有定义.

给定这么一个函数对象,然后构造一个Bundle Adjustment问题.

```
ceres::Problem problem;
for(int i = 0;i< bal_problem.num_observations(); ++i>)
{
    ceres::CostFunction* costFunction = SnavelyReprojectionError::Create(bal_problem.observations()[2 * i + 0],
    bal_problem.observations()[2 * i + 1]);
    problem.AddResidualBlock(costFunction, NULL,bal_problem.mutable_camera_for_observation(i),
    bal_problem.mutable_point_for_observation(i));
}
```

从上面构造的Bundle Adjustment的来看与之前介绍的曲线拟合的例子类似,每一个观测都添加到残差里面.

由于这是一个非常大的稀疏问题,一种解决这种问题的方式是把Solver设置为稀疏求解.
```
Solver::Options::linear_solver_type = SPARSE_NORMAL_CHOLESKY;
```

这样做是有原因的,因为如果问题是稀疏的,使用稀疏求解的方式能加快问题求解.Ceres提供三种特殊的Solver,总的被乘坐是给予Schur的Solver,这个例子中使用的是DENSE_SCHUR.

```
ceres::Solver::Options options;
options.linear_solver_type = ceres::DENSE_SCHUR;
options.minimizer_progress_to_stdout = true;
ceres::Solver::Summary summary;
ceres::Solve(options, &problem, &summary);
std::cout << summary.FullReport() << std::endl;
```

对于更复杂的Bundle Adjustment例子,使用更高级的 Bundle Adjustment特征,比如多种线性的Solver和更鲁棒的损失函数局部参数,在其他的例子中.

