#include "SpaceObjectTrajectory.h"

#define PI 3.14159265f

//EllipticalTrajectory stuff
EllipticalTrajectory::EllipticalTrajectory(glm::vec3 center, glm::mat3 caracteristic, float phase, float period):
	isFinal(true),
	center(center),
	caracteristic(caracteristic),
	phase(phase),
	period(period)
{

}

EllipticalTrajectory::EllipticalTrajectory(SpaceObjectTrajectory& e, glm::mat3 caracteristic, float phase, float period):
	isFinal(false),
	superTrajectory(&e),
	caracteristic(caracteristic),
	phase(phase),
	period(period)
{

}

glm::vec3 EllipticalTrajectory::getPosition(float time)
{
	return caracteristic*glm::vec3(cos(time*(2*PI)/period+phase),0.0f,sin(time*(2*PI)/period+phase))
		+ (isFinal ? center : superTrajectory->getPosition(time));
}
