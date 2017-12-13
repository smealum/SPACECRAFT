#ifndef PLANETGENERATOREARTH_TDQAA38S
#define PLANETGENERATOREARTH_TDQAA38S

#include "PlanetGenerator.h"
#include "noise/PlanetNoiseGenerator.h"
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

		// Donne la température en fonction de la position
		// Prend en compte
		// 		-l'axe de rotation de la planete
		//		-perturbation local par un bruit.
		// 		-la distance de la planete par rapport au soleil (TODO)
		//		-valeur intrinsèque de la planete (composition) (TODO)
		// Le résultat est à valeur dans [-1,1]
		// La position est une position dans le référentiel de la planète.
		float getTemperature(const glm::vec3& pos);

		// Donne l'humidité en fonction de la position
		// Prend en compte:
		//		-perturbation local par un bruit
		//		-valeur intrinsèque de la planete (composition) (TODO)
		// Le résultat est à valeur dans [-1,1]
		// La position est une position dans le référentiel de la planète.
		float getHumidity(const glm::vec3& pos);
};

#endif /* end of include guard: PLANETGENERATOREARTH_TDQAA38S */
