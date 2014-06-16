#ifndef PLANETGENERATOREARTH_TDQAA38S
#define PLANETGENERATOREARTH_TDQAA38S

#include "PlanetGenerator.h"
#include "noise/Tree.h"
#include <vector>


class PlanetGeneratorEarth : public PlanetGenerator
{
	public:
		PlanetGeneratorEarth(int nbThread);
		virtual ~PlanetGeneratorEarth();

		virtual void setPlanetInfo(PlanetInfo* p);
		virtual void generateWorldData(int threadId,chunkVal* data,
					int w, int h, int d, // array sizes (in chunks)
					int px, int py, int pz, // offset in world
					glm::vec3 origin, glm::vec3 v1, glm::vec3 v2); // toplevelCharacteristic

		virtual PlanetGeneratorResponse getCharacteristic(int threadId, const glm::vec3& pos);

		virtual float getElevation(int threadId, const glm::vec3 &coord);
	
	private:
		std::vector<PlanetNoiseGenerator*> generators;
		std::vector<Tree> treePool;

		void initGenerators();
		virtual float getTemperature(const glm::vec3& pos);
		virtual float getHumidity(const glm::vec3& pos);
};

#endif /* end of include guard: PLANETGENERATOREARTH_TDQAA38S */
