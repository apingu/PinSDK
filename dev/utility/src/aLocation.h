#ifndef LOCATION_H
#define LOCATION_H

#include <math.h>

static const double EARTH_RADIUS = 6378.137;
static const double ToKM         = 1.609344;
static const double ToSM         = 0.8684;

#ifndef _PI_
#define _PI_  3.14159f
#endif

inline double rad2deg(double radian) 
{
    // TODO Auto-generated method stub
    return radian * 180 / _PI_;
}

inline double deg2rad(double degree) 
{
    // TODO Auto-generated method stub
    return degree / 180 * _PI_;
}

inline double CoordinateDistance(double lon1, double lat1, double alt1, 
                                 double lon2, double lat2, double alt2)
{
    double theta = lon1 - lon2;

    double dist = sin(deg2rad(lat1)) * sin(deg2rad(lat2))   +
                  cos(deg2rad(lat1)) * cos(deg2rad(lat2)) * cos(deg2rad(theta));

    dist = acos(dist);

    dist = rad2deg(dist);

    double miles = dist * 60 * 1.1515;
    return miles * ToKM;
    
    /*
    double radLat1 = rad(lat1);
    double radLat2 = rad(lat2);
    double a = radLat1 - radLat2;
    double b = rad(lng1) - rad(lng2);
    double s = 2 * Math.asin(Math.sqrt(Math.pow(Math.sin(a/2),2) +
    Math.cos(radLat1)*Math.cos(radLat2)*Math.pow(Math.sin(b/2),2)));
    s = s * EARTH_RADIUS;
    s = Math.round(s * 10000) / 10000;
    return s;
    */
}


#endif