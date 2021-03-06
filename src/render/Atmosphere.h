#ifndef ATMOSPHERE_H
#define ATMOSPHERE_H

#define PI 3.14159265f
#define DELTA 1e-6f

#define NUMSAMPLES 4
#define KR 0.0025f
#define KM 0.0015f
#define ESUN 15.0f
#define G -0.85f
#define WAVELENGTH0 0.650f
#define WAVELENGTH1 0.570f
#define WAVELENGTH2 0.475f
#define RAYLEIGHDEPTH 0.25f
#define MIEDEPTH 0.1f

#include "utils/glm.h"
#include "utils/Sphere.h"
#include "render/Camera.h"
#include "utils/Singleton.h"

class AtmosphereInfo
{
	public:
		AtmosphereInfo();
		AtmosphereInfo(int seed);

		int m_nSamples;
		float m_Kr, m_Kr4PI;
		float m_Km, m_Km4PI;
		float m_ESun;
		float m_g;
		float m_fInnerRadius;
		float m_fOuterRadius;
		float m_fScale;
		glm::vec3 m_fWavelength;
		glm::vec3 m_fWavelength4;
		float m_fRayleighScaleDepth;
		float m_fMieScaleDepth;
};


// Il faudrait mettre en singleton.
// J'ai bien rebooter plusieurs fois mon ordi car trop de texture ont été générées.
class Atmosphere
{
	public:
		Atmosphere(AtmosphereInfo* ai);
		~Atmosphere();

		void makeOpticalDepthBuffer(void);
		void makePhaseBuffer(void);
		void generateVBO(void);

		void bind(Camera& c, glm::vec3 lightDirection, glm::vec3 position, float scale, ShaderProgram& sprogram);
		void bind(Camera& c, glm::vec3 lightDirection, glm::vec3 position, float scale);
		void draw(Camera& c, glm::vec3 lightDirection, glm::vec3 position, float scale);

	private:
		ShaderProgram& shader;

		AtmosphereInfo* info;
		
		GLuint depthTexture, phaseTexture;
		GLuint vao, vbo, ebo;

		int lod;

};

#endif
