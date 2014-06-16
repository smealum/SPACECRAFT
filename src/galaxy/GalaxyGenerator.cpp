#include "GalaxyGenerator.h"
#include "Galaxy.h"

#define PI 3.14159265f
#define PI2 (2.0*PI)


// caractéristiques ( a choisir )
//const double width = 2e10; // rayon de la galaxie
//const double density = 5000.0; // quantité d'étoiles (évolution linéaire)
//const double arms = 6; // nombre de bras de la galaxie
//const double spin = 3.0; // tendance à tourner des bras
//const double noiseQt = 0.1; // chaos dans la galaxie

const double width = 100; // rayon de la galaxie
const double density = 40.0; // quantité d'étoiles (évolution linéaire)
const double arms = 6; // nombre de bras de la galaxie
const double spin = 3.0; // tendance à tourner des bras
const double noiseQt = 0.1; // chaos dans la galaxie

// caractéristique déduites
const double rhoMax = width;
const double rhoInc = width/sqrt(density);
const double thetaMax = PI2;
const double thetaInc = thetaMax/sqrt(density);
const double spinRel = spin/width;
const double noiseRel = noiseQt*width;
const double noisePos = 100.0/width;

using namespace glm;
using namespace std;

const dvec3 p1(1.0,0.0,0.0);
const dvec3 p2(0.0,1.0,0.0);
const dvec3 p3(0.0,0.0,1.0);


void GalaxyGenerate(Galaxy* galaxy)
{
	for(double theta=0; theta<thetaMax; theta+=thetaInc)
	{
		double rInc = rhoInc * (1.0+0.9*sin(arms*theta));
		for(double rho=0; rho<rhoMax; rho+=rInc)
		{
			double r = r;
			double thetata = theta + spinRel*rho;

			dvec3 p = dvec3(
					rho*cos(thetata),
					0.0,
					rho*sin(thetata)
			);

			p+=noiseRel*dvec3(
					glm::simplex(noisePos*p+p1),
					glm::simplex(noisePos*p+p2),
					glm::simplex(noisePos*p+p3)
			);

			galaxy->pushSolarSystem(p);
			
		}
	}
}
