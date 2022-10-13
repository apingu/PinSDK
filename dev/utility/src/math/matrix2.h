// Magic Software, Inc.
// http://www.magic-software.com
// Copyright (c) 2000, All Rights Reserved
//
// Source code from Magic Software is supplied under the terms of a license
// agreement and may not be copied or disclosed except in accordance with the
// terms of that agreement.  The various license agreements may be found at
// the Magic Software web site.  This file is subject to the license
//
// FREE SOURCE CODE
// http://www.magic-software.com/License/free.pdf

#ifndef MGCMATRIX2_H
#define MGCMATRIX2_H

#include "MgcVector2.h"

// NOTE.  Rotation matrices are of the form
//   R = cos(t) -sin(t)
//       sin(t)  cos(t)
// where t > 0 indicates a counterclockwise rotation in the xy-plane.


class MgcMatrix2
{
public:
    // construction
    MgcMatrix2 ();
    MgcMatrix2 (const MgcReal aafEntry[2][2]);
    MgcMatrix2 (const MgcMatrix2& rkMatrix);
    MgcMatrix2 (MgcReal fEntry00, MgcReal fEntry01,
                MgcReal fEntry10, MgcReal fEntry11);

    // member access, allows use of construct mat[r][c]
    MgcReal* operator[] (int iRow) const;
    operator MgcReal* ();
    MgcVector2 GetColumn (int iCol) const;

    // assignment and comparison
    MgcMatrix2& operator= (const MgcMatrix2& rkMatrix);
    bool operator== (const MgcMatrix2& rkMatrix) const;
    bool operator!= (const MgcMatrix2& rkMatrix) const;

    // arithmetic operations
    MgcMatrix2 operator+ (const MgcMatrix2& rkMatrix) const;
    MgcMatrix2 operator- (const MgcMatrix2& rkMatrix) const;
    MgcMatrix2 operator* (const MgcMatrix2& rkMatrix) const;
    MgcMatrix2 operator- () const;

    // matrix * vector [2x2 * 2x1 = 2x1]
    MgcVector2 operator* (const MgcVector2& rkVector) const;

    // vector * matrix [1x2 * 2x2 = 1x2]
    friend MgcVector2 operator* (const MgcVector2& rkVector,
        const MgcMatrix2& rkMatrix);

    // matrix * scalar
    MgcMatrix2 operator* (MgcReal fScalar) const;

    // scalar * matrix
    friend MgcMatrix2 operator* (MgcReal fScalar, const MgcMatrix2& rkMatrix);

    // utilities
    MgcMatrix2 Transpose () const;
    bool Inverse (MgcMatrix2& rkInverse,
        MgcReal fTolerance = 1e-06) const;
    MgcMatrix2 Inverse (MgcReal fTolerance = 1e-06) const;
    MgcReal Determinant () const;

    // singular value decomposition
    void SingularValueDecomposition (MgcMatrix2& rkL,
        MgcVector2& rkS, MgcMatrix2& rkR) const;
    void SingularValueComposition (const MgcMatrix2& rkL,
        const MgcVector2& rkS, const MgcMatrix2& rkR);

    // Gram-Schmidt orthonormalization (applied to columns of rotation matrix)
    void Orthonormalize ();

    // orthogonal Q, diagonal D, upper triangular U stored as (u01)
    void QDUDecomposition (MgcMatrix2& rkQ, MgcVector2& rkD,
        MgcReal& rfU) const;

    MgcReal SpectralNorm () const;
    MgcReal L2NormSqr () const;

    // matrix must be orthonormal
    void ToAngle (MgcReal& rfRadians) const;
    void FromAngle (MgcReal fRadians);

    // eigensolver, matrix must be symmetric
    void EigenSolveSymmetric (MgcReal afEigenvalue[2],
        MgcVector2 akEigenvector[2]) const;

    static void TensorProduct (const MgcVector2& rkU, const MgcVector2& rkV,
        MgcMatrix2& rkProduct);

    static const MgcReal EPSILON;
    static const MgcMatrix2 ZERO;
    static const MgcMatrix2 IDENTITY;

protected:
    // support for eigensolver
    void Tridiagonal (MgcReal afDiag[2], MgcReal afSubDiag[2]);
    bool QLAlgorithm (MgcReal afDiag[2], MgcReal afSubDiag[2]);

    MgcReal m_aafEntry[2][2];
};

#endif
