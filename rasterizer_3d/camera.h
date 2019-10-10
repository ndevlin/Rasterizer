// Written by Nathan Devlin

#include <string>
#include <vector>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

using namespace glm;

using namespace std;

#ifndef CAMERA_H
#define CAMERA_H


class Camera
{
public:

    // Constructor

    Camera();


    // Member Variables

    vec4 forward;
    vec4 right;
    vec4 up;

    vec4 position;

    float fov;
    float nearClip;
    float farClip;
    float aspRatio;


    // Member Functions

    // Returns the View matrix given camera axes and position
    mat4 getViewMat();

    // Returns the Perspective Projection Matrix based
    // on the camera's clipping planes, aspect ratio,
    // and field of view
    mat4 getPerspProjMat();


    // Translate Camera

    // Translate camera along the forward axis
    void transForward(float move);

    // Translate camera along the right axis
    void transRight(float move);

    // Translate camera along the Up axis
    void transUp(float move);


    // Rotate Camera

    // Rotate camera about the forward axis
    void rotForward(float theta);

    // Rotate camera about the right axis
    void rotRight(float theta);

    // Rotate camera about the Up axis
    void rotUp(float theta);

    // Print the matrix for debugging purposes
    void printMat(mat4 matIn);

};

#endif // CAMERA_H
