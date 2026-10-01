RPM packaging
=============

Copyright 2026 Qore Technologies, s.r.o.

The canonical qore-kalman-module.spec supports Fedora, Enterprise Linux and
openSUSE. It requires the Qore 3.0 SDK and qore-rpm-macros from the same repository.
The default build includes module tests and a separate documentation package.
Dependencies on the installed Qore ABI and SDK version are generated from the
built module; do not replace them with an unversioned qore dependency.

Prepare a pinned source bundle with qore-packaging, then build it in the target
distribution with networking disabled::

    python3 tools/packaging.py prepare --repo ../module-kalman --ref COMMIT \
      --name qore-kalman-module --version 1.1.0 \
      --spec qore-kalman-module.spec --output work/kalman-source
    python3 tools/build-local.py --source work/kalman-source \
      --image TARGET_SDK_IMAGE --output results/kalman-build --jobs 2

These commands run from the qore-packaging repository. Source preparation uses
the committed tree. Install the SDK's language documentation index for complete
Doxygen cross-references. --without docs and --without tests are available for
local diagnosis; repository qualification uses the defaults and also runs the
suite against installed RPMs outside the checkout. Native modules retain the
distribution's normal ELF stripping and separate debug packages.

The module instantiates Eigen templates; the runtime RPM retains the installed
Eigen development package's complete license notices. All four filter, matrix
and factory suites run with debugging enabled during the offline build.
