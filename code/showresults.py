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
logL = particles_info[:,2]

# Remove particles corresponding to run whose info isn't available yet
which = (run_id < levels_logX.shape[0])
run_id, level_id, logL = run_id[which], level_id[which], logL[which]

# Prior weights
num_particles = len(run_id)
logw = np.empty(num_particles)
for i in range(0, num_particles):
    logw[i] = levels_logX[run_id[i], level_id[i]]
logw -= dn4.logsumexp(logw)

logp = logw + logL
logZ = dn4.logsumexp(logp)
print(logZ)
logP = logp - logZ

plt.plot(np.exp(logP)[logp > -1E300], "k-")
plt.xlabel("Particle")
plt.ylabel("Posterior weight")
plt.show()

