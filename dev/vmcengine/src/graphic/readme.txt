//===================================================================================
3D version

    1. 一個單位 1.0f 預設是100個pixels
    2. 所有的都以一個3*3的矩陣 當 座標資料

       ┌                    ┐
          tx(0), ty(1), tz(2)      ==>朝上的"單位"向量

          fx(3), fy(4), fz(5)      ==>朝前的"單位"向量

          px(6), py(7), pz(8)      ==>所在的位置
       └                    ┘

//===================================================================================


材質圖的頂點


─┼───────────────→X
  │
  │  (0,0)          (1,0)
  │
  │
  │
  │
  │  (0,1)          (1,1)
  │
  │
  │
  ↓
 
Y

//===================================================================================
gl 矩陣的作用堆積 觀念上是用 glMultMatrixf  而不是用 glLoadMatrixf=> 會把作用堆疊全部重設



使物体旋?到x1,y1,z1到x2,y2,z2向量的代?

        v1 = 0
        w1 = 1
        u2 = x2 - x1
        v2 = y2 - y1
        w2 = z2 - z1
        u = v1 * w2 - w1 * v2
        v = w1 * u2 - u1 * w2
        w = u1 * v2 - v1 * u2
        angle = asin(Sqr(u2 * u2 + v2 * v2) / Sqr(u2 * u2 + v2 * v2 + w2 * w2)) * 180 / 3.14159
        glRotated angle, u, v, w
        gluCylinder obj, 0.2, 0.2, Sqr(u2 * u2 + v2 * v2 + w2 * w2), 32, 20
        
        glRotated -angle, -u, -v, -w

 

void lookAt(GLfloat eyex, GLfloat eyey, GLfloat eyez,
	  GLfloat centerx, GLfloat centery, GLfloat centerz,
	  GLfloat upx, GLfloat upy, GLfloat upz)
{

   GLfloat m[16];
   GLfloat x[3], y[3], z[3];
   TReal mag;

   /* Make rotation matrix */

   /* Z vector */
   z[0] = eyex - centerx;
   z[1] = eyey - centery;
   z[2] = eyez - centerz;
   Math::Sqrt(mag, z[0] * z[0] + z[1] * z[1] + z[2] * z[2]);
   if (mag) {			/* mpichler, 19950515 */
      z[0] /= mag;
      z[1] /= mag;
      z[2] /= mag;
   }

   /* Y vector */
   y[0] = upx;
   y[1] = upy;
   y[2] = upz;

   /* X vector = Y cross Z */
   x[0] = y[1] * z[2] - y[2] * z[1];
   x[1] = -y[0] * z[2] + y[2] * z[0];
   x[2] = y[0] * z[1] - y[1] * z[0];

   /* Recompute Y = Z cross X */
   y[0] = z[1] * x[2] - z[2] * x[1];
   y[1] = -z[0] * x[2] + z[2] * x[0];
   y[2] = z[0] * x[1] - z[1] * x[0];

   /* mpichler, 19950515 */
   /* cross product gives area of parallelogram, which is < 1.0 for
    * non-perpendicular unit-length vectors; so normalize x, y here
    */

   Math::Sqrt(mag, x[0] * x[0] + x[1] * x[1] + x[2] * x[2]);
   if (mag) {
      x[0] /= mag;
      x[1] /= mag;
      x[2] /= mag;
   }

   Math::Sqrt(mag, y[0] * y[0] + y[1] * y[1] + y[2] * y[2]);
   if (mag) {
      y[0] /= mag;
      y[1] /= mag;
      y[2] /= mag;
   }

#define M(row,col)  m[col*4+row]
   M(0, 0) = x[0];
   M(0, 1) = x[1];
   M(0, 2) = x[2];
   M(0, 3) = 0.0;
   M(1, 0) = y[0];
   M(1, 1) = y[1];
   M(1, 2) = y[2];
   M(1, 3) = 0.0;
   M(2, 0) = z[0];
   M(2, 1) = z[1];
   M(2, 2) = z[2];
   M(2, 3) = 0.0;
   M(3, 0) = 0.0;
   M(3, 1) = 0.0;
   M(3, 2) = 0.0;
   M(3, 3) = 1.0;
#undef M
   glMultMatrixf(m);

   /* Translate Eye to Origin */
   glTranslatef(-eyex, -eyey, -eyez);

}

z[0] = centerx - eyex;
z[1] = centery - eyey;
z[2] = centerz - eyez;



//===================================================================================

以度數表示的角度，把數字乘以π/180便轉換成弧度；以弧度表示的角度，乘以180/π便轉換成度數。

45*π/180


//===================================================================================

    VMCString.h       ==> base memcpy func
    VMC16BitsFunc.h   ==> base 16bits func
    VMC32BitsFunc.h   ==> base 32bits func
    VMCMatrix.h       ==> 
    VMCClipper.h      ==> Cilpper func

    VMC.h
    VMC.cpp
    VMCBlt.cpp
    VMCAlpha.cpp
    VMCColorKey.cpp
    VMCGraphic.cpp
    VMCFont.cpp

    VMCDX.h
    VMCDX.cpp

    VMGDI.h
    VMGDI.cpp





//===================================================================================


    use VMC  DirectX


    VMCDX GDX;
	HPINMAP* VMCanvas; // back buffer

    Method A:



	    if( !GDX.DXCreateEX( hMainWnd, 800, 600, 16, false ) )// Set Direct X Mode
            exit(1);

	    VMCanvas = GDX.CreateVisualMemCanvas(800, 600);

        //argument 1 => windows headle
        //argument 2 => windows or fullscreen mode
        //argument 3 => screen width
        //argument 4 => screen height
        //argument 5 => bits size


    Method B:


	    if( !CreateDX(hWnd, width, height, dpp, screenflag) )
		    return false;
	
	    if( screenflag )
	    {
		    if(!CreateFullScreenPrimary(2))
			    return false;
	    }
	    else
	    {
            if(!CreateWindowsPrimary(hWnd))
			    return false;
	    }
	    INIT();




    use VMC GDI


	    VPage VGDI;
		HPINMAP* VMCanvas; // back buffer


		VGDI.CreateDC( MainFrame->m_hWnd, atoi(IniWidth), atoi(IniHeight), _PIXEL_BITS );
        VMCanvas = VGDI.CreatVMC( IniWidth, IniHeight, _PIXEL_BITS);
	    VGDI.JudgeVideoMask( PIXEL_555 );

