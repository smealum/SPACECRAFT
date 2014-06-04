#version 330
// atmospheric scattering fragment shader
// original code by sean p o'neill (http://sponeil.net/)
// fragment shader adaptation by smea (http://smealum.net)

in vec3 vPos;

out vec4 outColor;

const float PI=3.14159265;

uniform sampler2D tex;

uniform float m_fOuterRadius;
uniform float m_fScale;
uniform float m_fInnerRadius;
uniform float m_Kr;
uniform float m_Km;
uniform float m_Kr4PI;
uniform float m_Km4PI;
uniform vec3 m_fWavelength4;
uniform float m_g;
uniform float m_ESun;
uniform bool sky;

uniform vec3 cameraPosition, lightDirection;

uniform int m_nSamples;

const int texSize=128;
const float DELTA = 0.000001;

vec3 SetColor(vec3 v, vec3 c, vec3 lightDir)
{
	// Get the ray from the camera to the vertex, and its length (which is the far point of the ray passing through the atmosphere)
	vec3 vCamera = c;
	vec3 vRay = (v-vCamera);
	float fFar = length(vRay);
	vRay = (vRay/fFar);

	// Calculate the closest intersection of the ray with the outer atmosphere (which is the near point of the ray passing through the atmosphere)
	float B = 2.0f * (dot(vCamera, vRay));
	float C = (dot(vCamera, vCamera)) - m_fOuterRadius*m_fOuterRadius;
	float fDet = max(0.0f, B*B - 4.0f * C);
	float fNear = 0.5f * (-B - sqrt(fDet));

	bool bCameraInAtmosphere = false;
	bool bCameraAbove = true;
	vec4 fCameraDepth = vec4(0, 0, 0, 0);
	vec4 fLightDepth;
	vec4 fSampleDepth;

	if(fNear <= 0)
	{
		// If the near point is behind the camera, it means the camera is inside the atmosphere
		bCameraInAtmosphere = true;
		fNear = 0;
		float fCameraHeight = length(vCamera);
		float fCameraAltitude = (fCameraHeight - m_fInnerRadius) * m_fScale;
		bCameraAbove = fCameraHeight >= length(v);
		float fCameraAngle = dot((bCameraAbove ? (-vRay) : vRay), vCamera) / fCameraHeight;
		//m_pbOpticalDepth.Interpolate(fCameraDepth, fCameraAltitude, 0.5f - fCameraAngle * 0.5f);
		// fCameraDepth=Interpolate(fCameraAltitude, 0.5f - fCameraAngle * 0.5f);
		fCameraDepth=texture(tex, vec2(fCameraAltitude, 0.5f - fCameraAngle * 0.5f));
	}else{
		// Otherwise, move the camera up to the near intersection point
		vCamera = (vCamera+(vRay*fNear));
		fFar -= fNear;
		fNear = 0.0;
	}

	// If the distance between the points on the ray is negligible, don't bother to calculate anything
	if(fFar <= DELTA)
	{
		return vec3(0.0, 0.0, 0.0);
	}

	// Initialize a few variables to use inside the loop
	float fRayleighSum[3];
	float fMieSum[3];
	fRayleighSum[0] = 0;
	fRayleighSum[1] = 0;
	fRayleighSum[2] = 0;
	fMieSum[0] = 0;
	fMieSum[1] = 0;
	fMieSum[2] = 0;
	float fSampleLength = fFar / m_nSamples;
	float fScaledLength = fSampleLength * m_fScale;
	vec3 vSampleRay = (vRay*fSampleLength);

	// Start at the center of the first sample ray, and loop through each of the others
	v = (vCamera+(vSampleRay*0.5f));
	for(int i=0; i<m_nSamples; i++)
	{
		float fHeight = length(v);

		// Start by looking up the optical depth coming from the light source to this point
		float fLightAngle = dot(lightDir, v) / fHeight;
		float fAltitude = (fHeight - m_fInnerRadius) * m_fScale;
		//m_pbOpticalDepth.Interpolate(fLightDepth, fAltitude, 0.5f - fLightAngle * 0.5f);
		// fLightDepth=Interpolate(fAltitude, 0.5f - fLightAngle * 0.5f);
		fLightDepth=texture(tex, vec2(fAltitude, 0.5f - fLightAngle * 0.5f));

		// If no light light reaches this part of the atmosphere, no light is scattered in at this point
		if(fLightDepth[0] < DELTA)continue;

		// Get the density at this point, along with the optical depth from the light source to this point
		float fRayleighDensity = fScaledLength * fLightDepth[0];
		float fRayleighDepth = fLightDepth[1];
		float fMieDensity = fScaledLength * fLightDepth[2];
		float fMieDepth = fLightDepth[3];

		// If the camera is above the point we're shading, we calculate the optical depth from the sample point to the camera
		// Otherwise, we calculate the optical depth from the camera to the sample point
		if(bCameraAbove)
		{
			float fSampleAngle = -dot(vRay, v) / fHeight;
			//m_pbOpticalDepth.Interpolate(fSampleDepth, fAltitude, 0.5f - fSampleAngle * 0.5f);
			// fSampleDepth=Interpolate(fAltitude, 0.5f - fSampleAngle * 0.5f);
			fSampleDepth=texture(tex, vec2(fAltitude, 0.5f - fSampleAngle * 0.5f));
			fRayleighDepth += fSampleDepth[1] - fCameraDepth[1];
			fMieDepth += fSampleDepth[3] - fCameraDepth[3];
		}else{
			float fSampleAngle = dot(vRay, v) / fHeight;
			//m_pbOpticalDepth.Interpolate(fSampleDepth, fAltitude, 0.5f - fSampleAngle * 0.5f);
			// fSampleDepth=Interpolate(fAltitude, 0.5f - fSampleAngle * 0.5f);
			fSampleDepth=texture(tex, vec2(fAltitude, 0.5f - fSampleAngle * 0.5f));
			fRayleighDepth += fCameraDepth[1] - fSampleDepth[1];
			fMieDepth += fCameraDepth[3] - fSampleDepth[3];
		}

		// Now multiply the optical depth by the attenuation factor for the sample ray
		fRayleighDepth *= m_Kr4PI;
		fMieDepth *= m_Km4PI;

		// Calculate the attenuation factor for the sample ray
		float fAttenuation[3];
		fAttenuation[0] = exp(-fRayleighDepth / m_fWavelength4[0] - fMieDepth);
		fAttenuation[1] = exp(-fRayleighDepth / m_fWavelength4[1] - fMieDepth);
		fAttenuation[2] = exp(-fRayleighDepth / m_fWavelength4[2] - fMieDepth);

		fRayleighSum[0] += fRayleighDensity * fAttenuation[0];
		fRayleighSum[1] += fRayleighDensity * fAttenuation[1];
		fRayleighSum[2] += fRayleighDensity * fAttenuation[2];

		fMieSum[0] += fMieDensity * fAttenuation[0];
		fMieSum[1] += fMieDensity * fAttenuation[1];
		fMieSum[2] += fMieDensity * fAttenuation[2];

		// Move the position to the center of the next sample ray
		v += vSampleRay;
	}

	// Calculate the angle and phase values (this block of code could be handled by a small 1D lookup table, or a 1D texture lookup in a pixel shader)
	float fAngle = -dot(vRay, lightDir);
	float fPhase[2];
	float fAngle2 = fAngle*fAngle;
	float g2 = m_g*m_g;
	fPhase[0] = 0.75f * (1.0f + fAngle2);
	fPhase[1] = 1.5f * ((1 - g2) / (2 + g2)) * (1.0f + fAngle2) / pow(1 + g2 - 2*m_g*fAngle, 1.5f);
	fPhase[0] *= m_Kr * m_ESun;
	fPhase[1] *= m_Km * m_ESun;

	// Calculate the in-scattering color and clamp it to the max color value
	vec3 fColor;
	fColor[0] = fRayleighSum[0] * fPhase[0] / m_fWavelength4[0] + fMieSum[0] * fPhase[1];
	fColor[1] = fRayleighSum[1] * fPhase[0] / m_fWavelength4[1] + fMieSum[1] * fPhase[1];
	fColor[2] = fRayleighSum[2] * fPhase[0] / m_fWavelength4[2] + fMieSum[2] * fPhase[1];
	fColor[0] = min(fColor[0], 1.0f);
	fColor[1] = min(fColor[1], 1.0f);
	fColor[2] = min(fColor[2], 1.0f);

	// Last but not least, set the color
	return fColor;
}

void main()
{
	float intensity;
	vec3 vt=normalize(vPos);

	if(sky)vt*=m_fOuterRadius;///m;
	else vt*=m_fInnerRadius;

	vec4 color=vec4(0.0,0.0,0.0,0.0);

	vec3 t=SetColor(vt, cameraPosition, lightDirection);

	if(sky)color=vec4(t,length(t)*5);//t[1]);//length(vec3(t[0],t[1],t[2])));
	else color=vec4(t,1.0);

	//color=texelFetch(tex, ivec2((int(gl_FragCoord[0])%texSize),(int(gl_FragCoord[1])%texSize)), 0);
	// if(!sky)color*=gl_Color;
	outColor = color;
	// outColor = vec4(color.xyz,1.0);
	// outColor = vec4(normalize(vt),1.0);
	// outColor = vPos;
	// outColor = texture(tex, vec2(vPos));
	//gl_FragColor = gl_Color;
}
