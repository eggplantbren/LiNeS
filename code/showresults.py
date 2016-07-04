import numpy as np
import matplotlib.pyplot as plt

# Load and plot the levels
lns_log = np.loadtxt("lns_levels.txt")

logL = lns_log[0, :]
logX = np.empty(lns_log.shape[1])
for i in range(0, len(logX)):
    temp = lns_log[1:,i]
    biggest = np.max(temp)
    logX[i] = np.log(np.sum(np.exp(temp - biggest))) + biggest - np.log(len(temp))
good = logX > -1E300

plt.plot(logX[good], logL[good], "ko")
plt.xlabel("$\\ln(X)$", fontsize=16)
plt.ylabel("$\\ln(L)$", fontsize=16)
plt.show()

