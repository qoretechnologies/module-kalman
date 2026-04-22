/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
    kalman-module.cpp

    Qore Programming Language

    Copyright (C) 2026 Qore Technologies, s.r.o.

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
*/

#include "kalman-module.h"

static void kalman_module_init(QoreModuleInitContext& ctx, ExceptionSink& xsink);
static void kalman_module_ns_init(QoreNamespace* rns, QoreNamespace* qns, ExceptionSink& xsink);
static void kalman_module_delete();

extern "C" DLLEXPORT void kalman_qore_module_desc(QoreModuleInfo& mod_info) {
    mod_info.name = "kalman";
    mod_info.version = PACKAGE_VERSION;
    mod_info.desc = "Kalman filter module - Linear/Extended/Unscented Kalman filters on Eigen";
    mod_info.author = "Qore Technologies, s.r.o.";
    mod_info.url = "https://github.com/qoretechnologies/module-kalman";
    mod_info.api_major = QORE_MODULE_API_MAJOR;
    mod_info.api_minor = QORE_MODULE_API_MINOR;
    mod_info.init = kalman_module_init;
    mod_info.ns_init = kalman_module_ns_init;
    mod_info.del = kalman_module_delete;
    mod_info.license = QL_MIT;
    mod_info.license_str = "MIT";
}

// hashdecl pointers — qpp generates code that assigns these, but doesn't declare them.
const TypedHashDecl* hashdeclKalmanVersionInfo = nullptr;
const TypedHashDecl* hashdeclLinearFilterOptions = nullptr;
const TypedHashDecl* hashdeclKalmanFilterState = nullptr;

// NOTE: the class pointers QC_* and ids CID_* are DEFINED by qpp in the
// generated QC_*.cpp files — do not declare them here or the link step
// produces duplicate-symbol errors.

QoreNamespace KNS("Qore::Kalman");

static void kalman_module_init(QoreModuleInitContext& ctx, ExceptionSink& xsink) {
    // hashdecls first (referenced by functions/class methods below)
    hashdeclKalmanVersionInfo = init_hashdecl_KalmanVersionInfo(KNS);
    hashdeclLinearFilterOptions = init_hashdecl_LinearFilterOptions(KNS);
    hashdeclKalmanFilterState = init_hashdecl_KalmanFilterState(KNS);
    // classes
    QC_MATRIX = initMatrixClass(KNS);
    KNS.addSystemClass(QC_MATRIX);
    QC_LINEARFILTER = initLinearFilterClass(KNS);
    KNS.addSystemClass(QC_LINEARFILTER);
    QC_EXTENDEDFILTER = initExtendedFilterClass(KNS);
    KNS.addSystemClass(QC_EXTENDEDFILTER);
    // standalone functions last
    init_kalman_functions(KNS);
}

static void kalman_module_ns_init(QoreNamespace* rns, QoreNamespace* qns, ExceptionSink& xsink) {
    qns->addNamespace(KNS.copy());
}

static void kalman_module_delete() {
}
