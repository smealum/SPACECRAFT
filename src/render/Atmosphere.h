#ifndef ATMOSPHERE_H
#define ATMOSPHERE_H

#define PI 3.14159265
#define DELTA 1e-6f

#define NUMSAMPLES 4
#define KR 0.0025f
#define KM 0.0015f
#define ESUN 30.0f
#define G -0.85f
#define WAVELENGTH0 0.650f
#define WAVELENGTH1 0.570f
#define WAVELENGTH2 0.475f
#define RAYLEIGHDEPTH 0.25f
#define MIEDEPTH 0.1f

#include "utils/glm.h"
#include "render/Camera.h"

class Atmosphere
{
	public:
		Atmosphere();
		~Atmosphere();

		void initLightConstants(void);
		void makeOpticalDepthBuffer(void);
		void generateVBO(void);

		void bind(Camera& c);
		void draw(Camera& c);

	private:
		ShaderProgram& shader;

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

		float* opticalBuffer;
		
		GLuint texture;
		GLuint vao, vbo, ebo;
};

#endif