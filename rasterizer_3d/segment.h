// Written by Nathan Devlin

#ifndef SEGMENT_H
#define SEGMENT_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>


using namespace glm;

class Segment
{
public:

    // Constructors

    Segment();

    Segment(const vec4& firstIn, const vec4& secondIn);


    virtual ~Segment();


    // Member Functions

    // Calculate the slope of the line
    void calcSlope();

    // Compute X Intercept of Horizontal Line
    bool getIntersection(float y, float& x);


private:
    // Member Variables

    vec4 firstEnd;
    vec4 secondEnd;
    float dX;
    float dY;
    float slope;

};

#endif // SEGMENT_H
