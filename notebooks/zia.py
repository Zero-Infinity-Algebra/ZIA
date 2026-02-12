#==============================================================================
# Zero–Infinity Algebra (ZIA) — Official C++ Reference Implementation
# 
# Copyright (c) 2026 Mitchell Quinn. All rights reserved.
# 
# Licensed under the ZIA Research License (see LICENSE-RESEARCH.md).
# This is not an OSI-approved open-source license.
# 
# Permitted (under the Research License): research/academia, teaching, evaluation,
# and non-commercial experimentation.
# 
# Any production or operational use of any kind requires a Commercial License
# (see LICENSE-COMMERCIAL.md).
# 
# Contact: mitchell.quinn@starcroft.net
# 
# Optional SPDX tag (recommended for tooling):
# SPDX-License-Identifier: LicenseRef-ZIA-Research
#==============================================================================

"""
AUTO-GENERATED FILE — DO NOT EDIT.

This Python implementation exists solely to support the Jupyter notebooks in this repository
with minimal friction. It is generated from the C++ reference implementation.

Source of truth: the C++ implementation in this repository (ZIACore / ZIAValue).
If you need changes, change the C++ and regenerate this file.
"""


from __future__ import annotations

from dataclasses import dataclass
import numbers
from typing import Tuple, Any


def _as_float(x: Any) -> float:
    if isinstance(x, numbers.Real):
        return float(x)
    raise TypeError(f"expected a real number, got {type(x).__name__}")


def _cpp_to_string(x: float) -> str:
    # Closest analogue to std::to_string(double) (fixed, 6 decimals)
    return format(x, "f")


def _defaultfloat(x: float, precision: int) -> str:
    # Closest analogue to iostream defaultfloat + setprecision
    return format(x, f".{precision}g")


@dataclass(frozen=True, slots=True)
class ZValue:
    inf: float
    res: float

    def __post_init__(self) -> None:
        i = _as_float(self.inf)
        r = _as_float(self.res)
        # Canonicalise (0,0) to (+0.0,+0.0) as in the C++ ctor.
        if i == 0.0 and r == 0.0:
            i = 0.0
            r = 0.0
        object.__setattr__(self, "inf", i)
        object.__setattr__(self, "res", r)

    # --- constructors (static) ---

    @staticmethod
    def infinity(a: Any) -> "ZValue":
        return ZValue(_as_float(a), 0.0)

    @staticmethod
    def from_real(r: Any) -> "ZValue":
        return ZValue(0.0, _as_float(r))

    # --- common constants ---

    @staticmethod
    def zero() -> "ZValue":
        return ZValue(0.0, 0.0)

    @staticmethod
    def one() -> "ZValue":
        return ZValue(0.0, 1.0)

    # --- string representations ---

    def __str__(self) -> str:
        return f"inf={_cpp_to_string(self.inf)}, res={_cpp_to_string(self.res)}"

    def __repr__(self) -> str:
        return f"ZValue({self.__str__()})"

    def to_nice_string(self, precision: int = 6) -> str:
        # Normalise -0.0 to 0.0 for printing stability.
        i = 0.0 if self.inf == 0.0 else self.inf
        r = 0.0 if self.res == 0.0 else self.res

        if i == 0.0 and r == 0.0:
            return "0"
        if i == 0.0:
            return _defaultfloat(r, precision)
        if r == 0.0:
            return f"INF({_defaultfloat(i, precision)})"
        return f"INF({_defaultfloat(i, precision)}) (+) {_defaultfloat(r, precision)}"

    # --- arithmetic operators ---

    def __neg__(self) -> "ZValue":
        return ZValue(-self.inf, -self.res)

    def __add__(self, other: Any):
        if isinstance(other, ZValue):
            return ZValue(self.inf + other.inf, self.res + other.res)
        if isinstance(other, numbers.Real):
            return self + ZValue.from_real(float(other))
        return NotImplemented

    def __radd__(self, other: Any):
        if isinstance(other, numbers.Real):
            return ZValue.from_real(float(other)) + self
        return NotImplemented

    def __sub__(self, other: Any):
        if isinstance(other, ZValue):
            return ZValue(self.inf - other.inf, self.res - other.res)
        if isinstance(other, numbers.Real):
            return self - ZValue.from_real(float(other))
        return NotImplemented

    def __rsub__(self, other: Any):
        if isinstance(other, numbers.Real):
            return ZValue.from_real(float(other)) - self
        return NotImplemented

    def __mul__(self, other: Any):
        if isinstance(other, ZValue):
            a = self.inf
            r = self.res
            b = other.inf
            s = other.res
            return ZValue(
                a * b + r * b + s * a,  # divergence index
                r * s,                  # finite residue
            )
        if isinstance(other, numbers.Real):
            return self * ZValue.from_real(float(other))
        return NotImplemented

    def __rmul__(self, other: Any):
        if isinstance(other, numbers.Real):
            return ZValue.from_real(float(other)) * self
        return NotImplemented

    def __truediv__(self, other: Any):
        if isinstance(other, numbers.Real) and not isinstance(other, ZValue):
            return self / ZValue.from_real(float(other))
        if not isinstance(other, ZValue):
            return NotImplemented

        a = self.inf
        r = self.res
        b = other.inf
        s = other.res

        # 0 / 0  -> collapse
        if a == 0.0 and r == 0.0 and b == 0.0 and s == 0.0:
            return ZValue(0.0, 0.0)

        # real / real
        if a == 0.0 and b == 0.0 and s != 0.0:
            return ZValue(0.0, r / s)

        # real / 0  -> expansion
        if a == 0.0 and b == 0.0 and s == 0.0:
            return ZValue(r, 0.0)

        # real / infinity -> collapse
        if a == 0.0 and b != 0.0:
            return ZValue(0.0, 0.0)

        # infinity / real -> scale divergence
        if a != 0.0 and b == 0.0 and s != 0.0:
            return ZValue(a / s, 0.0)

        # infinity / infinity -> ratio of indices
        if a != 0.0 and b != 0.0:
            return ZValue(0.0, a / b)

        # infinity / 0 -> stable divergence
        if a != 0.0 and b == 0.0 and s == 0.0:
            return ZValue(a, 0.0)

        # Should be unreachable
        return ZValue(0.0, 0.0)

    def __rtruediv__(self, other: Any):
        if isinstance(other, numbers.Real):
            return ZValue.from_real(float(other)) / self
        return NotImplemented

    # --- comparisons ---

    def __eq__(self, other: Any):
        if not isinstance(other, ZValue):
            return NotImplemented
        return self.inf == other.inf and self.res == other.res

    def __ne__(self, other: Any):
        if not isinstance(other, ZValue):
            return NotImplemented
        return not (self == other)

    # --- helpers (exact / structural) ---

    def is_zero(self) -> bool:
        return self.inf == 0.0 and self.res == 0.0

    def is_real(self) -> bool:
        return self.inf == 0.0

    def is_infinite(self) -> bool:
        return self.inf != 0.0

    def is_pure_infinity(self) -> bool:
        return self.inf != 0.0 and self.res == 0.0

    def with_inf(self, new_inf: Any) -> "ZValue":
        return ZValue(_as_float(new_inf), self.res)

    def with_res(self, new_res: Any) -> "ZValue":
        return ZValue(self.inf, _as_float(new_res))

    def parts(self) -> Tuple[float, float]:
        return (self.inf, self.res)

    # --- conversions ---

    def as_real_strict(self) -> float:
        if self.inf == 0.0:
            return self.res
        raise RuntimeError(
            "ZIA as_real_strict undefined: value has nonzero infinity component"
        )

    def __float__(self) -> float:
        try:
            return self.as_real_strict()
        except RuntimeError as e:
            # pybind translates std::runtime_error -> ValueError
            raise ValueError(str(e)) from None

    # --- numerical convenience (ℝ-only) ---

    def is_finite_near_zero(self, tol: Any) -> bool:
        t = _as_float(tol)
        return self.inf == 0.0 and abs(self.res) <= t

    def is_real_close(self, other: "ZValue", tol: Any) -> bool:
        t = _as_float(tol)
        if self.inf != 0.0 or other.inf != 0.0:
            return False
        return abs(self.res - other.res) <= t

    # --- numerical convenience ---

    def approx_equal_pair(self, other: "ZValue", tol_inf: Any, tol_res: Any) -> bool:
        ti = _as_float(tol_inf)
        tr = _as_float(tol_res)
        return abs(self.inf - other.inf) <= ti and abs(self.res - other.res) <= tr

    # --- explicit inverse ---

    def inverse(self) -> "ZValue":
        # Only embedded non-zero reals are invertible in ZIA v0.5.x
        if self.inf == 0.0 and self.res != 0.0:
            return ZValue(0.0, 1.0 / self.res)

        raise RuntimeError(
            "ZIA inverse undefined: only (inf=0, res≠0) are invertible"
        )


__all__ = ["ZValue"]
