# Copyright (C) 2026 Qore Technologies, s.r.o.
# SPDX-License-Identifier: MIT
# Use the pinned source epoch for RPM headers and installed file timestamps.
%global source_date_epoch_from_changelog 1
%global use_source_date_epoch_as_buildtime 1
%if v"%{rpmversion}" >= v"4.20"
%global build_mtime_policy clamp_to_source_date_epoch
%else
%global clamp_mtime_to_source_date_epoch 1
%endif
%bcond_without tests
%bcond_without docs
Name: qore-kalman-module
Version: 1.1.0
Release: 1%{?dist}
Summary: Kalman filters and matrix operations for Qore
License: MIT AND MPL-2.0 AND BSD-3-Clause AND Apache-2.0
URL: https://github.com/qoretechnologies/module-kalman
Source0: %{name}-%{version}.tar.xz
BuildRequires: cmake >= 3.5
BuildRequires: make
BuildRequires: gcc-c++
BuildRequires: pkgconfig(eigen3) >= 3.3
# Eigen templates are instantiated in the native module.
Provides: bundled(eigen)
BuildRequires: qore-devel >= 3.0.0~
BuildRequires: qore-rpm-macros >= 3.0.0~
%if %{with docs}
BuildRequires: doxygen
%if 0%{?suse_version}
BuildRequires: util-linux
%else
BuildRequires: util-linux-core
%endif
%endif

%description
Linear and extended Kalman filters, matrix operations, and filter factories
for tracking, sensor fusion and noisy sequential observations. The native module
uses Eigen headers provided by the distribution.

%if %{with docs}
%package doc
Summary: Kalman module reference documentation
BuildArch: noarch
%description doc
API reference and examples for Qore's Kalman filters and matrix operations.
%endif

%prep
%autosetup
# Retain the actual distribution header package's license notices.
eigen_package=$(rpm -q --whatprovides 'pkgconfig(eigen3)' --qf '%%{NAME}\n')
rpm -q --licensefiles "$eigen_package" > eigen-license-files
mkdir Eigen-licenses
while IFS= read -r license; do
  if test -f "$license"; then
    cp -p "$license" Eigen-licenses/
  fi
done < eigen-license-files
test -s Eigen-licenses/COPYING.MPL2
%build
%{?set_build_flags}
. %{_rpmconfigdir}/qore/module-env.sh
qore_set_source_prefix_maps "%{qore_debug_source_dir}"
cmake -S . -B build -G 'Unix Makefiles' \
  -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS_RELEASE=-DNDEBUG \
  -DCMAKE_INSTALL_PREFIX=%{_prefix} \
  -DCMAKE_SKIP_RPATH=ON -DCMAKE_IGNORE_PREFIX_PATH=/usr/local \
  -DQore_DIR=%{_libdir}/cmake/Qore -DQORE_EXECUTABLE=/usr/bin/qore \
  -DQORE_QPP_EXECUTABLE=/usr/bin/qpp \
  -DCMAKE_DISABLE_FIND_PACKAGE_Doxygen=%{!?with_docs:ON}%{?with_docs:OFF}
cmake --build build -- %{?_smp_mflags}
%if %{with docs}
cmake --build build --target docs -- %{?_smp_mflags}
%endif
%install
DESTDIR=%{buildroot} cmake --install build
chmod 755 %{buildroot}%{_libdir}/qore-modules/kalman-api-*.qmod
%if %{with docs}
install -d %{buildroot}%{_docdir}/%{name}-doc
cp -a build/docs/kalman/html %{buildroot}%{_docdir}/%{name}-doc/
hardlink -t -O %{buildroot}%{_docdir}/%{name}-doc
%endif
%check
%if %{with tests}
python3 -B -W error test/test_uninstall.py -v
. %{_rpmconfigdir}/qore/module-env.sh
for test in test/*.qtest; do
  timeout 180 /usr/bin/qore -b --enable-debug \
    -l "$PWD/build/kalman-api-$(/usr/bin/qore --latest-module-api).qmod" "$test" -v
done
%endif
%files
%license COPYING.MIT Eigen-licenses
%doc README
%{_libdir}/qore-modules/kalman-api-*.qmod
%dir %{_datadir}/qore/metadata/kalman
%{_datadir}/qore/metadata/kalman/*.meta.json
%if %{with docs}
%files doc
%license COPYING.MIT
%doc %{_docdir}/%{name}-doc/
%endif
%changelog
* Thu Oct 01 2026 David Nichols <david@qore.org> - 1.1.0-1
- Package the native filter module, SDK metadata, documentation and offline tests.
