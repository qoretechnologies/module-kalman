/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
    QC_ExtendedFilter.h

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

#ifndef _QORE_QC_EXTENDEDFILTER_H
#define _QORE_QC_EXTENDEDFILTER_H

#include "kalman-module.h"

// Extended Kalman filter (EKF) — for non-linear state transition and /
// or non-linear measurement models. The user supplies two Qore
// callbacks: one evaluates the state-transition function f(x) plus its
// Jacobian F = ∂f/∂x, the other evaluates the measurement function
// h(x) plus its Jacobian H = ∂h/∂x. The filter linearises around the
// current state estimate at each step.
//
// Callback contract:
//   predict_fn(Matrix x_current) -> hash{"predicted": Matrix, "jacobian": Matrix}
//   measurement_fn(Matrix x_current) -> hash{"predicted": Matrix, "jacobian": Matrix}
//
// Predict's "jacobian" has shape state_size × state_size.
// Measurement's "jacobian" has shape measurement_size × state_size.
//
// Same thread-safety story as QoreLinearFilter: not safe to share.
class QoreExtendedFilter : public AbstractPrivateData {
public:
    DLLLOCAL QoreExtendedFilter(int state_size, int measurement_size);
    DLLLOCAL virtual ~QoreExtendedFilter();

    DLLLOCAL Eigen::Index stateSize() const { return n; }
    DLLLOCAL Eigen::Index measurementSize() const { return mdim; }
    DLLLOCAL const Eigen::MatrixXd& getState() const { return x; }
    DLLLOCAL const Eigen::MatrixXd& getCovariance() const { return P; }
    DLLLOCAL int64 getUpdateCount() const { return update_count; }
    DLLLOCAL int64 getPredictCount() const { return predict_count; }

    // Setters.
    DLLLOCAL void setQ(const Eigen::MatrixXd& Q_, ExceptionSink* xsink);
    DLLLOCAL void setR(const Eigen::MatrixXd& R_, ExceptionSink* xsink);
    DLLLOCAL void setState(const Eigen::MatrixXd& x_,
            const Eigen::MatrixXd& P_, ExceptionSink* xsink);

    // Callback ownership is taken by the filter — caller must pass a
    // reference-counted QoreValue (via ReferenceHolder or equivalent)
    // that the filter retains until destructor or next replacement.
    DLLLOCAL void setPredictFunction(QoreValue cb, ExceptionSink* xsink);
    DLLLOCAL void setMeasurementFunction(QoreValue cb, ExceptionSink* xsink);

    // Step operations.
    DLLLOCAL void predict(ExceptionSink* xsink);
    DLLLOCAL void update(const Eigen::MatrixXd& z, ExceptionSink* xsink);

private:
    Eigen::Index n;
    Eigen::Index mdim;
    Eigen::MatrixXd x;
    Eigen::MatrixXd P;
    Eigen::MatrixXd Q, R;
    QoreValue predict_cb;       // code reference or closure; retains a ref
    QoreValue measurement_cb;
    bool haveQ, haveR, haveState;
    int64 predict_count, update_count;
};

#endif
