#include "BFS.h"

//////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
const int MAX_STACK_LEN = 1024;

static DWORD    pNodeStack[2][MAX_STACK_LEN];    // searched nodes 
static int      nStackHead[2];                   // current head position in stack

static int dx[] = {  0, 1,  0, -1};
static int dy[] = {  1, 0, -1,  0};


void ADiffuse(int x, int y, int range,
                int DirectionCount, TARRAY<bool> walkable, TARRAY<short>* WeightMap)
{

    if(DirectionCount == 4)
	{
		int dx[] = {  0, 1,  0, -1};
        int dy[] = {  1, 0, -1,  0};
	}
	if(DirectionCount == 8)
	{
	    int dx[] = {  0,  1, 1, 1,  0, -1, -1, -1 };
        int dy[] = { -1, -1, 0, 1, -1, -1,  0, -1 };
	}

	int         depth;          // trace
    int         toggle;         // toggle to use pNodeStack[0] or pNodeStack[1]

    assert(range > 0);
	assert(!WeightMap->empty());

	WeightMap->SetValue(-1);

    (*WeightMap)(y, x) = range;
 
    // Push to stack0
    pNodeStack[0][0] = MAKELONG(y, x);      // search will from this node
    nStackHead[0] = 1;                      // and push new node to this stack

    // Clear stack1
    nStackHead[1] = 0;                      


    depth  = range - 1;
    toggle = 0;

    do
    {
        while (nStackHead[toggle])
        {
            // Pop from stack
            DWORD pos = pNodeStack[toggle][nStackHead[toggle] - 1];

            x = HIWORD(pos);
            y = LOWORD(pos);

            nStackHead[toggle]--;

            // Search all his branchs
            for (int i = 0; i < DirectionCount; i++)
            {
                int nx = x + dx[i];
                int ny = y + dy[i];

                if (walkable(ny, nx) == false)
                    continue;                   // block cell

                if ((*WeightMap)(ny, nx) >= 0)     
                    continue;                   // already searched

                (*WeightMap)(ny, nx) = depth;      // mark searched w/ depth

                // Push to stack
                pNodeStack[1 - toggle][nStackHead[1 - toggle]] = MAKELONG(ny, nx);
                nStackHead[1 - toggle]++;

                assert(nStackHead[1 - toggle] < MAX_STACK_LEN);
            }
        }   

        toggle = 1 - toggle;
        depth--;

    }   while(depth >= 0);
}

//////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
int ASearchTo(int fromx, int fromy, int tox, int toy, PCC::RList<PCPOINT> *RouteList, int maxStep,
                int DirectionCount, TARRAY<short> WeightMap)
{
	PCPOINT from(fromx, fromy);
	PCPOINT to(tox, toy);

    if(DirectionCount == 4)
	{
		int dx[] = {  0, 1,  0, -1};
        int dy[] = {  1, 0, -1,  0};
	}
	else if(DirectionCount == 8)
	{
	    int dx[] = {  0,  1, 1, 1,  0, -1, -1, -1 };
        int dy[] = { -1, -1, 0, 1, -1, -1,  0, -1 };
	}


	int  depth = WeightMap(to.y, to.x);

    if (depth == -1)
        return 0;

    int  step = WeightMap(from.y, from.x) - depth;

    assert(step > 0 && step <= maxStep);

    int  tail = step - 1;
	RouteList->push_front(to);

	PCPOINT npos;   
    while (depth != WeightMap(from.y, from.x))
    {
        for (int i = 0; i < DirectionCount; i++)
        {       
			npos(to.x + dx[i], to.y + dy[i]);

            if (WeightMap(npos.y, npos.x) > depth)
            {
                // Add this node to array
				RouteList->push_front(npos);
                
                to.x = npos.x;
                to.y = npos.y;

                break;
            }
        }     
        
        depth++;            
    }

	RouteList->GoHeader();
    return step;

}