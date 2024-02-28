#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <math.h>
#include <Matrix.h>
#include <Graphics.h>
#include <Physics.h>
#include <Input.h>
using namespace std;
GLFWwindow* window;

// Checking if DEBUGGING true in other scripts before using cout also ensures readable slow incremental output.
bool DEBUGGING = false;
void Debug()
{
    //debugging
    DEBUGGING = false;
    static double coutTimer = 0;
    coutTimer += deltaTime;
    if (coutTimer > 0.25 && coutTimer < 0.25 + deltaTime)
    {
        DEBUGGING = true;
        coutTimer = 0;
    }
     
    if (DEBUGGING)
    {
        std::cout << "--------GRAPHICS-------" << endl;
        std::cout << "FPS:" << fps << std::endl;
        std::cout << "Frame Time:" << 1.0 / (double)fps << std::endl;
        std::cout << "Meshes:" << Mesh::count << std::endl;
        std::cout << "Triangles Drawn:" << Mesh::worldTriangleDrawCount << std::endl;
    }
}

Mesh* giantText;
PhysicsObject* planet;
Mesh* moon;
Mesh* spaceShip;
Mesh* spaceShip2;
Mesh* spaceShip3;
CubeMesh* parent;
CubeMesh* child;
CubeMesh* grandchild;
CubeMesh* greatGrandchild;
PhysicsObject* physicsObj;    

void Init(GLFWwindow* window)
{
    glfwSetCursorPosCallback(window, OnMouseMoveEvent);
    glfwSetScrollCallback(window, OnScrollEvent);
    glfwSetMouseButtonCallback(window, OnMouseButtonEvent);
    glfwSetKeyCallback(window, OnKeyPressEvent);
    glfwGetWindowSize(window, &screenWidth, &screenHeight);
    glfwSetCursorPos(window, screenWidth / 2.0, screenHeight / 2.0);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

    glLineWidth(2);
    glPointSize(2);

    Graphics::matrixMode = true;
    //GraphicSettings::debugAxes = true;

    planet = new PhysicsObject(500.0, Direction::forward * 1200, Matrix3x3::identity, LoadMeshFromOBJFile("Planet.obj"), new SphereCollider());
    planet->mass = 100000;

    moon = LoadMeshFromOBJFile("Moon.obj");
    //moon->position += Direction::forward * 500;
    moon->scale *= 0.07;
    moon->SetParent(planet, false);
    moon->position += (-Direction::forward + 1.1 *Direction::left);

    giantText = LoadMeshFromOBJFile("PescadoTextThickLime.obj");// "PescadoText.obj");//"Hello3DWorldText.obj");
    giantText->scale *= 2.5;
    giantText->position = Vec3(50, 25, -490);
   // textHelloWorld->color = &Color::green;
    
    spaceShip = LoadMeshFromOBJFile("SpaceShip_2.2.obj");
    spaceShip->position = Direction::left * 30 + Direction::forward * 10;

    spaceShip2 = LoadMeshFromOBJFile("SpaceShip_3.obj");
    spaceShip2->position = Direction::right * 40 + Direction::forward * 100;
    spaceShip2->rotation = Matrix3x3::RotY(PI);

    spaceShip3 = LoadMeshFromOBJFile("SpaceShip_5.obj");
    spaceShip3->position = Direction::right * 20 + Direction::up * 10;

    parent = new CubeMesh(1, Vec3(0, 10, 2), Vec3(0, 45, 0));
    child = new CubeMesh(2, Vec3(0, 0, 2), Vec3(0, 45, 0));
    grandchild = new CubeMesh(2, Vec3(0, 0, 2), Vec3(0, 45, 0));
    greatGrandchild = new CubeMesh(1.0/2, Vec3(0, 0, 2), Vec3(0, 45, 0));
    child->SetParent(parent, false);
    grandchild->SetParent(child, false);
    greatGrandchild->SetParent(grandchild, false);
    parent->SetColor(Color::red);
    child->SetColor(Color::orange);
    grandchild->SetColor(Color::yellow);
    greatGrandchild->SetColor(Color::green);
    //Mesh* guitar = LoadMeshFromOBJFile("Objects/Guitar.obj");
    //guitar->position += (Camera::main->Forward() * 10) + Camera::main->Right();
    //Mesh* chair = LoadMeshFromOBJFile("Objects/Chair.obj");
    //chair->position += (Camera::main->Forward() * 10) + Camera::main->Left();

    //Plane* plane = new Plane(1, Vec3(0, 0, 0), Vec3(0, 0, 0));

    for (size_t i = 1; i < Camera::cameras.size(); i++)//starts at 1 to avoid projector camera
    {
       Mesh* cameraMesh = LoadMeshFromOBJFile("Camera.obj");
       cameraMesh->SetParent(Camera::cameras[i], false);
       cameraMesh->position += -Direction::forward;
    }

    //physicsObj->collider->isStatic = true;
    
    for (int i = -10; i < 10; i++)
    {
        for (int j = -10; j < 10; j++)
        {
            PhysicsObject* block = new PhysicsObject(new CubeMesh(), new BoxCollider(true));
            block->scale *= 2;
            block->position = Direction::down*15 + Direction::left*i*10 + Direction::back * j*10;
        }
    }

    
    /*
    PhysicsObject* ground = new PhysicsObject(100, Direction::down * 15, Matrix3x3::identity, new PlaneMesh(), new PlaneCollider(Direction::up, true));
    PhysicsObject* leftWall = new PhysicsObject(100, Direction::left * 200, Matrix3x3::RotZ(ToRad(-90)), new PlaneMesh(), new PlaneCollider(Direction::up, true));
    PhysicsObject* rightWall = new PhysicsObject(100, Direction::right * 200, Matrix3x3::RotZ(ToRad(90)), new PlaneMesh(), new PlaneCollider(Direction::up, true));
    PhysicsObject* backWall = new PhysicsObject(100, Direction::forward * 200, Matrix3x3::RotX(ToRad(90)), new PlaneMesh(), new PlaneCollider(Direction::up, true));
    PhysicsObject* frontWall = new PhysicsObject(new PlaneMesh(100, Direction::back * 200, Vec3(ToRad(-90), 0, 0)), new PlaneCollider(Direction::up, true));
    */
    
    physicsObj = new PhysicsObject(LoadMeshFromOBJFile("Sphere.obj"), new SphereCollider());
    physicsObj->scale *= 2;
    //physicsObj->mesh->SetVisibility(false);
    physicsObj->collider->mesh->SetVisibility(true);
    //physicsObj->collider->isTrigger = true;

}

void Update()
{
    if (CameraSettings::displayReticle)
    {
        Point::AddPoint(Point(Vec3(), Color::orange, 5));
    }
}

int main(void)
{
    //GLFWwindow* window;
    
    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(screenWidth, screenHeight, "Pescado Engine", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    
    //glewInit();
    
    {
        Init(window);
    }

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        {
            Time();
            Input();
            Physics();
            Update();
            Draw();
            Debug();
        }

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
