#include "noise/Tree.h"
#include "utils/maths.h"
#include "utils/dbg.h"
#include <ctime>

// helper to get the 1D index in a flattened 3D array, using a vec3
#define TPOS(V) ((V).x + size * ((V).y + (V).z * height))

Tree::Tree() :
	seed(0),
	array(NULL)
{
}

void Tree::generate()
{
	noise::module::Perlin rnd;
	// XXX
	seed = time(NULL);
	rnd.SetSeed(seed);

	height = 7 + (rnd.GetValue(0, 0, 0) + 1.f) * 2;
	size = height + (rnd.GetValue(5, 0, 0) + 1.f) * 3;

	array = new blockTypes::T[height * size * size];

	// air!!
	for (int i = 0; i < height * size * size; i++)
		array[i] = blockTypes::air;

	// on dessine la base (on creusera apres)
	glm::i32vec3 center(size/2, 0, size/2);
	glm::i32vec2 si(size/2, size/2);
	glm::i32vec3 pos = center - glm::i32vec3(si.x/2, 0, si.y/2);

	int maxBase = 3;
	for (int i = 0; i < maxBase; i++)
	{
		digSquare(pos, si, blockTypes::tree); // c'est en réalité du tree sur tous les cotés
		// on creuse sur la base pour ne faire que le contour
		if (si.x > 2)
		{
			digSquare(pos + glm::i32vec3(1, 0, 1), si - glm::i32vec2(2), blockTypes::air);
		} else {
			pos += glm::i32vec3(1, 1, 1);
			si -= glm::i32vec2(2);
			break;
		}
		pos += glm::i32vec3(1, 1, 1);
		si -= glm::i32vec2(2);
	}

	// on creuse un peu la base pour faire comme des racines

	// on créé le tronc
	
	pos -= glm::i32vec3(1, 1, 1);
	si += glm::i32vec2(2);
	for (int i = maxBase; i < height; i++)
	{
		digSquare(pos, si, blockTypes::tree);
		pos.y++;
	}
}

Tree::~Tree()
{
	if (array) delete[] array;
}

void Tree::digLine(const glm::i32vec3 &a, const glm::i32vec3 &b, blockTypes::T type)
{
	int i, err_1, err_2;
	glm::i32vec3 pixel,
				 d, // replace the usual dx, dy, dz
				 d2,
				 inc,
				 absinc; // l, m, n

	pixel = a;
	d = b - a;
	for (int i = 0; i < 3; i++)
	{
		inc[i] = (d[i] < 0) ? -1 : 1;
		absinc[i] = ABS(d[i]);
		d2[i] = absinc[i] << 1;
	}

	if ((absinc.x >= absinc.y) && (absinc.x >= absinc.z)) {
		err_1 = d2.y - absinc.x;
		err_2 = d2.z - absinc.x;
		for (i = 0; i < absinc.x; i++) {
			if (correct(pixel))
				return; // ouside ouf the tab
			array[TPOS(pixel)] = type; // air
			if (err_1 > 0) {
				pixel[1] += inc.y;
				err_1 -= d2.x;
			}
			if (err_2 > 0) {
				pixel[2] += inc.z;
				err_2 -= d2.x;
			}
			err_1 += d2.y;
			err_2 += d2.z;
			pixel[0] += inc.x;
		}
	} else if ((absinc.y >= absinc.x) && (absinc.y >= absinc.z)) {
		err_1 = d2.x - absinc.y;
		err_2 = d2.z - absinc.y;
		for (i = 0; i < absinc.y; i++) {
			if (correct(pixel))
				return; // ouside ouf the tab
			array[TPOS(pixel)] = type; // air
			if (err_1 > 0) {
				pixel[0] += inc.x;
				err_1 -= d2.y;
			}
			if (err_2 > 0) {
				pixel[2] += inc.z;
				err_2 -= d2.y;
			}
			err_1 += d2.x;
			err_2 += d2.z;
			pixel[1] += inc.y;
		}
	} else {
		err_1 = d2.y - absinc.z;
		err_2 = d2.x - absinc.z;
		for (i = 0; i < absinc.z; i++) {
			if (correct(pixel))
				return; // ouside ouf the tab
			array[TPOS(pixel)] = type; // air
			if (err_1 > 0) {
				pixel[1] += inc.y;
				err_1 -= d2.z;
			}
			if (err_2 > 0) {
				pixel[0] += inc.x;
				err_2 -= d2.z;
			}
			err_1 += d2.y;
			err_2 += d2.x;
			pixel[2] += inc.z;
		}
	}
	if (correct(pixel))
		return; // ouside ouf the tab
	array[TPOS(pixel)] = type; // air
}

void Tree::digSquare(const glm::i32vec3 &p, const glm::i32vec2 &si, blockTypes::T type)
{
	for (int i = 0; i < si.y; i++)
	{
		glm::i32vec3 a = p + glm::i32vec3(0, 0, i);
		digLine(a, a + glm::i32vec3(si.x, 0, 0), type);
	}
}

