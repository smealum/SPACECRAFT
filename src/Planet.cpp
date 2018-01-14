#include <cstdio>

#include "Application.h"
#include "MiniWorld.h"
#include "Planet.h"
#include "data/ContentHandler.h"
#include "glm/gtc/noise.hpp"
#include "render/Atmosphere.h"
#include "render/TileTexture.h"
#include "utils/dbg.h"
#include "utils/positionMath.h"
#include "world/BlockType.h"
#include "world/BlockType.h"

extern float PlanetFaceDetailsPower;

PlanetFace::PlanetFace(Planet* planet, glm::vec3 v[4], uint8_t id, int size)
    : planet(planet),
      father(NULL),
      toplevel(this),
      sons{NULL, NULL, NULL, NULL},
      tptr(new TrackerPointer<PlanetFace>(this, true)),
      miniworld(NULL),
      faceBuffer(NULL),
      waterBuffer(NULL),
      noBuffer(false),
      elevated(false),
      x(0),
      z(0),
      bufferID{-1, -1},
      elevation(1.0f),
      id(5 + id),
      depth(size - 1),
      size(0),
      isDisplayOk(false) {
  minElevation = elevation - 0.01f;
  childrenDepth = depth;
  uvertex[0] = v[0];
  uvertex[1] = v[1];
  uvertex[2] = v[2];
  uvertex[3] = v[3];
  finalize();
}

PlanetFace::PlanetFace(Planet* planet, PlanetFace* father, uint8_t id)
    : planet(planet),
      father(father),
      sons{NULL, NULL, NULL, NULL},
      miniworld(NULL),
      tptr(new TrackerPointer<PlanetFace>(this, true)),
      faceBuffer(NULL),
      waterBuffer(NULL),
      noBuffer(false),
      elevated(false),
      toplevel(father->toplevel),
      bufferID{-1, -1},
      elevation(1.0f),
      id(id),
      childrenDepth(0),
      isDisplayOk(false) {
  // Build the planetface from parent.
  //
  //   0-5-1
  //   |\ /|
  //   8-4-6
  //   |/ \|
  //   3-7-2

  //   [0 1]
  //   |   |
  //   [2 3]

  switch (id) {
    case 0:
      uvertex[0] = father->uvertex[0];
      uvertex[1] = father->uvertex[5];
      uvertex[2] = father->uvertex[4];
      uvertex[3] = father->uvertex[8];
      x = father->x * 2 + 0;
      z = father->z * 2 + 0;
      break;
    case 1:
      uvertex[0] = father->uvertex[5];
      uvertex[1] = father->uvertex[1];
      uvertex[2] = father->uvertex[6];
      uvertex[3] = father->uvertex[4];
      x = father->x * 2 + 1;
      z = father->z * 2 + 0;
      break;
    case 2:
      uvertex[0] = father->uvertex[8];
      uvertex[1] = father->uvertex[4];
      uvertex[2] = father->uvertex[7];
      uvertex[3] = father->uvertex[3];
      x = father->x * 2 + 0;
      z = father->z * 2 + 1;
      break;
    case 3:
      uvertex[0] = father->uvertex[4];
      uvertex[1] = father->uvertex[6];
      uvertex[2] = father->uvertex[2];
      uvertex[3] = father->uvertex[7];
      x = father->x * 2 + 1;
      z = father->z * 2 + 1;
      break;
    default:
      // TODO : exception ?
      break;
  }

  depth = father->depth + 1;
  size = father->size + 1;

  finalize();
}

PlanetFace::~PlanetFace() {
  if (faceBuffer)
    delete faceBuffer;
  removeMiniWorld();
}

void PlanetFace::deletePlanetFace(PlanetFaceBufferHandler* b,
                                  PlanetFaceBufferHandler* w) {
  if (b)
    b->deleteFace(this);
  if (w)
    w->deleteFace(this);

  // delete children
  for (int i = 0; i < 4; i++)
    if (sons[i])
      sons[i]->deletePlanetFace(b, w);

  // inform father
  if (father) {
    father->sons[id] = NULL;
    tptr->release();
  }
}

TrackerPointer<PlanetFace>* PlanetFace::getTptr(void) {
  return tptr;
}

void PlanetFace::finalize(void) {
  uvertex[4] = (uvertex[0] + uvertex[1] + uvertex[2] + uvertex[3]) * 0.25f;
  uvertex[5] = (uvertex[0] + uvertex[1]) * 0.5f;
  uvertex[6] = (uvertex[1] + uvertex[2]) * 0.5f;
  uvertex[7] = (uvertex[2] + uvertex[3]) * 0.5f;
  uvertex[8] = (uvertex[3] + uvertex[0]) * 0.5f;

  for (int i = 0; i < 9; i++)
    vertex[i] = glm::normalize(uvertex[i]);
  for (int i = 0; i < 4; i++)
    box[i] = vertex[i] * 0.95f;
  for (int i = 0; i < 4; i++)
    box[i + 4] = vertex[i] * 2.0f;

  planet->handler.requestContent(
      new PlanetElevationRequest(*planet, *this, vertex[4]));
}

// mise a jour de l'elevation, de la température et de l'humidité
void PlanetFace::updateElevation(float e, blockTypes::T t) {
  elevation = e;
  tile = t;
  minElevation = e - 2.0f / (1 << depth);
  elevated = true;
}

int randomSource = 4;
#define PLANET_ALTITUDETHRESHOLD (1.005f)

bool PlanetFace::shouldHaveMiniworld(Camera& c) {
  // if(planet->numMiniWorlds()>0 && !miniworld)return false; //TEMP DEBUG
  if (depth == MINIWORLD_DETAIL) {
    if (miniworld) {
      glm::vec3 p = planet->getCameraRelativePosition(c);

      // on ne prend plus en compte l'élévation, c'est fait exprès
      p = glm::normalize(p) *
          std::max(PLANET_ALTITUDETHRESHOLD, glm::length(p));
      const float l = glm::length(vertex[4] * PLANET_ALTITUDETHRESHOLD - p);
      return l * (2 << (depth)) < 20.0f * planet->planetInfo->size;
    } else {
      return (childrenDepth >= PLANET_ADDED_DETAIL);
    }
  }
  return false;
}

bool PlanetFace::isDetailedEnough(Camera& c) {
  if (depth > MINIWORLD_DETAIL + PLANET_ADDED_DETAIL + 1)
    return true;
  if (depth < 4)
    return false;

  glm::vec3 p = planet->getCameraRelativePosition(c);

  if (glm::dot(vertex[0] * 0.97f - p, vertex[0]) > 0.0f &&
      glm::dot(vertex[1] * 0.97f - p, vertex[1]) > 0.0f &&
      glm::dot(vertex[2] * 0.97f - p, vertex[2]) > 0.0f &&
      glm::dot(vertex[3] * 0.97f - p, vertex[3]) > 0.0f &&
      glm::dot(vertex[4] * 0.97f - p, vertex[4]) > 0.0f)
    return true;  // backface culling

  // if(depth<4)return false;

  //if(!c.isBoxInFrustum(box, 8, glm::mat4(1)))return true; //frustum culling

  // if (!sons[0])
  // if(!c.isPointInFrustum(vertex[4]))
  // return true; //frustum culling
  // float d=2.0f/(1<<(depth-3));
  // float d=2.0f/(1<<(depth-2));
  // if(length(vertex[4]*elevation-p)/d<1.2f)return false;
  // if(length(vertex[4]*elevation-p)*(2<<(depth))<40.0f) return false;
  // if(glm::length(vertex[4]*elevation-p)*(2<<(depth-1))<PlanetFaceDetailsPower)
  // return false;

  // on ne prend plus en compte l'élévation, c'est fait exprès
  p = glm::normalize(p) * std::max(PLANET_ALTITUDETHRESHOLD, glm::length(p));
  const float l = glm::length(vertex[4] * PLANET_ALTITUDETHRESHOLD - p);
  if (l * (1 << depth) <
      PlanetFaceDetailsPower * planet->planetInfo->size)
    return false;
  return true;
}

void PlanetFace::createMiniWorld(void) {
  if (miniworld)
    return;

  miniworld = new MiniWorld(planet, this);
  planet->addMiniWorld(miniworld);
}

void PlanetFace::removeMiniWorld(void) {
  if (!miniworld)
    return;

  planet->removeMiniWorld(miniworld);
  miniworld->destroyMiniWorld();
  miniworld = NULL;
}

PlanetFace* PlanetFace::getTopLevel(void) {
  return toplevel;
}

int PlanetFace::getID(void) {
  return id;
}

static inline int max(int a, int b) {
  return (a > b) ? a : b;
}

void PlanetFace::processLevelOfDetail(Camera& c,
                                      PlanetFaceBufferHandler* b,
                                      PlanetFaceBufferHandler* w) {
  bool detailedEnough = isDetailedEnough(c);

  if (faceBuffer) {
    if (b)
      printf("ERROR1 %d\n", depth);
    b = faceBuffer;
  } else if (!b && depth == 2)
    b = faceBuffer =
        new PlanetFaceBufferHandler(*this, PFBH_MAXSIZE, getV1(), getV2());

  if (waterBuffer) {
    if (w)
      printf("ERROR1.5 %d\n", depth);
    w = waterBuffer;
  } else if (!w && depth == 2)
    w = waterBuffer = new PlanetFaceBufferHandler(*this, PFBH_MAXSIZE, getV1(),
                                                  getV2(), 1, 0.9, true);

  // update childrenDepth
  childrenDepth = 0;
  for (int i = 0; i < 4; i++)
    if (sons[i])
      childrenDepth = max(childrenDepth, (sons[i]->childrenDepth + 1));

  // update isDisplayOk
  // isDisplayOk est vrai si on affiche une face ou bien un miniworld ou que
  // tout les enfants
  // sont présents et on isDisplayOk.
  isDisplayOk = isDrawingFace() || (miniworld && miniworld->isGenerated());
  if (!isDisplayOk) {
    isDisplayOk = true;
    for (int i = 0; i < 4; i++) {
      if ((!sons[i]) || !(sons[i]->isDisplayOk)) {
        isDisplayOk = false;
        break;
      }
    }
  }

  // face assez détaillé, on l'affiche
  if (detailedEnough) {
    // dessin de la face
    if (elevated) {
      if (b)
        b->addFace(this);
      else {
        printf("ERROR2 %d\n", depth);
      }
      if (w && elevation < planet->planetInfo->waterLevelElevation)
        w->addFace(this, planet->planetInfo->waterLevelElevation,
                   blockTypes::water);

      // suppresion des éventuels miniWorlds si on a la face qui s'affiche
      removeMiniWorld();

      // suppression des éventuels enfants
      for (int i = 0; i < 4; i++)
        if (sons[i])
          sons[i]->deletePlanetFace(b, w);
    }
  } else {
    // creation/destruction du miniWorld
    if (shouldHaveMiniworld(c)) {
      // creation du miniworld
      createMiniWorld();

      // effacement des enfants et de l'affichage de la face
      if (miniworld && miniworld->isGenerated()) {
        // suppression de la face
        if (b)
          b->deleteFace(this);
        if (w)
          w->deleteFace(this);
        // suppression des éventuels enfants
        for (int i = 0; i < 4; i++)
          if (sons[i])
            sons[i]->deletePlanetFace(b, w);
      }
    } else {
      // ajout des éventuels enfants
      bool done = true;
      if (elevated) {
        for (int i = 0; i < 4; i++) {
          if (!sons[i])
            sons[i] = new PlanetFace(planet, this, i);
          else
            sons[i]->processLevelOfDetail(c, b, w);

          done &= (sons[i]->isDisplayOk);
        }
      }

      // on peux ne plus afficher la face si les enfants affichent quelque
      // chose.
      if (done) {
        if (b)
          b->deleteFace(this);
        if (w)
          w->deleteFace(this);
        removeMiniWorld();
      }
    }
  }
}

glm::vec3 cubeArray[6][4] = {
    // bottom
    {glm::vec3(-1.0, -1.0, -1.0), glm::vec3(-1.0, -1.0, 1.0),
     glm::vec3(1.0, -1.0, 1.0), glm::vec3(1.0, -1.0, -1.0)},
    // top
    {glm::vec3(1.0, 1.0, 1.0), glm::vec3(-1.0, 1.0, 1.0),
     glm::vec3(-1.0, 1.0, -1.0), glm::vec3(1.0, 1.0, -1.0)},
    // left
    {glm::vec3(-1.0, 1.0, 1.0), glm::vec3(-1.0, -1.0, 1.0),
     glm::vec3(-1.0, -1.0, -1.0), glm::vec3(-1.0, 1.0, -1.0)},
    // right
    {glm::vec3(1.0, -1.0, -1.0), glm::vec3(1.0, -1.0, 1.0),
     glm::vec3(1.0, 1.0, 1.0), glm::vec3(1.0, 1.0, -1.0)},
    // near
    {glm::vec3(1.0, 1.0, -1.0), glm::vec3(-1.0, 1.0, -1.0),
     glm::vec3(-1.0, -1.0, -1.0), glm::vec3(1.0, -1.0, -1.0)},
    // far
    {glm::vec3(-1.0, -1.0, 1.0), glm::vec3(-1.0, 1.0, 1.0),
     glm::vec3(1.0, 1.0, 1.0), glm::vec3(1.0, -1.0, 1.0)}};

Planet::Planet(PlanetInfo* pi, ContentHandler& ch, std::string name)
    : planetInfo(pi),
      handler(ch),
      name(name),
      sunPosition(0),
      position(0),
      angle(0.0),
      scale(1.0f / (1 << (pi->size - 1))),
      size(pi->size) {
  for (int i = 0; i < 6; i++) {
    faces[i] = new PlanetFace(this, cubeArray[i], i, size);
  }

  if (pi->atmosphereInfo) {
    atmosphere = std::make_unique<Atmosphere>(pi->atmosphereInfo);
  }
}

void Planet::processLevelOfDetail(Camera& c) {
  if (!c.isBoxInFrustum(position - glm::vec3(1), glm::vec3(2, 0, 0),
                        glm::vec3(0, 2, 0), glm::vec3(0, 0, 2)))
    return;
  if (glm::length(c.getPosition(position)) > 2e2)
    return;
  for (int i = 0; i < 6; i++)
    faces[i]->processLevelOfDetail(c, NULL, NULL);
}

PlanetFaceBufferHandler::PlanetFaceBufferHandler(PlanetFace& pf,
                                                 int ms,
                                                 glm::vec3 v1,
                                                 glm::vec3 v2,
                                                 int index,
                                                 float alpha,
                                                 bool water)
    : planetFace(pf),
      max_size(ms),
      current_size(0),
      current_capacity(PFBH_MINCAP),
      v1(glm::normalize(v1)),
      v2(glm::normalize(v2)),
      vbo(0),
      vao(0),
      index(index),
      shader(ShaderProgram::loadFromFile("shader/CubeVertex/CubeVertex.vert",
                                         "shader/CubeVertex/CubeVertex.frag",
                                         "CubeVertex")),
      vertex_per_cube(water ? 6 : 30),
      water(water) {
  resizeVBO();
}

PlanetFaceBufferHandler::~PlanetFaceBufferHandler() {
  for (auto it = faces.begin(); it != faces.end(); ++it)
    (*it)->bufferID[index] = -1;

  if (vbo && vao) {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &vao);
    vbo = 0;
    vao = 0;
  }
}

int PlanetFaceBufferHandler::getSize(void) {
  return current_size;
}

void PlanetFaceBufferHandler::resizeVBO(void) {
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER,
               current_capacity * vertex_per_cube * sizeof(CubeVertex), NULL,
               GL_STATIC_DRAW);
  if (current_size != 0) {
    glBufferSubData(GL_ARRAY_BUFFER, 0,
                    current_size * vertex_per_cube * sizeof(CubeVertex),
                    (void*)&buffer[0]);
  }

  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  shader.setBuffers(vao, vbo, 0);
  shader.use();

  CubeVertex::SetAttributes(&shader);
}

// TODO : grouper les glBufferSubData de façon intelligente à chaque frame
// (glBufferSubData individuels pour les delete, mais groupé pour les add en
// queue ?)

void PlanetFaceBufferHandler::addFace(PlanetFace* pf) {
  float numblocks = pf->planet->getNumBlocks();
  addFace(pf, int(pf->elevation * numblocks - 1.0) / numblocks, pf->tile);
}

void PlanetFaceBufferHandler::addFace(PlanetFace* pf,
                                      float elevation,
                                      blockTypes::T tile) {
  if (current_size >= max_size || pf->bufferID[index] >= 0)
    return;

  if (current_size >= current_capacity) {
    while (current_size >= current_capacity)
      current_capacity *= 2;
    resizeVBO();
  }

  const glm::vec3 n = pf->uvertex[4];

  float topTile = getBlockID(tile, blockPlane::top) + 0.5;
  float sideTile = getBlockID(tile, blockPlane::side) + 0.5;

  faces.push_back(pf);

  /*
          0────1
         /│   /│
        3────2 │
        │ │  │ │
        │ 4──│─5
        │/   │/
        7────6
  */

  const float size = (1 << pf->size);
  const float inv_size = 1.f / size;
  glm::vec3 position[8] = {
      elevation * glm::normalize(n + inv_size * (-v1 + v2)),
      elevation * glm::normalize(n + inv_size * (v1 + v2)),
      elevation * glm::normalize(n + inv_size * (v1 - v2)),
      elevation * glm::normalize(n + inv_size * (-v1 - v2)),
      pf->minElevation * glm::normalize(n + inv_size * (-v1 + v2)),
      pf->minElevation * glm::normalize(n + inv_size * (v1 + v2)),
      pf->minElevation * glm::normalize(n + inv_size * (v1 - v2)),
      pf->minElevation * glm::normalize(n + inv_size * (-v1 - v2)),
  };

  // Always drawn
  {
    // Top
    buffer.push_back({position[0], position[0], {0.f, 0.f}, topTile});
    buffer.push_back({position[1], position[1], {1.f, 0.f}, topTile});
    buffer.push_back({position[2], position[2], {1.f, 1.f}, topTile});
    buffer.push_back({position[0], position[0], {0.f, 0.f}, topTile});
    buffer.push_back({position[2], position[2], {1.f, 1.f}, topTile});
    buffer.push_back({position[3], position[3], {0.f, 1.f}, topTile});
  }

  if (!water) {
    // normales

    const glm::vec3 n_left = glm::cross(position[3] - position[4],
                                        position[7] - position[0]);
    const glm::vec3 n_down = glm::cross(position[2] - position[7],
                                        position[7] - position[3]);

    const glm::vec3 n_right = -n_left;
    const glm::vec3 n_up = -n_down;

    // Left
    buffer.push_back({position[4], n_left, {0.f, 1.f}, sideTile});
    buffer.push_back({position[0], n_left, {0.f, 0.f}, sideTile});
    buffer.push_back({position[3], n_left, {1.f, 0.f}, sideTile});
    buffer.push_back({position[4], n_left, {0.f, 1.f}, sideTile});
    buffer.push_back({position[3], n_left, {1.f, 0.f}, sideTile});
    buffer.push_back({position[7], n_left, {1.f, 1.f}, sideTile});

    // Right
    buffer.push_back({position[6], n_right, {0.f, 1.f}, sideTile});
    buffer.push_back({position[2], n_right, {0.f, 0.f}, sideTile});
    buffer.push_back({position[1], n_right, {1.f, 0.f}, sideTile});
    buffer.push_back({position[6], n_right, {0.f, 1.f}, sideTile});
    buffer.push_back({position[1], n_right, {1.f, 0.f}, sideTile});
    buffer.push_back({position[5], n_right, {1.f, 1.f}, sideTile});

    // Up
    buffer.push_back({position[5], n_up, {0.f, 1.f}, sideTile});
    buffer.push_back({position[1], n_up, {0.f, 0.f}, sideTile});
    buffer.push_back({position[0], n_up, {1.f, 0.f}, sideTile});
    buffer.push_back({position[5], n_up, {0.f, 1.f}, sideTile});
    buffer.push_back({position[0], n_up, {1.f, 0.f}, sideTile});
    buffer.push_back({position[4], n_up, {1.f, 1.f}, sideTile});

    // Down
    buffer.push_back({position[7], n_down, {0.f, 1.f}, sideTile});
    buffer.push_back({position[3], n_down, {0.f, 0.f}, sideTile});
    buffer.push_back({position[2], n_down, {1.f, 0.f}, sideTile});
    buffer.push_back({position[7], n_down, {0.f, 1.f}, sideTile});
    buffer.push_back({position[2], n_down, {1.f, 0.f}, sideTile});
    buffer.push_back({position[6], n_down, {1.f, 1.f}, sideTile});
  }

  glBindBuffer(GL_ARRAY_BUFFER, vbo);

  glBufferSubData(GL_ARRAY_BUFFER,
                  current_size * sizeof(CubeVertex) * vertex_per_cube,
                  sizeof(CubeVertex) * vertex_per_cube,
                  (void*)&buffer[vertex_per_cube * current_size]);
  pf->bufferID[index] = current_size;
  current_size++;

  pf->isDisplayOk = true;
}

void PlanetFaceBufferHandler::deleteFace(PlanetFace* pf) {
  const int i = pf->bufferID[index];
  if (i >= current_size || i < 0)
    return;

  if (faces.size() > 1) {
    // Move the last element into 'i'.
    faces[i] = faces[current_size - 1];
    faces[i]->bufferID[index] = i;
    for (int j = 0; j < vertex_per_cube; ++j) {
      buffer[vertex_per_cube * i + j] =
          buffer[vertex_per_cube * (current_size - 1) + j];
    }

    // Update the OpenGL buffer.
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferSubData(GL_ARRAY_BUFFER, vertex_per_cube * sizeof(CubeVertex) * i,
                    vertex_per_cube * sizeof(CubeVertex),
                    (void*)&buffer[vertex_per_cube * i]);
  }

  // Remove the last element.
  pf->bufferID[index] = -1;
  faces.pop_back();
  for (int j = 0; j < vertex_per_cube; ++j) {
    buffer.pop_back();
  }
  current_size--;
  pf->isDisplayOk = false;
}

void PlanetFaceBufferHandler::draw(Camera& c, glm::vec3 lightdir) {
  if (current_size <= 0)
    return;
  shader.use();
  c.updateCamera(shader);

  shader.setUniform("lightdir", lightdir);

  glm::vec3 planet_position = planetFace.planet->position - c.getReference();
  glm::mat4 model = glm::translate(glm::mat4(1.0), planet_position) *
                    glm::mat4(planetFace.planet->model);
  shader.setUniform("model", model);

  // planetface_atmosphere test
  if (planetFace.planet->atmosphere) {
    planetFace.planet->atmosphere->bind(c, lightdir,
                                        planetFace.planet->position,
                                        planetFace.planet->scale, shader);
  }

  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);

  // bind la texture
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D_ARRAY, TileTexture::getInstance().get());
  shader.setUniform("Texture", 0);

  glDrawArrays(GL_TRIANGLES, 0,
               current_size * vertex_per_cube);
}

void PlanetFace::draw(Camera& c, glm::vec3 lightdir, bool water) {
  if (!c.isBoxInFrustum(box, 8, planet->getFullModel(c)))
    return;  // frustum culling
  if ((!faceBuffer && !water) || (!waterBuffer && water) || noBuffer) {
    if (sons[0])
      for (int i = 0; i < 4; i++)
        sons[i]->draw(c, lightdir, water);
  } else {
    if (water && waterBuffer)
      waterBuffer->draw(c, lightdir);
    else if (!water && faceBuffer)
      faceBuffer->draw(c, lightdir);
  }
}

void Planet::draw(Camera& camera, bool atmo) {
  if (!camera.isBoxInFrustum(position - glm::vec3(1), glm::vec3(2, 0, 0),
                             glm::vec3(0, 2, 0), glm::vec3(0, 0, 2)))
    return;

  lightdir = glm::normalize(sunPosition - position);

  if (atmo) {
    if (atmosphere)
      atmosphere->draw(camera, lightdir, position, scale);
  } else {
    for (int i = 0; i < 6; i++)
      faces[i]->draw(camera, lightdir);
    for (auto it(miniWorldList.begin()); it != miniWorldList.end(); ++it)
      (*it)->draw(camera);
    for (int i = 0; i < 6; i++)
      faces[i]->draw(camera, lightdir, true);
    for (auto it(miniWorldList.begin()); it != miniWorldList.end(); ++it)
      (*it)->draw(camera, false);
    // dessin des nuages
    // cloud.draw(c);
  }
}

glm::vec3 Planet::getCameraRelativePosition(Camera& c) {
  return invModel * c.getPosition(position);
}

glm::dvec3 Planet::getCameraRelativeDoublePosition(Camera& c) {
  return glm::dmat3(invModel) * c.getPositionDouble(glm::dvec3(position));
}

PlanetFace& Planet::getTopLevelForCamera(Camera& c) {
  glm::vec3 p = getCameraRelativePosition(c);
  for (int i = 0; i < 6; i++) {
    glm::vec3 p2 =
        (p / std::fabs(glm::dot(faces[i]->getN(), p))) - faces[i]->getOrigin();
    float vv1 = glm::dot(p2, faces[i]->getV1());
    float vv2 = glm::dot(p2, faces[i]->getV2());
    if (fabs(glm::dot(faces[i]->getN(), p2)) <= 1e-6 && vv1 >= 0.0f &&
        vv1 <= 4.0f && vv2 >= 0.0f && vv2 <= 4.0f)
      return *faces[i];
  }
  printf("ERROR ERROR\n");
  return *faces[0];
}

glm::mat4 Planet::getFullModel(Camera& c) {
  return glm::translate(glm::mat4(1), position - c.getReference()) *
         glm::mat4(model);
}

glm::mat3 Planet::getModel(void) {
  return model;
}

glm::mat3 Planet::getInvModel(void) {
  return invModel;
}

int Planet::getNumBlocks(void) {
  return planetInfo->numBlocks;
}

void Planet::addMiniWorld(MiniWorld* mw) {
  miniWorldList.push_back(mw);
}

void Planet::removeMiniWorld(MiniWorld* mw) {
  miniWorldList.remove(mw);
}

int Planet::numMiniWorlds(void) {
  return miniWorldList.size();
}

bool Planet::collidePoint(glm::dvec3 p, glm::dvec3 v, glm::dvec3& out) {
  // TODO : vrai raymarching sur les miniworlds/chunks.
  //(mais en attendant, ça ça marche et bouffe pas tant que ça pour des faibles
  // vitesses vu que de toute façon on cull les miniworlds)
  //(parce qu'on n'est pas complètement débile quand même, juste un peu
  // flemmard)
  bool gret = false, ret;
  do {
    ret = false;
    for (auto it(miniWorldList.begin()); it != miniWorldList.end(); ++it)
      ret = ret || (*it)->collidePoint(p, v);
    gret = ret || gret;
  } while (ret && glm::length(v) > 1e-6);
  out = p + v;
  return gret;
}

Chunk* Planet::selectBlock(glm::dvec3 p,
                           glm::dvec3 v,
                           glm::i32vec3& out,
                           int& dir) {
  // TODO : pareil que collidePoint
  // printf("in %f %f %f\n",p.x,p.y,p.z);
  for (int i = 0; i < 3; i++) {
    for (auto it(miniWorldList.begin()); it != miniWorldList.end(); ++it) {
      glm::dvec3 out2 = p;
      bool done = false;
      Chunk* ret = (*it)->selectBlock(p, v, out, out2, done, dir);
      if (ret)
        return ret;
      else if (done)
        return NULL;
      v -= out2 - p;
      p = out2;
    }
  }
  // printf("out %f %f %f\n",p.x,p.y,p.z);
  return NULL;
}

glm::dvec3 Planet::getGravityVector(glm::dvec3 p) {
  return glm::normalize(p - glm::dvec3(position));
}

glm::vec3 Planet::getPosition(void) {
  return position;
}

void Planet::setSunPosition(glm::vec3 p) {
  sunPosition = p;
}

// TODO : filtrer par toplevel (en pratique devrait pas être nécessaire, mais on
// sait jamais)
void Planet::changeBlock(glm::i32vec3 p, blockTypes::T v) {
  for (auto it(miniWorldList.begin()); it != miniWorldList.end(); ++it)
    (*it)->changeBlock(p, v);
}

void Planet::deleteBlock(glm::i32vec3 p) {
  changeBlock(p, blockTypes::air);
}

void Planet::update(float time) {
  if (planetInfo->trajectory)
    position = planetInfo->trajectory->getPosition(time);

  angle = time * 2 * PI / planetInfo->period;

  model = glm::mat3(glm::rotate(glm::mat4(1.0f), angle, planetInfo->axis));
  invModel = glm::transpose(model) / scale;
  model *= scale;
}

glm::vec3 PlanetFace::getOrigin(void) {
  return uvertex[0];
}

glm::vec3 PlanetFace::getV1(void) {
  return uvertex[1] - uvertex[0];
}

glm::vec3 PlanetFace::getV2(void) {
  return uvertex[3] - uvertex[0];
}

glm::vec3 PlanetFace::getN(void) {
  return vertex[4];
}

std::string Planet::getName(void) {
  return name;
}
