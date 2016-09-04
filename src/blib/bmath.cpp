// This file is ISC licensed

#include <utility>
#include <cmath>

#include "bmath.hpp"

bool aabb_overlap (float x1, float y1, float x2, float y2,// coordinate pair for first box
                   float x3, float y3, float x4, float y4)// second box
{
    float temp;
    if(x1 > x2)
    {   temp = x1; x1 = x2; x2 = temp;   }
    if(y1 > y2)
    {   temp = y1; y1 = y2; y2 = temp;   }
    if(x3 > x4)
    {   temp = x3; x3 = x4; x4 = temp;   }
    if(y3 > y4)
    {   temp = y3; y3 = y4; y4 = temp;   }
    if(x1 < x4 && y1 < y4 &&
       x3 < x2 && y3 < y2)
    {
        //printf("overlap test true at %f %f %f %f\n", x3, y3, x4, y4);
        return true;
    }
    else
    {
        //printf("overlap test false at %f %f %f %f\n", x3, y3, x4, y4);
        return false;
    }
} 
bool line_aabb_overlap (float x1, float y1, float x2, float y2,// coordinate pair of line
                        float x3, float y3, float x4, float y4)// box
{
    float temp;
    if(x3 > x4)
    {   temp = x3; x3 = x4; x4 = temp;   }
    if(y3 > y4)
    {   temp = y3; y3 = y4; y4 = temp;   }
    // 0 = left/top, 2 = right/bottom, 1 = center sector
    int dot1_gauge_x = (x1 < x3) ? 0 :
                       (x1 > x4) ? 2 :
                       1;
    int dot1_gauge_y = (y1 < y3) ? 0 :
                       (y1 > y4) ? 2 :
                       1;
    int dot2_gauge_x = (x2 < x3) ? 0 :
                       (x2 > x4) ? 2 :
                       1;
    int dot2_gauge_y = (y2 < y3) ? 0 :
                       (y2 > y4) ? 2 :
                       1;
    
    if((dot1_gauge_x == 1 and dot1_gauge_y == 1) or (dot2_gauge_x == 1 and dot2_gauge_y == 1)) // either dot is in the box
        return true;
    if((dot1_gauge_x == dot2_gauge_x and dot1_gauge_y == dot2_gauge_y)) // same sector
        return false;
    if(dot1_gauge_x == dot2_gauge_x and (dot1_gauge_x == 0 or dot1_gauge_x == 2)) // horizontal(sectorized) line bypasses box
        return false;
    if(dot1_gauge_y == dot2_gauge_y and (dot1_gauge_y == 0 or dot1_gauge_y == 2))
        return false;
    // else
    return (line_aabb_overlap((x1+x2)/2, (y1+y2)/2, x2, y2, x3, y3, x4, y4) or
            line_aabb_overlap(x1, y1, (x1+x2)/2, (y1+y2)/2, x3, y3, x4, y4)); // will not infinitely recurse
    
}

void HSBtoRGB( float hue, float saturation, float brightness, int rgb[3] )
{
    if ( saturation == 0 )
    {
        rgb[0] = rgb[1] = rgb[2] = (int) (brightness * 255.0f + 0.5f);
    }
    else
    {
        float h = (hue - (float)floor(hue)) * 6.0f;
        float f = h - (float)floor(h);
        float p = brightness * (1.0f - saturation);
        float q = brightness * (1.0f - saturation * f);
        float t = brightness * (1.0f - (saturation * (1.0f - f)));
        switch ( (int) h )
        {
            case 0:
                rgb[0] = (int) (brightness * 255.0f + 0.5f);
                rgb[1] = (int) (t * 255.0f + 0.5f);
                rgb[2] = (int) (p * 255.0f + 0.5f);
                break;
            case 1:
                rgb[0] = (int) (q * 255.0f + 0.5f);
                rgb[1] = (int) (brightness * 255.0f + 0.5f);
                rgb[2] = (int) (p * 255.0f + 0.5f);
                break;
            case 2:
                rgb[0] = (int) (p * 255.0f + 0.5f);
                rgb[1] = (int) (brightness * 255.0f + 0.5f);
                rgb[2] = (int) (t * 255.0f + 0.5f);
                break;
            case 3:
                rgb[0] = (int) (p * 255.0f + 0.5f);
                rgb[1] = (int) (q * 255.0f + 0.5f);
                rgb[2] = (int) (brightness * 255.0f + 0.5f);
                break;
            case 4:
                rgb[0] = (int) (t * 255.0f + 0.5f);
                rgb[1] = (int) (p * 255.0f + 0.5f);
                rgb[2] = (int) (brightness * 255.0f + 0.5f);
                break;
            case 5:
                rgb[0] = (int) (brightness * 255.0f + 0.5f);
                rgb[1] = (int) (p * 255.0f + 0.5f);
                rgb[2] = (int) (q * 255.0f + 0.5f);
                break;
        }
    }
}

void RGBtoHSB( int r, int g, int b, float hsb[3] )
{
    float hue, saturation, brightness;
    int cmax = (r > g) ? r : g;
    if ( b > cmax ) cmax = b;
    int cmin = (r < g) ? r : g;
    if ( b < cmin ) cmin = b;

    brightness = ((float) cmax) / 255.0f;
    if ( cmax != 0 )
        saturation = ((float) (cmax - cmin)) / ((float) cmax);
    else
        saturation = 0;
    if ( saturation == 0 )
        hue = 0;
    else
    {
        float redc = ((float) (cmax - r)) / ((float) (cmax - cmin));
        float greenc = ((float) (cmax - g)) / ((float) (cmax - cmin));
        float bluec = ((float) (cmax - b)) / ((float) (cmax - cmin));
        if ( r == cmax )
            hue = bluec - greenc;
        else if ( g == cmax )
            hue = 2.0f + redc - bluec;
        else
            hue = 4.0f + greenc - redc;
        hue = hue / 6.0f;
        if ( hue < 0 )
            hue = hue + 1.0f;
    }
    hsb[0] = hue;
    hsb[1] = saturation;
    hsb[2] = brightness;
}

double crop1( double a )
{
    if ( a < -1)
        return -1;
    if ( a > 1)
        return 1;
    return a;
}

int sign( double a )
{
    return (a > 0) - (a < 0);
}


//"safe" sign (returns positive for 0)
int ssign (double a)
{
    return (!(a < 0)) - (a < 0);
}

double absolute( double a )
{
    if ( a < 0 )
        return -a;
    return a;
}

double point_distance( double x1, double y1, double x2, double y2 )
{
    return sqrt( pow( x2 - x1, 2 ) + pow( y2 - y1, 2 ) );
}
double vector_length( float x, float y )
{
    return sqrt( pow( x, 2 ) + pow( y, 2 ) );
}

double sqdist( float x, float y )
{
    return pow( x, 2 ) + pow( y, 2 );
}

double point_direction( double x1, double y1, double x2, double y2 )
{
    return rad2deg(atan2( y1 - y2, x2 - x1 ));
}
double vector_direction( double x1, double y1 )
{
    return rad2deg(atan2( -y1 ,  x1 ));
}

