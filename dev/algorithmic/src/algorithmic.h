#include <vector>
#include <math.h>
#include <pf-def.h>

namespace Alogorithmic
{
    inline void LineTo(std::vector<int> outx, std::vector<int> outy, float dx, float dy)
    {
        if((outx.empty()) || (outy.empty()))
            return;

        float fx = outx[outx.size()-1];
        float fy = outy[outy.size()-1];

        float Dx        = (dx - fx);
        float Dy        = (dy - fy);

        //計算增量數
        int steps       = (int)(((double) __max(abs(Dx), abs(Dy))) + 1);

        double dx_step  = ((double) Dx / (double) steps);
        double dy_step  = ((double) Dy / (double) steps);

        float x         = fx + ((float) dx_step);// first must can't now situs;
        float y         = fy + ((float) dy_step);

        int lastposx=(int) fx;
        int lastposy=(int) fy;
        for(int u = 0; u < steps; u++)
        {
            int currposx=(int) (x);
            int currposy=(int) (y);

            if((currposx!=lastposx) || (currposy!=lastposy))
            {
                outx.push_back(currposx);
                outy.push_back(currposy);
            }

            lastposx=currposx; 
            lastposy=currposy;

            x = x + ((float) (dx_step));
            y = y + ((float) (dy_step));
        }
        
        if((outx[outx.size()-1]!=dx) ||
            (outy[outy.size()-1]!=dy))
        {
            outx.push_back((int) dx);
            outy.push_back((int) dy);
        }
    }
}