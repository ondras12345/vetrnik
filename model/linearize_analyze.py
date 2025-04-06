#!/usr/bin/env python3
import pickle
from linearize_result import Result
import control as ctl
import matplotlib.pyplot as plt
import numpy as np
import scipy

def export_mat(result, filename):
    scipy.io.savemat(filename, mdict={
        "A": result.A,
        "B": result.B,
        "C": result.C.astype(np.float32),
        "D": result.D.astype(np.float32),
    })


if __name__ == "__main__":
    with open("data/linearize.p", "rb") as f:
        p = pickle.load(f)

    # export first result for use in matlab
    export_mat(p["results"][0], "data/linearize_0.mat")

    systems = []
    for result in p["results"]:
        systems.append(ctl.ss(result.A, result.B, result.C, result.D).minreal())
    # transfer functions from b to omega
    b_tfs = [ctl.tf(sys)[0,0] for sys in systems]

    with open("data/linearize_b_tfs.p", "wb") as f:
        pickle.dump(b_tfs, f)

    # plot individual systems in parametric plane
    plt.title("Parametric plane")
    for tf in b_tfs:
        b0 = tf.num[0][0].item()
        a0 = tf.den[0][0][-1].item()
        plt.scatter(a0, b0)
    leg = [f"P_{{{i}}}" for i in range(len(systems))]
    plt.legend(leg)
    plt.xlabel("a_0")
    plt.ylabel("b_0")
    plt.grid()
    plt.show()

    # plot DC gains
    plt.title("DC gains")
    gains = [tf.dcgain() for tf in b_tfs]
    plt.bar(range(len(gains)), gains)
    plt.ylabel("dcgain")
    plt.xlabel("i (plant index)")
    plt.show()
