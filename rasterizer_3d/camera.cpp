// Written by Nathan Devlin

#include "camera.h"


// Constructor

Camera::Camera() : forward(vec4(0.f, 0.f, -1.f, 0.f)),
    right(vec4(1.f, 0.f, 0.f, 0.f)), up(vec4(0.f, 1.f, 0.f, 0.f)),
    position(vec4(0.f, 0.f, 10.f, 1.f)), fov(45.f),
    nearClip(0.01f), farClip(100.f), aspRatio(1.f)
{}


// Member Functions

// Returns the View matrix given camera axes and position
mat4 Camera::getViewMat()
{
    mat4 orient = mat4();
    mat4 trans = mat4();
    mat4 view = mat4();

    trans[3][0] = -1.f * position[0];
    trans[3][1] = -1.f * position[1];
    trans[3][2] = -1.f * position[2];

    orient[0] = right;
    orient[1] = up;
    orient[2] = forward;

    orient = transpose(orient);

    view = orient * trans;

    return view;
}

// Returns the Perspective Projection Matrix based
// on the camera's clipping planes, aspect ratio,
// and field of view
mat4 Camera::getPerspProjMat()
{
    float fovRad = glm::radians(fov);

    float p = farClip / (farClip - nearClip);
    float q = (-1.f * farClip * nearClip) / (farClip - nearClip);
    float s = 1 / (tan(fovRad / 2.f));
    float a = 1.f;

    mat4 perspMat = mat4();
    perspMat[0][0] = s / a;
    perspMat[1][1] = s;
    perspMat[2][2] = p;
    perspMat[3][2] = q;
    perspMat[2][3] = 1.f;
    perspMat[3][3] = 0.f;

    return perspMat;
}


// Translate Camera

// Translate camera along the forward axis
void Camera::transForward(float move)
{
    position += forward * move;
}

// Translate camera along the right axis
void Camera::transRight(float move)
{
    position += right * move;
}

// Translate camera along the Up axis
void Camera::transUp(float move)
{
    position += up * move;
}


// Rotate Camera

// Rotate camera about the forward axis
void Camera::rotForward(float theta)
{
    right = glm::rotate(theta, vec3(forward[0], forward[1], forward[2])) * right;
    up = glm::rotate(theta, vec3(forward[0], forward[1], forward[2])) * up;
}

// Rotate camera about the right axis
void Camera::rotRight(float theta)
{
    forward = glm::rotate(theta, vec3(right[0], right[1], right[2])) * forward;
    up = glm::rotate(theta, vec3(right[0], right[1], right[2])) * up;
}

// Rotate camera about the Up axis
void Camera::rotUp(float theta)
{
    forward = glm::rotate(theta, vec3(up[0], up[1], up[2])) * forward;
    right = glm::rotate(theta, vec3(up[0], up[1], up[2])) * right;
}


// Print the matrix for debugging purposes
void Camera::printMat(mat4 matIn)
{
    cout << matIn[0][0] << " " << matIn[1][0]  << " " << matIn[2][0] << " " << matIn[3][0] << endl;
    cout << matIn[0][1] << " " << matIn[1][1]  << " " << matIn[2][1] << " " << matIn[3][1] << endl;
    cout << matIn[0][2] << " " << matIn[1][2]  << " " << matIn[2][2] << " " << matIn[3][2] << endl;
    cout << matIn[0][3] << " " << matIn[1][3]  << " " << matIn[2][3] << " " << matIn[3][3] << endl;
    cout << endl;
}


