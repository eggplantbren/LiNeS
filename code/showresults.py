import numpy as np
import numpy.random as rng
import matplotlib.pyplot as plt
import dnest4.classic as dn4

def postprocess(plot=True, verbose=True, single_precision=False):
    # Load and plot the levels
    levels_logL = dn4.my_loadtxt("levels_logL.txt").flatten()
    levels_logX = dn4.my_loadtxt("levels_logX.txt")

    num_levels = len(levels_logL)

    # Loop over levels and average the X estimates
    logX = np.empty(num_levels)
    for i in range(0, num_levels):
        this_level = levels_logX[:,0] == i+1
        num_runs = this_level.sum()
        logX[i] = dn4.logsumexp(levels_logX[this_level,1])\
                            - np.log(num_runs)
    good = (logX > -1E300)

    # Make another version of logX with the prior in it
    logX_full = np.hstack([0.0, logX])

    # Load the particles
    particles_info = dn4.my_loadtxt("particles_info.txt")
    run_id = particles_info[:,0].astype("int64") - 1
    level_id = particles_info[:,1].astype("int64")
    logL = particles_info[:,2]

    # Remove particles corresponding to run whose info isn't available yet
    which = np.nonzero(run_id < levels_logX.shape[0])[0]
    run_id, level_id, logL = run_id[which], level_id[which], logL[which]
    num_particles = len(run_id)

    # Count the number of particles with each level id
    level_id_counts = np.zeros(1+level_id.max(), dtype="int64")
    for i in range(0, num_particles):
        level_id_counts[level_id[i]] += 1

    # Prior weights
    logw = np.empty(num_particles)
    for i in range(0, num_particles):
        logw[i] = dn4.logdiffexp(logX_full[level_id[i]],
                                    logX_full[level_id[i]+1])\
                    - np.log(level_id_counts[level_id[i]])

    logp = logw + logL
    logZ = dn4.logsumexp(logp)
    logP = logp - logZ
    H = np.sum(np.exp(logP)*(logP - logw))
    ESS = int(np.exp(-np.sum(np.exp(logP)*logP)))

    # Resample to uniform weight
    W = np.exp(logp - np.max(logp))
    rows = np.empty(ESS, dtype="int64")
    for i in range(0, ESS):
        while True:
            ii = which[rng.randint(len(which))]
            if np.random.rand() <= W[ii]:
                break
        rows[i] = ii

    sample = dn4.loadtxt_rows("particles.txt", set(rows), single_precision)
    posterior_sample = None

    if single_precision:
        posterior_sample = np.empty((ESS, sample["ncol"]), dtype="float32")
    else:
        posterior_sample = np.empty((ESS, sample["ncol"]))

    for i in range(0, ESS):
        posterior_sample[i, :] = sample[rows[i]]

    if single_precision:
        np.savetxt("posterior_sample.txt", posterior_sample, fmt="%.7e")
    else:
        np.savetxt("posterior_sample.txt", posterior_sample)
    np.savetxt("weights.txt", W)

    if verbose:
        print("ln(Z) =", logZ)
        print("H =", H, "nats")
        print("Effective sample size =", ESS)

    if plot:
        plt.figure(1)
        plt.plot(logX[good], levels_logL[good], "ko-")
        plt.xlabel("$\\ln(X)$", fontsize=16)
        plt.ylabel("$\\ln(L)$", fontsize=16)
        plt.title("Levels")

        plt.figure(2)
        yy = np.diff(logX[good])
        xx = np.arange(0, len(yy)) + 0.5

        plt.plot(xx, yy, "k-")
        plt.xlabel("Level")
        plt.ylabel("$\\ln(X)$ Gap")
        plt.axhline(-1.0, color="g")
        plt.ylim([1.05*yy.min(), 0.05])

        plt.figure(3)
        plt.plot(np.exp(logP)[logp > -1E300], "k-")
        plt.xlabel("Particle")
        plt.ylabel("Posterior weight")
        plt.show()

    return {"logZ": logZ}

if __name__ == "__main__":
    postprocess()


