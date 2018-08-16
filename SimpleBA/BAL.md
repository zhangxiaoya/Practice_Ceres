# Bundle Adjustment in the Large

最近有关SfM的工作表明从大规模的图像中恢复出几何结构是可能的,Bundle Adjustment是相机参数和点参数进行非线性联合调优问题,是SfM系统的关键,但是它的计算量很大,消耗存储也很大,随着图像集合的数量增加,Bundle Adjustment的尺度适应问题是目前很关键的问题.

在华盛顿大学的CRAIL实验室,完成这样一个工程,探索大规模的SfM问题的Bundle Adjustment.

## 数据
两种类型的数据

1. 在行驶的车辆上安装的Ladybug摄像头获取的图像,每个图像都有一个时间戳,用来匹配图像顺序和GPS信息;
2. 从Flickr.com下载的数据,然后使用[Building Rome in One Day](http://grail.cs.washington.edu/rome/)的系统匹配,使用的数据包括Trafalgar Square and the cities of Dubrovnik, Venice, and Rome.

## 相机模型

这里使用的针孔相机模型,每个相机(时间不同)都有这样几个参数:
- R 旋转矩阵,
- t 平移向量
- f 焦距
- k1,k2 径向扰动参数

根据这些参数,将空间中的3D点坐标 X,映射到2维的图像坐标需要经过以下几个步骤.

1. P  =  R * X + t    将点坐标从世界坐标系变换到相机坐标系
2. p  = -P / P.z      透视法,实际上将坐标进行其次处理
3. p' =  f * r(p) * p 将坐标转换成图像坐标系

其中r(p)是径向扰动函数,定义如下
r(p) = 1.0 + k1 * ||p||^2 + k2 * ||p||^4.

通过上面的映射,获取了图像坐标系下的像素点的坐标,在这个图像坐标系下,图像的原点是图像的正中心,x轴右侧是正,左侧是负,y轴上侧是正,下侧是负,y轴后向是正,前向是负.

## 数据格式

数据都是文本文件,但是都通过bzip2进行压缩打包,需要解压.

```
bzip2 -d problem-49-7776-pre.txt.bz2
```

在官网的格式给定的这样的
```
<num_cameras> <num_points> <num_observations>
<camera_index_1> <point_index_1> <x_1> <y_1>
...
<camera_index_num_observations> <point_index_num_observations> <x_num_observations> <y_num_observations>
<camera_1>
...
<camera_num_cameras>
<point_1>
...
<point_num_points>
```

前面的两行能对的上数据,
文件第一行就三个整数:
相机数量,点的数量,观测数量
后面每一行表示:相机编号,点编号,位置信息.

再往下对不上了.

