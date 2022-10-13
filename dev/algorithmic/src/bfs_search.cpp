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
//
//
#include <stack>
#include <vector>


#define BFSMAKELONG(a, b)      ((LONG)(((WORD)(a)) | ((DWORD)((WORD)(b))) << 16))
#define BFSLOWORD(l)           ((WORD)(l))
#define BFSHIWORD(l)           ((WORD)(((DWORD)(l) >> 16) & 0xFFFF))

 

//////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
int BfsSearch( int *map, int w, int h,
               int fx, int fy, int dx, int dy,
			   int dir, int maxStep, std::vector<int>* path )
{
	//return -1;

	//std::vector<int> path;

	short* d_x = new short[ dir ];
	short* d_y = new short[ dir ];

    if(dir == 4)
	{
		d_x[0] = 0; d_x[1] = 1; d_x[2] =  0; d_x[3] = -1;
        d_y[0] = 1; d_y[1] = 0; d_y[2] = -1; d_y[3] =  0;
	}
	if(dir == 8)
	{
		d_x[0] = -1; d_x[1] = -1; d_x[2] = -1; d_x[3] =  0;	d_x[4] = 1; d_x[5] = 1; d_x[6] =  1; d_x[7] =  0;
		d_y[0] = -1; d_y[1] =  0; d_y[2] =  1; d_y[3] =  1;	d_y[4] = 1; d_y[5] = 0; d_y[6] = -1; d_y[7] = -1;
	}

	int  depth = map[ (dx * w) + dy ];

    if (depth == -1)
        return 0;


    int  step = map[ (fx * w) + fy ];
	if( step <= 0 )
		return 0;

    assert(step > 0 && step <= maxStep);

	path->insert( path->begin(), BFSMAKELONG(dx, dy ) );
    int  tail = step - 1;

	short nposx;   
	short nposy;   
    while ( depth != map[ (fx * w) + fy ] )
    {
        for( int i = 0; i < dir; i++ )
        {   
			nposx = dx + d_x[i];
			nposy = dy + d_x[i];

			if( map[ (nposx * w) + nposy ] > depth )
            {
                // Add this node to array
				//path.push_front(npos);
				path->insert( path->begin(), BFSMAKELONG(nposx, nposy ) );
                dx = nposx;
                dy = nposy;
                break;
            }
        }     
        
        depth++;            
    }

	if( d_x != 0 ) delete [] d_x;
	if( d_x != 0 ) delete [] d_x;

    return step;

}

