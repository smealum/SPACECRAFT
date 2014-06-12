#ifndef __TREE_H__
#define __TREE_H__

#include "utils/glm.h"
#include <noise/noise.h>
#include "noise/noiseutils.h"
#include "world/BlockType.h"
#include <vector>

class Tree {
	private:
		int seed;
		int height, size;

		blockTypes::T *array; // 3D flattened array

		void digLine(const glm::i32vec3 &a, const glm::i32vec3 &b, blockTypes::T type);
		// creuse sur le plan xz
		void digSquare(const glm::i32vec3 &p, const glm::i32vec2 &si, blockTypes::T type);

		// make sure you can access the array with v
		inline bool correct(glm::i32vec3 &v)
		{
			bool changed = false;
			if (v.x < 0) v.x = 0, changed = true;
			else if (v.x >= size) v.x = size-1, changed = true;
			if (v.y < 0) v.y = 0, changed = true;
			else if (v.y >= height) v.y = height-1, changed = true;
			if (v.z < 0) v.z = 0, changed = true;
			else if (v.z >= size) v.z = size-1, changed = true;
			return changed;
		}

	public:
		Tree();
		~Tree();

		void generate();

		inline int getHeight() const { return height; }
		inline int getSize() const { return size; }

		inline blockTypes::T getBlock(int x, int y, int z)
		{
			return array[x + size * (y + z * height)];
		}
};

#endif
