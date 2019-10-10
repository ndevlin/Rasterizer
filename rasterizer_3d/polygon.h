// Original Code by Adam Malley, Modified by Nathan Devlin

#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <QString>
#include <QImage>
#include <QColor>

// A Vertex is a point in space that defines one corner of a polygon.
// Each Vertex has several attributes that determine how they contribute to the
// appearance of their Polygon, such as coloration.
struct Vertex
{
    glm::vec4 m_pos;    // The position of the vertex. In hw02, this is in pixel space.
    glm::vec3 m_color;  // The color of the vertex. X corresponds to Red, Y corresponds to Green, and Z corresponds to Blue.
    glm::vec4 m_normal; // The surface normal of the vertex (not yet used)
    glm::vec2 m_uv;     // The texture coordinates of the vertex (not yet used)

    Vertex(glm::vec4 p, glm::vec3 c, glm::vec4 n, glm::vec2 u)
        : m_pos(p), m_color(c), m_normal(n), m_uv(u)
    {}
};

// Each Polygon can be decomposed into triangles that fill its area.
struct Triangle
{
    // The indices of the Vertices that make up this triangle.
    // The indices correspond to the std::vector of Vertices stored in the Polygon
    // which stores this Triangle
    unsigned int m_indices[3];

    // Triangle's Bounding Box coordinates
    int xLeft;
    int xRight;
    int yUpper;
    int yLower;
};

class Polygon
{
public:
    std::vector<Triangle> m_tris;
    // The list of Vertices that define this polygon. This is already filled by the Polygon constructor.
    std::vector<Vertex> m_verts;
    // The name of this polygon, primarily to help you debug
    QString m_name;
    // The image that can be read to determine pixel color when used in conjunction with UV coordinates
    // Not used until homework 3.
    QImage* mp_texture;
    // The image that can be read to determine surface normal offset when used in conjunction with UV coordinates
    // Not used until homework 3
    QImage* mp_normalMap;

    // Polygon class constructors
    Polygon(const QString& name, const std::vector<glm::vec4>& pos, const std::vector<glm::vec3> &col);
    Polygon(const QString& name, int sides, glm::vec3 color, glm::vec4 pos, float rot, glm::vec4 scale);
    Polygon(const QString& name);
    Polygon();
    Polygon(const Polygon& p);
    ~Polygon();



    void Triangulate();

    // Copies the input QImage into this Polygon's texture
    void SetTexture(QImage*);

    // Copies the input QImage into this Polygon's normal map
    void SetNormalMap(QImage*);

    // Various getter, setter, and adder functions
    void AddVertex(const Vertex&);
    void AddTriangle(Triangle&);
    void ClearTriangles();

    Triangle& TriAt(unsigned int);
    Triangle TriAt(unsigned int) const;

    Vertex& VertAt(unsigned int);
    Vertex VertAt(unsigned int) const;


    // Additional member functions

    // Calculate the x and y bounds of the triangle, store in Triangle struct
    void calcBoundingBox(Triangle& t);

    // Return a vector of influences of vectors on an interior point of
    // a triangle using Barycentric interpolation
    std::vector<float> baryInterp2D(const Triangle& t, const glm::vec4& interiorPt);


    // Returns the zDepth of the fragment being considered
    glm::vec4 interpZDepth(const std::vector<float>& vertWeights, const Triangle& t,
                           const glm::vec4& worldPos0, const glm::vec4& worldPos1,
                           const glm::vec4& worldPos2, const glm::vec4& interiorPt,
                           const glm::vec4& camDepth);

    // Returns the color corresponding to the appropriate UV value at this point
    glm::vec3 baryInterpUVs(const std::vector<float>& vertWeights, const glm::vec4& depthVec,
                            const Vertex& v0, const Vertex& v1, const Vertex& V2, QImage*);

    // Returns a scalar multiple to attenuate color to simulate Lambertian lighting
    float baryInterpNormals(const std::vector<float>& vertWeights, const glm::vec4& depthVec,
                            const Vertex& v0, const Vertex& v1, const Vertex& v2,
                            const glm::vec4& lookVec);

    // Helper function to calculate the area of a triangle given three vectors
    float tArea2D(const glm::vec4& pt1, const glm::vec4& pt2, const glm::vec4& pt3);

    // Helper function to calculate the area of a triangle given three vectors
    float tArea3D(const glm::vec4& pt1, const glm::vec4& pt2, const glm::vec4& pt3);


};

// Returns the color of the pixel in the image at the specified texture coordinates.
// Returns white if the image is a null pointer
glm::vec3 GetImageColor(const glm::vec2 &uv_coord, const QImage* const image);
