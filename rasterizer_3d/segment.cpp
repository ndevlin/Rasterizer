// Written by Nathan Devlin

#include <float.h>
#include <cmath>

#include "segment.h"


// Constructors

Segment::Segment() : firstEnd(vec4(0.f, 0.f, 0.f, 1.f)),
    secondEnd(vec4(1.f, 0.f, 0.f, 1.f)), dX(1.f), dY(0.f), slope(0.f)
{}

Segment::Segment(const vec4& firstIn, const vec4& secondIn) : firstEnd(firstIn),
    secondEnd(secondIn), dX(secondIn[0] - firstIn[0]),
    dY(secondIn[1] - firstIn[1]), slope(0.f)
{
    calcSlope();
}


Segment::~Segment()
{}


// Member Functions

// Calculate the segment's slope
void Segment::calcSlope()
{
    // The segment is actually a point. Arbitrarily, keep slope at 0
    if(abs(dX) < 2.f * FLT_EPSILON && abs(dY) < 2.f * FLT_EPSILON)
    {
        return;
    }

    // Vertical line
    if(abs(dX) < 2.f * FLT_EPSILON)
    {
        slope = FLT_MAX; // Set slope to the closest float to infinity
        return;
    }

    slope = dY / dX;
}


// Find the x coordinate of the intersection of the line
// segment and the horizontal line with y coordinate y
bool Segment::getIntersection(float y, float& x)
{
    // Segment is above or below line
    if((firstEnd[1] > y && secondEnd[1] > y) ||
            (firstEnd[1] < y && secondEnd[1] < y))
    {
        return false;
    }

    // Segment lies on line; return false to ignore this case
    if((abs(firstEnd[1] - y) < 2.f * FLT_EPSILON) &&
            (abs(secondEnd[1] - y) < 2.f * FLT_EPSILON))
    {
        return false;
    }

    // Segment crosses line
    x = firstEnd[0] - ((firstEnd[1] - y) / slope);

    return true;
}

