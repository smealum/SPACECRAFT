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

uniform vec3 cameraPosition;
uniform vec3 lightDirection;

uniform int m_nSamples;

const int texSize=256;
const float DELTA = 0.000001;

// inline vec4 Interpolate(vec2 v)
// {
// 	v=min(max(v,0.0),1.0);

// 	return texture(tex,v);
// }

vec3 SetColor(vec3 v, vec3 c, vec3 lightDir)
{
	// Get the ray from the camera to the vertex, and its length (which is the far point of the ray passing through the atmosphere)
	vec3 vCamera = c;
	vec3 vRay = (v-vCamera);
	float fFar = length(vRay);
	vRay = (vRay/fFar);

	// Calculate the closest intersection of the ray with the outer atmosphere (which is the near point of the ray passing through the atmosphere)
	float B = 2.0 * (dot(vCamera, vRay));
	float C = (dot(vCamera, vCamera)) - m_fOuterRadius*m_fOuterRadius;
	float fDet = max(0.0, B*B-4.0*C);
	float fNear = 0.5 * (-B-sqrt(fDet));

	bool bCameraInAtmosphere = false;
	bool bCameraAbove = true;
	vec4 fCameraDepth=vec4(0.0);
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
		float fCameraAngle = dot((bCameraAbove ? (-vRay) : (vRay)), vCamera) / fCameraHeight;
		// fCameraDepth=Interpolate((vec2(fCameraAltitude, 0.5 - fCameraAngle * 0.5)));
		fCameraDepth=texture(tex, (vec2(fCameraAltitude, 0.5 - fCameraAngle * 0.5)));
	}
	else
	{
		// Otherwise, move the camera up to the near intersection point
		vCamera = (vCamera+(vRay*fNear));
		fFar -= fNear;
		fNear = 0;
	}

	// If the distance between the points on the ray is negligible, don't bother to calculate anything
	if(fFar <= DELTA)return vec3(0);

	// Initialize a few variables to use inside the loop
	vec3 fRayleighSum=vec3(0, 0, 0);
	vec3 fMieSum=vec3(0, 0, 0);
	float fSampleLength = fFar / m_nSamples;
	float fScaledLength = fSampleLength * m_fScale;
	vec3 vSampleRay = (vRay*fSampleLength);

	// Start at the center of the first sample ray, and loop through each of the others
	v = (vCamera+(vSampleRay*0.5));
	for(int i=0; i<m_nSamples; i++)
	{
		float fHeight = length(v);

		// Start by looking up the optical depth coming from the light source to this point
		float fLightAngle = dot(lightDir, v) / fHeight;
		float fAltitude = (fHeight - m_fInnerRadius) * m_fScale;
		// fLightDepth=Interpolate((vec2(fAltitude, 0.5 - fLightAngle * 0.5)));
		fLightDepth=texture(tex, (vec2(fAltitude, 0.5 - fLightAngle * 0.5)));

		// If no light light reaches this part of the atmosphere, no light is scattered in at this point
		if(fLightDepth.x < DELTA)continue;

		// Get the density at this point, along with the optical depth from the light source to this point
		float fRayleighDensity = fScaledLength * fLightDepth.x;
		float fRayleighDepth = fLightDepth.y;
		float fMieDensity = fScaledLength * fLightDepth.z;
		float fMieDepth = fLightDepth.w;

		// If the camera is above the point we're shading, we calculate the optical depth from the sample point to the camera
		// Otherwise, we calculate the optical depth from the camera to the sample point
		if(bCameraAbove)
		{
			float fSampleAngle = -dot(vRay, v) / fHeight;
			// fSampleDepth=Interpolate((vec2(fAltitude, 0.5 - fSampleAngle * 0.5)));
			fSampleDepth=texture(tex, (vec2(fAltitude, 0.5 - fSampleAngle * 0.5)));
			fRayleighDepth += fSampleDepth.y - fCameraDepth.y;
			fMieDepth += fSampleDepth.w - fCameraDepth.w;
		}
		else
		{
			float fSampleAngle = dot(vRay, v) / fHeight;
			// fSampleDepth=Interpolate((vec2(fAltitude, 0.5 - fSampleAngle * 0.5)));
			fSampleDepth=texture(tex, (vec2(fAltitude, 0.5 - fSampleAngle * 0.5)));
			fRayleighDepth += fCameraDepth.y - fSampleDepth.y;
			fMieDepth += fCameraDepth.w - fSampleDepth.w;
		}

		// Now multiply the optical depth by the attenuation factor for the sample ray
		fRayleighDepth *= m_Kr4PI;
		fMieDepth *= m_Km4PI;

		// Calculate the attenuation factor for the sample ray
		vec3 fAttenuation;
		fAttenuation = exp(-(fRayleighDepth/m_fWavelength4)-fMieDepth);

		fRayleighSum+=fRayleighDensity*fAttenuation;
		fMieSum+=fMieDensity*fAttenuation;

		// Move the position to the center of the next sample ray
		v = (v+vSampleRay);
	}

	// Calculate the angle and phase values (this block of code could be handled by a small 1D lookup table, or a 1D texture lookup in a pixel shader)
	float fAngle = -dot(vRay, lightDir);
	float fAngle2 = fAngle*fAngle;
	float g2 = m_g*m_g;
	vec2 fPhase;
	fPhase.x = 0.75 * (1.0 + fAngle2);
	fPhase.y = 1.5 * ((1 - g2) / (2 + g2)) * (1.0 + fAngle2) / pow(1 + g2 - 2*m_g*fAngle, 1.5);
	fPhase.x *= m_Kr * m_ESun;
	fPhase.y *= m_Km * m_ESun;

	// Calculate the in-scattering color and clamp it to the max color value
	vec3 fColor=fRayleighSum*fPhase.x/m_fWavelength4+fMieSum*fPhase.y;
	fColor=min(fColor, 1.0);
	return fColor;
}

void main()
{
	float intensity;
	vec3 vt=normalize(vPos);

	if(sky)vt*=m_fOuterRadius;///m;
	else vt*=m_fInnerRadius;

	vec4 color=vec4(0.0);

	vec3 t=SetColor(vt, cameraPosition, lightDirection);

	if(sky)color=vec4(t,length(t)*5);//t[1]);//length(vec3(t[0],t[1],t[2])));
	else color=vec4(t,1.0);

	//color=texelFetch(tex, ivec2((int(gl_FragCoord[0])%texSize),(int(gl_FragCoord[1])%texSize)), 0);
	// if(!sky)color*=gl_Color;
	outColor = color;
	// outColor = texture(tex,vec2(gl_FragCoord)/800);
	// outColor = vec4(color.xyz,1.0);
	// outColor = vec4(normalize(vt),1.0);
	// outColor = vec4(vPos+vec3(0.5,0,0),1.0);
	// outColor = texture(tex, vec2(vPos));
	//gl_FragColor = gl_Color;
}
