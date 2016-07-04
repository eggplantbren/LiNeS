import numpy as np
import matplotlib.pyplot as plt
import dnest4.classic as dn4

# Load and plot the levels
levels_logL = dn4.my_loadtxt("levels_logL.txt").flatten()
levels_logX = dn4.my_loadtxt("levels_logX.txt")

num_runs, num_levels = levels_logX.shape

# Loop over levels and average the X estimates
logX = np.empty(num_levels)
for i in range(0, num_levels):
    logX[i] = dn4.logsumexp(levels_logX[:, i]) - np.log(num_runs)
good = (logX > -1E300)

plt.plot(logX[good], levels_logL[good], "ko")
plt.xlabel("$\\ln(X)$", fontsize=16)
plt.ylabel("$\\ln(L)$", fontsize=16)
plt.show()

# Load the particles
particles_info = dn4.my_loadtxt("particles_info.txt")

