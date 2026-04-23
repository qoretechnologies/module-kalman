cmake_minimum_required(VERSION 3.5)

project(qore-kalman-module)

set(VERSION_MAJOR 1)
set(VERSION_MINOR 1)
set(VERSION_PATCH 0)

# where to look first for cmake modules, before ${CMAKE_ROOT}/Modules/ is checked
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

find_package(Qore 0.9 REQUIRED)

# Eigen's upstream ships its own CMake config (Eigen3Config.cmake). Prefer
# it when present; fall back to our FindEigen3.cmake for older systems
# or pkg-config-only setups. Eigen is header-only — the include dir is
# attached to the kalman target after add_library() below.
find_package(Eigen3 3.3 CONFIG QUIET)
if(Eigen3_FOUND AND TARGET Eigen3::Eigen)
    message(STATUS "Found Eigen3 via CMake config: ${Eigen3_VERSION}")
else()
    find_package(Eigen3 REQUIRED)
endif()

# ensure qdx can find the kalman binary module and user modules when processing .qc files
set(QORE_MODULE_DIR_FOR_DOCS "${CMAKE_BINARY_DIR}:${CMAKE_SOURCE_DIR}/qlib")

include_directories(${CMAKE_SOURCE_DIR}/src)

set(QPP_SRC
    src/ql_kalman.qpp
    src/QC_Matrix.qpp
    src/QC_LinearFilter.qpp
    src/QC_ExtendedFilter.qpp
)

set(CPP_SRC
    src/kalman-module.cpp
)

# C++17 required for structured bindings, Eigen 3.4+ templates, and the
# usual Qore module-C++ baseline.
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++17" COMPILER_SUPPORTS_CXX17)
if(COMPILER_SUPPORTS_CXX17)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
else()
    message(FATAL_ERROR "The compiler ${CMAKE_CXX_COMPILER} has no C++17 support. Please use a different C++ compiler.")
endif()

qore_wrap_qpp_value(QPP_SOURCES METALIST QPP_META_SOURCES ${QPP_SRC})

set(module_name "kalman")

set(QORE_DOX_TMPL_SRC
    docs/mainpage.doxygen.tmpl
)

add_library(${module_name} MODULE ${CPP_SRC} ${QPP_SOURCES})

if(TARGET Eigen3::Eigen)
    target_link_libraries(${module_name} PRIVATE Eigen3::Eigen)
else()
    target_include_directories(${module_name} PRIVATE ${EIGEN3_INCLUDE_DIR})
endif()

if(WIN32 AND MINGW AND MSYS)
    target_compile_definitions(${module_name} PUBLIC BUILDING_DLL)
endif()

if(DEFINED ENV{DOXYGEN_EXECUTABLE})
    set(DOXYGEN_EXECUTABLE $ENV{DOXYGEN_EXECUTABLE})
endif()

# Eigen is header-only — no link-time library to pass in.
qore_external_binary_module(${module_name} "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")

if(COMMAND qore_install_qpp_metadata)
    qore_install_qpp_metadata(${module_name} ${QPP_META_SOURCES})
else()
    install(FILES ${QPP_META_SOURCES}
            DESTINATION ${CMAKE_INSTALL_DATADIR}/qore/metadata)
endif()

qore_dist("${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")

qore_config_info()

if(DOXYGEN_FOUND)
    qore_wrap_dox(QORE_DOX_SRC ${QORE_DOX_TMPL_SRC})
    add_custom_target(QORE_MOD_DOX_FILES DEPENDS ${QORE_DOX_SRC})
    add_dependencies(docs-module QORE_MOD_DOX_FILES)
endif()
