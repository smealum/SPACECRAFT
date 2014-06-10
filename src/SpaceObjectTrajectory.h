#ifndef SPACEOBJECTTRAJECTORY_H
#define SPACEOBJECTTRAJECTORY_H

#include "utils/glm.h"

class SpaceObjectTrajectory
{
	public:
		virtual glm::vec3 getPosition(float time)=0;
};

class EllipticalTrajectory : public SpaceObjectTrajectory
{
	public:
		EllipticalTrajectory(glm::vec3 center, glm::mat3 caracteristic, float period);
		EllipticalTrajectory(SpaceObjectTrajectory& e, glm::mat3 caracteristic, float period);

		virtual glm::vec3 getPosition(float time);

	private:
		bool isFinal;
        // TODO fix this
		//union
		//{
			glm::vec3 center;
			SpaceObjectTrajectory* superTrajectory;
		//};

		glm::mat3 caracteristic;
		float period;
};

#endif
