/*==============================================================================
Zero–Infinity Algebra (ZIA) — Official C++ Reference Implementation

Copyright (c) 2026 Mitchell Quinn. All rights reserved.

Licensed under the ZIA Research License (see LICENSE-RESEARCH.md).
This is not an OSI-approved open-source license.

Permitted (under the Research License): research/academia, teaching, evaluation,
and non-commercial experimentation.

Any production or operational use of any kind requires a Commercial License
(see LICENSE-COMMERCIAL.md).

Contact: mitchell.quinn@starcroft.net

Optional SPDX tag (recommended for tooling):
SPDX-License-Identifier: LicenseRef-ZIA-Research
==============================================================================*/


#pragma once

#include <string>
#include <utility>

class ZIAValue {
public:
    ZIAValue(double inf, double res);
    static ZIAValue infinity(double a);

    double get_inf() const { return inf; }
    double get_res() const { return res; }

    std::string to_string() const;
    std::string to_display_string(int precision = 6) const;


    ZIAValue operator+(const ZIAValue& other) const;
    ZIAValue operator-(const ZIAValue& other) const;
    ZIAValue operator*(const ZIAValue& other) const;
    ZIAValue operator/(const ZIAValue& other) const;

    bool operator==(const ZIAValue& other) const;
    bool operator!=(const ZIAValue& other) const;

    ZIAValue inverse() const;
    ZIAValue operator-() const;

    static ZIAValue from_real(double r);
    static ZIAValue zero();
    static ZIAValue one();

    // Helpers (exact / structural)
    bool is_zero() const noexcept;
    bool is_real() const noexcept;
    bool is_infinite() const noexcept;
    bool is_pure_infinity() const noexcept;
    ZIAValue with_inf(double new_inf) const;
    ZIAValue with_res(double new_res) const;
    std::pair<double, double> parts() const noexcept;

    // Conversions
    double as_real_strict() const;

    // Numerical convenience (ℝ-only)
    bool is_finite_near_zero(double tol) const noexcept;
    bool is_real_close(const ZIAValue& other, double tol) const noexcept;

    // Numerical convenience
    bool approx_equal_pair(const ZIAValue& other, double tol_inf, double tol_res) const noexcept;


    private:
    double inf;
    double res;

};