// #error "THIS IS THE TEST_ZVALUE.CPP I EXPECT TO BE RUNNING"

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"

#include <cmath>
#include "include/ZIAValue.h"

// The following must NOT compile:
//
// ZValue z(1.0, 2.0);
// z.inf = 3.0;   // error: inf is private
// z.res = 4.0;   // error: res is private
//
// ZValue is immutable after construction.



static_assert(
    !std::is_assignable_v<double&, ZIAValue>,
    "ZValue components must not be assignable"
);

// #1 - 1.1.1
// TODO: IMPORTANT! NEW TEST CASES - UPDATE JUPYTER NOTEBOOK TEST SUITE! 
TEST_CASE("collapse: infinity-zero plus zero canonicalises to zero") {
    ZIAValue collapsed(0.0, 0.0);     // ∞₀ ⊕ 0
    ZIAValue zero_from_real = ZIAValue::from_real(0.0);
    ZIAValue zero_const = ZIAValue::zero();

    CHECK(collapsed == zero_from_real);
    CHECK(collapsed == zero_const);
}


// #1.1
// TODO: IMPORTANT! NEW TEST CASES - UPDATE JUPYTER NOTEBOOK TEST SUITE! 
TEST_CASE("constant: one is embedded real 1") {
    ZIAValue one = ZIAValue::one();
    CHECK(one.get_inf() == 0.0);
    CHECK(one.get_res() == 1.0);
}


// #2 - 1.1.2
TEST_CASE("collapse: no infinity-zero leaks via cancellation") {
    ZIAValue inf_pos(5.0, 0.0);   // ∞₅
    ZIAValue inf_neg(-5.0, 0.0);  // ∞₋₅

    ZIAValue sum = inf_pos + inf_neg;   // should collapse

    ZIAValue zero = ZIAValue::from_real(0.0);

    CHECK(sum == zero);
    CHECK(sum.get_inf() == 0.0);
    CHECK(sum.get_res() == 0.0);
}

// #3 - 1.2.1
TEST_CASE("real embedding: from_real(r) equals infinity-zero plus r") {
    ZIAValue x = ZIAValue::from_real(3.5);

    CHECK(x.get_inf() == 0.0);
    CHECK(x.get_res() == 3.5);
}

// #4 - 1.2.2
TEST_CASE("real embedding: addition preserved") {
    ZIAValue a = ZIAValue::from_real(2.0);
    ZIAValue b = ZIAValue::from_real(3.0);

    ZIAValue sum = a + b;
    ZIAValue expected = ZIAValue::from_real(5.0);

    CHECK(sum == expected);
    CHECK(sum.get_inf() == 0.0);
    CHECK(sum.get_res() == 5.0);
}


// #5 - 1.2.3
TEST_CASE("real embedding: multiplication preserved") {
    ZIAValue a = ZIAValue::from_real(4.0);
    ZIAValue b = ZIAValue::from_real(2.5);

    ZIAValue product = a * b;
    ZIAValue expected = ZIAValue::from_real(10.0);

    CHECK(product == expected);
    CHECK(product.get_inf() == 0.0);
    CHECK(product.get_res() == 10.0);
}


// #6 - 2.1.1
TEST_CASE("addition axiom: componentwise addition of full pairs") {
    ZIAValue x(2.0, 3.0);   // ∞₂ ⊕ 3
    ZIAValue y(5.0, 7.0);   // ∞₅ ⊕ 7

    ZIAValue sum = x + y;

    CHECK(sum.get_inf() == 7.0);   // 2 + 5
    CHECK(sum.get_res() == 10.0);  // 3 + 7
}


// #7 - 2.2.1
TEST_CASE("addition: pure infinities add by index") {
    ZIAValue x(3.0, 0.0);   // ∞₃
    ZIAValue y(4.5, 0.0);   // ∞₄․₅

    ZIAValue sum = x + y;

    CHECK(sum.get_inf() == 7.5);
    CHECK(sum.get_res() == 0.0);
}


// #8 - 2.2.2
TEST_CASE("addition: infinity plus real preserves residue") {
    ZIAValue inf(6.0, 0.0);           // ∞₆
    ZIAValue real = ZIAValue::from_real(4.0);  // ∞₀ ⊕ 4

    ZIAValue sum = inf + real;

    CHECK(sum.get_inf() == 6.0);
    CHECK(sum.get_res() == 4.0);
}


// #9 - 2.2.3
TEST_CASE("addition: real plus infinity preserves residue") {
    ZIAValue real = ZIAValue::from_real(4.0);  // ∞₀ ⊕ 4
    ZIAValue inf(6.0, 0.0);                  // ∞₆

    ZIAValue sum = real + inf;

    CHECK(sum.get_inf() == 6.0);
    CHECK(sum.get_res() == 4.0);
}


// #10 - 2.2.4
TEST_CASE("addition: opposite infinities cancel to zero") {
    ZIAValue inf_pos(8.0, 0.0);   // ∞₈
    ZIAValue inf_neg(-8.0, 0.0);  // ∞₋₈

    ZIAValue sum = inf_pos + inf_neg;
    ZIAValue zero = ZIAValue::from_real(0.0);

    CHECK(sum == zero);
    CHECK(sum.get_inf() == 0.0);
    CHECK(sum.get_res() == 0.0);
}


// #11 - 2.2.5
TEST_CASE("addition: full pair cancels to zero") {
    ZIAValue x(3.0, 5.0);        // ∞₃ ⊕ 5
    ZIAValue y(-3.0, -5.0);      // ∞₋₃ ⊕ −5

    ZIAValue sum = x + y;
    ZIAValue zero = ZIAValue::from_real(0.0);

    CHECK(sum == zero);
    CHECK(sum.get_inf() == 0.0);
    CHECK(sum.get_res() == 0.0);
}


// #12 - 2.3.1
TEST_CASE("addition: commutativity holds for general elements") {
    ZIAValue x(2.0, 7.0);    // ∞₂ ⊕ 7
    ZIAValue y(-3.0, 4.0);   // ∞₋₃ ⊕ 4

    ZIAValue sum_xy = x + y;
    ZIAValue sum_yx = y + x;

    CHECK(sum_xy == sum_yx);
    CHECK(sum_xy.get_inf() == -1.0);  // 2 + (-3)
    CHECK(sum_xy.get_res() == 11.0);  // 7 + 4
}


// #13 - 2.3.2
TEST_CASE("addition: associativity holds for general elements") {
    ZIAValue x(1.0, 2.0);    // ∞₁ ⊕ 2
    ZIAValue y(-4.0, 3.0);   // ∞₋₄ ⊕ 3
    ZIAValue z(6.0, -1.0);   // ∞₆ ⊕ −1

    ZIAValue left = (x + y) + z;
    ZIAValue right = x + (y + z);

    CHECK(left == right);
    CHECK(left.get_inf() == 3.0);   // 1 − 4 + 6
    CHECK(left.get_res() == 4.0);   // 2 + 3 − 1
}


// #14 - 2.3.3
TEST_CASE("addition: zero is the additive identity") {
    ZIAValue x(5.0, -3.0);          // ∞₅ ⊕ −3
    ZIAValue zero = ZIAValue::from_real(0.0);

    ZIAValue sum = x + zero;

    CHECK(sum == x);
    CHECK(sum.get_inf() == 5.0);
    CHECK(sum.get_res() == -3.0);
}


// #15 - 2.3.4
TEST_CASE("addition: additive inverse cancels element to zero") {
    ZIAValue x(4.0, 9.0);     // ∞₄ ⊕ 9
    ZIAValue neg = -x;        // ∞₋₄ ⊕ −9

    ZIAValue sum = x + neg;
    ZIAValue zero = ZIAValue::from_real(0.0);

    CHECK(sum == zero);
    CHECK(sum.get_inf() == 0.0);
    CHECK(sum.get_res() == 0.0);
}

// #16 - 3.1.1
TEST_CASE("multiplication axiom: full bilinear product") {
    // x = ∞₂ ⊕ 3
    // y = ∞₅ ⊕ 7
    ZIAValue x(2.0, 3.0);
    ZIAValue y(5.0, 7.0);

    ZIAValue product = x * y;

    // divergence: ab + rb + sa = (2*5) + (3*5) + (7*2) = 10 + 15 + 14 = 39
    // residue: rs = 3 * 7 = 21
    CHECK(product.get_inf() == 39.0);
    CHECK(product.get_res() == 21.0);
}


// #17 - 3.2.1
TEST_CASE("multiplication: pure infinities multiply by index") {
    ZIAValue x(4.0, 0.0);   // ∞₄
    ZIAValue y(2.5, 0.0);   // ∞₂․₅

    ZIAValue product = x * y;

    CHECK(product.get_inf() == 10.0);  // 4 * 2.5
    CHECK(product.get_res() == 0.0);
}


// #18 - 3.2.2
TEST_CASE("multiplication: real scales infinity (r * ∞a)") {
    ZIAValue real = ZIAValue::from_real(3.0);  // ∞₀ ⊕ 3
    ZIAValue inf(4.0, 0.0);                  // ∞₄

    ZIAValue product = real * inf;

    CHECK(product.get_inf() == 12.0);  // 3 * 4
    CHECK(product.get_res() == 0.0);
}


// #19 - 3.2.3
TEST_CASE("multiplication: infinity scales by real (∞a * r)") {
    ZIAValue inf(4.0, 0.0);                 // ∞₄
    ZIAValue real = ZIAValue::from_real(3.0); // ∞₀ ⊕ 3

    ZIAValue product = inf * real;

    CHECK(product.get_inf() == 12.0);  // 4 * 3
    CHECK(product.get_res() == 0.0);
}


// #20 - 3.2.4
TEST_CASE("multiplication: zero absorbs arbitrary element") {
    ZIAValue zero = ZIAValue::from_real(0.0);
    ZIAValue x(7.0, -4.0);   // ∞₇ ⊕ −4

    ZIAValue product = zero * x;

    CHECK(product.get_inf() == 0.0);
    CHECK(product.get_res() == 0.0);
    CHECK(product == zero);
}


// #21 - 3.2.5
TEST_CASE("multiplication: real one is multiplicative identity") {
    ZIAValue x(6.0, -2.5);           // ∞₆ ⊕ −2.5
    ZIAValue one = ZIAValue::from_real(1.0);  // ∞₀ ⊕ 1

    ZIAValue product = x * one;

    CHECK(product == x);
    CHECK(product.get_inf() == 6.0);
    CHECK(product.get_res() == -2.5);
}


// #22 - 3.3.1
TEST_CASE("multiplication: commutativity holds for general elements") {
    ZIAValue x(2.0, 3.0);    // ∞₂ ⊕ 3
    ZIAValue y(-4.0, 5.0);   // ∞₋₄ ⊕ 5

    ZIAValue xy = x * y;
    ZIAValue yx = y * x;

    CHECK(xy == yx);

    // Optional explicit check to pin the expected value
    // divergence: ab + rb + sa = (2 * -4) + (3 * -4) + (5 * 2) = -8 -12 +10 = -10
    // residue: rs = 3 * 5 = 15
    CHECK(xy.get_inf() == -10.0);
    CHECK(xy.get_res() == 15.0);
}


// #23 - 3.3.2
TEST_CASE("multiplication: associativity holds for general elements") {
    ZIAValue x(1.0, 2.0);    // ∞₁ ⊕ 2
    ZIAValue y(3.0, -1.0);   // ∞₃ ⊕ −1
    ZIAValue z(-2.0, 4.0);   // ∞₋₂ ⊕ 4

    ZIAValue left  = (x * y) * z;
    ZIAValue right = x * (y * z);

    CHECK(left == right);

    // Optional explicit pin (computed once, used twice)
    // x*y: inf = 1*3 + 2*3 + (-1)*1 = 3 + 6 - 1 = 8, res = 2 * -1 = -2
    // (x*y)*z: inf = 8*(-2) + (-2)*(-2) + 4*8 = -16 + 4 + 32 = 20, res = -2 * 4 = -8
    CHECK(left.get_inf() == 20.0);
    CHECK(left.get_res() == -8.0);
}


// #24 - 3.3.3
TEST_CASE("multiplication: distributivity over addition") {
    ZIAValue x(2.0, 1.0);     // ∞₂ ⊕ 1
    ZIAValue y(3.0, -2.0);    // ∞₃ ⊕ −2
    ZIAValue z(-1.0, 4.0);    // ∞₋₁ ⊕ 4

    ZIAValue left  = x * (y + z);
    ZIAValue right = (x * y) + (x * z);

    CHECK(left == right);

    // Optional explicit pin:
    // y+z = ∞₂ ⊕ 2
    // x*(y+z): inf = 2*2 + 1*2 + 2*2 = 4 + 2 + 4 = 10, res = 1*2 = 2
    CHECK(left.get_inf() == 10.0);
    CHECK(left.get_res() == 2.0);
}


// #25 - 4.1.1
TEST_CASE("division: real divided by zero expands to infinity") {
    ZIAValue a = ZIAValue::from_real(5.0);   // ∞₀ ⊕ 5
    ZIAValue zero = ZIAValue::from_real(0.0);

    ZIAValue result = a / zero;

    CHECK(result.get_inf() == 5.0);
    CHECK(result.get_res() == 0.0);
}


// #26 - 4.1.2
TEST_CASE("division: zero divided by zero collapses to zero") {
    ZIAValue zero = ZIAValue::from_real(0.0);

    ZIAValue result = zero / zero;

    CHECK(result.get_inf() == 0.0);
    CHECK(result.get_res() == 0.0);
    CHECK(result == zero);
}


// #28 - 4.1.3
TEST_CASE("division: real divided by infinity collapses to zero") {
    ZIAValue a = ZIAValue::from_real(7.0);   // ∞₀ ⊕ 7
    ZIAValue inf(3.0, 0.0);                // ∞₃

    ZIAValue result = a / inf;

    CHECK(result.get_inf() == 0.0);
    CHECK(result.get_res() == 0.0);
    CHECK(result == ZIAValue::from_real(0.0));
}


// #29 - 4.1.4
TEST_CASE("division: infinity divided by real rescales divergence index") {
    ZIAValue inf(10.0, 0.0);               // ∞₁₀
    ZIAValue r = ZIAValue::from_real(2.0);   // ∞₀ ⊕ 2

    ZIAValue result = inf / r;

    CHECK(result.get_inf() == 5.0);  // 10 / 2
    CHECK(result.get_res() == 0.0);
}


// #30 - 4.1.5
TEST_CASE("division: infinity divided by infinity yields real ratio") {
    ZIAValue inf_a(9.0, 0.0);    // ∞₉
    ZIAValue inf_b(3.0, 0.0);    // ∞₃

    ZIAValue result = inf_a / inf_b;

    CHECK(result.get_inf() == 0.0);
    CHECK(result.get_res() == 3.0);   // 9 / 3
    CHECK(result == ZIAValue::from_real(3.0));
}


// #31 - 4.1.6
TEST_CASE("division: infinity divided by zero remains infinite") {
    ZIAValue inf(7.0, 0.0);                // ∞₇
    ZIAValue zero = ZIAValue::from_real(0.0);

    ZIAValue result = inf / zero;

    CHECK(result.get_inf() == 7.0);
    CHECK(result.get_res() == 0.0);
    CHECK(result == inf);
}


// #32 - 4.2.1
TEST_CASE("division: operation is total and never produces NaN or throws") {
    ZIAValue real(0.0, 5.0);      // ∞₀ ⊕ 5
    ZIAValue zero(0.0, 0.0);      // 0
    ZIAValue inf(4.0, 0.0);       // ∞₄
    ZIAValue mixed(3.0, -2.0);    // ∞₃ ⊕ −2

    // Each of these should execute without throwing
    CHECK_NOTHROW(real / zero);     // expansion
    CHECK_NOTHROW(zero / zero);     // collapse
    CHECK_NOTHROW(real / inf);      // collapse
    CHECK_NOTHROW(inf / real);      // rescale
    CHECK_NOTHROW(inf / inf);       // ratio
    CHECK_NOTHROW(mixed / real);    // general case
    CHECK_NOTHROW(mixed / inf);     // collapse
}


// #33 - 4.2.2
TEST_CASE("division: infinity over infinity always yields finite embedded real") {
    ZIAValue inf_a(12.0, 0.0);   // ∞₁₂
    ZIAValue inf_b(-3.0, 0.0);   // ∞₋₃

    ZIAValue result = inf_a / inf_b;

    CHECK(result.get_inf() == 0.0);
    CHECK(result.get_res() == -4.0);   // 12 / -3
}

// #34 - 4.3.1.a
TEST_CASE("division: infinities have no multiplicative inverse") {
    ZIAValue inf(5.0, 0.0);   // ∞₅
    CHECK_THROWS(inf.inverse());
}

// #34 - 4.3.1.b
TEST_CASE("division: infinity divided by real never yields a unit") {
    ZIAValue inf(6.0, 0.0);               // ∞₆
    ZIAValue r = ZIAValue::from_real(3.0);  // 3

    ZIAValue result = inf / r;

    CHECK(result != ZIAValue::from_real(1.0));
}


// #35 - 4.3.2
TEST_CASE("division: infinity divided by real never re-enters the reals") {
    ZIAValue inf(8.0, 0.0);               // ∞₈
    ZIAValue r = ZIAValue::from_real(4.0);  // 4

    ZIAValue result = inf / r;

    CHECK(result.get_inf() != 0.0);
    CHECK(result.get_res() == 0.0);
}


// #36 - 5.1.1
TEST_CASE("inverse: additive inverse exists for all ZIA elements") {
    ZIAValue x(9.0, -4.0);   // ∞₉ ⊕ −4

    ZIAValue sum = x + (-x);

    CHECK(sum.get_inf() == 0.0);
    CHECK(sum.get_res() == 0.0);
    CHECK(sum == ZIAValue::from_real(0.0));
}


// #37 - 5.2.1
TEST_CASE("inverse: embedded non-zero reals have multiplicative inverse") {
    ZIAValue r = ZIAValue::from_real(4.0);   // ∞₀ ⊕ 4

    ZIAValue inv = r.inverse();

    CHECK(inv.get_inf() == 0.0);
    CHECK(inv.get_res() == 0.25);

    // sanity: r * inv == 1
    ZIAValue one = r * inv;
    CHECK(one == ZIAValue::from_real(1.0));
}


// #38 - 5.2.2
TEST_CASE("inverse: no multiplicative inverse outside embedded non-zero reals") {
    ZIAValue inf(3.0, 0.0);     // ∞₃
    ZIAValue mixed(2.0, 5.0);  // ∞₂ ⊕ 5
    ZIAValue zero = ZIAValue::from_real(0.0);

    CHECK_THROWS(inf.inverse());
    CHECK_THROWS(mixed.inverse());
    CHECK_THROWS(zero.inverse());
}


// #39 - 6.1
TEST_CASE("global: no operation ever produces NaN or invalid values") {
    ZIAValue a(3.0, 4.0);
    ZIAValue b(-2.0, 5.0);
    ZIAValue zero = ZIAValue::from_real(0.0);
    ZIAValue one  = ZIAValue::from_real(1.0);

    auto check_finite = [](const ZIAValue& z) {
        CHECK(std::isfinite(z.get_inf()));
        CHECK(std::isfinite(z.get_res()));
    };

    check_finite(a + b);
    check_finite(a - b);
    check_finite(a * b);
    check_finite(a / b);

    check_finite(a / zero);
    check_finite(zero / zero);
    check_finite(one / zero);
    check_finite(a / one);
}


// #40 - 6.2
TEST_CASE("global: closure under +, -, *, /") {
    ZIAValue x(5.0, -1.0);
    ZIAValue y(-3.0, 2.0);

    CHECK_NOTHROW(x + y);
    CHECK_NOTHROW(x - y);
    CHECK_NOTHROW(x * y);
    CHECK_NOTHROW(x / y);
}


// #41 - 6.3
TEST_CASE("global: ZIA is a commutative ring with unity, not a field") {
    ZIAValue x(4.0, 3.0);
    ZIAValue one = ZIAValue::from_real(1.0);

    // ring identity already tested, but assert again
    CHECK(x * one == x);

    // not a field: infinities have no inverse
    CHECK_THROWS(ZIAValue(2.0, 0.0).inverse());
}


// #42
// TODO: IMPORTANT! NEW TEST CASES - UPDATE JUPYTER NOTEBOOK TEST SUITE! 
TEST_CASE("helper: is_zero is exact / structural") {
    CHECK(ZIAValue::zero().is_zero());
    CHECK(ZIAValue::from_real(0.0).is_zero());

    CHECK_FALSE(ZIAValue::one().is_zero());
    CHECK_FALSE(ZIAValue::from_real(1e-12).is_zero());

    // cancellation should collapse to exact zero
    ZIAValue inf_pos(5.0, 0.0);
    ZIAValue inf_neg(-5.0, 0.0);
    CHECK((inf_pos + inf_neg).is_zero());
}

// #43
// TODO: IMPORTANT! NEW TEST CASES - UPDATE JUPYTER NOTEBOOK TEST SUITE! 
TEST_CASE("helper: is_zero and is_real are exact / structural") {
    CHECK(ZIAValue::from_real(0.0).is_zero());
    CHECK(ZIAValue::zero().is_zero());
    CHECK(ZIAValue::zero().is_real());

    CHECK(ZIAValue::from_real(1.25).is_real());
    CHECK_FALSE(ZIAValue::from_real(1.25).is_zero());

    CHECK_FALSE(ZIAValue(1.0, 0.0).is_real());
    CHECK_FALSE(ZIAValue(1.0, 0.0).is_zero());

    // cancellation should collapse to exact zero
    CHECK((ZIAValue(5.0, 0.0) + ZIAValue(-5.0, 0.0)).is_zero());
}

// #44
// TODO: IMPORTANT! NEW TEST CASES - UPDATE JUPYTER NOTEBOOK TEST SUITE! 
TEST_CASE("helper: is_infinite is exact / structural") {
    CHECK_FALSE(ZIAValue::zero().is_infinite());
    CHECK_FALSE(ZIAValue::from_real(0.0).is_infinite());
    CHECK_FALSE(ZIAValue::from_real(2.0).is_infinite());

    CHECK(ZIAValue(1.0, 0.0).is_infinite());
    CHECK(ZIAValue(1.0, 3.0).is_infinite());

    // consistency with is_real (structural predicates)
    CHECK(ZIAValue::zero().is_real());
    CHECK_FALSE(ZIAValue::zero().is_infinite());
    CHECK_FALSE(ZIAValue(1.0, 0.0).is_real());
}

// #45
// TODO: IMPORTANT! NEW TEST CASES - UPDATE JUPYTER NOTEBOOK TEST SUITE! 
TEST_CASE("helper: is_pure_infinity is exact / structural") {
    CHECK_FALSE(ZIAValue::zero().is_pure_infinity());
    CHECK_FALSE(ZIAValue::one().is_pure_infinity());
    CHECK_FALSE(ZIAValue::from_real(0.0).is_pure_infinity());
    CHECK_FALSE(ZIAValue::from_real(3.0).is_pure_infinity());

    CHECK(ZIAValue(2.0, 0.0).is_pure_infinity());
    CHECK(ZIAValue(-2.0, 0.0).is_pure_infinity());

    // infinite but not pure (has residue)
    CHECK_FALSE(ZIAValue(2.0, 0.5).is_pure_infinity());

    // pure infinity implies infinite
    CHECK(ZIAValue(2.0, 0.0).is_infinite());
}

// #46
// TODO: IMPORTANT! NEW TEST CASES - UPDATE JUPYTER NOTEBOOK TEST SUITE!
TEST_CASE("helper: as_real_strict returns residue iff value is real") {
    CHECK(ZIAValue::from_real(-1.25).as_real_strict() == doctest::Approx(-1.25));
    CHECK_THROWS_AS(ZIAValue(1.0, 0.0).as_real_strict(), std::runtime_error);
    CHECK_THROWS_AS(ZIAValue(1.0, 2.0).as_real_strict(), std::runtime_error);
}

// #47
// TODO: IMPORTANT! NEW TEST CASES - UPDATE JUPYTER NOTEBOOK TEST SUITE!
TEST_CASE("helper: is_finite_near_zero applies only to reals") {
    CHECK(ZIAValue::zero().is_finite_near_zero(0.0));
    CHECK(ZIAValue::zero().is_finite_near_zero(1e-12));

    CHECK(ZIAValue::from_real(1e-13).is_finite_near_zero(1e-12));
    CHECK_FALSE(ZIAValue::from_real(1e-9).is_finite_near_zero(1e-12));

    // Never treat an infinite value as "finite near zero"
    CHECK_FALSE(ZIAValue(2.0, 0.0).is_finite_near_zero(1e9));
    CHECK_FALSE(ZIAValue(2.0, 1e-15).is_finite_near_zero(1e9));
}

// #48
// TODO: IMPORTANT! NEW TEST CASES - UPDATE JUPYTER NOTEBOOK TEST SUITE!
TEST_CASE("helper: is_real_close compares only reals") {
    CHECK(ZIAValue::from_real(1.0).is_real_close(ZIAValue::from_real(1.0 + 1e-13), 1e-12));
    CHECK_FALSE(ZIAValue::from_real(1.0).is_real_close(ZIAValue::from_real(1.0 + 1e-9), 1e-12));

    // If either side is infinite/mixed, it must return false
    CHECK_FALSE(ZIAValue(1.0, 0.0).is_real_close(ZIAValue::from_real(0.0), 1e9));
    CHECK_FALSE(ZIAValue::from_real(0.0).is_real_close(ZIAValue(1.0, 0.0), 1e9));
    CHECK_FALSE(ZIAValue(1.0, 2.0).is_real_close(ZIAValue(1.0, 2.0), 1e9));
}

// #49
// TODO: IMPORTANT! NEW TEST CASES - UPDATE JUPYTER NOTEBOOK TEST SUITE!
TEST_CASE("constructor: infinity(a) creates pure infinity") {
    ZIAValue z = ZIAValue::infinity(3.25);

    CHECK(z.get_inf() == doctest::Approx(3.25));
    CHECK(z.get_res() == doctest::Approx(0.0));

    CHECK(z.is_infinite());
    CHECK(z.is_pure_infinity());
    CHECK_FALSE(z.is_real());
    CHECK_FALSE(z.is_zero());
}

// #50
// TODO: IMPORTANT! NEW TEST CASES - UPDATE JUPYTER NOTEBOOK TEST SUITE!
TEST_CASE("helper: with_inf and with_res return modified copies") {
    ZIAValue a = ZIAValue::from_real(2.0);           // (0, 2)
    ZIAValue b = a.with_inf(3.0);                  // (3, 2)
    ZIAValue c = b.with_res(5.0);                  // (3, 5)

    CHECK(a.get_inf() == doctest::Approx(0.0));
    CHECK(a.get_res() == doctest::Approx(2.0));

    CHECK(b.get_inf() == doctest::Approx(3.0));
    CHECK(b.get_res() == doctest::Approx(2.0));

    CHECK(c.get_inf() == doctest::Approx(3.0));
    CHECK(c.get_res() == doctest::Approx(5.0));

    // ensure original unchanged
    CHECK(a.get_inf() == doctest::Approx(0.0));
    CHECK(a.get_res() == doctest::Approx(2.0));
}

// #51
// TODO: IMPORTANT! NEW TEST CASES - UPDATE JUPYTER NOTEBOOK TEST SUITE!
TEST_CASE("helper: approx_equal_pair compares components with separate tolerances") {
    ZIAValue a(1.0, 2.0);
    ZIAValue b(1.0 + 1e-13, 2.0 + 5e-13);

    CHECK(a.approx_equal_pair(b, 1e-12, 1e-12));
    CHECK_FALSE(a.approx_equal_pair(b, 1e-14, 1e-12)); // inf too strict
    CHECK_FALSE(a.approx_equal_pair(b, 1e-12, 1e-14)); // res too strict
}

// #52
// TODO: IMPORTANT! NEW TEST CASES - UPDATE JUPYTER NOTEBOOK TEST SUITE!
TEST_CASE("helper: parts returns {inf,res}") {
    ZIAValue z(1.5, -2.25);
    auto p = z.parts();
    CHECK(p.first == doctest::Approx(1.5));
    CHECK(p.second == doctest::Approx(-2.25));
}

// #53
// TODO: IMPORTANT! NEW TEST CASES - UPDATE JUPYTER NOTEBOOK TEST SUITE!
TEST_CASE("format: to_nice_string basic forms") {
    CHECK(ZIAValue::zero().to_display_string() == "0");

    CHECK(ZIAValue::from_real(1.25).to_display_string(6).find("1.25") != std::string::npos);

    CHECK(ZIAValue::infinity(2.0).to_display_string(6).find("INF(") == 0);

    std::string s = ZIAValue(2.0, 0.5).to_display_string(6);
    CHECK(s.find("INF(") == 0);
    CHECK(s.find("(+)") != std::string::npos);
}

// * * * * * * * * * * * * * * * * * * * * * * * 
// New division cases

// #54
// (D2) real / real => r/s
// TODO: IMPORTANT! NEW TEST CASES - UPDATE JUPYTER NOTEBOOK TEST SUITE!
TEST_CASE("division: real divided by real matches embedded ℝ division") {
    ZIAValue x = ZIAValue::from_real(6.0);
    ZIAValue y = ZIAValue::from_real(3.0);

    ZIAValue result = x / y;

    CHECK(result == ZIAValue::from_real(2.0));
    CHECK(result.get_inf() == 0.0);
    CHECK(result.get_res() == 2.0);
}

// #55
// (D1) fully-populated RxR / RxR => a/b, residues ignored
// TODO: IMPORTANT! NEW TEST CASES - UPDATE JUPYTER NOTEBOOK TEST SUITE!
TEST_CASE("division: (∞a ⊕ r)/(∞b ⊕ s) returns a/b and discards residues when b!=0") {
    ZIAValue x(3.0, 7.0);     // ∞3 ⊕ 7
    ZIAValue y(6.0, 11.0);    // ∞6 ⊕ 11   (b!=0 selects D1)

    ZIAValue result = x / y;

    CHECK(result == ZIAValue::from_real(0.5));  // 3/6
    CHECK(result.get_inf() == 0.0);
    CHECK(result.get_res() == 0.5);
}

// #56
// (D1) real / (∞b ⊕ s) => 0, even when s!=0
// TODO: IMPORTANT! NEW TEST CASES - UPDATE JUPYTER NOTEBOOK TEST SUITE!
TEST_CASE("division: real divided by (∞b ⊕ s) collapses to 0 even if s!=0") {
    ZIAValue x = ZIAValue::from_real(1.0);
    ZIAValue y(2.0, 5.0);   // ∞2 ⊕ 5

    ZIAValue result = x / y;

    CHECK(result == ZIAValue::from_real(0.0));
    CHECK(result.get_inf() == 0.0);
    CHECK(result.get_res() == 0.0);
}

// #57
// (D2) mixed numerator / real => ∞(a/s), residue discarded
// TODO: IMPORTANT! NEW TEST CASES - UPDATE JUPYTER NOTEBOOK TEST SUITE!
TEST_CASE("division: (∞a ⊕ r)/(real s) rescales divergence and discards residue") {
    ZIAValue x(8.0, 7.0);               // ∞8 ⊕ 7
    ZIAValue y = ZIAValue::from_real(2.0);  // s=2

    ZIAValue result = x / y;

    CHECK(result.get_inf() == 4.0);  // 8/2
    CHECK(result.get_res() == 0.0);  // r discarded
}

// #58
// (D3) mixed numerator / 0 => ∞a, residue discarded
// TODO: IMPORTANT! NEW TEST CASES - UPDATE JUPYTER NOTEBOOK TEST SUITE!
TEST_CASE("division: (∞a ⊕ r)/0 keeps divergence index (stable divergence) and discards residue") {
    ZIAValue x(5.0, 7.0);                 // ∞5 ⊕ 7
    ZIAValue zero = ZIAValue::from_real(0.0);

    ZIAValue result = x / zero;

    CHECK(result == ZIAValue(5.0, 0.0));
    CHECK(result.get_inf() == 5.0);
    CHECK(result.get_res() == 0.0);
}
