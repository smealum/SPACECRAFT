#include "SpaceObjectTrajectory.h"

#define PI 3.14159265f

//EllipticalTrajectory stuff
EllipticalTrajectory::EllipticalTrajectory(glm::vec3 center, glm::mat3 caracteristic, float period):
	isFinal(true),
	center(center),
	caracteristic(caracteristic),
	period(period)
{

}

EllipticalTrajectory::EllipticalTrajectory(SpaceObjectTrajectory& e, glm::mat3 caracteristic, float period):
	isFinal(false),
	superTrajectory(&e),
	caracteristic(caracteristic),
	period(period)
{

}

glm::vec3 EllipticalTrajectory::getPosition(float time)
{
	return caracteristic*glm::vec3(cos(time*(2*PI)/period),0.0f,sin(time*(2*PI)/period))
		+ (isFinal ? center : superTrajectory->getPosition(time));
}
