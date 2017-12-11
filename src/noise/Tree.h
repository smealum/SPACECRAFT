#ifndef __TREE_H__
#define __TREE_H__

#ifndef __EMSCRITEN__
  #include <noise/noise.h>
#endif
#include <vector>

#include "noise/noiseutils.h"
#include "utils/glm.h"
#include "world/BlockType.h"

class Tree {
	private:
		int seed;
		int height, size;

		//generation variables
		int minHeight;
		int minSize;
		int halfRandomHeight;
		int halfRandomSize;

		// list of the blocks
		// pos relative to center -> block type
		std::list<std::pair<glm::i32vec3, blockTypes::T> > blockList;

		blockTypes::T *array; // 3D flattened array
#ifndef __EMSCRIPTEN__
		noise::module::Perlin rnd;
#endif

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

		void digRecursiveCube(const glm::i32vec3 &p, const glm::i32vec3 &si);
		void digCube(const glm::i32vec3 &p, const glm::i32vec3 &si, blockTypes::T type);

	public:

		inline int getHalfRandomSize() const { return halfRandomSize; }
		inline void setHalfRandomSize(int v) { halfRandomSize = v; }

		inline int getHalfRandomHeight() const { return halfRandomHeight; }
		inline void setHalfRandomHeight(int v) { halfRandomHeight = v; }

		inline int getMinSize() const { return minSize; }
		inline void setMinSize(int v) { minSize = v; }

		inline void setMinHeight(int v) { minHeight = v; }
		inline int getMinHeight() const { return minHeight; }

		Tree();
		~Tree();

		void generate(int seed);

		inline int getHeight() const { return height; }
		inline int getSize() const { return size; }

		inline blockTypes::T getBlock(int x, int y, int z)
		{
			return array[x + size * (y + z * height)];
		}

		// generate the list of blocks to add
		// pos is the offset used to calculate the index in the falttened array
		const std::list<std::pair<glm::i32vec3, blockTypes::T> >& generateList();

};

#endif
