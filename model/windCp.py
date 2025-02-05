#!/usr/bin/env python3
import math
import numpy as np
import pandas as pd
#import matplotlib.pyplot as plt
from OMPython import OMCSessionZMQ
omc = OMCSessionZMQ()

omc.sendExpression('loadModel(Modelica)')
omc.sendExpression('setModelicaPath(".")')
omc.sendExpression('loadModel(Vetrnik.windCp)')
beta = np.deg2rad(np.linspace(0, 20, 5))
lambdas = len(beta)*[x[0] for x in omc.sendExpression("lambda := [0.01:.01:20]")]
Cp = []
betas = []
for b in beta:
    print("beta=", b)
    c = [x[0] for x in omc.sendExpression(f"Vetrnik.windCp(lambda, {b})")]
    Cp.extend(c)
    betas.extend([b] * len(c))

data = {"beta": betas, "lambda": lambdas, "Cp": Cp}
# print({x: len(data[x]) for x in data})
df = pd.DataFrame(data)
df.to_csv("windCp.csv", index=False)

# with pd.plotting.plot_params.use("x_compat", True):
#     for b in beta:
#         df[df.beta == b].plot(x="lambda", y="Cp", )
# plt.grid()
# plt.show()
