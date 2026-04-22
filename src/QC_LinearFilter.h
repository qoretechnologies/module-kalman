/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
    QC_LinearFilter.h

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

#ifndef _QORE_QC_LINEARFILTER_H
#define _QORE_QC_LINEARFILTER_H

#include "kalman-module.h"

// Standard linear Kalman filter (LKF).
//
// State vector x (state_size x 1), covariance P (state_size x state_size).
// Transition F (state_size x state_size); observation H (measurement_size x
// state_size); process noise Q (state_size x state_size); measurement
// noise R (measurement_size x measurement_size); optional control input
// matrix B (state_size x control_size).
//
// Joseph form covariance update (P = (I - K·H)·P·(I - K·H)ᵀ + K·R·Kᵀ) is
// used in update() to preserve positive-definiteness of P under
// accumulated floating-point error — a stronger guarantee than the
// simpler P = (I - K·H)·P form.
//
// Thread safety: NOT safe to share across threads. Each filter holds
// mutable x / P / counters. Typical use is one filter per tracked
// object, with the filter owned by a single thread.
class QoreLinearFilter : public AbstractPrivateData {
public:
    DLLLOCAL QoreLinearFilter(int state_size, int measurement_size, int control_size);

    DLLLOCAL Eigen::Index stateSize() const { return n; }
    DLLLOCAL Eigen::Index measurementSize() const { return mdim; }
    DLLLOCAL Eigen::Index controlSize() const { return cdim; }

    DLLLOCAL const Eigen::MatrixXd& getState() const { return x; }
    DLLLOCAL const Eigen::MatrixXd& getCovariance() const { return P; }
    DLLLOCAL int64 getUpdateCount() const { return update_count; }
    DLLLOCAL int64 getPredictCount() const { return predict_count; }

    // Setters — each validates dimensions and raises
    // KALMAN-DIMENSION-ERROR on mismatch.
    DLLLOCAL void setF(const Eigen::MatrixXd& F_, ExceptionSink* xsink);
    DLLLOCAL void setH(const Eigen::MatrixXd& H_, ExceptionSink* xsink);
    DLLLOCAL void setQ(const Eigen::MatrixXd& Q_, ExceptionSink* xsink);
    DLLLOCAL void setR(const Eigen::MatrixXd& R_, ExceptionSink* xsink);
    DLLLOCAL void setB(const Eigen::MatrixXd& B_, ExceptionSink* xsink);
    DLLLOCAL void setState(const Eigen::MatrixXd& x_,
            const Eigen::MatrixXd& P_, ExceptionSink* xsink);

    // Step operations — raise a KALMAN-STATE-ERROR if setF/setH/setQ/setR
    // weren't called beforehand, or KALMAN-DIMENSION-ERROR on mismatched
    // inputs.
    DLLLOCAL void predict(ExceptionSink* xsink);
    DLLLOCAL void predictWithControl(const Eigen::MatrixXd& u, ExceptionSink* xsink);
    DLLLOCAL void update(const Eigen::MatrixXd& z, ExceptionSink* xsink);

private:
    Eigen::Index n;     // state size
    Eigen::Index mdim;  // measurement size
    Eigen::Index cdim;  // control size (0 = no control)
    Eigen::MatrixXd x;  // state
    Eigen::MatrixXd P;  // covariance
    Eigen::MatrixXd F, H, Q, R, B;
    bool haveF, haveH, haveQ, haveR, haveB, haveState;
    int64 predict_count, update_count;
};

#endif
