import numpy as np
import numpy.random as rng
import matplotlib.pyplot as plt
import dnest4.classic as dn4

def postprocess(plot=True, verbose=True, single_precision=False):
    # Load and plot the levels
    levels_logL = dn4.my_loadtxt("levels_logL.txt").flatten()
    levels_logX = dn4.my_loadtxt("levels_logX.txt")

    num_runs, num_levels = levels_logX.shape

    # Loop over levels and average the X estimates
    logX = np.empty(num_levels)
    for i in range(0, num_levels):
        logX[i] = dn4.logsumexp(levels_logX[:, i]) - np.log(num_runs)
    good = (logX > -1E300)

    # Load the particles
    particles_info = dn4.my_loadtxt("particles_info.txt")
    run_id = particles_info[:,0].astype("int64")
    level_id = particles_info[:,1].astype("int64")
    logL = particles_info[:,2]

    # Remove particles corresponding to run whose info isn't available yet
    which = np.nonzero(run_id < levels_logX.shape[0])[0]
    run_id, level_id, logL = run_id[which], level_id[which], logL[which]

    # Prior weights
    num_particles = len(run_id)
    logw = np.empty(num_particles)
    for i in range(0, num_particles):
        logw[i] = levels_logX[run_id[i], level_id[i]]
    logw -= dn4.logsumexp(logw)

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
        plt.plot(np.exp(logP)[logp > -1E300], "k-")
        plt.xlabel("Particle")
        plt.ylabel("Posterior weight")
        plt.show()

    return {"logZ": logZ}

if __name__ == "__main__":
    postprocess()


