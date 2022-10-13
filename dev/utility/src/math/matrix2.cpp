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

#include "MgcMatrix2.h"
#include "MgcRTLib.h"

const MgcReal MgcMatrix2::EPSILON = 1e-06;
const MgcMatrix2 MgcMatrix2::ZERO(0,0,0,0);
const MgcMatrix2 MgcMatrix2::IDENTITY(1,0,0,1);

//----------------------------------------------------------------------------
MgcMatrix2::MgcMatrix2 ()
{
    // For efficiency reasons, do not initialize matrix.
}
//----------------------------------------------------------------------------
MgcMatrix2::MgcMatrix2 (const MgcReal aafEntry[2][2])
{
    memcpy(m_aafEntry,aafEntry,4*sizeof(MgcReal));
}
//----------------------------------------------------------------------------
MgcMatrix2::MgcMatrix2 (const MgcMatrix2& rkMatrix)
{
    memcpy(m_aafEntry,rkMatrix.m_aafEntry,4*sizeof(MgcReal));
}
//----------------------------------------------------------------------------
MgcMatrix2::MgcMatrix2 (MgcReal fEntry00, MgcReal fEntry01, MgcReal fEntry10,
    MgcReal fEntry11)
{
    m_aafEntry[0][0] = fEntry00;
    m_aafEntry[0][1] = fEntry01;
    m_aafEntry[1][0] = fEntry10;
    m_aafEntry[1][1] = fEntry11;
}
//----------------------------------------------------------------------------
MgcReal* MgcMatrix2::operator[] (int iRow) const
{
    return (MgcReal*)&m_aafEntry[iRow][0];
}
//----------------------------------------------------------------------------
MgcMatrix2::operator MgcReal* ()
{
    return &m_aafEntry[0][0];
}
//----------------------------------------------------------------------------
MgcVector2 MgcMatrix2::GetColumn (int iCol) const
{
    assert( 0 <= iCol && iCol < 2 );
    return MgcVector2(m_aafEntry[0][iCol],m_aafEntry[1][iCol]);
}
//----------------------------------------------------------------------------
MgcMatrix2& MgcMatrix2::operator= (const MgcMatrix2& rkMatrix)
{
    memcpy(m_aafEntry,rkMatrix.m_aafEntry,4*sizeof(MgcReal));
    return *this;
}
//----------------------------------------------------------------------------
bool MgcMatrix2::operator== (const MgcMatrix2& rkMatrix) const
{
    for (int iRow = 0; iRow < 2; iRow++)
    {
        for (int iCol = 0; iCol < 2; iCol++)
        {
            if ( m_aafEntry[iRow][iCol] != rkMatrix.m_aafEntry[iRow][iCol] )
                return false;
        }
    }

    return true;
}
//----------------------------------------------------------------------------
bool MgcMatrix2::operator!= (const MgcMatrix2& rkMatrix) const
{
    return !operator==(rkMatrix);
}
//----------------------------------------------------------------------------
MgcMatrix2 MgcMatrix2::operator+ (const MgcMatrix2& rkMatrix) const
{
    MgcMatrix2 kSum;
    for (int iRow = 0; iRow < 2; iRow++)
    {
        for (int iCol = 0; iCol < 2; iCol++)
        {
            kSum.m_aafEntry[iRow][iCol] = m_aafEntry[iRow][iCol] +
                rkMatrix.m_aafEntry[iRow][iCol];
        }
    }
    return kSum;
}
//----------------------------------------------------------------------------
MgcMatrix2 MgcMatrix2::operator- (const MgcMatrix2& rkMatrix) const
{
    MgcMatrix2 kDiff;
    for (int iRow = 0; iRow < 2; iRow++)
    {
        for (int iCol = 0; iCol < 2; iCol++)
        {
            kDiff.m_aafEntry[iRow][iCol] = m_aafEntry[iRow][iCol] -
                rkMatrix.m_aafEntry[iRow][iCol];
        }
    }
    return kDiff;
}
//----------------------------------------------------------------------------
MgcMatrix2 MgcMatrix2::operator* (const MgcMatrix2& rkMatrix) const
{
    MgcMatrix2 kProd;
    for (int iRow = 0; iRow < 2; iRow++)
    {
        for (int iCol = 0; iCol < 2; iCol++)
        {
            kProd.m_aafEntry[iRow][iCol] =
                m_aafEntry[iRow][0]*rkMatrix.m_aafEntry[0][iCol] +
                m_aafEntry[iRow][1]*rkMatrix.m_aafEntry[1][iCol];
        }
    }
    return kProd;
}
//----------------------------------------------------------------------------
MgcVector2 MgcMatrix2::operator* (const MgcVector2& rkVector) const
{
    MgcVector2 kProd;
    for (int iRow = 0; iRow < 2; iRow++)
    {
        kProd[iRow] =
            m_aafEntry[iRow][0]*rkVector[0] +
            m_aafEntry[iRow][1]*rkVector[1];
    }
    return kProd;
}
//----------------------------------------------------------------------------
MgcVector2 operator* (const MgcVector2& rkVector, const MgcMatrix2& rkMatrix)
{
    MgcVector2 kProd;
    for (int iRow = 0; iRow < 2; iRow++)
    {
        kProd[iRow] =
            rkVector[0]*rkMatrix.m_aafEntry[0][iRow] +
            rkVector[1]*rkMatrix.m_aafEntry[1][iRow];
    }
    return kProd;
}
//----------------------------------------------------------------------------
MgcMatrix2 MgcMatrix2::operator- () const
{
    MgcMatrix2 kNeg;
    for (int iRow = 0; iRow < 2; iRow++)
    {
        for (int iCol = 0; iCol < 2; iCol++)
            kNeg[iRow][iCol] = -m_aafEntry[iRow][iCol];
    }
    return kNeg;
}
//----------------------------------------------------------------------------
MgcMatrix2 MgcMatrix2::operator* (MgcReal fScalar) const
{
    MgcMatrix2 kProd;
    for (int iRow = 0; iRow < 2; iRow++)
    {
        for (int iCol = 0; iCol < 2; iCol++)
            kProd[iRow][iCol] = fScalar*m_aafEntry[iRow][iCol];
    }
    return kProd;
}
//----------------------------------------------------------------------------
MgcMatrix2 operator* (MgcReal fScalar, const MgcMatrix2& rkMatrix)
{
    MgcMatrix2 kProd;
    for (int iRow = 0; iRow < 2; iRow++)
    {
        for (int iCol = 0; iCol < 2; iCol++)
            kProd[iRow][iCol] = fScalar*rkMatrix.m_aafEntry[iRow][iCol];
    }
    return kProd;
}
//----------------------------------------------------------------------------
MgcMatrix2 MgcMatrix2::Transpose () const
{
    MgcMatrix2 kTranspose;
    for (int iRow = 0; iRow < 2; iRow++)
    {
        for (int iCol = 0; iCol < 2; iCol++)
            kTranspose[iRow][iCol] = m_aafEntry[iCol][iRow];
    }
    return kTranspose;
}
//----------------------------------------------------------------------------
bool MgcMatrix2::Inverse (MgcMatrix2& rkInverse, MgcReal fTolerance) const
{
    MgcReal fDet = m_aafEntry[0][0]*m_aafEntry[1][1] -
        m_aafEntry[0][1]*m_aafEntry[1][0];

    if ( MgcMath::Abs(fDet) <= fTolerance )
        return false;

    MgcReal fInvDet = 1.0/fDet;
    rkInverse[0][0] = m_aafEntry[1][1]*fInvDet;
    rkInverse[1][0] = -m_aafEntry[1][0]*fInvDet;
    rkInverse[0][1] = -m_aafEntry[0][1]*fInvDet;
    rkInverse[1][1] = m_aafEntry[0][0]*fInvDet;

    return true;
}
//----------------------------------------------------------------------------
MgcMatrix2 MgcMatrix2::Inverse (MgcReal fTolerance) const
{
    MgcMatrix2 fInverse = MgcMatrix2::ZERO;
    Inverse(fInverse,fTolerance);
    return fInverse;
}
//----------------------------------------------------------------------------
MgcReal MgcMatrix2::Determinant () const
{
    return m_aafEntry[0][0]*m_aafEntry[1][1] -
           m_aafEntry[0][1]*m_aafEntry[1][0];
}
//----------------------------------------------------------------------------
void MgcMatrix2::SingularValueDecomposition (MgcMatrix2& rkL,
    MgcVector2& rkS, MgcMatrix2& rkR) const
{
    // compute M*M^t and M^t*M where M^t = Transpose(M)
    MgcReal fM00Sqr = m_aafEntry[0][0]*m_aafEntry[0][0];
    MgcReal fM01Sqr = m_aafEntry[0][1]*m_aafEntry[0][1];
    MgcReal fM10Sqr = m_aafEntry[1][0]*m_aafEntry[1][0];
    MgcReal fM11Sqr = m_aafEntry[1][1]*m_aafEntry[1][1];

    MgcReal afMMT[3], afMTM[3];
    afMMT[0] = fM00Sqr + fM01Sqr;
    afMMT[1] = m_aafEntry[0][0]*m_aafEntry[1][0] +
        m_aafEntry[0][1]*m_aafEntry[1][1];
    afMMT[2] = fM10Sqr + fM11Sqr;
    afMTM[0] = fM00Sqr + fM10Sqr;
    afMTM[1] = m_aafEntry[0][0]*m_aafEntry[0][1] +
        m_aafEntry[1][0]*m_aafEntry[1][1];
    afMTM[2] = fM01Sqr + fM11Sqr;

    // factor M*M^t = L*S^2*L^t
    MgcReal fCos, fSin, fInvLen;
    if ( MgcMath::Abs(afMMT[1]) > 0.0 )
    {
        MgcReal fTrace = afMMT[0] + afMMT[2];
        MgcReal fDet = afMMT[0]*afMMT[2] - afMMT[1]*afMMT[1];
        MgcReal fDiscr = fTrace*fTrace-4.0*fDet;
        MgcReal fRoot = MgcMath::Sqrt(MgcMath::Abs(fDiscr));
        rkS[0] = 0.5*(fTrace + fRoot);
        rkS[1] = 0.5*(fTrace - fRoot);

        fCos = afMMT[1];
        fSin = rkS[0] - afMMT[0];
        fInvLen = 1.0/MgcMath::Sqrt(fCos*fCos + fSin*fSin);
        fCos *= fInvLen;
        fSin *= fInvLen;

        rkL[0][0] = fCos;
        rkL[1][0] = fSin;
        rkL[0][1] = -fSin;
        rkL[1][1] = fCos;
    }
    else
    {
        rkS[0] = afMMT[0];
        rkS[1] = afMMT[2];
        rkL = IDENTITY;
    }

    // factor M^t*M = R^t*S^2*R
    if ( MgcMath::Abs(afMTM[1]) > 0.0 )
    {
        fCos = afMTM[1];
        fSin = rkS[0] - afMTM[0];
        fInvLen = 1.0/MgcMath::Sqrt(fCos*fCos + fSin*fSin);
        fCos *= fInvLen;
        fSin *= fInvLen;

        rkR[0][0] = fCos;
        rkR[1][0] = -fSin;
        rkR[0][1] = +fSin;
        rkR[1][1] = fCos;
    }
    else
    {
        rkR = IDENTITY;
    }

    rkS[0] = MgcMath::Sqrt(MgcMath::Abs(rkS[0]));
    rkS[1] = MgcMath::Sqrt(MgcMath::Abs(rkS[1]));

    MgcVector2 kDiag = rkS;

    MgcMatrix2 kTest;
    kTest.SingularValueComposition(rkL,kDiag,rkR);
    kTest = *this - kTest;
    int i = 0;
    MgcReal fMinNormSqr = kTest.L2NormSqr();

    kDiag[0] = -kDiag[0];
    kTest.SingularValueComposition(rkL,kDiag,rkR);
    kTest = *this - kTest;
    MgcReal fNormSqr = kTest.L2NormSqr();
    if ( fNormSqr < fMinNormSqr )
    {
        i = 1;
        fMinNormSqr = fNormSqr;
    }

    kDiag[1] = -kDiag[1];
    kTest.SingularValueComposition(rkL,kDiag,rkR);
    kTest = *this - kTest;
    fNormSqr = kTest.L2NormSqr();
    if ( fNormSqr < fMinNormSqr )
    {
        i = 2;
        fMinNormSqr = fNormSqr;
    }

    kDiag[0] = -kDiag[0];
    kTest.SingularValueComposition(rkL,kDiag,rkR);
    kTest = *this - kTest;
    fNormSqr = kTest.L2NormSqr();
    if ( fNormSqr < fMinNormSqr )
    {
        i = 3;
        fMinNormSqr = fNormSqr;
    }

    switch ( i )
    {
    case 1:
        rkR[0][0] = -rkR[0][0];
        rkR[0][1] = -rkR[0][1];
        break;
    case 2:
        rkR[0][0] = -rkR[0][0];
        rkR[0][1] = -rkR[0][1];
        rkR[1][0] = -rkR[1][0];
        rkR[1][1] = -rkR[1][1];
        break;
    case 3:
        rkR[1][0] = -rkR[1][0];
        rkR[1][1] = -rkR[1][1];
        break;
    }
}
//----------------------------------------------------------------------------
void MgcMatrix2::SingularValueComposition (const MgcMatrix2& rkL,
    const MgcVector2& rkS, const MgcMatrix2& rkR)
{
    int iRow, iCol;
    MgcMatrix2 kTmp;

    // product S*R
    for (iRow = 0; iRow < 2; iRow++)
    {
        for (iCol = 0; iCol < 2; iCol++)
            kTmp[iRow][iCol] = rkS[iRow]*rkR[iRow][iCol];
    }

    // product L*S*R
    for (iRow = 0; iRow < 2; iRow++)
    {
        for (iCol = 0; iCol < 2; iCol++)
        {
            m_aafEntry[iRow][iCol] = 0.0;
            for (int iMid = 0; iMid < 2; iMid++)
                m_aafEntry[iRow][iCol] += rkL[iRow][iMid]*kTmp[iMid][iCol];
        }
    }
}
//----------------------------------------------------------------------------
void MgcMatrix2::Orthonormalize ()
{
    // Algorithm uses Gram-Schmidt orthogonalization.  If 'this' matrix is
    // M = [m0|m1], then orthonormal output matrix is Q = [q0|q1],
    //
    //   q0 = m0/|m0|
    //   q1 = (m1-(q0*m1)q0)/|m1-(q0*m1)q0|
    //
    // where |V| indicates length of vector V and A*B indicates dot
    // product of vectors A and B.

    // compute q0
    MgcReal fInvLength = 1.0/MgcMath::Sqrt(m_aafEntry[0][0]*m_aafEntry[0][0]
        + m_aafEntry[1][0]*m_aafEntry[1][0]);

    m_aafEntry[0][0] *= fInvLength;
    m_aafEntry[1][0] *= fInvLength;

    // compute q1
    MgcReal fDot0 =
        m_aafEntry[0][0]*m_aafEntry[0][1] +
        m_aafEntry[1][0]*m_aafEntry[1][1];

    m_aafEntry[0][1] -= fDot0*m_aafEntry[0][0];
    m_aafEntry[1][1] -= fDot0*m_aafEntry[1][0];

    fInvLength = 1.0/MgcMath::Sqrt(m_aafEntry[0][1]*m_aafEntry[0][1] +
        m_aafEntry[1][1]*m_aafEntry[1][1]);

    m_aafEntry[0][1] *= fInvLength;
    m_aafEntry[1][1] *= fInvLength;
}
//----------------------------------------------------------------------------
void MgcMatrix2::QDUDecomposition (MgcMatrix2& rkQ, MgcVector2& rkD,
    MgcReal& rkU) const
{
    // Factor M = QR = QDU where Q is orthogonal, D is diagonal, 
    // and U is upper triangular with ones on its diagonal.  Algorithm uses
    // Gram-Schmidt orthogonalization (the QR algorithm).
    //
    // If M = [ m0 | m1 ] and Q = [ q0 | q1 ], then
    //
    //   q0 = m0/|m0| = (a,b)
    //   q1 = (-b,a)
    //
    // where |V| indicates length of vector V and A*B indicates dot
    // product of vectors A and B.  The matrix R has entries
    //
    //   r00 = q0*m0  r01 = q0*m1
    //   r10 = 0      r11 = q1*m1
    //
    // so D = diag(r00,r11) and U has entry u01 = r01/r00.

    // Q = rotation
    // D = scaling
    // U = shear

    // D stores the two diagonal entries r00, r11
    // U stores the entries u01

    // build orthogonal matrix Q
    MgcReal fInvLength = 1.0/MgcMath::Sqrt(m_aafEntry[0][0]*m_aafEntry[0][0]);
    rkQ[0][0] = m_aafEntry[0][0]*fInvLength;
    rkQ[1][0] = m_aafEntry[1][0]*fInvLength;

    MgcReal fDot = rkQ[0][0]*m_aafEntry[0][1] + rkQ[1][0]*m_aafEntry[1][1];
    rkQ[0][1] = m_aafEntry[0][1]-fDot*rkQ[0][0];
    rkQ[1][1] = m_aafEntry[1][1]-fDot*rkQ[1][0];
    fInvLength = 1.0/MgcMath::Sqrt(rkQ[0][1]*rkQ[0][1] +
        rkQ[1][1]*rkQ[1][1]);
    rkQ[0][1] *= fInvLength;
    rkQ[1][1] *= fInvLength;

    // guarantee that orthogonal matrix has determinant 1 (no reflections)
    MgcReal fDet = rkQ[0][0]*rkQ[1][1] - rkQ[0][1]*rkQ[1][0];
    if ( fDet < 0.0 )
    {
        rkQ[0][1] = -rkQ[0][1];
        rkQ[1][1] = -rkQ[1][1];
    }

    // build "right" matrix R
    MgcMatrix2 kR;
    kR[0][0] = rkQ[0][0]*m_aafEntry[0][0] + rkQ[1][0]*m_aafEntry[1][0];
    kR[0][1] = rkQ[0][0]*m_aafEntry[0][1] + rkQ[1][0]*m_aafEntry[1][1];
    kR[1][0] = 0.0;
    kR[1][1] = rkQ[0][1]*m_aafEntry[0][1] + rkQ[1][1]*m_aafEntry[1][1];

    // the scaling component
    rkD[0] = kR[0][0];
    rkD[1] = kR[1][1];

    // the shear component
    rkU = kR[0][1]/rkD[0];
}
//----------------------------------------------------------------------------
MgcReal MgcMatrix2::SpectralNorm () const
{
    MgcMatrix2 kP;
    int iRow, iCol;
    MgcReal fPmax = 0.0;
    for (iRow = 0; iRow < 2; iRow++)
    {
        for (iCol = 0; iCol < 2; iCol++)
        {
            kP[iRow][iCol] = 0.0;
            for (int iMid = 0; iMid < 2; iMid++)
            {
                kP[iRow][iCol] +=
                    m_aafEntry[iMid][iRow]*m_aafEntry[iMid][iCol];
            }
            if ( kP[iRow][iCol] > fPmax )
                fPmax = kP[iRow][iCol];
        }
    }

    MgcReal fInvPmax = 1.0/fPmax;
    for (iRow = 0; iRow < 2; iRow++)
    {
        for (iCol = 0; iCol < 3; iCol++)
            kP[iRow][iCol] *= fInvPmax;
    }

    MgcReal fDet = kP[0][0]*kP[1][1]-kP[0][1]*kP[1][0];
    MgcReal fTrace = -(kP[0][0]+kP[1][1]);
    MgcReal fRoot = 0.5*(-fTrace + MgcMath::Sqrt(MgcMath::Abs(fTrace*fTrace
        - 4.0*fDet)));
    MgcReal fNorm = MgcMath::Sqrt(fPmax*fRoot);
    return fNorm;
}
//----------------------------------------------------------------------------
MgcReal MgcMatrix2::L2NormSqr () const
{
    MgcReal fNormSqr = 0.0;
    for (int iRow = 0; iRow < 2; iRow++)
    {
        for (int iCol = 0; iCol < 2; iCol++)
        {
            MgcReal fSqr = m_aafEntry[iRow][iCol]*m_aafEntry[iRow][iCol];
            fNormSqr += fSqr;
        }
    }
    return fNormSqr;
}
//----------------------------------------------------------------------------
void MgcMatrix2::ToAngle (MgcReal& rfRadians) const
{
    rfRadians = MgcMath::ATan2(m_aafEntry[1][0],m_aafEntry[0][0]);
}
//----------------------------------------------------------------------------
void MgcMatrix2::FromAngle (MgcReal kRadians)
{
    MgcReal fCos = MgcMath::Cos(kRadians);
    MgcReal fSin = MgcMath::Sin(kRadians);
    
    m_aafEntry[0][0] = fCos;
    m_aafEntry[0][1] = -fSin;
    m_aafEntry[1][0] = fSin;
    m_aafEntry[1][1] = fCos;
}
//----------------------------------------------------------------------------
void MgcMatrix2::Tridiagonal (MgcReal afDiag[2], MgcReal afSubDiag[2])
{
    // matrix is already tridiagonal, repackage for QL algorithm
    afDiag[0] = m_aafEntry[0][0];
    afDiag[1] = m_aafEntry[1][1];
    afSubDiag[0] = m_aafEntry[0][1];
    afSubDiag[1] = 0.0;
    m_aafEntry[0][0] = 1.0;
    m_aafEntry[0][1] = 0.0;
    m_aafEntry[1][0] = 0.0;
    m_aafEntry[1][1] = 1.0;
}
//----------------------------------------------------------------------------
bool MgcMatrix2::QLAlgorithm (MgcReal afDiag[2], MgcReal afSubDiag[2])
{
    for (int i0 = 0; i0 < 2; i0++)
    {
        const int iMaxIter = 32;
        int iIter;
        for (iIter = 0; iIter < iMaxIter; iIter++)
        {
            int i1;
            for (i1 = i0; i1 <= 0; i1++)
            {
                MgcReal fSum = MgcMath::Abs(afDiag[i1]) +
                    MgcMath::Abs(afDiag[i1+1]);
                if ( MgcMath::Abs(afSubDiag[i1]) + fSum == fSum )
                    break;
            }
            if ( i1 == i0 )
                break;

            MgcReal fTmp0 = (afDiag[i0+1]-afDiag[i0]) /
                (2.0*afSubDiag[i0]);
            MgcReal fTmp1 = MgcMath::Sqrt(fTmp0*fTmp0 + 1.0);
            if ( fTmp0 < 0.0 )
                fTmp0 = afDiag[i1]-afDiag[i0]+afSubDiag[i0]/(fTmp0-fTmp1);
            else
                fTmp0 = afDiag[i1]-afDiag[i0]+afSubDiag[i0]/(fTmp0+fTmp1);
            MgcReal fSin = 1.0;
            MgcReal fCos = 1.0;
            MgcReal fTmp2 = 0.0;
            for (int i2 = i1-1; i2 >= i0; i2--)
            {
                MgcReal fTmp3 = fSin*afSubDiag[i2];
                MgcReal fTmp4 = fCos*afSubDiag[i2];
                if ( MgcMath::Abs(fTmp3) >= MgcMath::Abs(fTmp0) )
                {
                    fCos = fTmp0/fTmp3;
                    fTmp1 = MgcMath::Sqrt(fCos*fCos + 1.0);
                    afSubDiag[i2+1] = fTmp3*fTmp1;
                    fSin = 1.0/fTmp1;
                    fCos *= fSin;
                }
                else
                {
                    fSin = fTmp3/fTmp0;
                    fTmp1 = MgcMath::Sqrt(fSin*fSin+1.0);
                    afSubDiag[i2+1] = fTmp0*fTmp1;
                    fCos = 1.0/fTmp1;
                    fSin *= fCos;
                }
                fTmp0 = afDiag[i2+1]-fTmp2;
                fTmp1 = (afDiag[i2]-fTmp0)*fSin + 2.0*fTmp4*fCos;
                fTmp2 = fSin*fTmp1;
                afDiag[i2+1] = fTmp0+fTmp2;
                fTmp0 = fCos*fTmp1-fTmp4;

                for (int iRow = 0; iRow < 2; iRow++)
                {
                    fTmp3 = m_aafEntry[iRow][i2+1];
                    m_aafEntry[iRow][i2+1] = fSin*m_aafEntry[iRow][i2] +
                        fCos*fTmp3;
                    m_aafEntry[iRow][i2] = fCos*m_aafEntry[iRow][i2] -
                        fSin*fTmp3;
                }
            }
            afDiag[i0] -= fTmp2;
            afSubDiag[i0] = fTmp0;
            afSubDiag[i1] = 0.0;
        }

        if ( iIter == iMaxIter )
            // should not get here under normal circumstances
            return false;
    }

    return true;
}
//----------------------------------------------------------------------------
void MgcMatrix2::EigenSolveSymmetric (MgcReal afEigenvalue[2],
    MgcVector2 akEigenvector[2]) const
{
    MgcMatrix2 kMatrix = *this;
    MgcReal afSubDiag[3];
    kMatrix.Tridiagonal(afEigenvalue,afSubDiag);
    kMatrix.QLAlgorithm(afEigenvalue,afSubDiag);

    for (int i = 0; i < 2; i++)
    {
        akEigenvector[i][0] = kMatrix[0][i];
        akEigenvector[i][1] = kMatrix[1][i];
    }

    // make eigenvectors form a right--handed system
    MgcReal fDet = akEigenvector[0].Dot(akEigenvector[1]);
    if ( fDet < 0.0 )
    {
        akEigenvector[1][0] = - akEigenvector[1][0];
        akEigenvector[1][1] = - akEigenvector[1][1];
    }
}
//----------------------------------------------------------------------------
void MgcMatrix2::TensorProduct (const MgcVector2& rkU, const MgcVector2& rkV,
    MgcMatrix2& rkProduct)
{
    for (int iRow = 0; iRow < 2; iRow++)
    {
        for (int iCol = 0; iCol < 2; iCol++)
            rkProduct[iRow][iCol] = rkU[iRow]*rkV[iCol];
    }
}
//----------------------------------------------------------------------------
