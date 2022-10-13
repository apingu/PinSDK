//DES.cpp DES cryptography function
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
//                                 Pin
//                                    2002/06/17

#include <stdio.h>
#include <time.h>

//pa1[8]
int DECencrypt( FILE* f, FILE* g, int* pa1)
{
    int i, j, k, m, cipher[8], x31, x0;
    unsigned long right, left, buffer, trans;
    int key[8], y1, h;
    int plain[8];
    int xplain[64], xkey[64], y[56];
    int z1, z2, z3, z4, subkey[16][8], xcipher[64];
    int ycipher[64];
    double time1, time2;
    // printf("%04x\n", pa1);
    for( i=0; i<8; i++ )
    {
        key[i] = *pa1;    // printf("%02\n", key[i]);
        pa1 = pa1 + 1;
    }
    
    for( i=0; i<8; i++ )
    {
        for( j=0; j<8; j++ )
        {
            xkey[8*i+j] = key[i]>>(7-j);
            key[i] = key[i]-( ( key[i]>>(7-j) ) << (7-j) );
        }
    }
    
    for( i=0; i<56; i++ )
        y[i] = xkey[pc1[i]];
    for( i = 0; i<16; i++ )
    {
        if( ( i==0) || ( i==1 ) || ( i==8 ) || ( i==15 ) )
        {
            z1 = y[0];
            z2 = y[28];
            for( int j=0; j<27; j++ )
            {
                y[j]=y[j+1];
                y[28+j]=y[29+j];
            }
            y[27] = z1;
            y[55] = z2;
        }
        else
        {
            z1 = y[0];
            z2 = y[1];
            z3 = y[28];
            z4 = y[29];
            for( j=0; j<26; j++ )
            {
                y[j]=y[j+2];
                y[28+j]=y[30+j];
            }
            y[26] = z1;
            y[27] = z2;
            y[54] = z3;
            y[55] = z4;
        }
        k  = 0;
        y1 = 0;
        for( j = 0; j<48; j++ )
        {
            if( j%6==5 )
            {
                subkey[i][k] = y1+y[ pc2[j] ];
                k++;
                y1 = 0;
            }
            else
            {
                y1 = y1 + y[ pc2[j] ];
                y1 = y1<<1;
            }
        }
    }
    //for( i=0; i<8; i++ )
    //fprintf( g, "%d\t", subkey[16][i]);
    //=====encrypt the plaintext
    time1 = clock();
    start:
    
    for( k=0; k<8; k++ )
    {
        if( ( plain[k] = fgetc(f) ) == EOF )
        {
            for( i=k; i<8; i++ )
                plain[i] = '\x1a';
            goto start1;
        }
    }
    
    start1:
    h = plain[7];
    k = 0;
    for( i=0; i<8; i++ )
    {
        for( j=0; j<8; j++ )
        {
            xplain[k+j] = plain[i]>>(7-j);
            plain[i] = plain[i] - ( ( plain[i] >> ( 7 - j ) )<<( 7 - j ) );
        }
        k+=8;
    }
    
    //=====initial permutation
    left  = 0;
    right = 0;
    for( i=0; i<31; i++ )
    {
        left = left + xplain[ ip[i] ];
        left = left << 1;
    }
    left = left + xplain[ ip[31] ];
    
    for( i=32; i<63; i++ )
    {
        right = right + xplain[ ip[i] ];
        right = right << 1;
    }
    right = right + xplain[ ip[63] ];
    
    //=====enter the 16 round
    for( j=0; j<16; j++ )
    {
        buffer = right;
        trans  = 0;
        m      = 0;
        k      = 0;
        //======sbox operation
        s31 = right- ( ( right>>1 )<<1 );
        x0  = right >> 31;
        k   = right >> 27;
        m   = ( ( x31 << 5 ) + k ) ^subkey[j][0];
        trans = s[0][m];
        right = ( right << 3 ) >> 3;
        for( i=0; i<6; i++ )
        {
            k = ( right>>(23 - 4*i ) );
            m = k ^ subkey[j][i + 1];
            trans^=s[i+1][m];
            right = ( right<<(7 + 4 * i ) ) >> ( 7 + 4 * i );
        }
        k = ( right<<1 ) +x0;
        m = k ^subkey[j][7];
        trans ^= s[7][m];
        right = left^trans;
        left = buffer;
    }
    //=====end of sbox operation
 

    return 1;

}

int main(void)
{
    int i, userkey[8], *pa;
    char filename[81], filename1[81], filename2[81], qq;
    FILE *infile, *infile1, *infile2;
    clrscr();
}
    
