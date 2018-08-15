import numpy as np

m = 0.3
c = 0.1

x = np.arange(0, 5, 0.075)
y = np.exp(x * m + c)

np.random.seed(123456)

noise = np.random.randn(np.size(x)) + 0.2

yObserved = y + noise
print(x.shape)
print(y.shape)
print(yObserved.shape)

for [ye, yoe] in zip(y, yObserved):
    print("{} to {}".format(ye, yoe))
