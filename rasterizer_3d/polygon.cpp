// Declarations by Adam Malley, Implementation by Nathan Devlin

#include "polygon.h"
#include <glm/gtx/transform.hpp>

#include<iostream>
#include <cmath>

using namespace std;

using namespace glm;

// Constructors

// Creates a polygon from the input list of vertex positions and colors
Polygon::Polygon(const QString& name, const std::vector<glm::vec4>& pos, const std::vector<glm::vec3>& col)
    : m_tris(), m_verts(), m_name(name), mp_texture(nullptr), mp_normalMap(nullptr)
{
    for(unsigned int i = 0; i < pos.size(); i++)
    {
        m_verts.push_back(Vertex(pos[i], col[i], glm::vec4(), glm::vec2()));
    }
    Triangulate();
}

// Creates a regular polygon with a number of sides indicated by the "sides" input integer.
// All of its vertices are of color "color", and the polygon is centered at "pos".
// It is rotated about its center by "rot" degrees, and is scaled from its center by "scale" units
Polygon::Polygon(const QString& name, int sides, glm::vec3 color, glm::vec4 pos, float rot, glm::vec4 scale)
    : m_tris(), m_verts(), m_name(name), mp_texture(nullptr), mp_normalMap(nullptr)
{
    glm::vec4 v(0.f, 1.f, 0.f, 1.f);
    float angle = 360.f / sides;
    for(int i = 0; i < sides; i++)
    {
        glm::vec4 vert_pos = glm::translate(glm::vec3(pos.x, pos.y, pos.z))
                           * glm::rotate(rot, glm::vec3(0.f, 0.f, 1.f))
                           * glm::scale(glm::vec3(scale.x, scale.y, scale.z))
                           * glm::rotate(i * angle, glm::vec3(0.f, 0.f, 1.f))
                           * v;
        m_verts.push_back(Vertex(vert_pos, color, glm::vec4(), glm::vec2()));
    }

    Triangulate();
}


Polygon::Polygon(const QString &name)
    : m_tris(), m_verts(), m_name(name), mp_texture(nullptr), mp_normalMap(nullptr)
{}

Polygon::Polygon()
    : m_tris(), m_verts(), m_name("Polygon"), mp_texture(nullptr), mp_normalMap(nullptr)
{}

Polygon::Polygon(const Polygon& p)
    : m_tris(p.m_tris), m_verts(p.m_verts), m_name(p.m_name), mp_texture(nullptr), mp_normalMap(nullptr)
{
    if(p.mp_texture != nullptr)
    {
        mp_texture = new QImage(*p.mp_texture);
    }
    if(p.mp_normalMap != nullptr)
    {
        mp_normalMap = new QImage(*p.mp_normalMap);
    }
}

Polygon::~Polygon()
{
    delete mp_texture;
}


// Member Functions

// Add vertex indices corresponding to the points of triangles
void Polygon::Triangulate()
{

    unsigned n = m_verts.size() - 2;

    for(unsigned i = 0; i < n; i++)
    {
        Triangle t = {{0, i+1, i+2}};

        calcBoundingBox(t);

        m_tris.push_back(t);
    }
}


// Calculate the x and y bounds of the triangle, store in Triangle struct
void Polygon::calcBoundingBox(Triangle& t)
{
    vec4 vert0 = m_verts[t.m_indices[0]].m_pos;
    vec4 vert1 = m_verts[t.m_indices[1]].m_pos;
    vec4 vert2 = m_verts[t.m_indices[2]].m_pos;

    t.xLeft = glm::ceil(glm::min(std::min(vert0[0], vert1[0]), vert2[0]));
    t.xRight = glm::ceil(glm::max(std::max(vert0[0], vert1[0]), vert2[0]));

    t.yLower= glm::ceil(glm::max(std::max(vert0[1], vert1[1]), vert2[1]));
    t.yUpper= glm::ceil(glm::min(std::min(vert0[1], vert1[1]), vert2[1]));

    if(t.xLeft < 1)
    {
        t.xLeft = 1;
    }
    if(t.xRight > 511)
    {
        t.xRight = 511;
    }
    if(t.yUpper < 1)
    {
        t.yUpper = 0;
    }
    if(t.yLower > 511)
    {
        t.yLower = 511;
    }
}


// Return a vector of influences of vectors on an interior point of
// a triangle using Barycentric interpolation
vector<float> Polygon::baryInterp2D(const Triangle& t, const vec4& interiorPt)
{
    vec4 vert1 = m_verts[t.m_indices[0]].m_pos;
    vec4 vert2 = m_verts[t.m_indices[1]].m_pos;
    vec4 vert3 = m_verts[t.m_indices[2]].m_pos;

    float totalArea = tArea2D(vert1, vert2, vert3);
    float s1 = tArea2D(interiorPt, vert2, vert3);
    float s2 = tArea2D(interiorPt, vert3, vert1);
    float s3 = tArea2D(interiorPt, vert1, vert2);

    s1 /= totalArea;
    s2 /= totalArea;
    s3 /= totalArea;

    std::vector<float> vertWeights  = vector<float>();
    vertWeights.push_back(s1);
    vertWeights.push_back(s2);
    vertWeights.push_back(s3);

    return vertWeights;
}


// Returns the color corresponding to the appropriate UV value at this point
vec3 Polygon::baryInterpUVs(const vector<float>& vertWeights, const vec4& depthVec,
                            const Vertex& v1, const Vertex& v2, const Vertex& v3, QImage* i)
{
    vec2 c = depthVec[3] * (((v1.m_uv * vertWeights[0]) / depthVec[0]) +
            ((v2.m_uv * vertWeights[1]) / depthVec[1]) +
            ((v3.m_uv * vertWeights[2]) / depthVec[2]));

    if(c[0] < 0.f || c[1] < 0.f || c[0] > 1.f || c[1] > 1.f)
    {
        return vec3(0.f, 0.f, 0.f);
    }
    return GetImageColor(c, i);
}


// Returns a scalar multiple to attenuate color to simulate Lambertian lighting
float Polygon::baryInterpNormals(const vector<float>& vertWeights, const vec4& depthVec,
                                 const Vertex& v0, const Vertex& v1, const Vertex& v2,
                                 const vec4& lookVec)
{
    vec4 c = depthVec[3] * (((v0.m_normal * vertWeights[0]) / depthVec[0]) +
            ((v1.m_normal * vertWeights[1]) / depthVec[1]) +
            ((v2.m_normal * vertWeights[2]) / depthVec[2]));

    float scaleFactor = abs(dot(normalize(c), lookVec));

    // Add a bit of ambient lighting
    scaleFactor += 0.2;

    // Make the light brighter
    scaleFactor *= 1.3;

    return scaleFactor;
}


// Returns a vector of zDepth information of the fragment being considered (in cameraSpace)
vec4 Polygon::interpZDepth(const vector<float>& vertWeights, const Triangle& t,
                           const vec4& worldPos0, const vec4& worldPos1, const vec4& worldPos2,
                           const vec4& interiorPt, const vec4& camPos)
{
    float zDepth = 0.f;
    float z1 = glm::length(camPos - worldPos0);
    float z2 = glm::length(camPos - worldPos1);
    float z3 = glm::length(camPos - worldPos2);

    float zTemp = (vertWeights[0] / z1) + (vertWeights[1] / z2) + (vertWeights[2] / z3);

    zDepth = 1.f / zTemp;

    return vec4(z1, z2, z3, zDepth);
}


// Helper function to calculate the area of a triangle given three vectors
float Polygon::tArea2D(const vec4& pt1, const vec4& pt2, const vec4& pt3)
{
    vec3 newPt1 = vec3(pt1[0], pt1[1], 0.f);
    vec3 newPt2 = vec3(pt2[0], pt2[1], 0.f);
    vec3 newPt3 = vec3(pt3[0], pt3[1], 0.f);

    return 0.5 * glm::length(glm::cross(newPt1 - newPt2, newPt3 - newPt2));
}


// Helper function to calculate the area of a triangle given three vectors
float Polygon::tArea3D(const vec4& pt1, const vec4& pt2, const vec4& pt3)
{
    vec3 newPt1 = vec3(pt1[0], pt1[1], pt1[2]);
    vec3 newPt2 = vec3(pt2[0], pt2[1], pt2[2]);
    vec3 newPt3 = vec3(pt3[0], pt3[1], pt3[2]);

    return 0.5 * glm::length(glm::cross(newPt1 - newPt2, newPt3 - newPt2));
}



void Polygon::SetTexture(QImage* i)
{
    mp_texture = i;
}

void Polygon::SetNormalMap(QImage* i)
{
    mp_normalMap = i;
}

void Polygon::AddTriangle(Triangle& t)
{
    // Added to make bounding box work in 3D
    calcBoundingBox(t);

    m_tris.push_back(t);
}

void Polygon::AddVertex(const Vertex& v)
{
    m_verts.push_back(v);
}

void Polygon::ClearTriangles()
{
    m_tris.clear();
}

Triangle& Polygon::TriAt(unsigned int i)
{
    return m_tris[i];
}

Triangle Polygon::TriAt(unsigned int i) const
{
    return m_tris[i];
}

Vertex &Polygon::VertAt(unsigned int i)
{
    return m_verts[i];
}

Vertex Polygon::VertAt(unsigned int i) const
{
    return m_verts[i];
}

glm::vec3 GetImageColor(const glm::vec2 &uv_coord, const QImage* const image)
{

    if(image)
    {
        int X = glm::min(image->width() * uv_coord.x, image->width() - 1.0f);
        int Y = glm::min(image->height() * (1.0f - uv_coord.y), image->height() - 1.0f);

        if(X < 0.f  || Y < 0.f)
        {
            return vec3(128.f, 128.f, 128.f);
        }

        QColor color = image->pixel(X, Y);
        return glm::vec3(color.red(), color.green(), color.blue());
    }
    return glm::vec3(255.f, 255.f, 255.f);
}
