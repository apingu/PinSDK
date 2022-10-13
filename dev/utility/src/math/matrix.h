//
//
//
//
//
//
//
//
//
//
//
//
//
//                                               Copyright    (C)    2002    Pin
//

#ifndef CPMATRIX_H
#define CPMATRIX_H

#include "Math3D.h"

// NOTE.  The (x,y,z) coordinate system is assumed to be right-handed.
// Coordinate axis rotation matrices are of the form
//   RX =    1       0       0
//           0     cos(t) -sin(t)
//           0     sin(t)  cos(t)
// where t > 0 indicates a counterclockwise rotation in the yz-plane
//   RY =  cos(t)    0     sin(t)
//           0       1       0
//        -sin(t)    0     cos(t)
// where t > 0 indicates a counterclockwise rotation in the zx-plane
//   RZ =  cos(t) -sin(t)    0
//         sin(t)  cos(t)    0
//           0       0       1
// where t > 0 indicates a counterclockwise rotation in the xy-plane.


class CPMatrix3f
{
	float m_aafEntry[3][3];

protected:

    // support for eigensolver
    void Tridiagonal ( float afDiag[3], float afSubDiag[3] );
    bool QLAlgorithm ( float afDiag[3], float afSubDiag[3] );

    // support for singular value decomposition
    static void Bidiagonalize ( CPMatrix3f& kA, CPMatrix3f& kL, CPMatrix3f& kR );
    static void GolubKahanStep( CPMatrix3f& kA, CPMatrix3f& kL, CPMatrix3f& kR );
    // support for spectral norm
	static float MaxCubicRoot (float afCoeff[3]);

public:

    static const float EPSILON;
    static const CPMatrix3f ZERO;
	static const CPMatrix3f IDENTITY;
    static const float ms_fSvdEpsilon;
    static const int ms_iSvdMaxIterations;

    // construction
    CPMatrix3f ();
    CPMatrix3f (const float aafEntry[3][3]);
    CPMatrix3f (const CPMatrix3f& rkMatrix);
    CPMatrix3f (float fEntry00, float fEntry01, float fEntry02,
                float fEntry10, float fEntry11, float fEntry12,
                float fEntry20, float fEntry21, float fEntry22);

    // member access, allows use of construct mat[r][c]
    float* operator[] (int iRow) const;
    operator float* ();
    CPVertex3f GetColumn (int iCol) const;

    // assignment and comparison
    CPMatrix3f& operator= (const CPMatrix3f& rkMatrix);
    bool operator== (const CPMatrix3f& rkMatrix) const;
    bool operator!= (const CPMatrix3f& rkMatrix) const;

    // arithmetic operations
    CPMatrix3f operator+ (const CPMatrix3f& rkMatrix) const;
    CPMatrix3f operator- (const CPMatrix3f& rkMatrix) const;
    CPMatrix3f operator* (const CPMatrix3f& rkMatrix) const;
    CPMatrix3f operator- () const;

    // matrix * vector [3x3 * 3x1 = 3x1]
    CPVertex3f operator* (const CPVertex3f& rkVector) const;

    // vector * matrix [1x3 * 3x3 = 1x3]
    friend CPVertex3f operator* (const CPVertex3f& rkVector,
        const CPMatrix3f& rkMatrix);

    // matrix * scalar
    CPMatrix3f operator* (float fScalar) const;

    // scalar * matrix
    friend CPMatrix3f operator* (float fScalar, const CPMatrix3f& rkMatrix);

	// utilities
    CPMatrix3f Transpose () const;
    bool Inverse (CPMatrix3f& rkInverse, float fTolerance = 1e-06) const;
    CPMatrix3f Inverse (float fTolerance = 1e-06) const;
    float Determinant () const;

    // singular value decomposition
    void SingularValueDecomposition (CPMatrix3f& rkL, CPVertex3f& rkS,
        CPMatrix3f& rkR) const;
    void SingularValueComposition (const CPMatrix3f& rkL,
        const CPVertex3f& rkS, const CPMatrix3f& rkR);

    // Gram-Schmidt orthonormalization (applied to columns of rotation matrix)
    void Orthonormalize ();

    // orthogonal Q, diagonal D, upper triangular U stored as (u01,u02,u12)
    void QDUDecomposition (CPMatrix3f& rkQ, CPVertex3f& rkD,
        CPVertex3f& rkU) const;

    float SpectralNorm () const;

	/*
    // matrix must be orthonormal
    void ToAxisAngle (CPVertex3f& rkAxis, float& rfRadians) const;
    void FromAxisAngle (const CPVertex3f& rkAxis, float fRadians);
    float ExtractAngle (int i) const;

    // The matrix must be orthonormal.  The decomposition is yaw*pitch*roll
    // where yaw is rotation about the Up vector, pitch is rotation about the
    // Right axis, and roll is rotation about the Direction axis.
    bool ToEulerAnglesXYZ (float& rfYAngle, float& rfPAngle, float& rfRAngle);
    bool ToEulerAnglesXZY (float& rfYAngle, float& rfPAngle, float& rfRAngle);
    bool ToEulerAnglesYXZ (float& rfYAngle, float& rfPAngle, float& rfRAngle);
    bool ToEulerAnglesYZX (float& rfYAngle, float& rfPAngle, float& rfRAngle);
    bool ToEulerAnglesZXY (float& rfYAngle, float& rfPAngle, float& rfRAngle);
    bool ToEulerAnglesZYX (float& rfYAngle, float& rfPAngle, float& rfRAngle);
    void FromEulerAnglesXYZ (float fYAngle, float fPAngle, float fRAngle);
    void FromEulerAnglesXZY (float fYAngle, float fPAngle, float fRAngle);
    void FromEulerAnglesYXZ (float fYAngle, float fPAngle, float fRAngle);
    void FromEulerAnglesYZX (float fYAngle, float fPAngle, float fRAngle);
    void FromEulerAnglesZXY (float fYAngle, float fPAngle, float fRAngle);
    void FromEulerAnglesZYX (float fYAngle, float fPAngle, float fRAngle);

    // eigensolver, matrix must be symmetric
    void EigenSolveSymmetric (float afEigenvalue[3],
        CPVertex3f akEigenvector[3]) const;

    static void TensorProduct (const CPVertex3f& rkU, const CPVertex3f& rkV,
        CPMatrix3f& rkProduct);
		*/

    
};

#endif
