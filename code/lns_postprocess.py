import numpy as np
import matplotlib.pyplot as plt

lns_log = np.loadtxt("lns_log.txt")

logL = lns_log[0, :]
logX = np.empty(lns_log.shape[1])
for i in range(0, len(logX)):
    temp = lns_log[1:,i]
    biggest = np.max(temp)
    logX[i] = np.log(np.sum(np.exp(temp - biggest))) + biggest - np.log(len(temp))

plt.plot(logX, logL, "ko")
plt.xlabel("$\\ln(X)$", fontsize=16)
plt.ylabel("$\\ln(L)$", fontsize=16)
plt.show()

