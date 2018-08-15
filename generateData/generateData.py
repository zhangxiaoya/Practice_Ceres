import numpy as np
import matplotlib.pyplot as mpl

m = 0.3
c = 0.1

x = np.arange(0, 5, 0.075)
y = np.exp(x * m + c)

np.random.seed(123456)

noise = np.random.randn(np.size(x)) * 0.2

yObserved = y + noise
print(x.shape)
print(y.shape)
print(yObserved.shape)

for [ye, yoe] in zip(y, yObserved):
    print("{} to {}".format(ye, yoe))

mpl.figure()
mpl.plot(x, y, 'g*-')
mpl.plot(x, yObserved, 'r--')
mpl.xlabel("x")
mpl.ylabel("y")
mpl.show()


# plot reference: https://www.cnblogs.com/nxld/p/7435930.html