# Module Sandboxing Audit Report

## Module Information
- **Name**: kalman
- **Version**: 1.1
- **Type**: C++ (binary module)
- **Audit Date**: 2026-04-23
- **Safe for Sandbox Use**: Yes

## Domain
- **Functional Domain**: `QDOM_DEFAULT`
- **Rationale**: The kalman module performs pure computational linear-
  algebra and state-estimation operations via Eigen. It does not perform
  filesystem I/O, network I/O, process execution, environment variable
  access, or any other operation that would require a restricted
  functional domain.

## Filesystem Security
- [x] No file operations performed
- **Gaps Found**: None
- **Severity**: None
- **Notes**: All matrix construction takes row lists / column lists
  passed directly from Qore code. `LinearFilter` and `ExtendedFilter`
  hold their state and configuration in memory; nothing is read from
  or written to disk. Future serialization support (planned for 1.1)
  will operate on in-memory hashes, not files.

## Network Security
- [x] No network operations performed
- **Gaps Found**: None
- **Severity**: None

## Process / Environment Security
- [x] No subprocess invocation
- [x] No environment variable access
- **Gaps Found**: None
- **Severity**: None

## Resource Limits
- [x] No large allocations beyond user-requested matrix dimensions
- [x] `Matrix::inverse()`, `cholesky()`, `determinant()`, `trace()`
      bounded by matrix size
- [x] `LinearFilter::predict()` / `update()` and
      `ExtendedFilter::predict()` / `update()` are bounded by
      state/measurement sizes (fixed at construction)
- [x] No unbounded native threads created
- **Gaps Found**: User-controlled matrix dimensions can produce large
      allocations — e.g. `Matrix::identity(10000)` allocates 800 MB.
      This is by design; the caller determines the matrix size. Qore's
      standard memory / resource-limit mechanisms apply.
- **Severity**: Low (expected behaviour — matrix sizes are always caller-
      controlled, documented, and bounded by the arguments provided).

## Interrupt Support / Cooperative Cancellation
- [x] All per-step filter operations (`predict`, `update`) complete in
      microseconds on typical state sizes (≤10) — no polling needed
- [x] `LinearFilter::stepBatch()` uses Pattern-1 (pre-check) and
      Pattern-5 (per-iteration) cancellation so long observation
      batches remain killable via `cancel_thread()` or
      `SandboxManager::requestInterrupt()`.
- [x] `Matrix::inverse()` and `Matrix::cholesky()` call
      `qore_check_cancel()` at the head of each method (Pattern 1),
      so very-large-matrix decompositions are cooperatively
      cancellable even though the underlying Eigen call itself is
      not interruptible.
- **Gaps Found**: None in currently-shipped entry points.
- **Severity**: None

## Thread Safety Disclosure
- **Matrix**: fully immutable after construction; safe to share across
  threads without locking.
- **LinearFilter / ExtendedFilter**: hold mutable state (current state
  vector, covariance, step counters). **Not safe to share across
  threads.** Typical usage is one filter per tracked object, owned by
  a single thread. Documented in the class doxygen.
- **Module-level state**: none. The module has no global mutable state.

## Callback Handling (ExtendedFilter)

`ExtendedFilter` invokes user-supplied Qore callbacks for the non-linear
state-transition and observation functions. Security considerations:

- [x] Callback references are retained via `QoreValue::refSelf()` at
      registration and released via `QoreValue::discard()` at filter
      destruction or replacement — no reference leaks.
- [x] Callback return values are rigorously shape-validated before use
      — any non-hash return, missing keys, or wrong-shape matrices
      raise `KALMAN-CALLBACK-ERROR` / `KALMAN-DIMENSION-ERROR` rather
      than corrupting filter state.
- [x] Exceptions thrown from within a callback propagate through
      `predict()` / `update()` cleanly — filter state is not mutated
      until the callback returns successfully (see `QoreExtendedFilter::predict`:
      `x = x_next` happens after `callCallback` succeeds).
- **Gaps Found**: None
- **Severity**: None

## Summary
- **Compliance Level**: Full
- **Highest Severity Finding**: None
- **Recommendation**: Safe to use in sandbox environments. The module
  is computation-only, dependency is a single header-only library
  (Eigen), and there are no I/O or external-resource code paths.

## Specific Findings

No security-critical findings. The 1.0 follow-up (add
`qore_check_cancel()` to `Matrix::inverse()` / `Matrix::cholesky()`)
was closed in 1.1.
