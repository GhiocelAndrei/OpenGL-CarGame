#pragma once

#include "components/simple_scene.h"
#include "lab_m1/Tema2Car/lab_camera_Tema2Car.h"

namespace m1
{
    class Tema2Car : public gfxc::SimpleScene
    {
     public:
        Tema2Car();
        ~Tema2Car();

        void Init() override;

     private:
        void FrameStart() override;
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;

        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
        void OnWindowResize(int width, int height) override;

     protected:
        glm::mat4 modelMatrix;
        float translateX, translateY, translateZ;
        GLenum polygonMode;

        int viewportX, viewportY;
        int viewportWidth, viewportHeight;

        // ----------------------------------------
        // Functions for mesh creations
        void CreateMesh(const char* name, const std::vector<VertexFormat>& vertices, const std::vector<unsigned int>& indices);
        void InitPista();
        void InitTeren();
        void InitMasina();
        void CreateCopac();

        void FilterCopaci();
        void RenderCopaci(int onMinimap);
       
        void RenderMesh(Mesh* mesh, Shader* shader, glm::vec3 position, glm::vec3 scale);
        void RenderMesh(Mesh* mesh, Shader* shader, glm::mat4 modelMatrix);
        void RenderMesh(Mesh* mesh, Shader* shader, glm::vec3 position, glm::vec3 scale, glm::vec3 color);

        void RenderMeshOnMinimap(Mesh* mesh, Shader* shader, glm::vec3 position, glm::vec3 scale);
        void RenderMeshOnMinimap(Mesh* mesh, Shader* shader, glm::mat4 modelMatrix);

        bool checkIfOnPista(glm::vec3 newPosition);
        bool SameSide(glm::vec3 p1, glm::vec3 p2, glm::vec3 a, glm::vec3 b);
        bool PointInTriangle(glm::vec3 p, glm::vec3 a, glm::vec3 b, glm::vec3 c);

        // 0 si 7, 3.5 si 7 , 0 si 0, 3.5 si 0
        glm::vec3 pozitieRoata1 = glm::vec3(0, 0, 7);
        glm::vec3 pozitieRoata2 = glm::vec3(3.5, 0, 7);
        glm::vec3 pozitieRoata3 = glm::vec3(0, 0, 0);
        glm::vec3 pozitieRoata4 = glm::vec3(3.5, 0, 0);

        float translateAngle;
        std::vector<VertexFormat> pozitiiCopaci;
        std::vector<VertexFormat> pozitiiCopaciNoi;
        std::vector<VertexFormat> verticesPista;
        std::vector<unsigned int> indicesPista;

        std::vector<VertexFormat> vertices;
        std::vector<VertexFormat> verticesCopaci;

        // Camera :
        glm::mat4 projectionMatrix;
        implemented::CarCamera *camera;

        // Obstacol 1 :
        float distance11;
        float distance12;
        float time1;
        bool collision1;
        int idx1;
        glm::vec3 leftPoint1, rightPoint1;
        float x1, z1;

        // Obstacol 2 :
        float distance21;
        float distance22;
        float time2;
        bool collision2;
        int idx2;
        glm::vec3 leftPoint2, rightPoint2;
        float x2, z2;
    };
}   // namespace m1

