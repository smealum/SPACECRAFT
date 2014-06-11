#ifndef PLANETGENERATOR_PJW8VJTB
#define PLANETGENERATOR_PJW8VJTB

#include "world/BlockType.h"
#include "Chunk.h"


struct PlanetGeneratorResponse
{
	float elevation;	
	blockTypes::T blockType;
};

class PlanetGenerator
{
	public:

	PlanetGenerator(Planet& planet);

	// genere un miniworld
	virtual float generateWorldData(const chunkVal* data,
				int w, int h, int d, // array sizes (in chunks)
				int px, int py, int pz, // offset in world
				glm::vec3 origin, glm::vec3 v1, glm::vec3 v2); // toplevelCharacteristic

	// genere l'elevation,texture.
	virtual PlanetGeneratorResponse getCharacteristic(const glm::vec3& pos);

	private:
	// Donne la température en fonction de la position
	// Prend en compte
	// 		-l'axe de rotation de la planete
	//		-perturbation local par un bruit.
	// 		-la distance de la planete par rapport au soleil (TODO)
	//		-valeur intrinsèque de la planete (composition) (TODO)
	// Le résultat est à valeur dans [-1,1]
	// La position est une position dans le référentiel de la planète.
	virtual float getTemperature(const glm::vec3& pos);

	// Donne l'humidité en fonction de la position
	// Prend en compte:
	//		-perturbation local par un bruit
	//		-valeur intrinsèque de la planete (composition) (TODO)
	// Le résultat est à valeur dans [-1,1]
	// La position est une position dans le référentiel de la planète.
	virtual float getHumidity(const glm::vec3& pos);
};

#endif /* end of include guard: PLANETGENERATOR_PJW8VJTB */
