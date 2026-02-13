## Project Overview

This repository contains a C++ reference implementation (including Python bindings) of **Zero–Infinity Algebra (ZIA)** as specified in the [Zero–Infinity Algebra paper](https://doi.org/10.5281/zenodo.18280702).

 The core type (`ZIAValue`) implements the two-channel structure over ℝ×ℝ with defined addition, multiplication, and totalised division semantics. The implementation enforces invariants at construction and during arithmetic operations to ensure well-formed values throughout computation. A comprehensive unit test suite is included to validate algebraic identities, division cases, edge conditions, and mixed real / ZIA interactions. The project is built using CMake and is intended to compile on standard modern C++ toolchains. The repository contains only the reference implementation, tests, and build configuration required to use and verify the algebra in numerical code.


## Repository Contents

- **Core implementation**
    - `include/ZIAValue.h` — Public header defining the `ZIAValue` type and its interface.
    - `src/ZIAValue.cpp` — Implementation of arithmetic operations and invariant enforcement.
    - `bindings/zia_python.cpp` — Optional Python bindings for the C++ reference implementation.
- **Unit tests**
    - `tests/test_ziavalue.cpp` — Comprehensive test suite covering arithmetic operations, division cases, and edge conditions.
    - `doctest/doctest.h` — Embedded single-header testing framework used by the test suite.
- **CMake build configuration**
    - `CMakeLists.txt` — Top-level CMake configuration for building the library, tests, and optional bindings.
- **Licensing files**
    - `LICENSE-RESEARCH.md` — Research and evaluation license.
    - `LICENSE-COMMERCIAL.md` — Commercial license terms.
    - `LICENSING.md` — Licensing overview and usage conditions.
    - `PATENTS.md` — Patent-related terms and notices.
    - `THIRD_PARTY_NOTICES.md` — Notices for included third-party components.

*The Python bindings are included as source; packaging / wheels are on the roadmap*


## Intended Use

This library provides a reference implementation of Zero–Infinity Algebra (ZIA) as defined in the Zero–Infinity Algebra preprint v0.6.5.r0 , enabling structured representation of singular events within otherwise standard numeric code. ZIA values are intended to preserve information that would normally be lost at singularities such as division by zero, rather than collapsing to `NaN`, `Inf`, or an arbitrary repair value. In typical use, computation proceeds with standard numeric types (e.g., `double`) until a singular boundary condition is detected. At that point, code should branch and construct a `ZIAValue` to represent the singular event in a structured form. The resulting ZIA value may then propagate through subsequent arithmetic where the singular structure remains relevant.

ZIA is not intended to replace standard numeric types in routine computation, nor to serve as a universal drop-in numeric abstraction. It should be introduced only at well-defined singularity points or boundary conditions where representational collapse would otherwise occur. For example, if a denominator in a numerical routine evaluates to zero, the implementation would branch to create a `ZIAValue` instead of returning `Inf`, and subsequent operations would carry that structured value forward. This design supports explicit handling of singular events while leaving non-singular computation unchanged.


## Jupyter Notebooks

The `notebooks/` directory contains example Jupyter notebooks that demonstrate ZIA and ZIA-style shadow channels in practical numerical pipelines. They are intended for research, evaluation, and as executable documentation. They are not production solver code.

Included notebooks (by theme):
- `notebooks/zia-cfd_*.ipynb`
  - Positivity flooring in a minimal reactive transport model
  - A shadow channel that preserves undershoot severity after repair
  - Simple diagnostics and an adaptivity-style demo driven by the shadow signal
- `notebooks/zia-ml_*.ipynb`
  - Logit or LLR clipping and the induced identifiability failure in the saturated subset
  - A shadow channel that records discarded mass online and restores ranking within that subset
  - A ZIA ratio demonstration using a shifted log-index to avoid overflow

ZIA Python module dependency:
- The notebooks expect `zia.py` to be available in the same directory as the notebook (or otherwise importable on `PYTHONPATH`).
- `zia.py` is an automatically generated Python version of the C++ reference implementation, provided so the notebooks can run without requiring a compiled extension.

How to run:
1. Create and activate a Python virtual environment.
2. Install notebook dependencies (see `notebooks/requirements.txt` if present):
   - pip install -r notebooks/requirements.txt
3. Launch Jupyter from the repository root:
   - jupyter lab
   - or: jupyter notebook

Notes:
- Parameters and examples are chosen for clarity and reproducibility, not benchmark performance.
- The primary numeric state is left unchanged. The shadow signal is computed only at existing stabilization points (flooring, clipping, limiting, or bounds).
- Notebooks are covered by the same licensing terms as the rest of the repository.


## Build Instructions (Windows, CMake)

Assumes **MSVC** and **CMake** are installed and available on `PATH`. Uses an **out-of-source** build in `.\build`.

#### Configure

```bat
cmake -S . -B build
```

#### Build (Release)

```bat
cmake --build build --config Release
```

#### Run unit tests (Release)

```bat
cmake --build build --config Release --target RUN_TESTS
```

#### Build (Debug)

```bat
cmake --build build --config Debug
```

#### Run unit tests (Debug)

```bat
cmake --build build --config Debug --target RUN_TESTS
```


## Licensing

ZIA is **available under two licenses**:

- **Research License** (`LICENSE-RESEARCH.md`): free for **research/academia**, **teaching**, **evaluation**, and **non-commercial experimentation**.
- **Commercial License** (`LICENSE-COMMERCIAL.md`): required for **any production or operational use of any kind**, whether open source or closed source, including internal use, government/defence/intelligence operational use, hosted services (including internal services), and any use that creates **economic value through production/operational use**, including revenue, cost reduction, or deployed advantage.

Economic value includes value created for a **for-profit or non-profit organization**, whether or not money changes hands.

**This is not an OSI-approved open-source license.** It is a research/evaluation license with a separate commercial license for production.

There is **no open-source carve-out for production use**.

Good-faith use under the Research License is **not retroactively charged**. If a project transitions from research/evaluation to production/operational use, a Commercial License is required **from that point onward**.

Forks are permitted for uses allowed under the Research License. Under the **soft naming rule**, forks may refer to “Zero–Infinity Algebra” or “ZIA” only to truthfully describe that they are a fork or derivative (e.g., “Unofficial fork of ZIA”), and must not imply endorsement. See `LICENSE-RESEARCH.md`.

See `LICENSING.md` for a decision test and examples. For commercial licensing, see `LICENSE-COMMERCIAL.md` or contact: mitchell.quinn@starcroft.net.

---

## Roadmap (what to expect)

ZIA is being released in **small, composable slices**: core arithmetic first, then observability, then provenance tooling. The goal is that each step is usable on its own, and later steps don’t invalidate early adopters.

### Near-term (next releases)

- **Example Jupyter Notebooks:**
	- Machine learning
	- Quantum measurement pipeline
	- Computational fluid dynamics (with machine learning)
- **Logging Service v1 (opt-in observability)**
    - Global enable/disable, sink registration, and event filtering via bitmask.
    - Emits **division singularity events** (with explicit “reason” + “outcome” taxonomy) and **invariant violation events** (NaN/Inf attempts).
    - Lightweight per-thread context labels (RAII scope) for traceability.
    - Designed so it can later feed the ledger as a sink, without changing arithmetic semantics.

- **Provenance Ledger v1 (unconditional capture, in-memory)**
    - **Every created ZValue produces exactly one immutable ledger entry** in a single process-global ledger.
    - Ledger snapshots are immutable, safe to inspect concurrently, and are the primary interface for Python/export.
    - Explicit user-driven export (no automatic I/O) via writer interfaces (initially JSON + SQLite).

### Next (after v1 foundations land)

- **Ledger schema refinement: “operation + operands” (not just parents)**
    - Records _what operation created a value_ and _which ordered operands were used_, without claiming mathematical meaning.
    - Clean mapping to JSON/SQLite and survives language bindings.

- **Logging v2+ (granularity + quality-of-life)**
    - Severity thresholds / rate limiting, richer categories beyond division/invariant, optional per-op emissions, richer context (e.g. correlation IDs / source locations), async sink adapters, and tighter ledger integration.


### Explicit non-goals (for now)

To keep the core reliable and predictable, the following are intentionally **out of scope in v1**:

- Ledger: multiple ledgers per process, persistence across runs, automatic filtering, graph traversal semantics, visualization, provenance validation, cycle detection.
- Logging: async workers, full stack traces/source maps, tight coupling to external logging frameworks.
