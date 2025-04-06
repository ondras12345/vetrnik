from dataclasses import dataclass
import numpy.typing as npt

@dataclass
class Result:
    vwind: float
    b: float
    A: npt.ArrayLike
    B: npt.ArrayLike
    C: npt.ArrayLike
    D: npt.ArrayLike
