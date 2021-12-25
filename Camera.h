#ifndef CAMERA_H
#define CAMERA_H

#include <QWidget>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Camera
{
private:
    float CAMERA_DIST = 10.0f;
    glm::vec3 cameraPos, cameraFront, cameraUp;
    float fov, aspectRatio;

public:
    Camera(QWidget *window)
    {
        cameraPos = glm::vec3(0.0f, 0.0f, CAMERA_DIST);
        cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
        cameraFront = glm::vec3(0.0f, 0.0f, 0.0f);
        fov = 45.0f;
        aspectRatio = 1.0f * window->width() / window->height();
    }
    float getFov() { return fov; }
    void setFov(float f) { fov = f; }
    float getAspectRatio() { return aspectRatio; }
    void setAspectRatio(float asp) { aspectRatio = asp; }
    glm::vec3 getCameraPos() { return cameraPos; }
    void setCameraPos(glm::vec3 pos) { cameraPos = pos; }
    float getCameraDist() { return CAMERA_DIST; }
    void setCameraDist(float dist) { CAMERA_DIST = dist; }
    glm::mat4 getProjectionMatrix() { return glm::perspective(glm::radians(fov), aspectRatio, 0.1f, 100.0f); }
    glm::mat4 getViewMatrix() { return glm::lookAt(cameraPos, cameraFront, cameraUp); }
};

#endif // CAMERA_H
