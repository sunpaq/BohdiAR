#ifndef _MCUIBase_
#define _MCUIBase_
#include "monkc.h"

typedef struct {
    double x;
    double y;
} MCPoint;

typedef struct {
    double width;
    double height;
} MCSize;

typedef struct {
    MCPoint origin;
    MCSize size;
} MCRect;

typedef struct {
    unsigned R;
    unsigned G;
    unsigned B;
} MCColor;

MCInline MCPoint mc_point(double x, double y)
{
    MCPoint point = {x, y};
    return point;
}

MCInline MCPoint* mc_point_copy(MCPoint* A, MCPoint B)
{
    A->x = B.x;
    A->y = B.y;
    return A;
}

MCInline MCSize mc_size(double width, double height)
{
    MCSize point = {width, height};
    return point;
}

MCInline MCRect mc_rect(double x, double y, double width, double height)
{
    MCRect frame = {{x, y}, {width, height}};
    return frame;
}

MCInline int mc_rect_contains(MCRect* rect, MCPoint point)
{
    double deltax = point.x - rect->origin.x;
    double deltay = point.y - rect->origin.y;
    if(deltax >= 0 && deltax <= rect->size.width)
        if(deltay >= 0 && deltay <= rect->size.height)
            return 1;
    return 0;
}

MCInline MCColor mc_color(unsigned R, unsigned G, unsigned B)
{
    MCColor color = {R, G, B};
    return color;
}

MCInline MCColor mc_color_mix(MCColor A, MCColor B)
{
    MCColor mixed = {A.R+B.R, A.G+B.G, A.B+B.B};
    return mixed;
}

static const MCPoint mc_point_zero = {0,0};
static const MCSize mc_size_zero   = {0,0};
static const MCRect mc_rect_zero   = {0,0,0,0};

static const MCColor mc_color_white  = {255,255,255};
static const MCColor mc_color_black  = {0,0,0};
static const MCColor mc_color_red    = {255,0,0};
static const MCColor mc_color_green  = {0,255,0};
static const MCColor mc_color_blue   = {0,0,255};

#endif
