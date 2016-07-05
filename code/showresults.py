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
run_id = particles_info[:,0].astype("int64")
level_id = particles_info[:,1].astype("int64")

# Prior weights
logw = np.empty(particles_info.shape[0])

# Loop over runs
for i in range(0, np.max(run_id) + 1):
    # Find particles from this run
    which = (run_id == i)
    _particles_info = particles_info[which, :]
    logw[which] = levels_logX[level_id[which]]

logw = logw - dn4.logsumexp(logw)
logp = logw + particles_info[:,2]
logZ = dn4.logsumexp(logp)
print(logZ)

plt.plot(np.exp(logp - logZ), "k")
plt.show()

