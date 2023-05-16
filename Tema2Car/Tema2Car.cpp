#include "lab_m1/Tema2Car/Tema2Car.h"

#include <vector>
#include <string>
#include <iostream>

#include "lab_m1/Tema2Car/transform3D.h"

using namespace std;
using namespace m1;


/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */


Tema2Car::Tema2Car()
{
}


Tema2Car::~Tema2Car()
{
}

void Tema2Car::Init()
{
    polygonMode = GL_FILL;

    Mesh* mesh = new Mesh("box");
    mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "plane50.obj");
    meshes[mesh->GetMeshID()] = mesh;

    // Initialize tx, ty and tz (the translation steps)
    translateX = -1;
    translateZ = 1;


    viewportX = 50;
    viewportY = 50;
    viewportWidth = 320;
    viewportHeight = 180;

    translateAngle = 0; 

    InitPista();
    InitTeren();
    CreateCopac();
    FilterCopaci();
    InitMasina();

    // Camera :
    camera = new implemented::CarCamera();
    camera->Set(glm::vec3(-1.7, 0.37, 1), glm::vec3(-1, 0, 1), glm::vec3(0, 1, 0));
    //camera->RotateThirdPerson_OY(RADIANS(180));
    projectionMatrix = glm::perspective(RADIANS(60), window->props.aspectRatio, 0.01f, 200.0f);

    // Obstacol 1 :
    idx1 = 0;
    time1 = 0;
    x1 = verticesCopaci[0].position.x;
    z1 = verticesCopaci[0].position.z;
    leftPoint1 = verticesCopaci[0].position;
    rightPoint1 = verticesCopaci[1].position;
    distance11 = 0;
    distance12 = sqrt((rightPoint1.x - leftPoint1.x) * (rightPoint1.x - leftPoint1.x) + (rightPoint1.z - leftPoint1.z) * (rightPoint1.z - leftPoint1.z));
    collision1 = false;

    // Obstacol 2 :
    idx2 = verticesCopaci.size()/2;
    time2 = 0;
    x2 = verticesCopaci[verticesCopaci.size()/2].position.x;
    z2 = verticesCopaci[verticesCopaci.size()/2].position.z;
    leftPoint2 = verticesCopaci[verticesCopaci.size()/2].position;
    rightPoint2 = verticesCopaci[verticesCopaci.size()/2 + 1].position;
    distance21 = 0;
    distance22 = sqrt((rightPoint1.x - leftPoint1.x) * (rightPoint1.x - leftPoint1.x) + (rightPoint1.z - leftPoint1.z) * (rightPoint1.z - leftPoint1.z));
    collision2 = false;

    // Initializare Shader :

    Shader* shader = new Shader("LabShader");
    shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2Car", "shaders", "VertexShader.glsl"), GL_VERTEX_SHADER);
    shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2Car", "shaders", "FragmentShader.glsl"), GL_FRAGMENT_SHADER);
    shader->CreateAndLink();
    shaders[shader->GetName()] = shader;
}


void Tema2Car::FrameStart()
{
    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(0.55, 0.82, 0.9, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();
    glViewport(0, 0, resolution.x, resolution.y);
    
}


void Tema2Car::Update(float deltaTimeSeconds)
{
    glLineWidth(3);
    glPointSize(5);
    glPolygonMode(GL_FRONT_AND_BACK, polygonMode);

    // --------------- Randare Teren 
    RenderMesh(meshes["pista"], shaders["LabShader"], glm::vec3(0, 0, 0), glm::vec3(1.0F));

    //RenderMesh(meshes["teren2"], shaders["LabShader"], glm::vec3(0, -5, 0), glm::vec3(1.0F));
    RenderMesh(meshes["box"], shaders["LabShader"], glm::vec3(0, -0.1, 0), glm::vec3(1.0F), glm::vec3(0.49, 0.78, 0.31));

    // --------------- Randare Copaci
    
    RenderCopaci(0);

    // --------------- Randare Masina

    glm::mat4 translation = glm::translate(glm::mat4(1.0f), glm::vec3(translateX, 0.0f, translateZ));
    glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(translateAngle), glm::vec3(0.0f, 1.0f, 0.0f));
    modelMatrix = translation * rotation;

    RenderMesh(meshes["masina"], shaders["LabShader"], modelMatrix);

    // --------------- Randare obstacol 1 :
    x1 += deltaTimeSeconds * cos((atan2((rightPoint1.z - leftPoint1.z), (rightPoint1.x - leftPoint1.x))));
    z1 += deltaTimeSeconds * sin((atan2((rightPoint1.z - leftPoint1.z), (rightPoint1.x - leftPoint1.x))));

    distance11 = sqrt((leftPoint1.x - x1) * (leftPoint1.x - x1) + (leftPoint1.z - z1) * (leftPoint1.z - z1));
    if (distance11 >= distance12)
    {
        idx1 += 1;
        if (idx1 >= verticesCopaci.size()) idx1 = 0;
        x1 = rightPoint1.x;
        z1 = rightPoint1.z;
        leftPoint1 = rightPoint1;
        rightPoint1 = verticesCopaci[idx1].position;
        distance12 = sqrt((rightPoint1.x - leftPoint1.x) * (rightPoint1.x - leftPoint1.x) + (rightPoint1.z - leftPoint1.z) * (rightPoint1.z - leftPoint1.z));
    }
    
    float distantaX = (translateX - x1) * (translateX - x1);
    float distantaZ = (translateZ - z1) * (translateZ - z1);

    float collision = sqrt(distantaX + distantaZ);
    if (collision < 0.3) collision1 = true;
    if (collision1 == true)
    {
        time1 += deltaTimeSeconds;
        if (time1 > 2)
        {
            time1 = 0;
            collision1 = false;
        }
    }
    glm::mat4 translationObstacol1 = glm::translate(glm::mat4(1.0f), glm::vec3(x1, 0.0f, z1));
    glm::mat4 rotationObstacol1 = glm::rotate(glm::mat4(1.0f), - atan2((rightPoint1.z - leftPoint1.z), (rightPoint1.x - leftPoint1.x)), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 modelMatrixObstacol1 = translationObstacol1 * rotationObstacol1;
    RenderMesh(meshes["masina"], shaders["LabShader"], modelMatrixObstacol1);

    // --------------- Randare obstacol 2 :
    x2 += deltaTimeSeconds * cos((atan2((rightPoint2.z - leftPoint2.z), (rightPoint2.x - leftPoint2.x))));
    z2 += deltaTimeSeconds * sin((atan2((rightPoint2.z - leftPoint2.z), (rightPoint2.x - leftPoint2.x))));

    distance21 = sqrt((leftPoint2.x - x2) * (leftPoint2.x - x2) + (leftPoint2.z - z2) * (leftPoint2.z - z2));
    if (distance21 >= distance22)
    {
        idx2 += 1;
        if (idx2 >= verticesCopaci.size()) idx2 = 0;
        x2 = rightPoint2.x;
        z2 = rightPoint2.z;
        leftPoint2 = rightPoint2;
        rightPoint2 = verticesCopaci[idx2].position;
        distance22 = sqrt((rightPoint2.x - leftPoint2.x) * (rightPoint2.x - leftPoint2.x) + (rightPoint2.z - leftPoint2.z) * (rightPoint2.z - leftPoint2.z));
    }

    float distantaX2 = (translateX - x2) * (translateX - x2);
    float distantaZ2 = (translateZ - z2) * (translateZ - z2);

    float collision_2 = sqrt(distantaX2 + distantaZ2);
    if (collision_2 < 0.3) collision2 = true;
    if (collision2 == true)
    {
        time2 += deltaTimeSeconds;
        if (time2 > 2)
        {
            time2 = 0;
            collision2 = false;
        }
    }
    glm::mat4 translationObstacol2 = glm::translate(glm::mat4(1.0f), glm::vec3(x2, 0.0f, z2));
    glm::mat4 rotationObstacol2 = glm::rotate(glm::mat4(1.0f), -atan2((rightPoint2.z - leftPoint2.z), (rightPoint2.x - leftPoint2.x)), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 modelMatrixObstacol2 = translationObstacol2 * rotationObstacol2;
    RenderMesh(meshes["masina"], shaders["LabShader"], modelMatrixObstacol2);

    // --------------- Randare Minimap 
    
    glLineWidth(3);
    glPointSize(5);
    glPolygonMode(GL_FRONT_AND_BACK, polygonMode);

    glViewport(viewportX, viewportY, viewportWidth, viewportHeight);

    RenderMeshOnMinimap(meshes["pista"], shaders["VertexColor"], glm::vec3(0, 0, 0), glm::vec3(1.0F));

    RenderMeshOnMinimap(meshes["teren"], shaders["VertexColor"], glm::vec3(0, 0, 0), glm::vec3(1.0F));
    
    RenderMeshOnMinimap(meshes["masina"], shaders["LabShader"], modelMatrixObstacol1);
    RenderMeshOnMinimap(meshes["masina"], shaders["LabShader"], modelMatrixObstacol2);

    RenderCopaci(1);

    RenderMeshOnMinimap(meshes["masina"], shaders["VertexColor"], modelMatrix);
    
}


void Tema2Car::FrameEnd()
{
    //DrawCoordinateSystem();
}


/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see `input_controller.h`.
 */


void Tema2Car::OnInputUpdate(float deltaTime, int mods)
{
    if (collision1 == false && collision2 == false)
    {
        if (window->KeyHold(GLFW_KEY_W))
        {
            glm::vec3 newPosition = glm::vec3(translateX - cos(RADIANS(180 - translateAngle)) * deltaTime, 0, translateZ - sin(RADIANS(180 - translateAngle)) * deltaTime);

            if (checkIfOnPista(newPosition))
            {
                translateX -= cos(RADIANS(180 - translateAngle)) * deltaTime;
                translateZ -= sin(RADIANS(180 - translateAngle)) * deltaTime;
                camera->MoveForward(deltaTime);
            }
        }

        if (window->KeyHold(GLFW_KEY_S))
        {
            glm::vec3 newPosition = glm::vec3(translateX + cos(RADIANS(180 - translateAngle)) * deltaTime, 0, translateZ + sin(RADIANS(180 - translateAngle)) * deltaTime);

            if (checkIfOnPista(newPosition))
            {
                translateX += cos(RADIANS(180 - translateAngle)) * deltaTime;
                translateZ += sin(RADIANS(180 - translateAngle)) * deltaTime;
                camera->MoveForward(-deltaTime);
            }
        }

        if (window->KeyHold(GLFW_KEY_A))
        {
            translateAngle += 50 * deltaTime;
            camera->RotateThirdPerson_OY(RADIANS(50 * deltaTime));
        }

        if (window->KeyHold(GLFW_KEY_D))
        {
            translateAngle -= 50 * deltaTime;
            camera->RotateThirdPerson_OY(RADIANS(-50 * deltaTime));
        }
    }
}


void Tema2Car::OnKeyPress(int key, int mods)
{
    // Add key press event
    if (key == GLFW_KEY_SPACE)
    {
        switch (polygonMode)
        {
        case GL_POINT:
            polygonMode = GL_FILL;
            break;
        case GL_LINE:
            polygonMode = GL_POINT;
            break;
        default:
            polygonMode = GL_LINE;
            break;
        }
    }
}


void Tema2Car::OnKeyRelease(int key, int mods)
{
    // Add key release event
}


void Tema2Car::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    // Add mouse move event
}


void Tema2Car::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button press event
}


void Tema2Car::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button release event
}


void Tema2Car::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}


void Tema2Car::OnWindowResize(int width, int height)
{
}

// ----------------------------------------
// Functions for mesh creations

void Tema2Car::InitPista()
{
    vertices =
    {
        VertexFormat(glm::vec3(-9.82,   0,  1.22), glm::vec3(0.19, 0.19, 0.19), glm::vec3(0.2, 0.8, 0.6)),
        VertexFormat(glm::vec3(-10.76,  0,  -1.8), glm::vec3(0.19, 0.19, 0.19), glm::vec3(0.2, 0.8, 0.6)),
        VertexFormat(glm::vec3(-9.36,   0,  -4.44), glm::vec3(0.19, 0.19, 0.19), glm::vec3(0.2, 0.8, 0.6)),
        VertexFormat(glm::vec3(-6.48,   0,  -6.12), glm::vec3(0.19, 0.19, 0.19), glm::vec3(0.2, 0.8, 0.6)),
        VertexFormat(glm::vec3(-2.6,    0,  -6.52), glm::vec3(0.19, 0.19, 0.19), glm::vec3(0.2, 0.8, 0.6)),
        VertexFormat(glm::vec3(1,       0,  -4), glm::vec3(0.19, 0.19, 0.19), glm::vec3(0.2, 0.8, 0.6)),
        VertexFormat(glm::vec3(3.26,    0,  -3.15), glm::vec3(0.19, 0.19, 0.19), glm::vec3(0.2, 0.8, 0.6)),
        VertexFormat(glm::vec3(5.76,    0,  -3.56), glm::vec3(0.19, 0.19, 0.19), glm::vec3(0.2, 0.8, 0.6)),
        VertexFormat(glm::vec3(8.16,    0,  -6), glm::vec3(0.19, 0.19, 0.19), glm::vec3(0.2, 0.8, 0.6)),
        VertexFormat(glm::vec3(10.86,   0,  -4.96), glm::vec3(0.19, 0.19, 0.19), glm::vec3(0.2, 0.8, 0.6)),
        VertexFormat(glm::vec3(11.79,   0,  0.32), glm::vec3(0.19, 0.19, 0.19), glm::vec3(0.2, 0.8, 0.6)),
        VertexFormat(glm::vec3(9.92,    0,  3.01), glm::vec3(0.19, 0.19, 0.19), glm::vec3(0.2, 0.8, 0.6)),
        VertexFormat(glm::vec3(7.12,    0,  4.88), glm::vec3(0.19, 0.19, 0.19), glm::vec3(0.2, 0.8, 0.6)),
        VertexFormat(glm::vec3(4.31,    0,  4.14), glm::vec3(0.19, 0.19, 0.19), glm::vec3(0.2, 0.8, 0.6)),
        VertexFormat(glm::vec3(2.08,    0,  2.36), glm::vec3(0.19, 0.19, 0.19), glm::vec3(0.2, 0.8, 0.6)),
        VertexFormat(glm::vec3(-0.83,   0,  0.94), glm::vec3(0.19, 0.19, 0.19), glm::vec3(0.2, 0.8, 0.6)),
        VertexFormat(glm::vec3(-3.13,   0,  1.92), glm::vec3(0.19, 0.19, 0.19), glm::vec3(0.2, 0.8, 0.6)),
        VertexFormat(glm::vec3(-4.61,   0,  3.05), glm::vec3(0.19, 0.19, 0.19), glm::vec3(0.2, 0.8, 0.6)),
        VertexFormat(glm::vec3(-7.69,   0,  3.63), glm::vec3(0.19, 0.19, 0.19), glm::vec3(0.2, 0.8, 0.6))
    };

    verticesCopaci = vertices;

    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < vertices.size() - 1; j = j+2)
        {
            glm::vec3 diferenta = (vertices[j + 1].position - vertices[j].position) / 2.F;
            glm::vec3 newPosition = vertices[j].position + diferenta;
            vertices.insert(vertices.begin() + j + 1, VertexFormat(newPosition, glm::vec3(0.19, 0.19, 0.19), glm::vec3(0.2, 0.8, 0.6)));
        }
    }

    vector<VertexFormat> upPoints, downPoints;
    for (int i = 0; i < vertices.size() - 1; i++)
    {
        glm::vec3 dir;
        dir = vertices[i + 1].position - vertices[i].position;

        glm::vec3 up = glm::vec3(0, 1, 0);

        glm::vec3 p = glm::normalize(glm::cross(dir, up));

        float distRosu = 0.5;
        float distAlbastru = 0.75;

        glm::vec3 R = vertices[i].position + distRosu * p;
        glm::vec3 A = vertices[i].position - distAlbastru * p;

        upPoints.push_back(VertexFormat(R, glm::vec3(0.19, 0.19, 0.19), glm::vec3(0.2, 0.8, 0.6)));
        downPoints.push_back(VertexFormat(A, glm::vec3(0.19, 0.19, 0.19), glm::vec3(0.2, 0.8, 0.6)));

    }
    
    for (int i = 0; i < verticesCopaci.size() - 1; i++)
    {
        glm::vec3 dir;
        dir = vertices[i + 1].position - verticesCopaci[i].position;

        glm::vec3 up = glm::vec3(0, 1, 0);

        glm::vec3 p = glm::normalize(glm::cross(dir, up));

        float distCopacRosu = 1.6;
        float distCopacAlbastru = 1.6;

        glm::vec3 copacR = verticesCopaci[i].position + distCopacRosu * p;
        glm::vec3 copacA = verticesCopaci[i].position - distCopacAlbastru * p;

        pozitiiCopaci.push_back(VertexFormat(copacA, glm::vec3(0.19, 0.19, 0.19), glm::vec3(0.2, 0.8, 0.6)));
        pozitiiCopaci.push_back(VertexFormat(copacR, glm::vec3(0.19, 0.19, 0.19), glm::vec3(0.2, 0.8, 0.6)));
    }

    glm::vec3 dir;
    dir = vertices[0].position - vertices[vertices.size() - 1].position;

    glm::vec3 up = glm::vec3(0, 1, 0);

    glm::vec3 p = glm::normalize(glm::cross(dir, up));

    float distRosu = 0.5;
    float distAlbastru = 0.75;

    glm::vec3 R = vertices[vertices.size() - 1].position + distRosu * p;
    glm::vec3 A = vertices[vertices.size() - 1].position - distAlbastru * p;

    upPoints.push_back(VertexFormat(R, glm::vec3(0.19, 0.19, 0.19), glm::vec3(0.2, 0.8, 0.6)));
    downPoints.push_back(VertexFormat(A, glm::vec3(0.19, 0.19, 0.19), glm::vec3(0.2, 0.8, 0.6)));

    float distCopacRosu = 1.35;
    float distCopacAlbastru = 1.35;

    glm::vec3 copacR = vertices[vertices.size() - 1].position + distCopacRosu * p;
    glm::vec3 copacA = vertices[vertices.size() - 1].position - distCopacAlbastru * p;
    copacR.y = -5;
    copacA.y = -5;

    pozitiiCopaci.push_back(VertexFormat(copacA, glm::vec3(0.19, 0.19, 0.19), glm::vec3(0.2, 0.8, 0.6)));
    pozitiiCopaci.push_back(VertexFormat(copacR, glm::vec3(0.19, 0.19, 0.19), glm::vec3(0.2, 0.8, 0.6)));

    //vector<VertexFormat> verticesPista;
    //vector<unsigned int> indicesPista;

    verticesPista.push_back(upPoints[0]);
    verticesPista.push_back(downPoints[0]);

    int indice = 3;
    for (int i = 1; i < upPoints.size(); i++)
    {
        verticesPista.push_back(upPoints[i]);
        verticesPista.push_back(downPoints[i]);

        indicesPista.push_back(indice);
        indicesPista.push_back(indice - 2);
        indicesPista.push_back(indice - 3);
        indicesPista.push_back(indice);
        indicesPista.push_back(indice - 3);
        indicesPista.push_back(indice - 1);

        indice += 2;
    }

    indicesPista.push_back(1);
    indicesPista.push_back(vertices.size() * 2 - 1);
    indicesPista.push_back(vertices.size() * 2 - 2);
    indicesPista.push_back(1);
    indicesPista.push_back(vertices.size() * 2 - 2);
    indicesPista.push_back(0);
    CreateMesh("pista", verticesPista, indicesPista);
}

void Tema2Car::InitTeren()
{
    vector<VertexFormat> vertices
    {
        VertexFormat(glm::vec3(-30, -0.1, -30), glm::vec3(0.49, 0.78, 0.31), glm::vec3(0.12, 0.84, 0.37)),
        VertexFormat(glm::vec3(-30, -0.1,  30), glm::vec3(0.49, 0.78, 0.31), glm::vec3(0.12, 0.84, 0.37)),
        VertexFormat(glm::vec3(30, -0.1,  30), glm::vec3(0.49, 0.78, 0.31), glm::vec3(0.12, 0.84, 0.37)),
        VertexFormat(glm::vec3(30 , -0.1,  -30), glm::vec3(0.49, 0.78, 0.31), glm::vec3(0.12, 0.84, 0.37))
    };

    vector<unsigned int> indices =
    {
       3, 0, 1,
       3, 1, 2
    };

    CreateMesh("teren", vertices, indices);
}

void Tema2Car::InitMasina()
{
    vector<VertexFormat> vertices
    {
        VertexFormat(glm::vec3(7*0.07, 0,  3.5 * 0.07), glm::vec3(0, 1, 1), glm::vec3(0.2, 0.8, 0.6)),
        VertexFormat(glm::vec3(0, 0,  3.5 * 0.07), glm::vec3(0, 1, 1), glm::vec3(0.2, 0.8, 0.6)),
        VertexFormat(glm::vec3(7 * 0.07, 3 * 0.07,  3.5 * 0.07), glm::vec3(0, 1, 1), glm::vec3(0.2, 0.8, 0.6)),
        VertexFormat(glm::vec3(0, 3 * 0.07,  3.5 * 0.07), glm::vec3(0, 1, 1), glm::vec3(0.2, 0.8, 0.6)),
        VertexFormat(glm::vec3(7 * 0.07, 0,  0), glm::vec3(0, 1, 1), glm::vec3(0.2, 0.8, 0.6)),
        VertexFormat(glm::vec3(0, 0,  0), glm::vec3(0, 1, 1), glm::vec3(0.2, 0.8, 0.6)),
        VertexFormat(glm::vec3(7 * 0.07, 3 * 0.07, 0), glm::vec3(0, 1, 1), glm::vec3(0.2, 0.8, 0.6)),
        VertexFormat(glm::vec3(0 * 0.07, 3 * 0.07, 0), glm::vec3(0, 1, 1), glm::vec3(0.2, 0.8, 0.6))
    };
    
    vector<unsigned int> indices =
    {
        0, 1, 2,
        1, 3, 2,
        2, 3, 7,
        2, 7, 6,
        1, 7, 3,
        1, 5, 7,
        6, 7, 4,
        7, 5, 4,
        0, 4, 1,
        1, 4, 5,
        2, 6, 4,
        0, 2, 4
    };

    CreateMesh("masina", vertices, indices);
}

void Tema2Car::CreateCopac()
{
    vector<VertexFormat> verticesTrunchi
    {
        VertexFormat(glm::vec3(0, 0,  2), glm::vec3(0.45, 0.36, 0.29), glm::vec3(0.2, 0.8, 0.6)),
        VertexFormat(glm::vec3(2, 0,  2), glm::vec3(0.45, 0.36, 0.29), glm::vec3(0.2, 0.8, 0.6)),
        VertexFormat(glm::vec3(0, 5,  2), glm::vec3(0.45, 0.36, 0.29), glm::vec3(0.2, 0.8, 0.6)),
        VertexFormat(glm::vec3(2, 5,  2), glm::vec3(0.45, 0.36, 0.29), glm::vec3(0.2, 0.8, 0.6)),
        VertexFormat(glm::vec3(0, 0,  0), glm::vec3(0.45, 0.36, 0.29), glm::vec3(0.2, 0.8, 0.6)),
        VertexFormat(glm::vec3(2, 0,  0), glm::vec3(0.45, 0.36, 0.29), glm::vec3(0.2, 0.8, 0.6)),
        VertexFormat(glm::vec3(0, 5, 0), glm::vec3(0.45, 0.36, 0.29), glm::vec3(0.2, 0.8, 0.6)),
        VertexFormat(glm::vec3(2, 5, 0), glm::vec3(0.45, 0.36, 0.29), glm::vec3(0.2, 0.8, 0.6))
    };

    vector<unsigned int> indicesTrunchi =
    {
        0, 1, 2,
        1, 3, 2,
        2, 3, 7,
        2, 7, 6,
        1, 7, 3,
        1, 5, 7,
        6, 7, 4,
        7, 5, 4,
        0, 4, 1,
        1, 4, 5,
        2, 6, 4,
        0, 2, 4
    };

    CreateMesh("trunchi", verticesTrunchi, indicesTrunchi);

    vector<VertexFormat> verticesCoroana
    {
        VertexFormat(glm::vec3(-2, 5,  4), glm::vec3(0.33, 0.48, 0.27), glm::vec3(0.2, 0.8, 0.6)),
        VertexFormat(glm::vec3(4, 5,  4), glm::vec3(0.33, 0.48, 0.27), glm::vec3(0.2, 0.8, 0.6)),
        VertexFormat(glm::vec3(-2, 9.5,  4), glm::vec3(0.33, 0.48, 0.27), glm::vec3(0.2, 0.8, 0.6)),
        VertexFormat(glm::vec3(4, 9.5,  4), glm::vec3(0.33, 0.48, 0.27), glm::vec3(0.2, 0.8, 0.6)),
        VertexFormat(glm::vec3(-2, 5,  -2), glm::vec3(0.33, 0.48, 0.27), glm::vec3(0.2, 0.8, 0.6)),
        VertexFormat(glm::vec3(4, 5,  -2), glm::vec3(0.33, 0.48, 0.27), glm::vec3(0.2, 0.8, 0.6)),
        VertexFormat(glm::vec3(-2, 9.5, -2), glm::vec3(0.33, 0.48, 0.27), glm::vec3(0.2, 0.8, 0.6)),
        VertexFormat(glm::vec3(4, 9.5, -2), glm::vec3(0.33, 0.48, 0.27), glm::vec3(0.2, 0.8, 0.6))
    };

    vector<unsigned int> indicesCoroana =
    {
        0, 1, 2,
        1, 3, 2,
        2, 3, 7,
        2, 7, 6,
        1, 7, 3,
        1, 5, 7,
        6, 7, 4,
        7, 5, 4,
        0, 4, 1,
        1, 4, 5,
        2, 6, 4,
        0, 2, 4
    };

    CreateMesh("coroana", verticesCoroana, indicesCoroana);
}

void Tema2Car::CreateMesh(const char* name, const std::vector<VertexFormat>& vertices, const std::vector<unsigned int>& indices)
{
    unsigned int VAO = 0;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    unsigned int VBO = 0;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * vertices.size(), &vertices[0], GL_STATIC_DRAW);
    unsigned int IBO = 0;

    glGenBuffers(1, &IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indices.size(), &indices[0], GL_STATIC_DRAW);
    

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), 0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(sizeof(glm::vec3)));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(2 * sizeof(glm::vec3)));

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(2 * sizeof(glm::vec3) + sizeof(glm::vec2)));


    if (GetOpenGLError() == GL_INVALID_OPERATION)
    {
        cout << "\t[NOTE] : For students : DON'T PANIC! This error should go away when completing the tasks." << std::endl;
        cout << "\t[NOTE] : For developers : This happens because OpenGL core spec >=3.1 forbids null VAOs." << std::endl;
    }

    meshes[name] = new Mesh(name);
    meshes[name]->InitFromBuffer(VAO, static_cast<unsigned int>(indices.size()));
}

void Tema2Car::FilterCopaci()
{
    for (int i = 0; i < pozitiiCopaci.size(); i++)
    {
        for (int j = 0; j < indicesPista.size(); j = j + 3)
        {
            if (PointInTriangle(pozitiiCopaci[i].position, 1.F * verticesPista[indicesPista[j]].position,
                1.F * verticesPista[indicesPista[j + 1]].position,
                1.F * verticesPista[indicesPista[j + 2]].position) == false)
            {
                pozitiiCopaciNoi.push_back(pozitiiCopaci[i]);
                break;
            }
        }
    }
}
void Tema2Car::RenderCopaci(int onMinimap)
{
    if (onMinimap == 0)
    {
        for (int i = 0; i < pozitiiCopaciNoi.size(); i++)
        {
            RenderMesh(meshes["trunchi"], shaders["LabShader"], glm::vec3(pozitiiCopaciNoi[i].position.x, -0.5, pozitiiCopaciNoi[i].position.z), glm::vec3(0.2F));
            RenderMesh(meshes["coroana"], shaders["LabShader"], glm::vec3(pozitiiCopaciNoi[i].position.x, -0.5, pozitiiCopaciNoi[i].position.z), glm::vec3(0.2F));
        }
    }
    else
    {
        for (int i = 0; i < pozitiiCopaciNoi.size(); i++)
        {
            RenderMeshOnMinimap(meshes["trunchi"], shaders["VertexColor"], pozitiiCopaciNoi[i].position, glm::vec3(0.2F));
            RenderMeshOnMinimap(meshes["coroana"], shaders["VertexColor"], pozitiiCopaciNoi[i].position, glm::vec3(0.2F));
        }
    }
}


void Tema2Car::RenderMesh(Mesh* mesh, Shader* shader, glm::vec3 position, glm::vec3 scale)
{
    if (!mesh || !shader || !shader->program)
        return;

    // Render an object using the specified shader and the specified position
    shader->Use();
    glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
    glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

    glm::mat4 model(1);
    model = glm::translate(model, position);
    model = glm::scale(model, scale);
    glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(model));
    
    int positionLocation = glGetUniformLocation(shader->program, "position1");
    glUniform3f(positionLocation, translateX, 0, translateZ);

    int isTeren = glGetUniformLocation(shader->program, "isTeren");
    glUniform1i(isTeren, 0);

    mesh->Render();
}

void Tema2Car::RenderMesh(Mesh* mesh, Shader* shader, glm::mat4 modelMatrix)
{
    if (!mesh || !shader || !shader->program)
        return;


    // Render an object using the specified shader and the specified position
    shader->Use();

    glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
    glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
    glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    int positionLocation = glGetUniformLocation(shader->program, "position1");
    glUniform3f(positionLocation, translateX, 0, translateZ);

    int isTeren = glGetUniformLocation(shader->program, "isTeren");
    glUniform1i(isTeren, 0);

    mesh->Render();
}

void Tema2Car::RenderMesh(Mesh* mesh, Shader* shader, glm::vec3 position, glm::vec3 scale, glm::vec3 color)
{
    if (!mesh || !shader || !shader->program)
        return;

    int object_color = glGetUniformLocation(shader->program, "object_color");
    glUniform3f(object_color, color.r, color.g, color.b);

    int isTeren = glGetUniformLocation(shader->program, "isTeren");
    glUniform1i(isTeren, 1);

    // Render an object using the specified shader and the specified position
    shader->Use();
    glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
    glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
    
    glm::mat4 model(1);
    model = glm::translate(model, position);
    model = glm::scale(model, scale);
    glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(model));

    int positionLocation = glGetUniformLocation(shader->program, "position1");
    glUniform3f(positionLocation, translateX, 0, translateZ);

    mesh->Render();
}

void Tema2Car::RenderMeshOnMinimap(Mesh* mesh, Shader* shader, glm::vec3 position, glm::vec3 scale)
{
    if (!mesh || !shader || !shader->program)
        return;

    // Render an object using the specified shader and the specified position
    shader->Use();

    // Generam view matrix pt privire de sus
    glm::vec3 eye(0.0f, 10.0f, 0.0f);
    glm::vec3 target(0.0f, 0.0f, 0.0f);
    glm::vec3 up(0.0f, 0.0f, -1.0f);
    glm::mat4 view = glm::lookAt(eye, target, up);

    glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(view));

    // Generam projection matrix pt privire de sus
    float left = -10.0f;
    float right = 10.0f;
    float bottom = -10.0f;
    float top = 10.0f;
    float near = 0.1f;
    float far = 100.0f;
    glm::mat4 projection = glm::ortho(left, right, bottom, top, near, far);

    glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projection));

    glm::mat4 model(1);
    model = glm::translate(model, position);
    model = glm::scale(model, scale);
    glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(model));
    mesh->Render();
}

void Tema2Car::RenderMeshOnMinimap(Mesh* mesh, Shader* shader, glm::mat4 modelMatrix)
{
    if (!mesh || !shader || !shader->program)
        return;

    // Render an object using the specified shader and the specified position
    shader->Use();

    // Generam view matrix pt privire de sus
    glm::vec3 eye(0.0f, 10.0f, 0.0f);
    glm::vec3 target(0.0f, 0.0f, 0.0f);
    glm::vec3 up(0.0f, 0.0f, -1.0f);
    glm::mat4 view = glm::lookAt(eye, target, up);

    glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(view));

    // Generam projection matrix pt privire de sus
    float left = -10.0f;
    float right = 10.0f;
    float bottom = -10.0f;
    float top = 10.0f;
    float near = 0.1f;
    float far = 100.0f;
    glm::mat4 projection = glm::ortho(left, right, bottom, top, near, far);

    glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projection));

    glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    mesh->Render();
}

bool Tema2Car::SameSide(glm::vec3 p1, glm::vec3 p2, glm::vec3 a, glm::vec3 b) {
    glm::vec3 cp1 = glm::cross(b - a, p1 - a);
    glm::vec3 cp2 = glm::cross(b - a, p2 - a);
    if (glm::dot(cp1, cp2) >= 0) {
        return true;
    }
    else {
        return false;
    }
}

bool Tema2Car::PointInTriangle(glm::vec3 p, glm::vec3 a, glm::vec3 b, glm::vec3 c) {
    if (SameSide(p, a, b, c) && SameSide(p, b, a, c) && SameSide(p, c, a, b)) {
        return true;
    }
    else {
        return false;
    }
}

bool Tema2Car::checkIfOnPista(glm::vec3 newPosition)
{
    bool onPista = false;
    for (int i = 0; i < indicesPista.size(); i = i + 3)
    {
        if (PointInTriangle(newPosition, 1.F * verticesPista[indicesPista[i]].position,
            1.F * verticesPista[indicesPista[i + 1]].position,
            1.F * verticesPista[indicesPista[i + 2]].position))
        {
            onPista = true;
            break;
        }
        else 
        {
            onPista = false;
        }
    }

    return onPista;
}