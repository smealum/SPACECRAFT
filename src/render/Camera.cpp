#include "render/Camera.h"
#include "Application.h"
#include "utils/maths.h"
#include "render/CameraManager.h"

using namespace glm;

Camera::Camera(float znear, float zfar):
	view(1.f),
    proj(perspective(DEG2RAD(90.f), Application::getInstance().getWindowRatio(), znear, zfar)),
    cameraManager(NULL),
    znear(znear),
    zfar(zfar)
{

}

void Camera::updateCamera(ShaderProgram &prog)
{
	prog.setUniform("view", view);
    prog.setUniform("proj", proj);
    prog.setUniform("znear", znear);
	prog.setUniform("zfar", zfar);
}

glm::vec3 Camera::getPosition(void)
{
	return glm::vec3(glm::inverse(view)[3]);
}

void Camera::updateFrustum(void)
{
	final=proj*view;

    vec4 rowX = row(final, 0);
    vec4 rowY = row(final, 1);
    vec4 rowZ = row(final, 2);
    vec4 rowW = row(final, 3);

    frustumPlane[0] = normalize(rowW + rowX);
    frustumPlane[1] = normalize(rowW - rowX);
    frustumPlane[2] = normalize(rowW + rowY);
    frustumPlane[3] = normalize(rowW - rowY);
    frustumPlane[4] = normalize(rowW + rowZ);
    frustumPlane[5] = normalize(rowW - rowZ);
}

bool Camera::isPointInFrustum(vec3 p)
{
    for(int i=0;i<6;i++)if(dot(vec4(p,1.0f),frustumPlane[i])<0.0f)return false;
    return true;
}

vec3 box[]={vec3(0.f,0.f,0.f),
            vec3(1.f,0.f,0.f),
            vec3(0.f,1.f,0.f),
            vec3(0.f,0.f,1.f),
            vec3(1.f,1.f,0.f),
            vec3(1.f,0.f,1.f),
            vec3(0.f,1.f,1.f),
            vec3(1.f,1.f,1.f)};

//on cherche s'il y a un plan tel que tous les points de la boite sont dehors
//on aura des faux positifs, mais peu (d'autant que notre far est très loin),
//et "Assarsson and Moller report that they found no observable penalty in the rendering when skipping further tests"
bool Camera::isBoxInFrustum(vec3 o, vec3 v1, vec3 v2, vec3 v3)
{
    mat3 bm(v1,v2,v3);
    for(int i=0;i<6;i++)
    {
        int in=0, out=0;
        for(int j=0;j<8 && (!in || !out);j++)
        {
            const vec3 p=o+bm*box[j];
            if(dot(vec4(p,1.0f),frustumPlane[i])<0.0f)out++;
            else in++;
        }
        if(!in)return false;
    }
	return true;
}

void Camera::setCameraManager(CameraManager* c)
{
    cameraManager=c;
}
    
void Camera::update()
{
    if (cameraManager)
    {
        cameraManager->update(*this);
    }
}
