// Original Code by Adam Malley, Modified by Nathan Devlin

#pragma once
#include <polygon.h>
#include <QImage>


#include <array>
#include "camera.h"

class Rasterizer
{
private:
    //This is the set of Polygons loaded from a JSON scene file
    std::vector<Polygon> m_polygons;
public:
    Rasterizer(const std::vector<Polygon>& polygons);
    QImage RenderScene();
    void ClearScene();

    // Added Member variables
    Camera camera;
    glm::mat4 perspPovMat;

    std::array<float, 262144> currentScreen;

};
