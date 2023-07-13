#pragma once
#include <Matrix.h>
#include <math.h>
#include <vector>
#include <algorithm>
#include <string.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <Utility.h>
using namespace std;

#ifndef GRAPHICS_H
#define GRAPHICS_H

//-----------GRAPHICS---------------

struct GraphicSettings
{
    static bool frustumCulling;
    static bool backFaceCulling;
    static bool invertNormals;
    static bool debugNormals;
    static bool debugVertices;
    static bool debugAxes;
    static bool perspective;
    static bool fillTriangles;
    static bool displayWireFrames;
    static bool lighting;
    static bool vfx;
    static bool matrixMode;
};
bool GraphicSettings::frustumCulling = true;
bool GraphicSettings::backFaceCulling = true;
bool GraphicSettings::invertNormals = false;
bool GraphicSettings::debugNormals = false;
bool GraphicSettings::debugVertices = false;
bool GraphicSettings::debugAxes = false;
bool GraphicSettings::perspective = true;
bool GraphicSettings::fillTriangles = true;
bool GraphicSettings::displayWireFrames = false;
bool GraphicSettings::lighting = true;
bool GraphicSettings::vfx = false;
bool GraphicSettings::matrixMode = false;

#define Color Vector3<float>
struct RGB : Vector3<float>
{
    static Color black;
    static Color white;
    static Color red;
    static Color green;
    static Color blue;
    static Color pink;
    static Color yellow;
    static Color turquoise;
    static Color orange;
};
Color RGB::black = Color(0, 0, 0);
Color RGB::white = Color(255, 255, 255);
Color RGB::red = Color(255, 0, 0);
Color RGB::green = Color(0, 255, 0);
Color RGB::blue = Color(0, 0, 255);
Color RGB::pink = Color(255, 0, 255);
Color RGB::yellow = Color(255, 255, 0);
Color RGB::turquoise = Color(0, 255, 255);
Color RGB::orange = Color(255, 158, 0);

struct Vec3D
{
    static Vec3 forward;
    static Vec3 back;
    static Vec3 right;
    static Vec3 left;
    static Vec3 up;
    static Vec3 down;
};
Vec3 Vec3D::forward = Vec3(0, 0, -1);
Vec3 Vec3D::back = Vec3(0, 0, 1);
Vec3 Vec3D::right = Vec3(1, 0, 0);
Vec3 Vec3D::left = Vec3(-1, 0, 0);
Vec3 Vec3D::up = Vec3(0, 1, 0);
Vec3 Vec3D::down = Vec3(0, -1, 0);

static float worldScale = 1;
int screenWidth = 800;//700;//screen.width - 20;
int screenHeight = 800; //screen.height - 20; //screen.height;// - 30;
float aspectRatio = screenWidth / screenHeight;
float nearClippingPlane = -0.1;
float farClippingPlane = -100000.0;
float fieldOfViewDeg = 60;
float fov = ToRad(fieldOfViewDeg);

// Perspective Projection Matrix
float persp[4][4] = {
    {1 / tan(fov / 2), 0, 0, 0},
    {0, 1 / tan(fov / 2), 0, 0},
    {0, 0, 1, 0},
    {0, 0, -1, 0}
};

//Orthographic Projection Matrix
float ortho[4][4] = {
    {1, 0, 0, 0},
    {0, 1, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 1}
};

Matrix4x4 perspectiveProjectionMatrix = persp;
Matrix4x4 orthographicProjectionMatrix = ortho;

void FOV(int deg)
{
    fieldOfViewDeg = deg;
    fov = ToRad(deg);
    float newPerspectiveProjectionMatrix[4][4] = {
        {1/tan(fov/2), 0, 0, 0},
        {0, 1/tan(fov/2), 0, 0},
        {0, 0, 1, 0},
        {0, 0, -1, 0}
    };

    perspectiveProjectionMatrix = newPerspectiveProjectionMatrix;
}

Matrix4x4 ProjectionMatrix()
{
    if (GraphicSettings::perspective) {
        return perspectiveProjectionMatrix;
    }
    else {
        return orthographicProjectionMatrix;
    }
}

Vec4 ProjectPoint(Vec4 point)
{
    point = ProjectionMatrix() * point;

    point.x *= worldScale;
    point.y *= worldScale;
    return point;
}

struct Point
{
    Vec3 point;
    Color color;
    int size;

    Point(Vec3 point, Color color = RGB::white, int size = 2)
    {
        this->point = point;
        this->color = color;
        this->size = size;
    }

    void Draw()
    {
        glPointSize(size);
        glColor3ub(color.x, color.y, color.z);

        glBegin(GL_POINTS);
        glVertex2f(point.x, point.y);
        glEnd();
    }
};

struct Line
{
    Vec3 from;
    Vec3 to;
    Color color;
    int width;

    Line(Vec3 from, Vec3 to, Color color = RGB::white, int width = 2)
    {
        this->from = from;
        this->to = to;
        this->color = color;
        this->width= width;
    }

    void Draw()
    {
        glLineWidth(width);
        glColor3ub(color.x, color.y, color.z);

        glBegin(GL_LINES);
        glVertex2f(from.x, from.y);
        glVertex2f(to.x, to.y);
        glEnd();
    }
};

List<Point>* points = new List<Point>();
void Points(Point point)
{
    points->emplace_back(point);
}
List<Line>* lines = new List<Line>();
void Lines(Line line)
{
    lines->emplace_back(line);
}

class Mesh;
struct Triangle : Plane
{
    //Vec3 verts[3];
    //Vec3 normal;
    Vec4 centroid;
    Color color;
    Mesh* mesh;

    Triangle()
    {
        color = RGB::white;
        centroid = Vec3(0, 0, 0);
        normal = Vec3(0, 0, 0);
    };

    Triangle(Vec3 p1, Vec3 p2, Vec3 p3)
    {
        this->verts[0] = p1;
        this->verts[1] = p2;
        this->verts[2] = p3;
        color = RGB::white;
        centroid = Centroid();
    }
    /*
    Vec4 Normal()
    {
        // Calculate triangle suface Normal
        Vec3 a = verts[2] - verts[0];
        Vec3 b = verts[1] - verts[0];
        Vec3 normal = (CrossProduct(a, b)).Normalized();

        return normal;
    }*/

    Vec4 Centroid()
    {
        centroid = Vec4(
            (verts[0].x + verts[1].x + verts[2].x) / 3.0,
            (verts[0].y + verts[1].y + verts[2].y) / 3.0,
            (verts[0].z + verts[1].z + verts[2].z) / 3.0,
            centroid.w
        );

        return centroid;
    }

    void Draw()
    {
        Vec4 p1 = verts[0];
        Vec4 p2 = verts[1];
        Vec4 p3 = verts[2];

        if (GraphicSettings::fillTriangles == false) 
        {
            GraphicSettings::displayWireFrames = true;
        }

        //glColor3ub(255, 255, 255);
        if (GraphicSettings::matrixMode)
        {
            glColor3ub(0, 255, 0);
        }

        if (GraphicSettings::fillTriangles)
        {   glColor3ub(color.x, color.y, color.z);
            glBegin(GL_TRIANGLES);
            glVertex2f(p1.x, p1.y);
            glVertex2f(p2.x, p2.y);
            glVertex2f(p3.x, p3.y);
            glEnd();
        }

        if (GraphicSettings::displayWireFrames)
        {
            glBegin(GL_LINES);
            if (GraphicSettings::fillTriangles)
            {
                float c = Clamp(1.0 / (0.000001 + (color.x + color.y + color.z) / 3), 0, 255);
                glColor3ub(c, c, c);
            }
            glVertex2f(p1.x, p1.y);
            glVertex2f(p2.x, p2.y);

            glVertex2f(p2.x, p2.y);
            glVertex2f(p3.x, p3.y);

            glVertex2f(p3.x, p3.y);
            glVertex2f(p1.x, p1.y);
            glEnd();
        }
        
        glColor3ub(255, 255, 255);
    }
};

List<Triangle>* triBuffer = new List<Triangle>(100000);

//-------------------------------TRANSFORM---------------------------------------------
class Transform
{
public:
    Vec3 scale = Vec3(1, 1, 1);
    Vec3 position = Vec3(0, 0, 0);
    Matrix3x3 rotation = Identity3x3;
    Vec3 Forward() { return this->rotation * Vec3D::forward; }
    Vec3 Back() { return this->rotation * Vec3D::back; }
    Vec3 Right() { return this->rotation * Vec3D::right; }
    Vec3 Left() { return this->rotation * Vec3D::left; }
    Vec3 Up() { return this->rotation * Vec3D::up; }
    Vec3 Down() { return this->rotation * Vec3D::down; }
    Transform* parent;

    Transform(float scale = 1, Vec3 position = Vec3(0, 0, 0), Vec3 rotationEuler = Vec3(0, 0, 0))
    {
        this->scale.x = scale;
        this->scale.y = scale;
        this->scale.z = scale;
        this->position = position;
        this->rotation = YPR(rotationEuler.x, rotationEuler.y, rotationEuler.z);    
    }

    Matrix4x4 ScaleMatrix4x4()
    {
        float matrix[4][4] =
{
            {this->scale.x, 0, 0, 0},
            {0, this->scale.y, 0, 0},
            {0, 0, this->scale.z, 0},
            {0, 0, 0, 1}
        };

        return Matrix4x4(matrix);
    }

    Matrix4x4 RotationMatrix4x4() 
    {
        float matrix[4][4] = 
        {
            {this->rotation.m[0][0], this->rotation.m[0][1], this->rotation.m[0][2], 0},
            {this->rotation.m[1][0], this->rotation.m[1][1], this->rotation.m[1][2], 0},
            {this->rotation.m[2][0], this->rotation.m[2][1], this->rotation.m[2][2], 0},
            {0, 0, 0, 1}
        };

        return Matrix4x4(matrix);
    }

    Matrix4x4 TranslationMatrix4x4() 
    {
        float matrix[4][4] = 
        {
            {1, 0, 0, this->position.x},
            {0, 1, 0, this->position.y},
            {0, 0, 1, this->position.z},
            {0, 0, 0, 1}
        };

        return Matrix4x4(matrix);
    }

    Matrix4x4 TranslationMatrix4x4Inverse() 
    {
        float matrix[4][4] = 
        {
            {1, 0, 0, -this->position.x},
            {0, 1, 0, -this->position.y},
            {0, 0, 1, -this->position.z},
            {0, 0, 0, 1}
        };

        return Matrix4x4(matrix);
    }

    Matrix4x4 TRS() 
    {
        float matrix[4][4] =
        {
            {this->rotation.m[0][0] * scale.x, this->rotation.m[0][1] * scale.y, this->rotation.m[0][2] * scale.z, position.x},
            {this->rotation.m[1][0] * scale.x, this->rotation.m[1][1] * scale.y, this->rotation.m[1][2] * scale.z, position.y},
            {this->rotation.m[2][0] * scale.x, this->rotation.m[2][1] * scale.y, this->rotation.m[2][2] * scale.z, position.z},
            {0,                                         0,                                  0,                      1}
        };

        if (parent) 
        {
            return parent->TRS() * matrix;
        }

        return matrix; //return TranslationMatrix4x4() * RotationMatrix4x4() * ScaleMatrix4x4();
        
    }

    Matrix4x4 TR() 
    {
        if (parent) 
        {
            return parent->TR() * TranslationMatrix4x4() * RotationMatrix4x4();
        }

        return TranslationMatrix4x4() * RotationMatrix4x4();
    }

    // R^-1T^-1
    Matrix4x4 TRInverse() 
    {
        if (parent) 
        {
            return  Matrix4x4::Transpose(RotationMatrix4x4()) * TranslationMatrix4x4Inverse() * parent->TRInverse();
        }

        return Matrix4x4::Transpose(RotationMatrix4x4()) * TranslationMatrix4x4Inverse();
    }
};

//-----------------------------CAMERA-------------------------------------------------
class Camera : public Transform
{
public:
    static Camera* main;
    static List<Camera*> cameras;
    static int cameraCount;

    std::string name;

    Camera(Vec3 position = Vec3(0, 0, 0), Vec3 rotationEuler = Vec3(0, 0, 0))
    : Transform(1, position, rotationEuler)
    {
        cameras.emplace(cameras.begin() + cameraCount++, this);
        name = "Camera " + cameraCount;
    }
};
List<Camera*> Camera::cameras = List<Camera*>(1);
int Camera::cameraCount = 0;
Camera* cam = new Camera();
Camera* Camera::main = cam;

//---------------------------------MESH---------------------------------------------
class Mesh : public Transform, public ManagedObjectPool<Mesh>
{
public:
    //static List<Mesh*> meshes;
    //static int meshCount;
    static int worldTriangleDrawCount;
    
    List<Vec3>* vertices;
    List<Triangle>* triangles;
    virtual List<Triangle>* MapVertsToTriangles() { return triangles; }
    Color color = RGB::white;

    Mesh(float scale = 1, Vec3 position = Vec3(0, 0, 0), Vec3 rotationEuler = Vec3(0, 0, 0))
    : Transform(scale, position, rotationEuler), ManagedObjectPool(this)
    {
        //Mesh::objects.emplace(objects.begin()+meshCount++, this);
        //name = "Mesh "+meshCount;
        MapVertsToTriangles();
    }

    ~Mesh()
    {
        delete vertices;
        delete triangles;
    }

    //Convert to world coordinates
    List<Vec3> WorldVertices()
    {
        List<Vec3> verts = *vertices;
        Matrix4x4 matrix = TRS();
        for (size_t i = 0; i < verts.size(); i++)
        {
            verts[i] = (Vec3)(matrix * ((Vec4)verts[i]));
        }

        return verts;
    }

    static void DrawMeshes()
    {
        static bool init = false;
        if (!init) {
            Mesh::objects.resize(count);
            Mesh::objects.shrink_to_fit();
            init = true;
        }

        // ---------- Transform -----------
        for (int i = 0; i < Mesh::count; i++)
        {
            if (GraphicSettings::frustumCulling)
            {
                // Scale/Distance ratio culling
                bool tooSmallToSee = Mesh::objects[i]->scale.SqrMagnitude() / (Mesh::objects[i]->position - Camera::main->position).SqrMagnitude() < 0.000000125;
                if (tooSmallToSee) {
                    return;
                }
                bool behindCamera = DotProduct(Mesh::objects[i]->position - Camera::main->position, Camera::main->Forward()) < 0.0;
                if (behindCamera) {
                    continue;
                }
                /*
                if (Mesh::objects[i]->vertices)
                {
                    List<Vec3> verts = *(Mesh::objects[i]->vertices);
                    for (size_t j = 0; j < verts.size(); j++)
                    {
                        verts[j] = ProjectionMatrix() * Camera::main->TRInverse() * Mesh::objects[i]->TRS() * verts[j];
                    }

                    Range range = ProjectVertsOntoAxis(verts.data(), verts.size(), Vec3D::left);
                    if (range.min >= 1 && range.max >= 1) {
                        continue;
                    }
                    range = ProjectVertsOntoAxis(verts.data(), verts.size(), Vec3D::right);
                    if (range.min >= 1 && range.max >= 1) {
                        continue;
                    }
                    range = ProjectVertsOntoAxis(verts.data(), verts.size(), Vec3D::up);
                    if (range.min >= 1 && range.max >= 1) {
                        continue;
                    }
                    range = ProjectVertsOntoAxis(verts.data(), verts.size(), Vec3D::down);
                    if (range.min >= 1 && range.max >= 1) {
                        continue;
                    }
                }*/
                
                // ---------- Debug -----------
                if (GraphicSettings::debugAxes)
                {
                    Matrix4x4 mvp = ProjectionMatrix() * Camera::main->TRInverse() * Mesh::objects[i]->TRS();

                    Vec3 center_p = mvp * Vec4(0, 0, 0, 1);
                    Vec3 right_p = mvp * (Vec4)Vec3D::right;
                    Vec3 up_p = mvp * (Vec4)Vec3D::up;
                    Vec3 forward_p = mvp * (Vec4)Vec3D::forward;

                    Points(Point(center_p, RGB::red, 5));
                    Lines(Line(center_p, right_p, RGB::red));
                    Lines(Line(center_p, up_p, RGB::yellow));
                    Lines(Line(center_p, forward_p, RGB::blue));
                }
            }
            
            Mesh::objects[i]->transformTriangles();
        }

        Mesh::worldTriangleDrawCount = triBuffer->size();

        // ---------- Sort (Painter's algorithm) -----------
        sort(triBuffer->begin(), triBuffer->end(), [](const Triangle& triA, const Triangle& triB) -> bool
        {
           return triA.centroid.w > triB.centroid.w;
        });

        // ---------- Draw -----------
        for (int i = 0; i < triBuffer->size(); i++)
        {
            (*triBuffer)[i].Draw();
        }

        triBuffer->clear();
    }

private:
    void transformTriangles() 
    {
        // Scale/Distance ratio culling
        bool tooSmallToSee = scale.SqrMagnitude() / (position - Camera::main->position).SqrMagnitude() < 0.000000125;
        if (tooSmallToSee) {
            return;
        }
        
        Matrix4x4 modelToWorldMatrix = this->TRS();
        Matrix4x4 worldToViewMatrix = Camera::main->TRInverse();
        Matrix4x4 projectionMatrix = ProjectionMatrix();

        //Transform Triangles
        List<Triangle>* tris = MapVertsToTriangles();
        for (int i = 0; i < tris->size(); i++)
        {
            Triangle tri = (*tris)[i];
            tri.mesh = this;
            tri.color = this->color;
            Triangle worldSpaceTri = tri;
            Triangle camSpaceTri = tri;
            Triangle projectedTri = tri;
            for (int j = 0; j < 3; j++)
            {
                // Homogeneous coords (x, y, z, w=1)
                Vec4 vert = tri.verts[j];

                // =================== WORLD SPACE ===================
                // Transform local coords to world-space coords.
                Vec4 worldPoint = modelToWorldMatrix * vert;
                worldSpaceTri.verts[j] = worldPoint;

                // ================ VIEW/CAM/EYE SPACE ================
                // Transform world coordinates to view coordinates.
                Vec4 cameraSpacePoint = worldToViewMatrix * worldPoint;
                camSpaceTri.verts[j] = cameraSpacePoint;

                // ================ SCREEN SPACE ==================
                // Project to screen space (image space)
                Vec4 projectedPoint = projectionMatrix * cameraSpacePoint;
                projectedTri.verts[j] = projectedPoint;
            };
            
            //------------------Ray casting (world & view space)--------------------------
            
            Vec3 lineStart = Camera::cameras[1]->position;
            Vec3 lineEnd = lineStart + Camera::cameras[1]->Forward() * abs(farClippingPlane);
            Vec3 pointOfIntersection;
            if (LinePlaneIntersecting(lineStart, lineEnd, worldSpaceTri, &pointOfIntersection))
            {
                Matrix4x4 matrix = projectionMatrix * worldToViewMatrix;

                Vec4 pointOfIntersection_p = matrix * pointOfIntersection;
                if (PointInsideTriangle(pointOfIntersection_p, projectedTri.verts))
                {
                    // ---------- Debugging -----------
                    //Vec4 pointOfIntersectionProj = projectionMatrix * worldToViewMatrix * pointOfIntersection;
                    Vec4 from_p = matrix * lineStart;
                    Vec4 to_p = matrix * (pointOfIntersection);// +lineEnd);
                    Lines(Line(from_p, pointOfIntersection_p));
                    Points(Point(pointOfIntersection_p, RGB::black, 5));

                    // Reflect
                    Vec3 n = worldSpaceTri.Normal();
                    Vec3 v = (lineEnd - lineStart);
                    Vec3 reflection = Reflect(v, n);
                    Lines(Line(pointOfIntersection_p, (Vec3)(matrix * (pointOfIntersection + reflection)), RGB::red));

                    Vec3 vecPlane = ProjectOnPlane(v, n);
                    Lines(Line(pointOfIntersection_p, (Vec3)(matrix * (pointOfIntersection + vecPlane.Normalized())), RGB::black));

                    projectedTri.color = RGB::white;
                    if (DEBUGGING) { std::cout << (projectedTri.mesh) << endl; }// delete projectedTri.mesh; }
                }
            }

            //------------------- Normal/Frustum Culling (view space)------------------------
            Vec3 p1_c = camSpaceTri.verts[0];
            Vec3 p2_c = camSpaceTri.verts[1];
            Vec3 p3_c = camSpaceTri.verts[2];
            camSpaceTri.Centroid();

            if (GraphicSettings::frustumCulling)
            {
                bool tooCloseToCamera = (p1_c.z >= nearClippingPlane || p2_c.z >= nearClippingPlane || p3_c.z >= nearClippingPlane || camSpaceTri.centroid.z >= nearClippingPlane);
                bool tooFarFromCamera = (p1_c.z <= farClippingPlane || p2_c.z <= farClippingPlane || p3_c.z <= farClippingPlane || camSpaceTri.centroid.z <= farClippingPlane);
                bool behindCamera = DotProduct((Vec3)camSpaceTri.centroid, Vec3D::forward) <= 0.0;
                if (tooCloseToCamera || tooFarFromCamera || behindCamera) {
                    continue; // Skip triangle if it's out of cam view.
                }
                
                Range range = ProjectVertsOntoAxis(projectedTri.verts, 3, Vec3D::left);
                if (range.min >= 1 && range.max >= 1) {
                    continue;
                }
                range = ProjectVertsOntoAxis(projectedTri.verts, 3, Vec3D::right);
                if (range.min >= 1 && range.max >= 1) {
                    continue;
                }
                range = ProjectVertsOntoAxis(projectedTri.verts, 3, Vec3D::up);
                if (range.min >= 1 && range.max >= 1) {
                    continue;
                }
                range = ProjectVertsOntoAxis(projectedTri.verts, 3, Vec3D::down);
                if (range.min >= 1 && range.max >= 1) {
                    continue;
                }
            }

            // Calculate triangle suface Normal
            camSpaceTri.Normal();

            if (GraphicSettings::invertNormals) {
                camSpaceTri.normal = ((Vec3) camSpaceTri.normal) * -1.0;
            }

            // Back-face Culling - Checks if the triangles backside is facing the camera.
            // Condition makes this setting optional when drawing wireframes alone, but will force culling if triangles are filled.
            if (GraphicSettings::backFaceCulling || GraphicSettings::fillTriangles)
            {
                Vec3 posRelativeToCam = camSpaceTri.centroid;// Since camera is (0,0,0) in view space, the displacement vector from camera to centroid IS the centroid itself.
                bool faceInvisibleToCamera = DotProduct(posRelativeToCam, (Vec3) camSpaceTri.normal) >= 0;
                if (faceInvisibleToCamera) {
                    continue;// Skip triangle if it's out of cam view or it's part of the other side of the mesh.
                }
            }

            //------------------------ Lighting (world space)------------------------
            
            if (GraphicSettings::lighting && GraphicSettings::fillTriangles)
            {
                Vec3 lightSource = Vec3D::up + Vec3D::right + Vec3D::back * .3;
                float amountFacingLight = DotProduct((Vec3)worldSpaceTri.Normal(), lightSource);
                Color colorLit = (projectedTri.color* Clamp(amountFacingLight, 0.15, 1));

                projectedTri.color = colorLit;
            }

            if (GraphicSettings::vfx)
            {
                Vec3 screenLeftSide = Vec3(-1, 0, 0);
                Vec3 screenRightSide = Vec3(1, 0, 0);
                bool leftHalfScreenX = DotProduct(screenLeftSide, (Vec3)projectedTri.Centroid()) < 0.0;

                if (leftHalfScreenX) {
                    projectedTri.color = Color(0, 0, 255);// std::cout << "Inside" << std::endl;
                }
                else {
                    projectedTri.color = RGB::red;
                }
            }
            // ---------- Debugging -----------
            if (GraphicSettings::debugNormals)
            {
                //---------Draw point at centroid and a line from centroid to normal (view space & projected space)-----------
                Vec2 centroidToNormal_p = projectionMatrix * ((Vec3)camSpaceTri.centroid + camSpaceTri.normal);
                Vec2 centroid_p = projectionMatrix * camSpaceTri.centroid;
                Points(Point(centroid_p));
                Lines(Line(centroid_p, centroidToNormal_p));
            }

            //Add projected tri
            projectedTri.centroid = projectionMatrix * camSpaceTri.centroid;
            triBuffer->emplace_back(projectedTri);
        }
    }
};
//List<Mesh*> Mesh::objects = List<Mesh*>(1000);
//int Mesh::meshCount = 0;
int Mesh::worldTriangleDrawCount = 0;

//------------------------------------CUBE MESH------------------------------------------
class CubeMesh : public Mesh
{
public:
    CubeMesh(int scale = 1, Vec3 position = Vec3(0, 0, 0), Vec3 rotationEuler = Vec3(0, 0, 0))
        :Mesh(scale, position, rotationEuler) { }

    List<Triangle>* MapVertsToTriangles()
    {
        static int calls = 0;
        if (calls < 1)
        {
            // Local Space (Object Space)
            this->vertices = new List<Vec3>({//new Vec3[8] {
                //south
                Vec3(-1, -1, 1),
                Vec3(-1, 1, 1),
                Vec3(1, 1, 1),
                Vec3(1, -1, 1),
                //north
                Vec3(-1, -1, -1),
                Vec3(-1, 1, -1),
                Vec3(1, 1, -1),
                Vec3(1, -1, -1)
            });

            triangles = new List<Triangle>();
            triangles->reserve(36 / 3);

            //South
            triangles->emplace_back((*vertices)[0], (*vertices)[1], (*vertices)[2]);
            triangles->emplace_back((*vertices)[0], (*vertices)[2], (*vertices)[3]);
            //North                           
            triangles->emplace_back((*vertices)[7], (*vertices)[6], (*vertices)[5]);
            triangles->emplace_back((*vertices)[7], (*vertices)[5], (*vertices)[4]);
            //Right
            triangles->emplace_back((*vertices)[3], (*vertices)[2], (*vertices)[6]);
            triangles->emplace_back((*vertices)[3], (*vertices)[6], (*vertices)[7]);
            //Left
            triangles->emplace_back((*vertices)[4], (*vertices)[5], (*vertices)[1]);
            triangles->emplace_back((*vertices)[4], (*vertices)[1], (*vertices)[0]);
            //Top
            triangles->emplace_back((*vertices)[1], (*vertices)[5], (*vertices)[6]);
            triangles->emplace_back((*vertices)[1], (*vertices)[6], (*vertices)[2]);
            //Bottom
            triangles->emplace_back((*vertices)[3], (*vertices)[7], (*vertices)[4]);
            triangles->emplace_back((*vertices)[3], (*vertices)[4], (*vertices)[0]);
        }

        return triangles;
    }
};

//---------------------------------PLANE---------------------------------------------
class PlaneMesh : Mesh
{
public:
    PlaneMesh(int scale = 1, Vec3 position = Vec3(0, 0, 0), Vec3 rotationEuler = Vec3(0, 0, 0))
        :Mesh(scale, position, rotationEuler) {}
    
    List<Triangle>* MapVertsToTriangles()
    {
        static int calls = 0;
        if (calls < 1)
        {
            this->vertices = new List<Vec3> {
                Vec3(-1, -1, 0),
                Vec3(-1, 1, 0),
                Vec3(1, 1, 0),
                Vec3(1, -1, 0)
            };

            this->triangles = new List<Triangle>{
                Triangle((*vertices)[0], (*vertices)[1], (*vertices)[2]),
                Triangle((*vertices)[0], (*vertices)[2], (*vertices)[3])
            };
        }

        return triangles;
    }
};

//------------------------------HELPER FUNCTIONS------------------------------------------------

Mesh* LoadMeshFromOBJFile(string objFile) 
{
    // ----------- Read object file -------------
    List<string> strings;
    string line;
    std::ifstream file;

    file.open(objFile);
    if (file.is_open())
    {
        while (file) {
            // 1st. Gets the next line.
            // 2nd. Seperates each word from that line then stores each word into the strings array.
            getline(file, line);
            string s;
            stringstream ss(line);
            while (getline(ss, s, ' ')) {
                strings.emplace_back(s);
            }
        }
    }
    file.close();
    // -----------------Construct new mesh-------------------
    Mesh* mesh = new Mesh();
    List<Vec3>* verts = new List<Vec3>();
    List<Triangle>* triangles = new List<Triangle>();
    verts->reserve(10);
    triangles->reserve(10);

    for (size_t i = 0; i < strings.size(); i++)
    {
        // v = vertex
        // f = face.
        string str = strings[i];

        if (str == "v") {
            float x = stof(strings[++i]);
            float y = stof(strings[++i]);
            float z = stof(strings[++i]);
            verts->emplace_back(Vec3(x, y, z));
        }
        else if (str == "f") { //f means the next 3 strings will be the indices for mapping vertices
            int p3Index = stof(strings[++i]);
            int p2Index = stof(strings[++i]);
            int p1Index = stof(strings[++i]);
            Triangle tri = Triangle(verts->at(p1Index - 1), verts->at(p2Index - 1), verts->at(p3Index - 1));
            triangles->emplace_back(tri);
        }
    }

    mesh->vertices = verts;
    mesh->triangles = triangles;

    return mesh;
}
#endif