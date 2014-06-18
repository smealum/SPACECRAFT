#include "noise/Tree.h"
#include "utils/dbg.h"

int main(void)
{
    Tree tree;

	tree.generate(1);

	debug("size: %d, height: %d", tree.getSize(), tree.getHeight());

	// check if values are ok
	bool check = false;
	for (int x = 0; x < tree.getSize(); x++)
		for (int y = 0; y < tree.getHeight(); y++)
			for (int z = 0; z < tree.getSize(); z++)
            {
                blockTypes::T ty = tree.getBlock(x, y, z);
				if (ty != blockTypes::air)
                {
					check = true;
                    if (ty != blockTypes::tree &&
                            ty != blockTypes::tree_top &&
                            ty != blockTypes::tree_foliage &&
                            ty != blockTypes::tree_foliage_opaque &&
                            ty != blockTypes::air)
                    {
                        log_err("unexpected blockType at (%d, %d, %d): %u", x, y, z, ty);
                        return 1;
                    }
                }
            }

	assert(check);

	return 0;
}
