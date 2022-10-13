// search.c : implementation of path searching module (version 1.1)
//             
//  Developer   Date    Comments
//  Devil       ?       first build  
//  yann        100398  change the type of PathPos from short to int
//                      when searching in large map which will cause error
//                      NOTE: In this version the result will be wrong
//                            while the searching just go to the bounary of
//                            the map because for the speed i don't do a 
//                            faultless bounary checking in GetAround() 
//                            where the "*" line
//              050501  rewrite to support variable map size on demand


#include <stdlib.h>
#include "Search.h"

char	PathDir[1024];		// search result (direction)
							//      7:L-U 0:U 1:R-U
                            //      6:L       2:R
                            //      5:L-D 4:D 3:R-D
int		PathPos[1024];		// search result (map position)

int    *MapData;			// map[MapWidth * MapHeight];
                            //	1	 : walkable  
							//  10000: nonwalkable
int     MapWidth;     
int     MapHeight;
int     MapSize;

unsigned short  *MapSearch = NULL; 

/* fixed map size !!

#define MapWidth   128   
#define MapHeight  128

#define POS_TO_X(pos)		((pos) & 0x07f)
#define POS_TO_Y(pos)		((pos) >> 7)
#define XY_TO_POS(x, y)     (((y) << 7) | (x))
*/

inline int  POS_TO_X(int pos)
{
	return pos % MapWidth;
}

inline int  POS_TO_Y(int pos)
{
	return pos / MapWidth;
}

inline int  XY_TO_POS(int x, int y)
{
	return y * MapWidth + x;
}

inline void DWMEMSET(void *dest, int value, unsigned int count)
// NOTE: count is bytes
{
    _asm
    {
        mov     edi, (dest)
        mov     eax, (value)
        mov     ecx, (count)

        xor     ebx, ebx
        shr     ecx, 1
        adc     bl, bl
        shr     ecx, 1
        adc     bh, bh
        rep     stosd
        mov     cl, bh
        rep     stosw
        mov     cl, bl
        rep     stosb
    }
}


#define MAX_QUEUE_LEN	2048

static struct
{
    int  pos;
    int  distance;

}			Queue[MAX_QUEUE_LEN];

static int  QueueTail;


static void PushToQueue(int pos, int distance);
static void ShrinkQueue(void);


static void GetAround(int pos, int *around);
static int  GetDistance(int x1, int y1, int x2, int y2);

static char GetDir(int pos2, int pos1);


/////////////////////////////////////////////////////////////////////////////
// Search path

int  InitPathSearch(int *map, int width, int height)
{
	MapData = map;		

	MapWidth  = width;     
	MapHeight = height;
	
	int size = width * height;

	if (MapSearch == NULL || size != MapSize)
	{
		delete [] MapSearch;

		MapSearch = new unsigned short [size];

		MapSize = size;
	}

	if (MapSearch == NULL)
		return 0;

	return 1;
}


void ClosePathSearch(void)
{
	delete [] MapSearch;

	MapSearch = NULL;
}


int  SearchPathInMap(int x1, int y1, int x2, int y2)
{
    int  source, target, xa, ya;
    int  i, j;
    int  old_data;
    int  near_dist, near_pos;
	int  pos, around, arounds[8];
	int  cost, dist, limit, count;

    source = XY_TO_POS(x1, y1);
    target = XY_TO_POS(x2, y2);

    i = abs(x2 - x1);
    j = abs(y2 - y1);

    if (i == 0 && j == 1 && MapData[target] == 10000)
            return 0;   // don't need to search
    if (i == 1 && j == 0 && MapData[target] == 10000)
            return 0;   // don't need to search

    if (source == target)
            return 0;   // don't need to search

    // Initilizing ----------------------------------------------------------

    DWMEMSET(MapSearch, 0x7FFF7FFF, sizeof(unsigned short) * MapSize);
    
    old_data = MapData[source];			// backup old map data

    MapData[source]   = 1;				// set start pos as walkable
    
	MapSearch[source] = 0; 
    
	Queue[1].pos      = source;			// push source pos to queue
    Queue[1].distance = GetDistance(x1, y1, x2, y2);

    QueueTail = 1;

	limit = Queue[1].distance << 1;		// NOTE: set ur searching depth here

    count = 0;

    near_dist = 0x7FFF;

    // Start ----------------------------------------------------------------
    do
    {
        pos = Queue[1].pos;
    
		if (pos == target)
            break;

        ShrinkQueue();
        
		GetAround(pos, arounds);
        
		for (i = 0; i < 8; i++)
        {
            around = arounds[i];
        
			if (around)
            {
                cost = MapData[around] + MapSearch[pos];
            
				if (cost < MapSearch[around])
                {
                    count++;

                    MapSearch[around] = cost;

                    xa = POS_TO_X(around);
                    ya = POS_TO_Y(around);

                    dist = GetDistance(x2, y2, xa, ya);

                    cost += dist;
                    
					PushToQueue(around, cost);
                
					if (dist < near_dist)
                    {
                        near_pos  = around;
                        near_dist = dist;
                    }
                }
            }
        }

        if (count > limit)
        {
            pos = near_pos;

            break;
        }

    } while (QueueTail > 0);

    if (pos == source)
    {
        MapData[source] = old_data;

		return 0;
    }

	// Backtrack to source
    count = 0;

    near_dist = 0x7FFF;

    while (near_dist > 0)
    {
        PathPos[count++] = pos;

        near_dist = 0x7FFF;

        GetAround(pos, arounds);
        
		for (i = 0; i < 8; i++)
		{
            if (arounds[i])
			{
				dist = MapSearch[arounds[i]];

                if (dist < near_dist)
                {
                    near_dist = dist;
                    j = i;
                }
			}
		}

        pos = arounds[j];
    }

    PathPos[count] = pos;

    MapData[source] = old_data;    // restore old map data

	// Convert PathPos to PathDir
    for (i = 0; i < count; i++)
        PathDir[i] = GetDir(PathPos[count - i], PathPos[count - i - 1]);

    return count;
}


void PushToQueue(int pos, int distance)
{
	int  count;

    QueueTail++;

    if (QueueTail == MAX_QUEUE_LEN)
    {
        QueueTail--;
        return;
    }

    count = QueueTail;

    while (Queue[count >> 1].distance > distance)
    {
        Queue[count] = Queue[count >> 1];
        count >>= 1;
    }

    Queue[count].pos      = pos;
    Queue[count].distance = distance;
}

void ShrinkQueue(void)
{
    int  vpos, vdist;
    int  count, i;

    Queue[1] = Queue[QueueTail];

    QueueTail--;

    vpos  = Queue[1].pos;
    vdist = Queue[1].distance;

    count = 1;

    while (count <= (QueueTail >> 1))
    {
        i = count << 1;

        if (Queue[i].distance > Queue[i + 1].distance)
		{
            if (i < QueueTail)
                i++;
		}

        if (vdist <= Queue[i].distance)
            break;

        Queue[count] = Queue[i];    
        count = i;
    }

    Queue[count].pos      = vpos;
    Queue[count].distance = vdist;
}


void GetAround(int pos, int *around)
{
    static int table[8] = 
    { 
        -MapWidth, 1, MapWidth, -1,
        -(MapWidth-1), (MapWidth+1), (MapWidth-1), -(MapWidth+1) 
    };

    int  i, j;

    for (i = 0; i < 8; i++)
    {
        j = pos + table[i];
        
        // Check map boundary
        // * NOTE: still has some situations will cause out of map
        if (j < 0 || j >= MapSize)
            around[i] = 0;
        else if (MapData[j] < 10000)
            around[i] = j;
        else
            around[i] = 0;
    }

    if (around[0] == 0) { around[4] = 0; around[7] = 0; }
    if (around[1] == 0) { around[4] = 0; around[5] = 0; }
    if (around[2] == 0) { around[5] = 0; around[6] = 0; }
    if (around[3] == 0) { around[6] = 0; around[7] = 0; }
}

int  GetDistance(int x1, int y1, int x2, int y2)
{
    int  dx, dy, dis;

    dx = abs(x1 - x2);
    dy = abs(y1 - y2);

    if (dx > dy)
        dis = dx * 10 + dy * 4;
    else
        dis = dy * 10 + dx * 4;

    return dis;
}

char GetDir(int pos2, int pos1)
// 7:L-U 0:U 1:R-U
// 6:L       2:R
// 5:L-D 4:D 3:R-D

{
    static char table[] = 
    {
        7,  0, 1, -1,
        6, -1, 2, -1,
        5,  4, 3 
    };
    
    int  dx, dy;

    dx = POS_TO_X(pos1) - POS_TO_X(pos2) + 1;   // 0, 1, 2
    dy = POS_TO_Y(pos1) - POS_TO_Y(pos2) + 1;   // 0, 1, 2

    dx += (dy << 2);

    return table[dx];   // will not be -1

/*
    int  dx, dy;

    dx = POS_TO_X(pos1) - POS_TO_X(pos2);   // -1, 0, 1
    dy = POS_TO_Y(pos1) - POS_TO_Y(pos2);   // -1, 0, 1

    if (dx > 0)
    {
        switch (dy)
        {
        case -1: return 1;             
        case 0:  return 2;             
        case 1:  return 3;                                                                                
        }                                 
    }    
    else if (dx < 0)                      
    {
        switch (dy)                       
        {                                 
        case -1: return 7;
        case 0:  return 6;
        case 1:  return 5;
        }
    }
    // else dx == 0
        return (dy == -1 ? 0 : 4); */
}



#define CELL_W  16
#define CELL_H  8

int  PathX[512];
int  PathY[512];

int  FindPath(int x1, int y1, int x2, int y2)
{
    x1 /= CELL_W;
    y1 /= CELL_H;
    x2 /= CELL_W;
    y2 /= CELL_H;

    int  ret = SearchPathInMap(x1, y1, x2, y2);

    if (ret == 0)
        return 0;

    // Convert PathDir[] to PathX[] and 
    //                   0 U      1 R-U   2 R     3 R-D   4 D      5 L-D   6 L      7 L-U
    static int dx[] = {  0,       CELL_W, CELL_W, CELL_W, 0,      -CELL_W, -CELL_W, -CELL_W };
    static int dy[] = { -CELL_H, -CELL_H, 0,      CELL_H, CELL_H,  CELL_H,  0,      -CELL_H };

    PathX[0] = x1 * CELL_W + CELL_W / 2 + dx[PathDir[0]];
    PathY[0] = y1 * CELL_H + CELL_H / 2 + dy[PathDir[0]];

    if (ret == 1)
        return 1;

    int  step  = 1;
    int  last_dir = PathDir[0];
    int  last_dx  = dx[last_dir];
    int  last_dy  = dy[last_dir];

    for (int i = 1; i < ret; i++)
    {
        if (last_dir != PathDir[i])
        {
            PathX[step] = last_dx + PathX[step - 1];
            PathY[step] = last_dy + PathY[step - 1];

            step++;

            last_dir = PathDir[i];
            last_dx  = dx[last_dir];
            last_dy  = dy[last_dir];
        }
        else
        {
            last_dx += dx[last_dir];
            last_dy += dy[last_dir];
        }
    }

    PathX[step] = last_dx + PathX[step - 1];
    PathY[step] = last_dy + PathY[step - 1];

    step++;

    return step;
}






/* a simple test program 

#include <stdio.h>

void main(void)
{
    short *map;
    int    i;
    int    step;

#ifdef CREATE_MAP

    MapHeight = 128;
    MapWidth  = 128;

    map = new short [MapHeight * MapWidth];

    for (i = 0; i < MapHeight * MapWidth; i++)
        map[i] = 1;

    map[10 + 10 * MapWidth] = 10000;  // put a stone here
    map[11 + 10 * MapWidth] = 10000;  // put a stone here
    map[10 + 11 * MapWidth] = 10000;  // put a stone here
    map[11 + 11 * MapWidth] = 10000;  // put a stone here

    InitPathSearch(map, MapWidth, MapHeight);        

#else // LOAD_MAP

    FILE *infile = fopen("0100b.map", "rb");
  
    if (infile == NULL)
        return;    // no stuffs

    short  w, h;

    fread(&w, sizeof(short), 1, infile);
    fread(&h, sizeof(short), 1, infile);

    map = new short [w * h];

    fread(map, sizeof(short), w * h, infile);

    for (i = 0; i < w * h; i++)
    {
        if (map[i])
            map[i] = 10000;
        else
            map[i] = 1;
    }

    fclose(infile);

    InitPathSearch(map, w, h); 

#endif

    step = SearchPathInMap(34, 110, 34, 87);

    step = SearchPathInMap(34, 110, 34, 87);

    printf("%d\n", step);

    for (i = 0; i < step; i++)
        printf("%d ", PathDir[i]);

    ClosePathSearch();
}

*/
