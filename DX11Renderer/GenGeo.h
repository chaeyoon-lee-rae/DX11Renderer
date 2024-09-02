#pragma once

#include <vector>
#include "Vertex.h"

struct MeshInfo {
    std::vector<Vertex> vertices;
    std::vector<uint16_t> indices;
};

class GenGeo {
  public:
    static MeshInfo MakeBox(const float width, const float height, const int numSplit,
                            const int order);
};

