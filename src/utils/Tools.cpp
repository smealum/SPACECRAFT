#include "utils/Tools.h"
//#include <cstdlib>
//#include <cstdio>
//#include <sys/types.h>
//#include <sys/stat.h>

//#if defined(_WIN32) || defined(__WIN32__)
	//#include <direct.h>     // _mkdir
//#endif

bool createDir(const char *dir)
{
  return true;
	//bool success = false;

	//#if defined(_WIN32) || defined(__WIN32__)
		//success = _mkdir(dir) == 0;
	//#else
		//success = mkdir(dir, 0755) == 0;
	//#endif

	//if (!success && !dirExists(dir))
		//printf("Couldn't create the directory: %s\n", dir);

	//return success;
}

bool dirExists(const char *dir)
{
	//#if defined(_WIN32) || defined(__WIN32__)
		//struct _stat buf;
		//if(_stat(dir, &buf)==0)return (buf.st_mode & S_IFDIR) != 0;
	//#else
		//struct stat buf;
		//if(stat(dir, &buf)==0)return (buf.st_mode & S_IFDIR) != 0;
	//#endif
	return false;
}

