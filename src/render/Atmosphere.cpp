#include <cstdlib>
#include "Atmosphere.h"
#include "utils/SphereManager.h"

Atmosphere::Atmosphere():
	shader(ShaderProgram::loadFromFile("shader/atmosphere/atmosphere.vert", "shader/atmosphere/atmosphere.frag", "atmosphere")),
	m_fInnerRadius(1.0f),
	m_fOuterRadius(1.05f),
	lod(6)
{
	initLightConstants();

	makeOpticalDepthBuffer();
	makePhaseBuffer();

	shader.use();
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

	m_fWavelength = glm::vec3(WAVELENGTH0, WAVELENGTH1, WAVELENGTH2);
	m_fWavelength4[0] = powf(m_fWavelength[0], 4.0f);
	m_fWavelength4[1] = powf(m_fWavelength[1], 4.0f);
	m_fWavelength4[2] = powf(m_fWavelength[2], 4.0f);

	m_fRayleighScaleDepth = RAYLEIGHDEPTH;
	m_fMieScaleDepth = MIEDEPTH;
}

#include <cstdio>

//TODO : passer sur producer
//(attention aux appels GL)
void Atmosphere::makeOpticalDepthBuffer(void)
{
	const int nSize = 256;
	const int nSamples = 50;
	const float fScale = 1.0f / (m_fOuterRadius - m_fInnerRadius);
	const int m_nChannels=4;
	
	float* opticalBuffer=(float*)malloc(sizeof(float)*m_nChannels*nSize*nSize);

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

	glGenTextures(1, &depthTexture);
	glBindTexture(GL_TEXTURE_2D, depthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, nSize, nSize, 0, GL_RGBA, GL_FLOAT, opticalBuffer);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

	//important
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	//important
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	
	free(opticalBuffer);
}

//TODO : idem
void Atmosphere::makePhaseBuffer(void)
{
	const int nSize = 8192; //on est en 1D, on peut se le permettre
	float* phaseBuffer=(float*)malloc(sizeof(float)*nSize*4);
	int k=0;
	for(int i=0;i<nSize;i++)
	{
		float fAngle=float(2*i)/(nSize)-1.0f;
		float fAngle2=fAngle*fAngle;
		float g2 =m_g*m_g;
		glm::vec2 fPhase;
		fPhase.x = 0.75f * (1.0f + fAngle2);
		fPhase.y = 1.5f * ((1.0f - g2) / (2.0f + g2)) * (1.0f + fAngle2) / pow(1.0f + g2 - 2*m_g*fAngle, 1.5f);
		fPhase.x *= m_Kr * m_ESun;
		fPhase.y *= m_Km * m_ESun;
		phaseBuffer[k++]=fPhase.x;
		phaseBuffer[k++]=fPhase.y;
		phaseBuffer[k++]=1.0f;
		phaseBuffer[k++]=1.0f;
	}

	glGenTextures(1, &phaseTexture);
	glBindTexture(GL_TEXTURE_1D, phaseTexture);
	glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA32F, nSize, 0, GL_RGBA, GL_FLOAT, phaseBuffer);

	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAX_LEVEL, 0);

	//important
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	//important
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

	free(phaseBuffer);
}

extern float testAngle;

void Atmosphere::bind(Camera& c, glm::vec3 lightDirection, glm::vec3 position)
{
	bind(c,lightDirection,position,shader);
}

void Atmosphere::bind(Camera& c, glm::vec3 lightDirection, glm::vec3 position, ShaderProgram& sprogram)
{
	sprogram.use();

	sprogram.setUniform("cameraPosition", c.getPosition(position));
	sprogram.setUniform("lightDirection", lightDirection);
	sprogram.setUniform("m_fWavelength4", m_fWavelength4);
	sprogram.setUniform("m_g", m_g);
	sprogram.setUniform("m_ESun", m_ESun);
	sprogram.setUniform("m_Kr", m_Kr);
	sprogram.setUniform("m_Km", m_Km);
	sprogram.setUniform("m_Kr4PI", m_Kr4PI);
	sprogram.setUniform("m_Km4PI", m_Km4PI);
	sprogram.setUniform("m_fInnerRadius", m_fInnerRadius);
	sprogram.setUniform("m_fScale", m_fScale);
	sprogram.setUniform("m_fOuterRadius", m_fOuterRadius);
	sprogram.setUniform("m_nSamples", m_nSamples);

	sprogram.setUniform("depthTex",0);
	sprogram.setUniform("phaseTex",1);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_1D, phaseTexture);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, depthTexture);
}

void Atmosphere::draw(Camera& c, glm::vec3 lightDirection, glm::vec3 position)
{
	bind(c, lightDirection, position);
	
	shader.setUniform("sky", true);
	shader.setUniform("model", glm::translate(glm::mat4(1.f),position-c.getReference()));

	c.updateCamera(shader);

	//TODO : check la bonne valeur ?
	glFrontFace(GL_CW);
		SphereManager::getInstance().draw(c,shader,lod);
	glFrontFace(GL_CCW);
}

Atmosphere::~Atmosphere()
{
}
