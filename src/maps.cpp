#include "maps.hpp"

#include "components/allcomponents.hpp"
#include "quadtree.hpp"

#include <SDL2/SDL.h>

#define STBI_ONLY_PNG
#define STBI_ONLY_BMP
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <iostream> // debugging

void getRGB(const unsigned char* char_triad, unsigned char* r, unsigned char* g, unsigned char * b)
{
    *r = char_triad[0];
    *g = char_triad[1];
    *b = char_triad[2];
}
void getRGB(const unsigned char* char_triad, int bpp, int h, int w, int y, int x, unsigned char* r, unsigned char* g, unsigned char * b)
{
    getRGB(((unsigned char*)char_triad)+((x%w)+(y%h)*w)*bpp, r, g, b);
}

namespace Maps
{
    long width;
    long height;
    
    void load_background(const char * filename)
    {
        auto background = new Sys::BackgroundDrawable(Ent::New());
        background->init(filename);
    }
    void load_wallmask(const char * filename)
    {
        int w,h,n;
        int bpp = 3;
        unsigned char *data = stbi_load(filename, &w, &h, &n, bpp);
        if(!data)
        {
            printf("stbi_load %s: %s\n", filename, stbi_failure_reason());
            throw; // can't recover
        }
        unsigned char r;
        unsigned char g;
        unsigned char b;
        int scale = 4;
        width  = w * scale;
        height = h * scale;
        
        Sys::QuadTree = new Sys::QuadNode(0, 0, width, height);
        
        for (long y = 0; y < h; ++y)
        {
            if (y%10 == 0)
                std::cout << "Row " << y << "\n";
            long rect_x = -1;
            
            for (long x = 0; x < w; ++x)
            {
                getRGB(data, 3, h, w, y, x, &r, &g, &b);
                if(r < 127 and rect_x == -1) // new rect
                {
                    rect_x = x;
                }
                else if(r >= 127 and rect_x != -1) // horizontal end of rect
                {
                    new Sys::BoxDrawable(Ent::New(), scale*rect_x, scale*y, scale*(x-rect_x), scale);
                    rect_x = -1;
                }
            }
            if(rect_x != -1) // end of rect
            {
                new Sys::BoxDrawable(Ent::New(), scale*rect_x, scale*y, scale*(w-rect_x), scale);
            }
        }
        for(auto box : Sys::BoxDrawables)
        {
            auto p = box->position;
            auto h = box->hull;
            Sys::QuadTree->add(box, p->x, p->y, p->x+h->w, p->y+h->h);
        }
        
        stbi_image_free(data);
    }
}
