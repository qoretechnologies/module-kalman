# - Find Eigen3 (header-only C++ template linear algebra library)
# Fallback find-module used when Eigen3's upstream CMake config package
# (Eigen3Config.cmake, shipped by eigen >= 3.3) is unavailable.
#
# Once done this will define:
#  Eigen3_FOUND       - system has Eigen3 headers
#  EIGEN3_INCLUDE_DIR - the Eigen3 include directory
#
# Supported platforms: Ubuntu (libeigen3-dev), Debian (libeigen3-dev),
# Alpine (eigen-dev), Fedora/RHEL (eigen3-devel), macOS Homebrew
# (eigen), macOS MacPorts (eigen3), FreeBSD (math/eigen3).

find_package(PkgConfig QUIET)
if(PKG_CONFIG_FOUND)
    pkg_check_modules(PC_EIGEN3 QUIET eigen3)
endif()

# Eigen header directory has an `Eigen/Dense` top-level — test for that.
find_path(EIGEN3_INCLUDE_DIR Eigen/Dense
    HINTS
        ${PC_EIGEN3_INCLUDEDIR}
        ${PC_EIGEN3_INCLUDE_DIRS}
        ${EIGEN3_DIR}
        $ENV{EIGEN3_DIR}
    PATHS
        /usr/include/eigen3         # Debian/Ubuntu (libeigen3-dev)
        /usr/include
        /usr/local/include/eigen3
        /usr/local/include
        /opt/local/include/eigen3   # MacPorts
        /opt/homebrew/include/eigen3 # Homebrew ARM
        /usr/local/opt/eigen/include/eigen3  # Homebrew x86
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Eigen3 DEFAULT_MSG EIGEN3_INCLUDE_DIR)

mark_as_advanced(EIGEN3_INCLUDE_DIR)
