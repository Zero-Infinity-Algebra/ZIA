#include <pybind11/pybind11.h>
#include <pybind11/operators.h>
#include "ZIAValue.h"

namespace py = pybind11;

PYBIND11_MODULE(zia, m) {
    m.doc() = "Zero Infinity Algebra (ZIA) Python bindings";

py::class_<ZIAValue>(m, "ZIAValue")
    // constructors
    .def(py::init<double, double>(),
         py::arg("inf"), py::arg("res"))

    .def_static("infinity", &ZIAValue::infinity, py::arg("a"))

    // from_real static
    .def_static("from_real", &ZIAValue::from_real)

    // read-only properties
    .def_property_readonly("inf", &ZIAValue::get_inf)
    .def_property_readonly("res", &ZIAValue::get_res)

    // string representations
    .def("__repr__", [](const ZIAValue& z) {
        return "ZIAValue(" + z.to_string() + ")";
    })
    .def("__str__", &ZIAValue::to_string)

    .def("to_nice_string", &ZIAValue::to_display_string, py::arg("precision") = 6)

    // arithmetic operators
    .def(py::self + py::self)
    .def(py::self - py::self)
    .def(py::self * py::self)
    .def(py::self / py::self)
    .def(-py::self)

    // comparisons
    .def(py::self == py::self)
    .def(py::self != py::self)

    // common constants
    .def_static("zero", &ZIAValue::zero)
    .def_static("one", &ZIAValue::one)

    // helpers
    .def("is_zero", &ZIAValue::is_zero)
    .def("is_real", &ZIAValue::is_real)
    .def("is_infinite", &ZIAValue::is_infinite)
    .def("is_pure_infinity", &ZIAValue::is_pure_infinity)
    .def("with_inf", &ZIAValue::with_inf, py::arg("new_inf"))
    .def("with_res", &ZIAValue::with_res, py::arg("new_res"))
    .def("parts", &ZIAValue::parts)

    // Conversions
    .def("as_real_strict", &ZIAValue::as_real_strict)

    // Numerical convenience (ℝ-only)
    .def("is_finite_near_zero", &ZIAValue::is_finite_near_zero, py::arg("tol"))
    .def("is_real_close", &ZIAValue::is_real_close, py::arg("other"), py::arg("tol"))

    // Numerical convenience
    .def("approx_equal_pair",
     &ZIAValue::approx_equal_pair,
     py::arg("other"),
     py::arg("tol_inf"),
     py::arg("tol_res"))

    // float interoperability
    // --- float interop sugar (routes through existing C++ ops) ---
    .def("__mul__", [](const ZIAValue& z, double x) {
        return z * ZIAValue::from_real(x);
    }, py::is_operator())

    .def("__rmul__", [](const ZIAValue& z, double x) {
        return ZIAValue::from_real(x) * z;
    }, py::is_operator())

    .def("__truediv__", [](const ZIAValue& z, double x) {
        return z / ZIAValue::from_real(x);
    }, py::is_operator())

    .def("__rtruediv__", [](const ZIAValue& z, double x) {
        return ZIAValue::from_real(x) / z;
    }, py::is_operator())

    .def("__add__", [](const ZIAValue& z, double x) {
        return z + ZIAValue::from_real(x);
    }, py::is_operator())

    .def("__radd__", [](const ZIAValue& z, double x) {
        return ZIAValue::from_real(x) + z;
    }, py::is_operator())

    .def("__sub__", [](const ZIAValue& z, double x) {
        return z - ZIAValue::from_real(x);
    }, py::is_operator())

    .def("__rsub__", [](const ZIAValue& z, double x) {
        return ZIAValue::from_real(x) - z;
    }, py::is_operator())

    .def("__float__", [](const ZIAValue& z) {
        try {
                return z.as_real_strict();
            } catch (const std::runtime_error& e) {
            throw py::value_error(e.what());
        }
    })

     // explicit inverse
     .def("inverse", &ZIAValue::inverse);
}
