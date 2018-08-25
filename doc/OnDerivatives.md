# 关于导数

ceres与其他的基于梯度进行优化的算法类似,都是根据目标函数以及目标函数在定义域区间内的任意一点的导数.
在Ceres中,关键是如何根据实际的优化问题定义目标函数和雅克比行列式,如何正确的并且高效的定义雅克比行列式,是Ceres能有一个好的性能的关键.

Ceres提供了足够的灵活性,使用者可以按照自己的需求定义导数,并把导数提供给Ceres.具体有三种计算导数的形式:

1. 分析导数,Analytic Derivatives,是使用者根据自己优化问题,自己分析目标函数,并且手动或者利用其他工具计算导数(公式);
2. 数值导数,Numerical Derivatives,Ceres使用有限差分在数值上计算导数.(不是利用求导公式,而是利用有限差分计算导数);
3. 自动导数,Automatic Derivatives,Ceres自动的根据重载定义的模板和操作符自动计算分析导数.

这几种求导数的方式可以组合使用,也可以单独使用,根据应用场景和自己的期望进行权衡,但是在数值分析的教科书中,并没有详细的介绍这些细节,而是让用户自己决定.

ceres的这篇章节就是介绍这三种求导数的方法不同.

## Spivak Notation
这里面简要的介绍了关于导数的符号表示,对于单个变量很容易,对于多个变量,变量是向量的形式,那么对每个变量求偏导数,然后组合成矩阵的形式,这就是雅克比矩阵.

比如将一个维度是n的向量,经过函数变换到m维的向量,对应的雅克比矩阵是m*n.

## 分析导数

例子在文档中有,公式比较复杂,在[文档](http://www.ceres-solver.org/analytical_derivatives.html).

分析导数就是自己分析目标函数和需要的优化变量,自己给出雅克比行列式的计算,比如在文档中给出的代码,直接根据公式计算的.

```
class Rat43Analytic : public SizedCostFunction<1,4> {
   public:
     Rat43Analytic(const double x, const double y) : x_(x), y_(y) {}
     virtual ~Rat43Analytic() {}
     virtual bool Evaluate(double const* const* parameters,
                           double* residuals,
                           double** jacobians) const {
       const double b1 = parameters[0][0];
       const double b2 = parameters[0][1];
       const double b3 = parameters[0][2];
       const double b4 = parameters[0][3];

       residuals[0] = b1 *  pow(1 + exp(b2 -  b3 * x_), -1.0 / b4) - y_;

       if (!jacobians) return true;
       double* jacobian = jacobians[0];
       if (!jacobian) return true;

       jacobian[0] = pow(1 + exp(b2 - b3 * x_), -1.0 / b4);
       jacobian[1] = -b1 * exp(b2 - b3 * x_) *
                     pow(1 + exp(b2 - b3 * x_), -1.0 / b4 - 1) / b4;
       jacobian[2] = x_ * b1 * exp(b2 - b3 * x_) *
                     pow(1 + exp(b2 - b3 * x_), -1.0 / b4 - 1) / b4;
       jacobian[3] = b1 * log(1 + exp(b2 - b3 * x_)) *
                     pow(1 + exp(b2 - b3 * x_), -1.0 / b4) / (b4 * b4);
       return true;
     }

    private:
     const double x_;
     const double y_;
 };
```

这段代码还是有很多冗余计算,可以用一些临时的变量记住重复计算的中结果.

```
class Rat43AnalyticOptimized : public SizedCostFunction<1,4> {
   public:
     Rat43AnalyticOptimized(const double x, const double y) : x_(x), y_(y) {}
     virtual ~Rat43AnalyticOptimized() {}
     virtual bool Evaluate(double const* const* parameters,
                           double* residuals,
                           double** jacobians) const {
       const double b1 = parameters[0][0];
       const double b2 = parameters[0][1];
       const double b3 = parameters[0][2];
       const double b4 = parameters[0][3];

       const double t1 = exp(b2 -  b3 * x_);
       const double t2 = 1 + t1;
       const double t3 = pow(t2, -1.0 / b4);
       residuals[0] = b1 * t3 - y_;

       if (!jacobians) return true;
       double* jacobian = jacobians[0];
       if (!jacobian) return true;

       const double t4 = pow(t2, -1.0 / b4 - 1);
       jacobian[0] = t3;
       jacobian[1] = -b1 * t1 * t4 / b4;
       jacobian[2] = -x_ * jacobian[1];
       jacobian[3] = b1 * log(t2) * t3 / (b4 * b4);
       return true;
     }

   private:
     const double x_;
     const double y_;
 };
```

### 什么时候应该用分析导数

1. 表达式尽可能的简单,大多数是线性的;
2. 可以用一些工具(线性代数工具)计算目标函数的微分,并且产生C++代码;
3. 计算复杂度尽量要清晰,可以优化
4. 没有其他的方式计算导数
5. 使用者熟悉链式规则,并且喜欢手动求解导数.

## 数值导数

用极限的方式写出导数.公式在官网,自己看.

### 前向差分

这种看似最简单.

1. 首先定义一个残差块的计算;
2. 然后,使用NumericDiffCostFunction来定义损失函数.

下面这段代码就是这两个步骤:
```
struct Rat43CostFunctor {
  Rat43CostFunctor(const double x, const double y) : x_(x), y_(y) {}

  bool operator()(const double* parameters, double* residuals) const {
    const double b1 = parameters[0];
    const double b2 = parameters[1];
    const double b3 = parameters[2];
    const double b4 = parameters[3];
    residuals[0] = b1 * pow(1.0 + exp(b2 -  b3 * x_), -1.0 / b4) - y_;
    return true;
  }

  const double x_;
  const double y_;
}

CostFunction* cost_function =
  new NumericDiffCostFunction<Rat43CostFunctor, FORWARD, 1, 4>(
    new Rat43CostFunctor(x, y));
```

### 实现细节

这可能是最简单的方式,这样在于NumericDiffCostFunction这个类实现了通用的导数计算函数.导数的计算可以通过泰勒展开式进行近似计算.

这上面的一段代码,类似与用分析导数的方式获取.
```
class Rat43NumericDiffForward : public SizedCostFunction<1,4> {
   public:
     Rat43NumericDiffForward(const Rat43Functor* functor) : functor_(functor) {}
     virtual ~Rat43NumericDiffForward() {}
     virtual bool Evaluate(double const* const* parameters,
                           double* residuals,
                           double** jacobians) const {
       functor_(parameters[0], residuals);
       if (!jacobians) return true;
       double* jacobian = jacobians[0];
       if (!jacobian) return true;

       const double f = residuals[0];
       double parameters_plus_h[4];
       for (int i = 0; i < 4; ++i) {
         std::copy(parameters, parameters + 4, parameters_plus_h);
         const double kRelativeStepSize = 1e-6;
         const double h = std::abs(parameters[i]) * kRelativeStepSize;
         parameters_plus_h[i] += h;
         double f_plus;
         functor_(parameters_plus_h, &f_plus);
         jacobian[i] = (f_plus - f) / h;
       }
       return true;
     }

   private:
     scoped_ptr<Rat43Functor> functor_;
 };
```

### 中间差分
前向差分求导是可以的,但是不足够好,使用中间差分求导是更好的方式.具体公式看文档,残差块的计算还是一样的,不同的是在构建Cost Function的时候,模板参数不一样.

```
CostFunction* cost_function =
  new NumericDiffCostFunction<Rat43CostFunctor, CENTRAL, 1, 4>(
    new Rat43CostFunctor(x, y));
```

两者看起来并没有什么不同,只是在实际使用的时候,步长精度的不同,收敛速度和精度不同.

从官方文档中的图中可以看出(从右往左看):
1. 随着步长减小,误差降低,但是central 差分要比forward 差分收敛的更快,但是随着步长降低到一定数值,误差反而增加;
2. 能用central就不要用forward,除非是central方法带来的计算原大于收益(目标函数很复杂)
3. 当步长设置的很糟糕的时候,两种方法都没有效果

### Ridders Method

这种方法收敛速度更快,比central更好.

### 建议
当不能显示的计算出导数公式的时候,用数值导数或者自动差分导数;
当使用数值差分导数的时候,不要用forward,用central,如果不解释效率,或者不知道步长的时候,用ridder方法.

## 自动差分求导

与数值计算导数类似,先定一个函数对象,里面定义了残差块如何实现,然后定Cost function.
区别是,除了定义Cost function不同,在定义残差的时候也不同.

自动求导是利用模板函数,而数值求导是用的一个函数.

下面是自动求导的过程
```
struct Rat43CostFunctor {
  Rat43CostFunctor(const double x, const double y) : x_(x), y_(y) {}

  template <typename T>
  bool operator()(const T* parameters, T* residuals) const {
    const T b1 = parameters[0];
    const T b2 = parameters[1];
    const T b3 = parameters[2];
    const T b4 = parameters[3];
    residuals[0] = b1 * pow(1.0 + exp(b2 -  b3 * x_), -1.0 / b4) - y_;
    return true;
  }

  private:
    const double x_;
    const double y_;
};


CostFunction* cost_function =
      new AutoDiffCostFunction<Rat43CostFunctor, 1, 4>(
        new Rat43CostFunctor(x, y));
```

在数值求导的时候,使用的是
```
bool operator()(const double* parameters, double* residuals) const;
```

而自动求导是使用的

```
template <typename T> bool operator()(const T* parameters, T* residuals) const;
```

## 参考

