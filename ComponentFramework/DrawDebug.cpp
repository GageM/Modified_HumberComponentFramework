#include "DrawDebug.h"
#include <QMath.h>
#include <memory>

// 1D Primitives

void DrawDebug::DrawLine(const Vec3& start, const Vec3& end)
{
	std::vector<Vec3> vertices;

	GLuint vao = 0;
	GLuint vbo = 0;
	size_t dataLength = 0;
	GLenum drawmode = GL_LINES;

    vertices.push_back(start);

    vertices.push_back(end);

    StoreMeshData(drawmode, vertices, vao, vbo, dataLength);

    /// give back the memory used in these vectors. The data is safely stored in the GPU now
    vertices.clear();

    glBindVertexArray(vao);
    glDrawArrays(drawmode, 0, dataLength);
    glBindVertexArray(0);
}

void DrawDebug::DrawRay(const Vec3& start, const Vec3& dir, const float& length)
{
    DrawLine(start, start + (dir * length));
}

// 2D Primitives

void DrawDebug::DrawCircle(const Vec3& pos, const Quaternion& orientation, const float& r, const int& segments)
{
    float stepAngle = 360 / segments;
    stepAngle *= DEGREES_TO_RADIANS;

    // Create a start and end position for each segment
    Vec3 lineStart = Vec3();
    Vec3 lineEnd = Vec3();

    for (int i = 0; i <= segments; i++)
    {
        lineStart.x = cos(i * stepAngle);
        lineStart.y = sin(i * stepAngle);
        lineStart.z = 0.0f;

        lineEnd.x = cos((i + 1) * stepAngle);
        lineEnd.y = sin((i + 1) * stepAngle);
        lineStart.z = 0.0f;

        lineStart *= r;
        lineEnd *= r;

        lineStart = QMath::rotate(lineStart, orientation);
        lineEnd = QMath::rotate(lineEnd, orientation);

        lineStart += pos;
        lineEnd += pos;

        DrawLine(lineStart, lineEnd);

        lineStart = Vec3();
        lineEnd = Vec3();
    }
}

void DrawDebug::DrawArc(const Vec3& pos, const Quaternion& orientation, const float& startAngle, const float& endAngle, const float& radius, const int& segments)
{
    float arcSpan = 180 - abs(abs(startAngle - endAngle) - 180);

    if (arcSpan < 0.0f)
    {
        arcSpan += 360.0f;
    }

    float stepAngle = (arcSpan / segments) * DEGREES_TO_RADIANS;
    float stepOffset = startAngle * DEGREES_TO_RADIANS;

    // Create a start and end position for each segment
    Vec3 lineStart = Vec3();
    Vec3 lineEnd = Vec3();

    for (int i = 0; i <= segments; i++)
    {
        float stepStart = i * stepAngle + stepOffset;
        float stepEnd = (i + 1) * stepAngle + stepOffset;

        lineStart.x = cos(stepStart);
        lineStart.y = sin(stepStart);
        lineStart.z = 0.0f;

        lineEnd.x = cos(stepEnd);
        lineEnd.y = sin(stepEnd);
        lineStart.z = 0.0f;

        lineStart *= radius;
        lineEnd *= radius;

        lineStart = QMath::rotate(lineStart, orientation);
        lineEnd = QMath::rotate(lineEnd, orientation);

        lineStart += pos;
        lineEnd += pos;

        DrawLine(lineStart, lineEnd);

        lineStart = Vec3();
        lineEnd = Vec3();
    }
}

void DrawDebug::DrawTriangle(const Vec3& v1, const Vec3& v2, const Vec3& v3)
{
    DrawLine(v1, v2);
    DrawLine(v2, v3);
    DrawLine(v3, v1);
}

void DrawDebug::DrawRectangle(const Vec3& pos, const Quaternion& orientation, const Vec2& halfExtents)
{
    Vec3 offsetX, offsetY;

    offsetX = Vec3::right() * halfExtents.x;
    offsetY = Vec3::up() * halfExtents.y;

    Vec3 offsetA, offsetB, offsetC, offsetD;

    offsetA = QMath::rotate( (offsetX + offsetY), orientation);
    offsetB = QMath::rotate((-offsetX + offsetY), orientation); 
    offsetC = QMath::rotate((-offsetX - offsetY), orientation); 
    offsetD = QMath::rotate( (offsetX - offsetY), orientation); 

    DrawLine(pos + offsetA, pos + offsetB);
    DrawLine(pos + offsetB, pos + offsetC);
    DrawLine(pos + offsetC, pos + offsetD);
    DrawLine(pos + offsetD, pos + offsetA);
}

// 3D Primitives

void DrawDebug::DrawSphere(const Vec3& pos, const Quaternion& orientation, const float& r, const int& segments)
{
    float verticalStep = 180.0f / segments;

    for (int i = 0; i < segments; i++)
    {
        DrawCircle(pos, orientation * QMath::angleAxisRotation(verticalStep * i, Vec3(0.0f, 1.0f, 0.0f)) * Quaternion(), r, segments);
    }

    // Horizontal Rings
    Vec3 verticalOffset = Vec3();
    float parallelAngleStep = M_PI / segments;
    float stepRadius = 0.0f;
    float stepAngle = 0.0f;

    for (int i = 0; i < segments; i++)
    {
        stepAngle = parallelAngleStep * i;
        verticalOffset = orientation * Vec3::up() * cos(stepAngle) * r;
        stepRadius = sin(stepAngle) * r;

        //DrawCircle(pos + verticalOffset, orientation * QMath::angleAxisRotation(90.0f, Vec3(1.0f, 0.0f, 0.0f)) * Quaternion(), stepRadius, segments);
    }
}

void DrawDebug::DrawBox(const Vec3& pos, const Quaternion& orientation, const Vec3& halfExtents)
{
    Vec3 offsetX, offsetY, offsetZ;

    offsetX = QMath::rotate(Vec3::right() * halfExtents.x, orientation);
    offsetY = QMath::rotate(Vec3::up() * halfExtents.y, orientation);
    offsetZ = QMath::rotate(Vec3::forward() * halfExtents.z, orientation);

    Vec3 offsetA = -offsetX + offsetY;
    Vec3 offsetB = offsetX + offsetY;
    Vec3 offsetC = offsetX - offsetY;
    Vec3 offsetD = -offsetX - offsetY;

    // Draw the front and back of the box
    DrawRectangle(pos - offsetZ, orientation, Vec2(halfExtents.x, halfExtents.y));
    DrawRectangle(pos + offsetZ, orientation, Vec2(halfExtents.x, halfExtents.y));

    // Connect the corners of the front & back faces
    DrawLine(pos + offsetA - offsetZ, pos + offsetA + offsetZ);
    DrawLine(pos + offsetB - offsetZ, pos + offsetB + offsetZ);
    DrawLine(pos + offsetC - offsetZ, pos + offsetC + offsetZ);
    DrawLine(pos + offsetD - offsetZ, pos + offsetD + offsetZ);
}

void DrawDebug::DrawCylinder(const Vec3& capA, const Vec3& capB, const Quaternion& orientation, const float& r, const int& segments)
{
    // Draw Caps
    DrawCircle(capA, orientation * QMath::angleAxisRotation(90.0f, Vec3(1.0f, 0.0f, 0.0f)), r, segments);
    DrawCircle(capB, orientation * QMath::angleAxisRotation(90.0f, Vec3(1.0f, 0.0f, 0.0f)), r, segments);

    // Connect Caps
    float stepAngle = 360 / segments;
    stepAngle *= DEGREES_TO_RADIANS;

    // Create a start and end position for each segment
    Vec3 lineStart, lineEnd;

    for (int i = 0; i < segments; i++)
    {
        lineStart.x = cos(i * stepAngle);
        lineStart.y = sin(i * stepAngle);

        lineStart *= r;

        lineStart = QMath::rotate(lineStart, orientation);

        lineEnd = lineStart;

        lineStart += capA;
        lineEnd += capB;

        DrawLine(lineStart, lineEnd);
    }
}

void DrawDebug::DrawCapsule(const Vec3& capA, const Vec3& capB, const Quaternion& orientation, const float& r, const int& segments)
{
    // Draw base cylinder
    DrawCylinder(capA, capB, orientation, r, segments);

    float stepAngle = 360 / segments;
    Quaternion q = Quaternion();

    // Draw end half spheres
    for (int i = 0; i < segments; i++)
    {
        // Draw top half sphere
        DrawArc(capA, orientation * q, 0.0f, 180.0f, r, segments);

        // Draw bottom half sphere
        DrawArc(capA, orientation * q, 180.0f, 360.0f, r, segments);

        q = QMath::angleAxisRotation(stepAngle, Vec3(0.0f, 1.0f, 0.0f)) * q;
    }
}


// Helper Functions

void DrawDebug::StoreMeshData(GLenum drawmode, std::vector<Vec3>& vertices, GLuint& vao, GLuint& vbo, size_t& dataLength)
{
    /// These just make the code easier for me to read
    // working out the size of these buffers
    // for example each float is 4 bytes, so a Vec3 is 4*3 = 12 bytes
#define VERTEX_LENGTH 	(vertices.size() * (sizeof(MATH::Vec3)))

    // these are the channels
    const int verticiesLayoutLocation = 0;

    /// create and bind the VAO
    glGenVertexArrays(1, &vao);
    // Bind means, hey! Im talking to you!
    glBindVertexArray(vao);
    /// Create and initialize vertex buffer object VBO
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    // Generate memory for the VRAM of the GPU buffer
    // one giant array
    glBufferData(GL_ARRAY_BUFFER, VERTEX_LENGTH, nullptr, GL_STATIC_DRAW);

    /// assigns the addr of "points" to be the beginning of the array buffer "sizeof(points)" in length
    // vertex stuff will start at zero and go VERTEX_LENGTH
    glBufferSubData(GL_ARRAY_BUFFER, 0, VERTEX_LENGTH, &vertices[0]);

    // the attributes are the per-vertex stuff like vertices, normals, UVs
    glEnableVertexAttribArray(verticiesLayoutLocation);
    // 3 of them is the stride
    // floating point long using GL_FLOAT. So striding by 12 bytes
    // the last argument is a void*, but where does it begin?
    // Its an old C trick to declare a "I don't care" pointer
    // a void * is an integer. A 4 byte unsigned integer
    // and we start at the beginning
    // need a very brutal reinterpret_cast means Dammit!
    glVertexAttribPointer(verticiesLayoutLocation, 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<void*>(0));

    dataLength = vertices.size();

    /// give back the memory used in these vectors. The data is safely stored in the GPU now
    vertices.clear();

    /// Don't need these defines sticking around anymore
#undef VERTEX_LENGTH
}