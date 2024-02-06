#pragma once
#include <GLFW/glfw3.h>
#include <math.h>
#include <Matrix.h>
#include <Graphics.h>
#include <Utility.h>
using namespace std;

#ifndef PHYSICS_H
#define PHYSICS_H
extern bool DEBUGGING;
extern GLFWwindow* window;
Vec3 gravity = Vec3(0, -9.81, 0);
const float defaultAcceleration = 50;
float accel = defaultAcceleration;
float decel = -5;
float rotateSpeed = PI / 2;
bool isKinematic = false;
bool dampenersActive = true;
Vec3 moveDir = Vec3(0, 0, 0);
Vec3 velocity = Vec3(0, 0, 0);

extern Mesh* planet;
extern Mesh* spaceShip;
extern Mesh* spaceShip2;
extern Mesh* spaceShip3;

class Physics
{
public:
    static bool collisionDetection;
    static bool dynamics;
    static bool raycasting;
    static bool gravity;
};
bool Physics::collisionDetection = true;
bool Physics::dynamics = true;
bool Physics::raycasting = false;
bool Physics::gravity = false;

double deltaTime = 0;
int fps = 0;

void Time()
{
    static double prevTime = 0;
    double currentTime = glfwGetTime();
    deltaTime = currentTime - prevTime;
    prevTime = currentTime;

    static double t = 0;
    static int frames = 0;

    t += deltaTime;
    frames++;
    fps = ((double)frames) / t;
    if (t >= 1.0)
    {
        t = 0;
        frames = 0;
    }
}

struct BoundingBox : public Transform
{
public:
    Vec3 vertices[8] = {
        //south
        Vec3(-0.5, -0.5, 0.5),
        Vec3(-0.5, 0.5, 0.5),
        Vec3(0.5, 0.5, 0.5),
        Vec3(0.5, -0.5, 0.5),
        //north
        Vec3(-0.5, -0.5, -0.5),
        Vec3(-0.5, 0.5, -0.5),
        Vec3(0.5, 0.5, -0.5),
        Vec3(0.5, -0.5, -0.5)
    };
};
/*
template <typename T>
class Node : public Transform
{
public:
    int level = 0;
    Node<T> *root = nullptr;
    Node<T> *parent = nullptr;
    List<Node<T>> nodes;
    Node<T>()
    {
        if (!root)
        {
            root = this;
        }
    }
    Node<T>(int num) 
    {
        if (!root)
        {
            root = this;
        }

        nodes = List<Node<T>>(num);
        for (int i = 0; i < num; i++)
        {
            nodes[i] = Node<T>();
        }
    }
};
template <typename T>

class OctTree
{
public:
    int depth = 4;
    Node<BoundingBox> node = Node<BoundingBox>(8);
    OctTree()
    {
        node.Scale(10);
        for (size_t i = 0; i < 8; i++)
        {
            Node<BoundingBox>* n = &(node.nodes)[i];
            n->SetParent(node);

            for (int ii = -8; ii < 8; ii++)
            {
                PhysicsObject* block = new PhysicsObject(new CubeMesh(), new BoxCollider(true));
                block->Scale(2);
                block->position = Direction::down * 15 + Direction::left * i * 10 + Direction::back * j * 10;
            }
        }
            //positioning
            //test
        }
    }
};*/

class PhysicsObject;

class Component : public Transform
{
public:
    PhysicsObject* object;
};

class RigidBody
{
public:
    Vec3 centerOfMass = Vec3::zero;
    float mass = 1;
    bool isKinematic = false;
    Vec3 velocity = Vec3::zero;
    Vec3 angularVelocity = Vec3::zero;
};

class Collider: public Component
{
   
public:
    Mesh* mesh;
    bool isStatic = false;
    bool isTrigger = false;
    float coefficientRestitution = 1.0;
    
    Collider(bool isStatic = false, bool isTrigger = false)
    {
        this->isStatic = isStatic;
        this->isTrigger = isTrigger;
    }

    virtual ~Collider()
    {
        delete mesh;
    }

    virtual void RecalculateBounds()
    {

    }
};

class BoxCollider: public Collider, public ManagedObjectPool<BoxCollider>
{
public:
    BoxCollider(bool isStatic = false, bool isTrigger = false) : Collider(isStatic, isTrigger), ManagedObjectPool<BoxCollider>(this) 
    { 
        mesh = new CubeMesh();
        mesh->SetColor(&Color::red);
        mesh->SetParent(this);
        mesh->SetVisibility(false); 
    }

    List<Vec3> WorldBounds()
    {
        return mesh->WorldVertices();
    }
};

class SphereCollider: public Collider, public ManagedObjectPool<SphereCollider>
{
public:
    float radius = 1;
    SphereCollider(bool isStatic = false, bool isTrigger = false) : Collider(isStatic, isTrigger), ManagedObjectPool<SphereCollider>(this) 
    {
        mesh = LoadMeshFromOBJFile("Sphere.obj");
        mesh->SetColor(&Color::red);
        mesh->SetParent(this);
        mesh->SetVisibility(false);
    }

    void RecalculateBounds() override
    {
        float r = root->Scale().x;
        if (r < root->Scale().y) {
            r = root->Scale().y;
        }
        if (r < root->Scale().z) {
            r = root->Scale().z;
        }

        radius = r;
    }
};

class PlaneCollider: public Collider, public ManagedObjectPool<PlaneCollider>
{
public:
    Vec3 normal;
    PlaneCollider(Vec3 normal, bool isStatic = false, bool isTrigger = false) : Collider(isStatic, isTrigger), ManagedObjectPool<PlaneCollider>(this)
    {
        this->normal = normal;
        mesh = new PlaneMesh();
        mesh->SetColor(&Color::red);
        mesh->SetParent(this);
        mesh->SetVisibility(false);
    }
};

class PhysicsObject: public Transform, public RigidBody, public ManagedObjectPool<PhysicsObject>
{ 
public:
    Collider* collider;
    Mesh* mesh;

    PhysicsObject(Mesh* mesh, Collider* collider) : ManagedObjectPool<PhysicsObject>(this)
    {
        collider->Scale(mesh->Scale());
        collider->position = mesh->position;
        collider->rotation = mesh->rotation;
        
        SetCollider(collider);
        SetMesh(mesh);
    }

    PhysicsObject(float scale, Vec3 position, Matrix3x3 rotation, Mesh* mesh, Collider* collider): ManagedObjectPool<PhysicsObject>(this)
    {
        collider->Scale(mesh->Scale());
        collider->position = mesh->position;
        collider->rotation = mesh->rotation;
        SetCollider(collider);

        SetMesh(mesh);

        this->Scale(scale);
        this->position = position;
        this->rotation = rotation;
    }

    virtual ~PhysicsObject()
    {
        delete collider;
        delete mesh;
    }
    
    void Scale(float scale) override
    {
        this->scale = Vec3(scale, scale, scale);
        collider->RecalculateBounds();
    }

    void Scale(Vec3 scale) override
    {
        this->scale = scale;
        collider->RecalculateBounds();
    }

    void SetCollider(Collider* collider)
    {
        if (collider)
        {
            delete this->collider;
            this->collider = collider;
            this->collider->object = this;
            this->collider->SetParent(this);
        }
    }

    void SetMesh(Mesh* mesh)
    {
        if (mesh)
        {
            delete this->mesh;
            this->mesh = mesh;
            //this->mesh->object = this;
            this->mesh->SetParent(this);
        }
    }
};

struct CollisionInfo
{
    bool colliding = false;
};

struct BoxCollisionInfo: public CollisionInfo
{
    float minOverlap = 0;
    Vec3 minOverlapAxis = Vec3::zero;
};

struct SphereCollisionInfo: public CollisionInfo
{
    Vec3 pointOfContact = Vec3::zero;
    Vec3 lineOfImpact = Vec3::zero;
};

void CalculateCollision(Vec3 lineOfImpact, float m1, float m2, Vec3& v1, Vec3& v2, float e = 1.0)
{
    /* Elastic collision (conserves both momentum and kinetic energy)
    Conservation Momentum: m1*v1 + m2*v2 = m1*v1' + m2*v2'
    Conservation Kinetic Energy: v1 + v1' = v2 + v2'
    Coefficient of Restitution: e = v2'-v1' / v1-v2
        e = 1 Perfectly elastic
        0 < e < 1 inelastic
        e = 0 Perfectly inelastic
    */
    lineOfImpact.Normalize();
    Vec3 v1LineOfImpact = lineOfImpact * DotProduct(v1, lineOfImpact);
    Vec3 v2LineOfImpact = lineOfImpact * DotProduct(v2, lineOfImpact);
    Vec3 v1LineOfImpactFinal = (v1LineOfImpact * m1 + v2LineOfImpact * m2 * 2.0 - v1LineOfImpact * m2) * (1.0 / (m1 + m2));
    Vec3 v2LineOfImpactFinal = ((v1LineOfImpact - v2LineOfImpact) * e) + v1LineOfImpactFinal;// e(v1-v2)+v1' = v2'
    Vec3 v1PerpendicularFinal = (v1 - v1LineOfImpact);//Perpendicular Velocity is the same before and after impact
    Vec3 v2PerpendicularFinal = (v2 - v2LineOfImpact);//Perpendicular Velocity is the same before and after impact
    Vec3 v1Final = v1LineOfImpactFinal + v1PerpendicularFinal;
    Vec3 v2Final = v2LineOfImpactFinal + v2PerpendicularFinal;

    v1 = v1Final;
    v2 = v2Final;
}

void CalculateStaticCollision(Vec3 lineOfImpact, Vec3& v1, float e = 1.0)
{
    Vec3 v1LineOfImpact = lineOfImpact * DotProduct(v1, lineOfImpact);
    Vec3 v1PerpendicularFinal = (v1 - v1LineOfImpact);//Perpendicular Velocity is the same before and after impact
    v1 = (v1LineOfImpact * -e) + v1PerpendicularFinal;
}

bool SpherePlaneColliding(SphereCollider& sphere, PlaneCollider& plane, SphereCollisionInfo& collisionInfo, bool resolve = true)
{
    Vec3 sphereCenter = sphere.Position();
    Vec3 v = sphereCenter - plane.Position();
    Vec3 normal = plane.Rotation() * plane.normal;
    Vec3 vPerp = normal * (DotProduct(v, normal));//ProjectOnPlane(v, plane.plane.normal);
    Vec3 closestPointOnPlane = sphereCenter - vPerp;
    
    if ((closestPointOnPlane - sphereCenter).SqrMagnitude() < sphere.radius * sphere.radius)
    {
        collisionInfo.colliding = true;
        if (resolve) 
        {
            Vec3 pointOnSphere = ClosestPointOnSphere(sphereCenter, sphere.radius, closestPointOnPlane);
            Vec3 offset = pointOnSphere - closestPointOnPlane;//overlapping
            sphere.root->position -= offset;
            collisionInfo.lineOfImpact = normal *-1.0;
        }        
    }

    if (Graphics::debugPlaneCollisions)
    {
        Vec3 vProj = ProjectOnPlane(v, normal);
        Line::AddWorldLine(Line(plane.Position(), plane.Position() + normal, Color::gray));
        Line::AddWorldLine(Line(sphereCenter, closestPointOnPlane, Color::red));
        Point::AddWorldPoint(Point(sphereCenter, Color::gray, 10));
        Point::AddWorldPoint(Point(closestPointOnPlane, Color::red, 10));
    }

    return collisionInfo.colliding;
}

bool SpheresColliding(SphereCollider& sphere1, SphereCollider& sphere2, SphereCollisionInfo& collisionInfo, bool resolve = true)
{
    Vec3 sphere1Pos = sphere1.Position();
    Vec3 sphere2Pos = sphere2.Position();
    Vec3 pointOnSphere1 = ClosestPointOnSphere(sphere1Pos, sphere1.radius, sphere2Pos);
    Vec3 pointOnSphere2 = ClosestPointOnSphere(sphere2Pos, sphere2.radius, sphere1Pos);
    float squareMagToPointOnSphere2 = (pointOnSphere2 - sphere1Pos).SqrMagnitude();
    collisionInfo.colliding = squareMagToPointOnSphere2 < sphere1.radius * sphere1.radius;
    if (collisionInfo.colliding)
    {
        Vec3 offset = (pointOnSphere1 - pointOnSphere2);
        collisionInfo.lineOfImpact = offset;
        if (resolve) {
            offset *= 0.5;
            sphere1.root->position -= offset;
            sphere2.root->position += offset;
            collisionInfo.pointOfContact = (pointOnSphere1 + pointOnSphere2) * 0.5;
        }
    }

    if (Graphics::debugSphereCollisions)
    {
        Point::AddWorldPoint(Point(pointOnSphere1, Color::red, 4));
        Point::AddWorldPoint(Point(pointOnSphere2, Color::red, 4));
        Line::AddWorldLine(Line(pointOnSphere1, pointOnSphere2));
    }

    return collisionInfo.colliding;
}

// Oriented Bounding Box (OBB) with Separating Axis Theorem (SAT) algorithm
bool OBBSATColliding(BoxCollider& physObj1, BoxCollider& physObj2, BoxCollisionInfo& collisionInfo, bool resolve = true)
{
    bool gap = true;

    collisionInfo = BoxCollisionInfo();

    if (physObj1.isStatic && physObj2.isStatic)
    {
        return false;
    }

    List<Vec3> physObj1Verts = physObj1.WorldBounds();
    List<Vec3> physObj2Verts = physObj2.WorldBounds();
    List<Vec3> physObj1Normals = List<Vec3>{ physObj1.Right(), physObj1.Up(), physObj1.Forward() };// mesh1.WorldXYZNormals();
    List<Vec3> physObj2Normals = List<Vec3>{ physObj2.Right(), physObj2.Up(), physObj2.Forward() }; //mesh2.WorldXYZNormals();

    // Note: Collision detection stops if at any time a gap is found.
    // Note: Cache the minimum distance projection and axis for later use to resolve the collision if needed.
    // Step 1: Project both meshes onto Mesh A's normal axes.
    // Step 2: Project both meshes onto Mesh B's normal axes.
    //Initialize minimum projection distance and axis
    for (size_t i = 0; i < 2; i++)
    {
        auto normals = i == 0 ? &physObj1Normals : &physObj2Normals;
        for (size_t ii = 0; ii < normals->size(); ii++)
        {
            Vec3 axis = (*normals)[ii];
            Range rangeA = ProjectVertsOntoAxis(physObj1Verts.data(), physObj1Verts.size(), axis);
            Range rangeB = ProjectVertsOntoAxis(physObj2Verts.data(), physObj2Verts.size(), axis);
            gap = !((rangeA.max >= rangeB.min && rangeB.max >= rangeA.min));// || (mesh1Range.max < mesh2Range.min && mesh2Range.max < mesh1Range.min));
            if (gap) {
                collisionInfo.colliding = false;
                return false;
            }

            //Compare and cache minimum projection distance and axis for later use if needed for collision resolution.
            float potentialMinOverlap = 0;
            if (rangeA.max > rangeB.max) {
                potentialMinOverlap = rangeB.max - rangeA.min;
                axis *= -1.0;// Reverse push direction since object B is behind object A and we will always push A backwards and B forwards.
            } else {
                potentialMinOverlap = rangeA.max - rangeB.min;
            }

            if (i == 0)
            {
                collisionInfo.minOverlap = potentialMinOverlap;
                collisionInfo.minOverlapAxis = axis;
            }
            else if (potentialMinOverlap < collisionInfo.minOverlap)
            {
                collisionInfo.minOverlap = potentialMinOverlap;
                collisionInfo.minOverlapAxis = axis;
            }
        }
    }

    // Step 3: Must continue searching for possible 3D Edge-Edge collision
    if (!gap)
    {
        for (size_t i = 0; i < physObj1Normals.size(); i++)
        {
            Vec3 nA = physObj1Normals[i];
            for (size_t j = 0; j < physObj2Normals.size(); j++)
            {
                Vec3 nB = physObj2Normals[j];
                
                //Make sure normals are not the same before using them to calculate the cross product (otherwise the axis would be <0, 0, 0>).
                float dot = DotProduct(nA, nB);
                bool sameAxis = dot >= 1.0 || dot <= -1.0;
                if (sameAxis)
                {
                    if ((j + 1) >= physObj2Normals.size()) {
                        nB = physObj2Normals[j - 1];
                    }
                    else {
                        nB = physObj2Normals[j + 1];
                    }
                }
                
                // Search for possible 3D Edge-Edge collision
                Vec3 axis = CrossProduct(nA, nB);
                Range rangeA = ProjectVertsOntoAxis(physObj1Verts.data(), physObj1Verts.size(), axis);
                Range rangeB = ProjectVertsOntoAxis(physObj2Verts.data(), physObj2Verts.size(), axis);
                gap = !((rangeA.max >= rangeB.min && rangeB.max >= rangeA.min));// || (mesh1Range.max < mesh2Range.min && mesh2Range.max < mesh1Range.min));
                if (gap) {
                    collisionInfo.colliding = false;
                    return false;
                }
               /* To-Do...
                //Compare and cache minimum projection distance and axis for later use if needed for collision resolution.
                float potentialMinOverlap = 0;
                if (rangeA.max > rangeB.max) {
                    potentialMinOverlap = rangeB.max - rangeA.min;
                    axis *= -1.0;// Reverse push direction since object B is behind object A and we will always push A backwards and B forwards.
                } 
                else {
                    potentialMinOverlap = rangeA.max - rangeB.min;
                }

                if (potentialMinOverlap < collisionInfo.minOverlap)
                {
                    collisionInfo.minOverlap = potentialMinOverlap;
                    collisionInfo.minOverlapAxis = axis;
                }*/
            }
        }
    }

    collisionInfo.colliding = !gap;

    if (collisionInfo.colliding && resolve)
    {
        Vec3 offset = collisionInfo.minOverlapAxis * collisionInfo.minOverlap;

        bool neitherStatic = !physObj1.isStatic && !physObj2.isStatic;
        if (neitherStatic)
        {
            offset *= 0.5;
            physObj1.root->position -= (offset*1.01);
            physObj2.root->position += (offset * 1.01);
        }
        //Only one is movable at this stage
        else if (physObj1.isStatic) {
            physObj2.root->position += offset;
        }
        else {
            physObj1.root->position -= offset;
        }
    }

    return collisionInfo.colliding;
}

void DetectCollisions()
{
    // How nested loop algorithm works: 
    // Gets colliders A, B, C, D, E...
    // Compare A:B, A:C, A:D, A:E
    // Compare B:C, B:D, B:E
    // Compare C:D, C:E
    // Compare D:E
    for (size_t i = 0; i < BoxCollider::count; i++)
    {
        // exit if this is the last Collider
        if ((i + 1) >= BoxCollider::count) {
            break;
        }
        
        // Current Collider
        BoxCollider* box1 = BoxCollider::objects[i];

        for (size_t j = i + 1; j < BoxCollider::count; j++)
        { 
            // Next Collider
            BoxCollider* box2 = BoxCollider::objects[j];
            
            if (box1->isStatic && box2->isStatic) {
                continue;
            }

            BoxCollisionInfo collisionInfo;
            bool resolveIfNotTrigger = !(box1->isTrigger || box2->isTrigger);
            if (OBBSATColliding(*box1, *box2, collisionInfo, resolveIfNotTrigger))
            {   
                if (Physics::dynamics)
                {   
                    if (box1->object->isKinematic || box2->object->isKinematic) {
                        continue;
                    }
                    /*
                    Although static objects themselves are not effected by momentum
                    transfers, their velocity variable may still be updating from new collisions.
                    Consequently, objects touching a static collider would be effected, so the
                    velocity is zeroed out to prevent this.
                    */
                    if (box1->isStatic) 
                    {
                        box1->object->velocity = Vec3::zero;
                    }
                    else if (box2->isStatic) 
                    {
                        box2->object->velocity = Vec3::zero;
                    }

                    CalculateCollision(
                        collisionInfo.minOverlapAxis, 
                        box1->object->mass, 
                        box2->object->mass, 
                        box1->object->velocity, 
                        box2->object->velocity, 
                        1.0
                    );
                }
            }
        }
    }

    for (size_t i = 0; i < SphereCollider::count; i++)
    {
        // Current Collider
        SphereCollider* sphere1 = SphereCollider::objects[i];

        // SPHERE-SPHERE COLLISIONS
        for (size_t j = i + 1; j < SphereCollider::count; j++)
        {
            // Next Collider
            SphereCollider* sphere2 = SphereCollider::objects[j];

            if (sphere1->isStatic && sphere2->isStatic) {
                continue;
            }

            SphereCollisionInfo collisionInfo;
            bool resolveIfNotTrigger = !(sphere1->isTrigger || sphere2->isTrigger);
            if (SpheresColliding(*sphere1, *sphere2, collisionInfo, resolveIfNotTrigger))
            {
                if (Physics::dynamics)
                {
                    if (sphere1->object->isKinematic || sphere2->object->isKinematic) {
                        continue;
                    }
                    /*
                    Although static objects themselves are not effected by momentum
                    transfers, their velocity variable may still be updating from new collisions.
                    Consequently, objects touching a static collider would be effected, so the
                    velocity is zeroed out to prevent this.
                    */
                    if (sphere1->isStatic)
                    {
                        CalculateStaticCollision(collisionInfo.lineOfImpact, sphere2->object->velocity, sphere2->coefficientRestitution);
                    }
                    else if (sphere2->isStatic)
                    {
                        CalculateStaticCollision(collisionInfo.lineOfImpact, sphere1->object->velocity, sphere1->coefficientRestitution);
                    }
                    else
                    {
                        CalculateCollision(
                            collisionInfo.lineOfImpact,
                            sphere1->object->mass,
                            sphere2->object->mass,
                            sphere1->object->velocity,
                            sphere2->object->velocity,
                            1.0
                        );
                    }
                }
            }
        }

        // SPHERE-PLANE COLLISIONS
        for (size_t ii = 0; ii < PlaneCollider::count; ii++)
        {
            // Next Collider
            PlaneCollider* plane = PlaneCollider::objects[ii];

            if (sphere1->isStatic && plane->isStatic) {
                continue;
            }

            SphereCollisionInfo collisionInfo;
            bool resolveIfNotTrigger = !(sphere1->isTrigger || plane->isTrigger);
            if (SpherePlaneColliding(*sphere1, *plane, collisionInfo, resolveIfNotTrigger))
            {
                if (Physics::dynamics)
                {
                    if (sphere1->object->isKinematic || plane->object->isKinematic) {
                        continue;
                    }
                    if (sphere1->isStatic)
                    {
                        //...
                    }
                    else if (plane->isStatic)
                    {
                        CalculateStaticCollision(collisionInfo.lineOfImpact, sphere1->object->velocity, sphere1->coefficientRestitution);                        
                    }
                    else 
                    {
                        CalculateCollision(
                            collisionInfo.lineOfImpact,
                            sphere1->object->mass,
                            plane->object->mass,
                            sphere1->object->velocity,
                            plane->object->velocity,
                            1.0
                        );
                    }
                }
            }
        }
    }
}

template <typename T>
struct RaycastInfo
{
    T* objectHit = NULL;
    Vec3 contactPoint = Vec3::zero;

    RaycastInfo() {};
    RaycastInfo(T* obj, Vec3 contactPoint)
    {
        objectHit = obj;
        this->contactPoint = contactPoint;
    }
};

template <typename T>
bool Raycast(Vec3 from, Vec3 to, RaycastInfo<T>& raycastInfo)
{
    Vec3 rayDir = (to - from).Normalized();
    float closestSqrDistHit = (to - from).SqrMagnitude();
    float random = Clamp(-1.0, 1.0, rand());
    
    Vec3 rayZ = rayDir * -1.0;
    Vec3 rayX = CrossProduct(rayZ, Vec3(random, random, random));
    Vec3 rayY = CrossProduct(rayZ, rayX);
    rayX.Normalize();
    rayY.Normalize();
    Transform ray = Transform();
    float rotationMatrix[3][3] = {
        { rayX.x, rayY.x, rayZ.x },
        { rayX.y, rayY.y, rayZ.y },
        { rayX.z, rayY.z, rayZ.z }
    };
    ray.rotation = rotationMatrix;
    ray.position = from;

    for (size_t i = 0; i < ManagedObjectPool<T>::objects.size(); i++)
    {
        auto triangles = ManagedObjectPool<T>::objects[i]->MapVertsToTriangles();
        for (size_t j = 0; j < triangles->size(); j++)
        {
            Triangle worldSpaceTri = (*triangles)[j];
            for (size_t k = 0; k < 3; k++) {
                worldSpaceTri.verts[k] = ManagedObjectPool<T>::objects[i]->TRS() * worldSpaceTri.verts[k];
            }
            //------------------Ray casting (World & Ray Space)--------------------------
            Vec3 pointOfIntersection;
            if (LinePlaneIntersecting(from, to, worldSpaceTri, &pointOfIntersection))
            {
                Line::AddWorldLine(Line(from, to, Color::red));
                
                Matrix4x4 worldToRaySpaceMatrix = ray.TRInverse();
                Vec4 pointOfIntersection_v = worldToRaySpaceMatrix * pointOfIntersection;
                Triangle viewSpaceTri = (*triangles)[j];
                for (size_t k = 0; k < 3; k++) {
                    viewSpaceTri.verts[k] = worldToRaySpaceMatrix * worldSpaceTri.verts[k];
                }

                if (PointInsideTriangle(pointOfIntersection_v, viewSpaceTri.verts))
                {
                    // ---------- Debugging -----------
                    
                    Point::AddWorldPoint(Point(pointOfIntersection, Color::red, 5));
                    /*
                    // Reflect
                    Vec3 n = worldSpaceTri.Normal();
                    Vec3 v = (pointOfIntersection - from);
                    Vec3 reflection = Reflect(v, n);
                    Line::AddLine(Line(pointOfIntersection_p, (Vec3)(worldToViewToProjectedMatrix * (pointOfIntersection + reflection)), Color::red));

                    // Project
                    Vec3 vecPlane = ProjectOnPlane(v, n);
                    Line::AddLine(Line(pointOfIntersection_p, (Vec3)(worldToViewToProjectedMatrix * (pointOfIntersection + vecPlane)), Color::black));
                    */
               
                    float sqrDist = (pointOfIntersection - from).SqrMagnitude();
                    if (sqrDist <= closestSqrDistHit)
                    {
                        closestSqrDistHit = sqrDist;
                        raycastInfo.objectHit = ManagedObjectPool<T>::objects[i];
                        raycastInfo.contactPoint = pointOfIntersection;
                    }
                }
            }
        }
    }

    return raycastInfo.objectHit != NULL;
}
extern Transform* grabbing;
extern Vec3 grabOffset;
static void Physics()
{
    Camera* cam;
    if (CameraSettings::outsiderViewPerspective)
    {
        cam = Camera::projector;
    }
    else {
        cam = Camera::main;
    }
    //---------- Physics Update ------------
    if (isKinematic) 
    {
        cam->position += moveDir * accel * deltaTime;
    }
    else 
    {
        velocity += moveDir * accel * deltaTime;
        if (Physics::gravity) {
            velocity += gravity * deltaTime;
        }
        else if (dampenersActive) {
            velocity += velocity * decel * deltaTime;
        }
        if (velocity.SqrMagnitude() < 0.0001) {
            velocity = Vec3::zero;
        }
        cam->position += velocity * deltaTime;
    }

    if (planet) {
        float planetRotationSpeed = (1.5 * PI / 180) * deltaTime;
        planet->rotation = Matrix3x3::RotX(-planetRotationSpeed) * Matrix3x3::RotY(planetRotationSpeed + 0.000001) * planet->rotation;// MatrixMultiply(YPR(angle * ((screenWidth / 2)), angle * -((screenWidth / 2)), 0), Mesh.meshes[1].rotation);
    }

    if (spaceShip)
    {
        float shipRotationSpeed = (10 * PI / 180) * deltaTime;
        spaceShip->rotation = Matrix3x3::RotY(-shipRotationSpeed) * spaceShip->rotation;// MatrixMultiply(YPR(angle * ((screenWidth / 2)), angle * -((screenWidth / 2)), 0), Mesh.meshes[1].rotation);
        spaceShip->position += spaceShip->Forward() * 20 * deltaTime;
    }
    if (spaceShip2)
    {
        float shipRotationSpeed = (5 * PI / 180) * deltaTime;
        spaceShip2->rotation = Matrix3x3::RotZ(shipRotationSpeed) * Matrix3x3::RotY(-shipRotationSpeed) * spaceShip2->rotation;// *spaceShip2->rotation;// MatrixMultiply(YPR(angle * ((screenWidth / 2)), angle * -((screenWidth / 2)), 0), Mesh.meshes[1].rotation);
        spaceShip2->position += spaceShip2->Forward() * 10 * deltaTime;
    }
    if (spaceShip3)
    {
        float shipRotationSpeed = (20*PI / 180) * deltaTime;
        spaceShip3->rotation = Matrix3x3::RotY(shipRotationSpeed) * Matrix3x3::RotZ(shipRotationSpeed) * spaceShip3->rotation;// *spaceShip2->rotation;// MatrixMultiply(YPR(angle * ((screenWidth / 2)), angle * -((screenWidth / 2)), 0), Mesh.meshes[1].rotation);
        spaceShip3->position += spaceShip3->Forward() * 15 * deltaTime;

    }

    if (Physics::dynamics)
    {
        for (size_t i = 0; i < ManagedObjectPool<PhysicsObject>::count; i++)
        {
            PhysicsObject* obj = ManagedObjectPool<PhysicsObject>::objects[i];
            if (!obj->collider->isStatic && !obj->isKinematic)
            {
                if (Physics::gravity) {
                    obj->velocity += gravity * deltaTime;
                }
                obj->position += obj->velocity * deltaTime;
            }
        }
    }

    if (Physics::collisionDetection)
    {
        DetectCollisions();
    }
    
    if (Physics::raycasting)
    {
        RaycastInfo<Mesh> info;
        if (Raycast<Mesh>(Camera::cameras[1]->position, Camera::cameras[1]->position + Camera::cameras[1]->Forward() * 50, info))
        {
            cout << "RAYCAST HIT" << '\n';
            Line::AddWorldLine(Line(Camera::cameras[1]->position, Camera::cameras[1]->position + Camera::cameras[1]->Forward() * 50, Color::green, 3));
            Point::AddWorldPoint(Point(info.contactPoint, Color::green, 10));
            info.objectHit->SetColor(&Color::blue);
        }
        RaycastInfo<Mesh> info2;
        if (Raycast<Mesh>(Camera::cameras[2]->position, Camera::cameras[2]->position + Camera::cameras[2]->Forward() * 50, info2))
        {
            cout << "RAYCAST HIT" << '\n';
            Line::AddWorldLine(Line(Camera::cameras[2]->position, Camera::cameras[2]->position + Camera::cameras[2]->Forward() * 50, Color::green, 3));
            Point::AddWorldPoint(Point(info2.contactPoint, Color::green, 10));
            info2.objectHit->SetColor(&Color::red);
        }
    }

    if (DEBUGGING) 
    { 
        std::cout << "--------PHYSICS-------" << endl;

        string onoff = Physics::collisionDetection ? "On" : "Off";
        std::cout << "Collisions: " << onoff << " (press P)" << endl;

        onoff = Physics::gravity ? "On" : "Off";
        std::cout << "Gravity: " << onoff << " (press G)" << endl;
        
        std::cout << "Sphere Colliders: " << SphereCollider::count << endl;
        std::cout << "Box Colliders: " << BoxCollider::count << endl;
        std::cout << "Plane Colliders: " << PlaneCollider::count << endl;
        std::cout << "Colliders: " << PhysicsObject::count << endl;
        
        std::cout << "--------PLAYER-------" << endl;

        onoff = isKinematic ? "On" : "Off";
        std::cout << "Kinematic: " << onoff << " (press X)" << endl;

        onoff = dampenersActive ? "On" : "Off";
        std::cout << "Inertial Dampeners: " << onoff << " (press Z)" << endl;

        std::cout << "Position: (" << Camera::main->position.x << ", " << Camera::main->position.y << ", " << Camera::main->position.z << ")" << endl;
        std::cout << "Velocity: <" << velocity.x << ", " << velocity.y << ", " << velocity.z << ">" << endl;
    }
}
#endif