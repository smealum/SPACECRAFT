#ifndef CAMERAKEYBOARD_FEF6DBSV
#define CAMERAKEYBOARD_FEF6DBSV

#include "render/CameraManager.h"

class CameraKeyboard : public CameraManager {
 public:
  virtual void update(Camera& camera);
  float speed = 80;
};

#endif /* end of include guard: CAMERAKEYBOARD_FEF6DBSV */
