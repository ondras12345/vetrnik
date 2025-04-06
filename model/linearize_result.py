from dataclasses import dataclass
import numpy as np
import numpy.typing as npt


def _array_eq(arr1, arr2):
    return (isinstance(arr1, np.ndarray) and
            isinstance(arr2, np.ndarray) and
            arr1.shape == arr2.shape and
            (arr1 == arr2).all())


@dataclass(eq=False)
class Result:
    vwind: float
    b: float
    tsr: float  # lambda (tip speed ratio)
    A: npt.ArrayLike
    B: npt.ArrayLike
    C: npt.ArrayLike
    D: npt.ArrayLike

    def __eq__(self, other):
        if not isinstance(other, Result):
            return False
        else:
            return (
                _array_eq(self.A, other.A)
                and _array_eq(self.B, other.B)
                and _array_eq(self.C, other.C)
                and _array_eq(self.D, other.D)
                and self.vwind == other.vwind
                and self.b == other.b
            )
