from showresults import postprocess
import numpy as np
import os
import matplotlib.pyplot as plt

logZ = np.array([])

for i in range(0, 1000000):
    os.system("./main > junk")
    result = postprocess(plot=False, verbose=False)
    logZ = np.hstack([logZ, result["logZ"]])

    error = np.sqrt(np.mean((logZ - np.log(101))**2))
    print("Done {k} runs. RMS error = {error}".format(k=i+1, error=error))

