#include "noise/Tree.h"
#include "utils/maths.h"
#include "utils/dbg.h"
#include "Chunk.h"

// helper to get the 1D index in a flattened 3D array, using a vec3
#define TPOS(V) ((V).x + size * ((V).y + (V).z * height))

Tree::Tree() :
	seed(0),
	minHeight(7),
	minSize(3),
	halfRandomHeight(9),
	halfRandomSize(7),
	array(NULL)
{
}

void Tree::generate(int seed)
{
	this->seed = seed;
	rnd.SetSeed(seed);

	height = minHeight + (rnd.GetValue(0.1, seed/10000, 0) + 1.f) * halfRandomHeight;
	size = minSize + (rnd.GetValue(0.2, seed/10000, 0) + 1.f) * halfRandomSize;
	log_info("seed: %d -> %d, %d", seed, height, size);

	array = new blockTypes::T[height * size * size];

	// air!!
	for (int i = 0; i < height * size * size; i++)
		array[i] = blockTypes::air;

	// on dessine la base (on creusera apres)
	glm::i32vec3 center(size/2, 0, size/2);
	glm::i32vec2 si(size/2, size/2);
	glm::i32vec3 pos = center - glm::i32vec3(si.x/2, 0, si.y/2);

	int maxBase = 0; // XXX
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


	// on dessine le feuillage
	
	pos = glm::i32vec3(size/2, height/1.5f, size/2);
	int maxH = size;
	glm::i32vec3 cubeSize(size, height/3, size);
	while (cubeSize.x >= 3)
	{
		digCube(pos, cubeSize, blockTypes::tree_foliage_opaque);
		maxH = pos.y - cubeSize.y / 2;
		// dig the line to make it round
		glm::i32vec3 a = pos - cubeSize/2,
					 b = a;
		b.y += cubeSize.y -1 ;
		digLine(a, b, blockTypes::air);
		
		a.x += cubeSize.x-1;
		b = a;
		b.y += cubeSize.y;
		digLine(a, b, blockTypes::air);

		a.z += cubeSize.z-1;
		b = a;
		b.y += cubeSize.y;
		digLine(a, b, blockTypes::air);

		a.x -= (cubeSize.x-1);
		b = a;
		b.y += cubeSize.y;
		digLine(a, b, blockTypes::air);

		pos.y += cubeSize.y-1;
		cubeSize -= glm::i32vec3(2);
		cubeSize.y++;
		if (cubeSize.y < 1)
			cubeSize.y = 1;
	}
	//digRecursiveCube(pos, cubeSize);

	// on créé le tronc
	cubeSize += glm::i32vec3(2);
	maxH--;
	pos = glm::i32vec3(size/2, 0, size/2);
	si = glm::i32vec2(size % 2?1:2);
	for (int i = 0; i < maxH; i++)
	{
		digSquare(pos-glm::i32vec3(si.x/2, 0, si.y/2), si, blockTypes::tree);
		pos.y++;
	}

	// the lis in not valid anymore
	blockList.clear();
}

void Tree::digCube(const glm::i32vec3 &p, const glm::i32vec3 &si, blockTypes::T type)
{
	glm::i32vec3 pos = p - si / 2;
	for (int i = 0; i < si.y; i++)
	{
		pos.y = p.y - si.y / 2 + i;
		digSquare(pos, glm::i32vec2(si.x, si.z), type);
	}
}

void Tree::digRecursiveCube(const glm::i32vec3 &p, const glm::i32vec3 &si)
{
	glm::i32vec3 pos = p - si / 2;
	for (int i = 0; i < si.y; i++)
	{
		pos.y = p.y - si.y / 2 + i;
		digSquare(pos, glm::i32vec2(si.x, si.z), blockTypes::tree_foliage_opaque);
	}

	if (si.x <= 1 || si.y <= 1)
		return;

	digRecursiveCube(p + glm::i32vec3(si.x/2, si.y/2, si.z/2), si / 2);
	digRecursiveCube(p + glm::i32vec3(si.x/2, si.y/2, -si.z/2), si / 2);
	//digRecursiveCube(p + glm::i32vec3(si.x/2, -si.y/2, si.z/2), si / 2);
	//digRecursiveCube(p + glm::i32vec3(si.x/2, -si.y/2, -si.z/2), si / 2);
	digRecursiveCube(p + glm::i32vec3(-si.x/2, si.y/2, si.z/2), si / 2);
	digRecursiveCube(p + glm::i32vec3(-si.x/2, si.y/2, -si.z/2), si / 2);
	//digRecursiveCube(p + glm::i32vec3(-si.x/2, -si.y/2, si.z/2), si / 2);
	digRecursiveCube(p + glm::i32vec3(-si.x/2, -si.y/2, -si.z/2), si / 2);
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
	if (a == b)
	{
		array[TPOS(pixel)] = type; // air
		return;
	}

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
		digLine(a, a + glm::i32vec3(si.x-1, 0, 0), type);
	}
}

//#define CHUNKPOS(V) ((V).x + (CHUNK_N+2) * ((V).y + (V).z * (CHUNK_N+2)))
const std::list<std::pair<glm::i32vec3, blockTypes::T> >& Tree::generateList()
{
	if (!blockList.empty())
		return blockList;

	glm::i32vec3 center(glm::i32vec3(size)/2); // position in the Tree array
	center.y = 0;
	glm::i32vec3 p;

	for (int x = 0; x < size; x++)
		for(int y = 0; y < height; y++)
			for (int z = 0; z < size; z++)
			{
				p = glm::i32vec3(x, y, z);
				blockTypes::T type = array[TPOS(p)];
				if (type != blockTypes::air)
				{
					p-=center;
					blockList.push_back({p, type});
				}
			}

	return blockList;

}
