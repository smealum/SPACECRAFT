#ifndef PLANETGENERATOR_PJW8VJTB
#define PLANETGENERATOR_PJW8VJTB

#include "world/BlockType.h"
#include "Chunk.h"


struct PlanetGeneratorResponse
{
	float elevation;	
	blockTypes::T tile;
};


class PlanetInfo;

class PlanetGenerator
{
	public:
		PlanetGenerator(int nbThread);
		virtual ~PlanetGenerator();
		void setPlanetInfo(PlanetInfo* p);

		// genere un miniworld
		virtual void generateWorldData(int threadId,chunkVal* data,
					int w, int h, int d, // array sizes (in chunks)
					int px, int py, int pz, // offset in world
					glm::vec3 origin, glm::vec3 v1, glm::vec3 v2) const; // toplevelCharacteristic

		// genere l'elevation,texture.
		virtual PlanetGeneratorResponse getCharacteristic(int threadId, const glm::vec3& pos) const ;

		// genere seulement l'elevation
		virtual float getElevation(int threadId, const glm::vec3& pos) const;

	protected:
		virtual void initGenerators();
		int nbThread;
		std::vector<PlanetNoiseGenerator*> generators;
		PlanetInfo* planetInfo;
		// Donne la température en fonction de la position
		// Prend en compte
		// 		-l'axe de rotation de la planete
		//		-perturbation local par un bruit.
		// 		-la distance de la planete par rapport au soleil (TODO)
		//		-valeur intrinsèque de la planete (composition) (TODO)
		// Le résultat est à valeur dans [-1,1]
		// La position est une position dans le référentiel de la planète.
		virtual float getTemperature(const glm::vec3& pos) const;

		// Donne l'humidité en fonction de la position
		// Prend en compte:
		//		-perturbation local par un bruit
		//		-valeur intrinsèque de la planete (composition) (TODO)
		// Le résultat est à valeur dans [-1,1]
		// La position est une position dans le référentiel de la planète.
		virtual float getHumidity(const glm::vec3& pos) const;

};

#endif /* end of include guard: PLANETGENERATOR_PJW8VJTB */
