#ifndef DSMATRIX_H
#define DSMATRIX_H


namespace DrawSprit
{

//
    #define _MATRIX4_LEN  16
    template <class _Type>
    class Matrix4
    {
    public:

        _Type m[_MATRIX4_LEN];

        Matrix4()
        {
            m[0] =0; m[1] =0; m[2] =0; m[3] =0;
            m[4] =0; m[5] =0; m[6] =0; m[7] =0;
            m[8] =0; m[9] =0; m[10]=0; m[11]=0;
            m[12]=0; m[13]=0; m[14]=0; m[15]=0;
        }

        Matrix4(_Type m00, _Type m01, _Type m02, _Type m03,
                _Type m04, _Type m05, _Type m06, _Type m07,
                _Type m08, _Type m09, _Type m10, _Type m11,
                _Type m12, _Type m13, _Type m14, _Type m15)
        {
            m[0]=m00;  m[1]=m01;  m[2]=m02;  m[3]=m03;
            m[4]=m04;  m[5]=m05;  m[6]=m06;  m[7]=m07;
            m[8]=m08;  m[9]=m09;  m[10]=m10; m[11]=m11;
            m[12]=m12; m[13]=m13; m[14]=m14; m[15]=m15;
        }


        LG::Vertex3<_Type> operator*(const LG::Vertex3<_Type>& rkVector) const
        {
            LG::Vertex3<_Type>    kProd;
            kProd.x = rkVector.x*m[ 0 ] +
                      rkVector.y*m[ 4 ] +
                      rkVector.z*m[ 8 ] ;

            kProd.y = rkVector.x*m[ 1 ] +
                      rkVector.y*m[ 5 ] +
                      rkVector.z*m[ 9 ] ;

            kProd.z = rkVector.x*m[ 2 ]  +
                      rkVector.y*m[ 6 ]  +
                      rkVector.z*m[ 10 ] ;
            return kProd;
        }


        //
        // v.x     m[0]  m[4]  m[8]   m[12]     vx*m[0] + v.y*m[4] + v.z*m[8] + v.w*m[12]
        // v.y *   m[1]  m[5]  m[9]   m[13]  =  vx*m[1] + v.y*m[5] + v.z*m[9] + v.w*m[13]
        // v.z     m[2]  m[6]  m[10]  m[14]     vx*m[2] + v.y*m[6] + v.z*m[10] + v.w*m[14]
        // v.w     m[3]  m[7]  m[11]  m[15]     vx*m[3] + v.y*m[7] + v.z*m[11] + v.w*m[15]
        //
        LG::Vertex4<_Type> operator*(const LG::Vertex4<_Type>& rkVector) const
        {
            LG::Vertex4<_Type>    kProd;
            kProd.x = rkVector.x*m[ 0 ] +
                      rkVector.y*m[ 4 ] +
                      rkVector.z*m[ 8 ] +
                      rkVector.w*m[ 12 ];

            kProd.y = rkVector.x*m[ 1 ] +
                      rkVector.y*m[ 5 ] +
                      rkVector.z*m[ 9 ] +
                      rkVector.w*m[ 13 ];

            kProd.z = rkVector.x*m[ 2 ]  +
                      rkVector.y*m[ 6 ]  +
                      rkVector.z*m[ 10 ] +
                      rkVector.w*m[ 14 ];

            kProd.w = rkVector.x*m[ 3 ] +
                      rkVector.y*m[ 7 ] +
                      rkVector.z*m[ 11 ] +
                      rkVector.w*m[ 15 ];
            return kProd;
        }


        //
        // m[0]  m[4]  m[8]   m[12]     m[0]  m[4]  m[8]   m[12]
        // m[1]  m[5]  m[9]   m[13] *   m[1]  m[5]  m[9]   m[13]  =
        // m[2]  m[6]  m[10]  m[14]     m[2]  m[6]  m[10]  m[14]
        // m[3]  m[7]  m[11]  m[15]     m[3]  m[7]  m[11]  m[15]
        //
        // ([0]*[0])+([1]*[4])+([2]*[8]) +([3]*[12]), ([4]*[0])+([5]*[4])+([6]*[8]) +([7]*[12]), ([8]*[0])+([9]*[4])+([10]*[8]) +([11]*[12]), ([12]*[0])+([13]*[4])+([14]*[8]) +([15]*[12])
        // ([0]*[1])+([1]*[5])+([2]*[9]) +([3]*[13]), ([4]*[1])+([5]*[5])+([6]*[9]) +([7]*[13]), ([8]*[1])+([9]*[5])+([10]*[9]) +([11]*[13]), ([12]*[1])+([13]*[5])+([14]*[9]) +([15]*[13])
        // ([0]*[2])+([1]*[6])+([2]*[10])+([3]*[14]), ([4]*[2])+([5]*[6])+([6]*[10])+([7]*[14]), ([8]*[2])+([9]*[6])+([10]*[10])+([11]*[14]), ([12]*[2])+([13]*[6])+([14]*[10])+([15]*[14])
        // ([0]*[3])+([1]*[7])+([2]*[11])+([3]*[15]), ([4]*[3])+([5]*[7])+([6]*[11])+([7]*[15]), ([8]*[3])+([9]*[7])+([10]*[11])+([11]*[15]), ([12]*[3])+([13]*[7])+([14]*[11])+([15]*[15])
        //
        LG::Matrix4<_Type> operator*(const LG::Matrix4<_Type>& rkMatrix) const
        {
            LG::Matrix4<_Type>    kProd;

            for (int i = 0; i < 4; i++)
            {
                for (int j = 0; j < 4; j++)
                {
                    kProd.m[i*4+j] = m[i*4+0]*rkMatrix.m[0*4+j] +
                                     m[i*4+1]*rkMatrix.m[1*4+j] +
                                     m[i*4+2]*rkMatrix.m[2*4+j] +
                                     m[i*4+3]*rkMatrix.m[3*4+j];
                }
            }
            return kProd;
        }


    };

    typedef Matrix4<float> Matrix4f;

}

#endif
