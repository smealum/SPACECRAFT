#ifndef PLANET_H
#define PLANET_H

typedef struct
{
	int seed;
}planetInfo_s;

class Planet
{
	public:
		Planet(planetInfo_s pi);
		const planetInfo_s planetInfo; //read only
	private:
};

#endif
