// Declarations by Adam Malley, Implementation by Nathan Devlin

#include "rasterizer.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>
#include <cmath>
#include <array>
#include "segment.h"
#include "camera.h"

using namespace glm;

using namespace std;


Rasterizer::Rasterizer(const std::vector<Polygon>& polygons)
    : m_polygons(polygons), camera(Camera()), perspPovMat(camera.getPerspProjMat())
{}

QImage Rasterizer::RenderScene()
{
    QImage result(512, 512, QImage::Format_RGB32);
    // Fill the image with black pixels.
    // Note that qRgb creates a QColor,
    // and takes in values [0, 255] rather than [0, 1].
    result.fill(qRgb(0.f, 0.f, 0.f));

    // Set scene to be in 3D or in 2D
    bool threeD = true;

    // Use a fish eye or a normal pinhole lens
    bool fishEye = false;
    // Fractional focal length if a fish eye lens is used
    double focalLength = 0.3;


    bool paint = false;

    // Render 2D scene
    if(threeD == false)
    {
        float red = 0.f;
        float blue = 0.f;
        float green = 0.f;

        std::array<float, 262144> currentScreen;

        currentScreen.fill(1.f);

        for(Polygon p : m_polygons)
        {
            for(Triangle t : p.m_tris)
            {
                Vertex vert0 = p.m_verts[t.m_indices[0]];
                Vertex vert1 = p.m_verts[t.m_indices[1]];
                Vertex vert2 = p.m_verts[t.m_indices[2]];

                Segment s0(vert0.m_pos, vert1.m_pos);
                Segment s1(vert1.m_pos, vert2.m_pos);
                Segment s2(vert2.m_pos, vert0.m_pos);

                // Iterate through each pixel
                for(int y = t.yUpper; y < t.yLower && y < 512; y++)
                {
                    float x0 = -1.f;
                    float x1 = -1.f;
                    float x2 = -1.f;

                    float enter = -1.f;
                    float exit = -1.f;

                    int numIntersections = 0;

                    if(s0.getIntersection((float)y, x0))
                    {
                        numIntersections++;
                    }
                    if(s1.getIntersection((float)y, x1))
                    {
                        numIntersections++;
                    }
                    if(s2.getIntersection((float)y, x2))
                    {
                        numIntersections++;
                    }

                    if(numIntersections < 1)
                    {
                        continue;
                    }
                    else if(numIntersections == 1)
                    {
                        if(x0 == -1.f)
                        {
                            enter = x0;
                        }
                        else if(x1 != -1.f)
                        {
                            enter = x1;
                        }
                        else
                        {
                            enter = x2;
                        }
                    }
                    else
                    {
                        if(x0 == -1.f)
                        {
                            enter = x1;
                            exit = x2;
                        }
                        else
                        {
                            enter = x0;
                            if(x1 == -1.f)
                            {
                                exit = x2;
                            }
                            else
                            {
                                exit = x1;
                            }
                        }
                    }

                    if(exit < enter)
                    {
                        float temp = exit;
                        exit = enter;
                        enter = temp;
                    }

                    paint = false;

                    for(int x = t.xLeft; x < t.xRight && x < 512; x++)
                    {
                        if(x >= enter)
                        {
                            paint = true;
                        }

                        if(x > exit)
                        {
                            paint = false;
                        }

                        if(paint == true)
                        {
                            vec4 intPt = vec4(float(x), float(y), vert0.m_pos[2], 1.0);
                            vector<float> vertWeights = p.baryInterp2D(t, intPt);

                            if(vert0.m_pos[2] <= currentScreen[x + 512 * y])
                            {
                                currentScreen[x + 512 * y] = vert0.m_pos[2];

                                red = vert0.m_color[0] * vertWeights[0] +
                                        vert1.m_color[0] * vertWeights[1] +
                                        vert2.m_color[0] * vertWeights[2];

                                green = vert0.m_color[1] * vertWeights[0] +
                                        vert1.m_color[1] * vertWeights[1] +
                                        vert2.m_color[1] * vertWeights[2];

                                blue = vert0.m_color[2] * vertWeights[0] +
                                        vert1.m_color[2] * vertWeights[1] +
                                        vert2.m_color[2] * vertWeights[2];

                                result.setPixel(x, y, qRgb(red, green, blue));
                            }
                        }
                    }
                }
            }
        }
    }


    // Render 3D Scene
    if(threeD == true)
    {   
        // Calculate the Camera's Matrix
        mat4 viewMat = camera.getViewMat();
        mat4 compositionMat = perspPovMat * viewMat;

        currentScreen.fill(1000.f);

         for(Polygon p : m_polygons)
        {
             // Make a copy so we retain access to both world
             // and screen space coordinates
            Polygon pCopy = p;

             for(Vertex& v : pCopy.m_verts)
             {
                 vec4 pos = v.m_pos;

                 if(fishEye == false)
                 {
                     // Normal Pinhole camera

                     // Multiply verts by the matrices
                     pos = compositionMat * pos;

                     // Divide by W
                     pos /= pos[3];

                     // Convert to Pixel Space
                     pos[0] = (pos[0] + 1.f) * 256;
                     pos[1] = (1 - pos[1]) * 256;
                 }
                 else
                 {
                     // Fish Eye lens (Equidistant F Theta camera)

                     vec4 camSpace = viewMat * v.m_pos;

                     camSpace = normalize(camSpace);

                     double phi = atan2(camSpace.y, camSpace.x);
                     double length = std::sqrt(camSpace.x * camSpace.x + camSpace.y * camSpace.y);
                     double theta = asin(length);

                     //Equidistant projection
                     double r = focalLength * theta;

                     double x = -r * cos(phi);
                     double y = -r * sin(phi);

                     // Convert to pixel space
                     x += 0.5;
                     y += 0.5;
                     pos[0] = x * 512.0;
                     pos[1] = y * 512.0;
                 }

                 // Set the Vertex to the newly calculated position
                 v.m_pos = pos;
             }

            for(Triangle t : pCopy.m_tris)
            {
                Vertex vert0 = pCopy.m_verts[t.m_indices[0]];
                Vertex vert1 = pCopy.m_verts[t.m_indices[1]];
                Vertex vert2 = pCopy.m_verts[t.m_indices[2]];

                // Back-face Culling
                if(dot(camera.forward, vert0.m_normal) > 0.f &&
                        dot(camera.forward, vert1.m_normal) > 0.f &&
                        dot(camera.forward, vert2.m_normal) > 0.f)
                {
                    continue;
                }

                // World Space positions
                vec4 worldPos0 = p.m_verts[t.m_indices[0]].m_pos;
                vec4 worldPos1 = p.m_verts[t.m_indices[1]].m_pos;
                vec4 worldPos2 = p.m_verts[t.m_indices[2]].m_pos;

                pCopy.calcBoundingBox(t);

                Segment s0(vert0.m_pos, vert1.m_pos);
                Segment s1(vert1.m_pos, vert2.m_pos);
                Segment s2(vert2.m_pos, vert0.m_pos);

                // Iterate through each pixel
                for(int y = t.yUpper; y < t.yLower && y < 512; y++)
                {
                    float x0 = -1.f;
                    float x1 = -1.f;
                    float x2 = -1.f;

                    float enter = -1.f;
                    float exit = -1.f;

                    int numIntersections = 0;

                    if(s0.getIntersection((float)y, x0))
                    {
                        numIntersections++;
                    }
                    if(s1.getIntersection((float)y, x1))
                    {
                        numIntersections++;
                    }
                    if(s2.getIntersection((float)y, x2))
                    {
                        numIntersections++;
                    }

                    if(numIntersections < 1)
                    {
                        continue;
                    }
                    else if(numIntersections == 1)
                    {
                        if(x0 == -1.f)
                        {
                            enter = x0;
                        }
                        else if(x1 != -1.f)
                        {
                            enter = x1;
                        }
                        else
                        {
                            enter = x2;
                        }
                    }
                    else
                    {
                        if(x0 == -1.f)
                        {
                            enter = x1;
                            exit = x2;
                        }
                        else
                        {
                            enter = x0;
                            if(x1 == -1.f)
                            {
                                exit = x2;
                            }
                            else
                            {
                                exit = x1;
                            }
                        }
                    }

                    if(exit < enter)
                    {
                        float temp = exit;
                        exit = enter;
                        enter = temp;
                    }

                    paint = false;

                    for(int x = t.xLeft; x < t.xRight && x < 512; x++)
                    {
                        if(x >= enter)
                        {
                            paint = true;
                        }
                        if(x > exit)
                        {
                            paint = false;
                        }

                        if(paint == true)
                        {
                            vec4 intPt = vec4(float(x), float(y), 0.f, 1.0);

                            vector<float> vertWeights = pCopy.baryInterp2D(t, intPt);

                            vec4 depthVec = pCopy.interpZDepth(vertWeights, t, worldPos0, worldPos1,
                                                              worldPos2, intPt, camera.position);
                            float zDepth = depthVec[3];

                            // Too close to camera
                            if(zDepth < 1.f)
                            {
                                continue;
                            }

                            if(zDepth <= currentScreen[x + 512 * y])
                            {
                                currentScreen[x + 512 * y] = zDepth;

                                // Get Texture per pixel
                                vec3 color = pCopy.baryInterpUVs(vertWeights, depthVec,
                                                  vert0, vert1, vert2, pCopy.mp_texture);

                                // Lighting
                                float scaleFactor = pCopy.baryInterpNormals(vertWeights, depthVec,
                                                            vert0, vert1, vert2, camera.forward);
                                color *= scaleFactor;

                                // Color Clamping
                                if(color[0] > 255.f)
                                {
                                    color /= (color[0] / 255.f);
                                }
                                if(color[1] > 255.f)
                                {
                                    color /= (color[1] / 255.f);
                                }
                                if(color[2] > 255.f)
                                {
                                    color /= (color[2] / 255.f);
                                }

                                result.setPixel(x, y, qRgb(color[0], color[1], color[2]));
                            }
                        }
                    }
                }
            }
        }
    }

    return result;
}


void Rasterizer::ClearScene()
{
    m_polygons.clear();
}
