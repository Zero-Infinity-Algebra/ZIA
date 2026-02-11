#include <stdexcept>
#include <cmath>
#include <sstream>
#include <iomanip>
#include "ZIAValue.h"


ZIAValue::ZIAValue(double inf, double res) {
    // Invariant: both components must be finite (no NaN, no +/-Inf)
    if (!std::isfinite(inf) || !std::isfinite(res)) {
        std::ostringstream oss;
        oss << "ZIA ZValue invariant violated: components must be finite (no NaN/Inf). "
            << "inf=" << inf << ", res=" << res;
        throw std::domain_error(oss.str());
    }

    // Canonicalize -0.0 to 0.0 for storage stability
    const double i = (inf == 0.0) ? 0.0 : inf;
    const double r = (res == 0.0) ? 0.0 : res;

    // Canonical zero
    if (i == 0.0 && r == 0.0) {
        this->inf = 0.0;
        this->res = 0.0;
    } else {
        this->inf = i;
        this->res = r;
    }
}



ZIAValue ZIAValue::infinity(double a) {
    return ZIAValue(a, 0.0);
}


std::string ZIAValue::to_string() const {
    return "inf=" + std::to_string(inf) + ", res=" + std::to_string(res);
}


std::string ZIAValue::to_display_string(int precision) const {
    // normalize -0.0 to 0.0 for printing stability
    const double i = (inf == 0.0) ? 0.0 : inf;
    const double r = (res == 0.0) ? 0.0 : res;

    std::ostringstream oss;
    oss << std::setprecision(precision) << std::defaultfloat;

    if (i == 0.0 && r == 0.0) {
        return "0";
    }

    if (i == 0.0) {
        oss << r;
        return oss.str();
    }

    if (r == 0.0) {
        oss << "INF(" << i << ")";
        return oss.str();
    }

    oss << "INF(" << i << ") (+) " << r;
    return oss.str();
}


// Helpers

ZIAValue ZIAValue::zero() {
    return ZIAValue(0.0, 0.0);
}

ZIAValue ZIAValue::one() {
    return ZIAValue(0.0, 1.0);
}

bool ZIAValue::is_zero() const noexcept {
    return inf == 0.0 && res == 0.0;
}

bool ZIAValue::is_real() const noexcept {
    return inf == 0.0;
}

bool ZIAValue::is_infinite() const noexcept {
    return inf != 0.0;
}

ZIAValue ZIAValue::from_real(double r) {
    return ZIAValue(0.0, r);   // ∞₀ ⊕ r
}

bool ZIAValue::is_pure_infinity() const noexcept {
    return inf != 0.0 && res == 0.0;
}

ZIAValue ZIAValue::with_inf(double new_inf) const {
    return ZIAValue(new_inf, res);
}

ZIAValue ZIAValue::with_res(double new_res) const {
    return ZIAValue(inf, new_res);
}

std::pair<double, double> ZIAValue::parts() const noexcept {
    return {inf, res};
}



// Conversions
double ZIAValue::as_real_strict() const {
    if (inf == 0.0) {
        return res;
    }

    throw std::runtime_error(
        "ZIA as_real_strict undefined: value has nonzero infinity component"
    );
}


// Numerical convenience (ℝ-only)
bool ZIAValue::is_finite_near_zero(double tol) const noexcept {
    return inf == 0.0 && std::fabs(res) <= tol;
}


bool ZIAValue::is_real_close(const ZIAValue& other, double tol) const noexcept {
    if (inf != 0.0 || other.inf != 0.0) {
        return false;
    }
    return std::fabs(res - other.res) <= tol;
}

// Numerical convenience
bool ZIAValue::approx_equal_pair(const ZIAValue& other, double tol_inf, double tol_res) const noexcept {
    return (std::fabs(inf - other.inf) <= tol_inf) &&
           (std::fabs(res - other.res) <= tol_res);
}


// Operators

ZIAValue ZIAValue::operator+(const ZIAValue& other) const {
    return ZIAValue(
        this->inf + other.inf,
        this->res + other.res
    );
}


ZIAValue ZIAValue::operator-() const {
    return ZIAValue(-inf, -res);
}


ZIAValue ZIAValue::operator-(const ZIAValue& other) const {
    return ZIAValue(
        this->inf - other.inf,
        this->res - other.res
    );
}


ZIAValue ZIAValue::operator*(const ZIAValue& other) const {
    const double a = this->inf;
    const double r = this->res;
    const double b = other.inf;
    const double s = other.res;

    return ZIAValue(
        a * b + r * b + s * a,  // divergence index
        r * s                  // finite residue
    );
}


ZIAValue ZIAValue::operator/(const ZIAValue& other) const
{
    const double a = this->inf;
    const double r = this->res;
    const double b = other.inf;
    const double s = other.res;

    // 0 / 0  -> collapse
    if (a == 0.0 && r == 0.0 && b == 0.0 && s == 0.0) {
        return ZIAValue(0.0, 0.0);
    }

    // real / real
    if (a == 0.0 && b == 0.0 && s != 0.0) {
        return ZIAValue(0.0, r / s);
    }

    // real / 0  -> expansion
    if (a == 0.0 && b == 0.0 && s == 0.0) {
        return ZIAValue(r, 0.0);
    }

    // real / infinity -> collapse
    if (a == 0.0 && b != 0.0) {
        return ZIAValue(0.0, 0.0);
    }

    // infinity / real -> scale divergence
    if (a != 0.0 && b == 0.0 && s != 0.0) {
        return ZIAValue(a / s, 0.0);
    }

    // infinity / infinity -> ratio of indices
    if (a != 0.0 && b != 0.0) {
        return ZIAValue(0.0, a / b);
    }

    // infinity / 0 -> stable divergence
    if (a != 0.0 && b == 0.0 && s == 0.0) {
        return ZIAValue(a, 0.0);
    }

    // Should be unreachable
    return ZIAValue(0.0, 0.0);
}


ZIAValue ZIAValue::inverse() const {
    // Only embedded non-zero reals are invertible in ZIA v0.5.x
    if (inf == 0.0 && res != 0.0) {
        return ZIAValue(0.0, 1.0 / res);
    }

    throw std::runtime_error(
        "ZIA inverse undefined: only (inf=0, res≠0) are invertible"
    );
}

// equality operators

bool ZIAValue::operator==(const ZIAValue& other) const {
    return this->inf == other.inf && this->res == other.res;
}

bool ZIAValue::operator!=(const ZIAValue& other) const {
    return !(*this == other);
}
