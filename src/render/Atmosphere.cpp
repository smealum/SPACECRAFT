#include <cstdlib>
#include "Atmosphere.h"

Atmosphere::Atmosphere():
	shader(ShaderProgram::loadFromFile("shader/atmosphere/atmosphere.vert", "shader/atmosphere/atmosphere.frag", "atmosphere")),
	// shader(ShaderProgram::loadFromFile("shader/basic/basic.vert", "shader/basic/basic.frag", "basic")),
	m_fInnerRadius(1.0f),
	m_fOuterRadius(1.01f),
	opticalBuffer(NULL)
{
	initLightConstants();
	makeOpticalDepthBuffer();
	generateVBO();

	shader.use();
	glBindFragDataLocation(shader.getHandle(), 0, "outColor");
	shader.setAttribute("position", 3, GL_FALSE, 10, 0);
	shader.setAttribute("color", 4, GL_FALSE, 10, 3);
	shader.setAttribute("normal", 3, GL_FALSE, 10, 7);
}

static GLfloat vertices[] = {
	//     POSITION    |      COLOR           |     NORMAL
	// x positif
	+1.0 , -1.0 , -1.0 , 1.0 , 0.0 , 0.0 , 1.f, +1.0 , 0.0 , 0.0 ,
	+1.0 , +1.0 , -1.0 , 1.0 , 0.0 , 0.0 , 1.f, +1.0 , 0.0 , 0.0 ,
	+1.0 , +1.0 , +1.0 , 1.0 , 0.0 , 0.0 , 1.f, +1.0 , 0.0 , 0.0 ,
	+1.0 , -1.0 , +1.0 , 1.0 , 0.0 , 0.0 , 1.f, +1.0 , 0.0 , 0.0 ,
	// x negatif
	-1.0 , -1.0 , -1.0 , 0.0 , 1.0 , 0.0 , 1.f, -1.0 , 0.0 , 0.0 ,
	-1.0 , -1.0 , +1.0 , 0.0 , 1.0 , 0.0 , 1.f, -1.0 , 0.0 , 0.0 ,
	-1.0 , +1.0 , +1.0 , 0.0 , 1.0 , 0.0 , 1.f, -1.0 , 0.0 , 0.0 ,
	-1.0 , +1.0 , -1.0 , 0.0 , 1.0 , 0.0 , 1.f, -1.0 , 0.0 , 0.0 ,
	// y positif
	-1.0 , +1.0 , -1.0 , 0.0 , 0.0 , 1.0 , 1.f, 0.0 , +1.0 , 0.0 ,
	-1.0 , +1.0 , +1.0 , 0.0 , 0.0 , 1.0 , 1.f, 0.0 , +1.0 , 0.0 ,
	+1.0 , +1.0 , +1.0 , 0.0 , 0.0 , 1.0 , 1.f, 0.0 , +1.0 , 0.0 ,
	+1.0 , +1.0 , -1.0 , 0.0 , 0.0 , 1.0 , 1.f, 0.0 , +1.0 , 0.0 ,
	// y negatif
	-1.0 , -1.0 , -1.0 , 0.0 , 1.0 , 1.0 , 1.f, 0.0 , -1.0 , 0.0 ,
	+1.0 , -1.0 , -1.0 , 0.0 , 1.0 , 1.0 , 1.f, 0.0 , -1.0 , 0.0 ,
	+1.0 , -1.0 , +1.0 , 0.0 , 1.0 , 1.0 , 1.f, 0.0 , -1.0 , 0.0 ,
	-1.0 , -1.0 , +1.0 , 0.0 , 1.0 , 1.0 , 1.f, 0.0 , -1.0 , 0.0 ,
	// z positif
	-1.0 , -1.0 , +1.0 , 1.0 , 0.0 , 1.0 , 1.f, 0.0 , 0.0 , +1.0 ,
	+1.0 , -1.0 , +1.0 , 1.0 , 0.0 , 1.0 , 1.f, 0.0 , 0.0 , +1.0 ,
	+1.0 , +1.0 , +1.0 , 1.0 , 0.0 , 1.0 , 1.f, 0.0 , 0.0 , +1.0 ,
	-1.0 , +1.0 , +1.0 , 1.0 , 0.0 , 1.0 , 1.f, 0.0 , 0.0 , +1.0 ,
	// z negatif
	-1.0 , -1.0 , -1.0 , 1.0 , 1.0 , 0.0 , 1.f, 0.0 , 0.0 , -1.0 ,
	-1.0 , +1.0 , -1.0 , 1.0 , 1.0 , 0.0 , 1.f, 0.0 , 0.0 , -1.0 ,
	+1.0 , +1.0 , -1.0 , 1.0 , 1.0 , 0.0 , 1.f, 0.0 , 0.0 , -1.0 ,
	+1.0 , -1.0 , -1.0 , 1.0 , 1.0 , 0.0 , 1.f, 0.0 , 0.0 , -1.0 ,
};


static GLuint elements[12*3] = {
	0,1,2,      0,2,3, // face 1
	4,5,6,      4,6,7, // face 2
	8,9,10,     8,10,11, // face 3
	12,13,14,   12,14,15, // face 4
	16,17,18,   16,18,19, // face 5
	20,21,22,   20,22,23, // face 6
};

//TODO : créer juste un VBO sphère global ?
void Atmosphere::generateVBO(void)
{
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	shader.setBuffers(vao, vbo, ebo);
}

void Atmosphere::initLightConstants(void)
{
	m_nSamples = NUMSAMPLES;
	m_Kr = KR;
	m_Kr4PI = m_Kr*4.0f*PI;
	m_Km = KM;
	m_Km4PI = m_Km*4.0f*PI;
	m_ESun = ESUN;
	m_g = G;

	m_fScale = 1 / (m_fOuterRadius - m_fInnerRadius);

	m_fWavelength[0] = WAVELENGTH0;
	m_fWavelength[1] = WAVELENGTH1;
	m_fWavelength[2] = WAVELENGTH2;
	m_fWavelength4[0] = powf(m_fWavelength[0], 4.0f);
	m_fWavelength4[1] = powf(m_fWavelength[1], 4.0f);
	m_fWavelength4[2] = powf(m_fWavelength[2], 4.0f);

	m_fRayleighScaleDepth = RAYLEIGHDEPTH;
	m_fMieScaleDepth = MIEDEPTH;
}

//TODO : passer sur producer
//(attention aux appels GL)
void Atmosphere::makeOpticalDepthBuffer(void)
{
	const int nSize = 128;
	const int nSamples = 50;
	const float fScale = 1.0f / (m_fOuterRadius - m_fInnerRadius);
	const int m_nChannels=4;
	
	opticalBuffer=(float*)malloc(sizeof(float)*m_nChannels*nSize*nSize);

	//Init(nSize, nSize, 1, 4, GL_RGBA, GL_FLOAT);
	int nIndex = 0;
	for(int nAngle=0; nAngle<nSize; nAngle++)
	{
		// As the y tex coord goes from 0 to 1, the angle goes from 0 to 180 degrees
		float fCos = 1.0f - (nAngle+nAngle) / (float)nSize;
		float fAngle = acosf(fCos);
		glm::vec3 vRay(sinf(fAngle), cosf(fAngle), 0);	// Ray pointing to the viewpoint
		for(int nHeight=0; nHeight<nSize; nHeight++)
		{
			// As the x tex coord goes from 0 to 1, the height goes from the bottom of the atmosphere to the top
			float fHeight = DELTA + m_fInnerRadius + ((m_fOuterRadius - m_fInnerRadius) * nHeight) / nSize;
			glm::vec3 vPos(0, fHeight, 0);				// The position of the camera

			// If the ray from vPos heading in the vRay direction intersects the inner radius (i.e. the planet), then this spot is not visible from the viewpoint
			float B = 2.0f * glm::dot(vPos,vRay);
			float Bsq = B * B;
			float Cpart = glm::dot(vPos, vPos);
			float C = Cpart - m_fInnerRadius*m_fInnerRadius;
			float fDet = Bsq - 4.0f * C;
			bool bVisible = ((fDet < 0 )|| ((0.5f * (-B - sqrtf(fDet)) <= 0) && (0.5f * (-B + sqrtf(fDet)) <= 0)));
			float fRayleighDensityRatio;
			float fMieDensityRatio;
			if(bVisible)
			{
				fRayleighDensityRatio = expf(-(fHeight - m_fInnerRadius) * fScale / m_fRayleighScaleDepth);
				fMieDensityRatio = expf(-(fHeight - m_fInnerRadius) * fScale / m_fMieScaleDepth);
			}
			else
			{
				// Smooth the transition from light to shadow (it is a soft shadow after all)
				fRayleighDensityRatio = opticalBuffer[nIndex - nSize*m_nChannels] * 0.75f;
				fMieDensityRatio = opticalBuffer[nIndex+2 - nSize*m_nChannels] * 0.75f;
			}

			// Determine where the ray intersects the outer radius (the top of the atmosphere)
			// This is the end of our ray for determining the optical depth (vPos is the start)
			C = Cpart - m_fOuterRadius*m_fOuterRadius;
			fDet = Bsq - 4.0f * C;
			float fFar = 0.5f * (-B + sqrtf(fDet));

			// Next determine the length of each sample, scale the sample ray, and make sure position checks are at the center of a sample ray
			float fSampleLength = fFar / nSamples;
			float fScaledLength = fSampleLength * fScale;
			glm::vec3 vSampleRay = (vRay*fSampleLength);
			vPos = (vPos+(vSampleRay*0.5f));

			// Iterate through the samples to sum up the optical depth for the distance the ray travels through the atmosphere
			float fRayleighDepth = 0;
			float fMieDepth = 0;
			for(int i=0; i<nSamples; i++)
			{
				float fHeight = glm::length(vPos);
				float fAltitude = (fHeight - m_fInnerRadius) * fScale;
				if(fAltitude<0.0f)fAltitude=0.0f;
				fRayleighDepth += expf(-fAltitude / m_fRayleighScaleDepth);
				fMieDepth += expf(-fAltitude / m_fMieScaleDepth);
				vPos = (vPos+vSampleRay);
			}

			// Multiply the sums by the length the ray traveled
			fRayleighDepth *= fScaledLength;
			fMieDepth *= fScaledLength;

			// Store the results for Rayleigh to the light source, Rayleigh to the camera, Mie to the light source, and Mie to the camera
			opticalBuffer[nIndex++] = fRayleighDensityRatio;
			opticalBuffer[nIndex++] = fRayleighDepth;
			opticalBuffer[nIndex++] = fMieDensityRatio;
			opticalBuffer[nIndex++] = fMieDepth;
		}
	}

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, nSize, nSize, 0, GL_RGBA, GL_FLOAT, opticalBuffer);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
	glBindTexture(GL_TEXTURE_2D, texture);
}

void Atmosphere::bind(Camera& c)
{
	glm::vec3 lightDirection(1,0,0);

	shader.setUniform("cameraPosition", c.getPosition());
	shader.setUniform("lightDirection", lightDirection);
	shader.setUniform("m_fWavelength4", m_fWavelength4);
	shader.setUniform("m_g", m_g);
	shader.setUniform("m_ESun", m_ESun);
	shader.setUniform("m_Kr", m_Kr);
	shader.setUniform("m_Km", m_Km);
	shader.setUniform("m_Kr4PI", m_Kr4PI);
	shader.setUniform("m_Km4PI", m_Km4PI);
	shader.setUniform("m_fInnerRadius", m_fInnerRadius);
	shader.setUniform("m_fScale", m_fScale);
	shader.setUniform("m_fOuterRadius", m_fOuterRadius);
	shader.setUniform("m_nSamples", m_nSamples);

	glBindTexture(GL_TEXTURE_2D, texture);
}

void Atmosphere::draw(Camera& c)
{
	bind(c);
	shader.setUniform("sky", false);

	shader.setUniform("model", glm::mat4(1.f));

	shader.use();
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	c.updateCamera(shader);

	//TODO : check la bonne valeur ?
	glFrontFace(GL_CW);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	glFrontFace(GL_CCW);
}

Atmosphere::~Atmosphere()
{
	free(opticalBuffer);
}
