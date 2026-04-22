/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
    kalman-module.h

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

#ifndef _QORE_KALMAN_MODULE_H
#define _QORE_KALMAN_MODULE_H

#ifdef HAVE_CONFIG_H
#include "../config.h"
#endif

#include <qore/Qore.h>
#include <Eigen/Dense>

// hashdecl pointers
DLLLOCAL extern const TypedHashDecl* hashdeclKalmanVersionInfo;
DLLLOCAL extern const TypedHashDecl* hashdeclLinearFilterOptions;
DLLLOCAL extern const TypedHashDecl* hashdeclKalmanFilterState;

// class pointers + IDs
DLLLOCAL extern QoreClass* QC_MATRIX;
DLLLOCAL extern qore_classid_t CID_MATRIX;
DLLLOCAL extern QoreClass* QC_LINEARFILTER;
DLLLOCAL extern qore_classid_t CID_LINEARFILTER;
DLLLOCAL extern QoreClass* QC_EXTENDEDFILTER;
DLLLOCAL extern qore_classid_t CID_EXTENDEDFILTER;

// QPP-generated init functions
DLLLOCAL void init_kalman_functions(QoreNamespace& ns);
DLLLOCAL TypedHashDecl* init_hashdecl_KalmanVersionInfo(QoreNamespace& ns);
DLLLOCAL TypedHashDecl* init_hashdecl_LinearFilterOptions(QoreNamespace& ns);
DLLLOCAL TypedHashDecl* init_hashdecl_KalmanFilterState(QoreNamespace& ns);
DLLLOCAL QoreClass* initMatrixClass(QoreNamespace& ns);
DLLLOCAL QoreClass* initLinearFilterClass(QoreNamespace& ns);
DLLLOCAL QoreClass* initExtendedFilterClass(QoreNamespace& ns);

#endif
