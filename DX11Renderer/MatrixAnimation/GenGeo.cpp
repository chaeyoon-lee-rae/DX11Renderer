#include "GenGeo.h"

using std::vector;
using DirectX::SimpleMath::Vector2;
using DirectX::SimpleMath::Vector3;

MeshInfo GenGeo::MakeBox(const float width, const float height, const int numSplit,
                         const int order) {
    const float splitHeight = height / float(numSplit);

    vector<Vector3> pos;
    vector<Vector3> normal;
    vector<Vector2> uv; // ÅØ½ºÃç ÁÂÇ¥

    const float wHalf = width / 2.0f;
    const float hSplit = splitHeight / 2.0f;

    Vector3 v1 = Vector3(-wHalf, hSplit, -wHalf);
    Vector3 v2 = Vector3(wHalf, hSplit, -wHalf);
    Vector3 v3 = Vector3(wHalf, hSplit, wHalf);
    Vector3 v4 = Vector3(-wHalf, hSplit, wHalf);
    Vector3 v5 = Vector3(-wHalf, -hSplit, -wHalf);
    Vector3 v6 = Vector3(wHalf, -hSplit, -wHalf);
    Vector3 v7 = Vector3(wHalf, -hSplit, wHalf);
    Vector3 v8 = Vector3(-wHalf, -hSplit, wHalf);

    Vector3 normalTop = Vector3(0.0f, 1.0f, 0.0f);
    Vector3 normalBot = Vector3(0.0f, -1.0f, 0.0f);
    Vector3 normalFront = Vector3(0.0f, 0.0f, -1.0f);
    Vector3 normalBack = Vector3(0.0f, 0.0f, 1.0f);
    Vector3 normalLeft = Vector3(-1.0f, 0.0f, 0.0f);
    Vector3 normalRight = Vector3(1.0f, 0.0f, 0.0f);

    const float vOffset = (1.0f / float(numSplit));

    Vector2 uv1 = Vector2(0.0f, 0.0f);
    Vector2 uv2 = Vector2(1.0f, 0.0f);
    Vector2 uv3 = Vector2(1.0f, 1.0f);
    Vector2 uv4 = Vector2(0.0f, 1.0f);

    Vector2 uvSide1 = Vector2(0.0f, vOffset * order);
    Vector2 uvSide2 = Vector2(1.0f, vOffset * order);
    Vector2 uvSide3 = Vector2(1.0f, vOffset * (order + 1));
    Vector2 uvSide4 = Vector2(0.0f, vOffset * (order + 1));

    // À­¸é
    pos.push_back(v4);
    pos.push_back(v3);
    pos.push_back(v2);
    pos.push_back(v1);
    normal.push_back(normalTop);
    normal.push_back(normalTop);
    normal.push_back(normalTop);
    normal.push_back(normalTop);
    uv.push_back(uv1);
    uv.push_back(uv2);
    uv.push_back(uv3);
    uv.push_back(uv4);

    // ¾Æ·§¸é
    pos.push_back(v7);
    pos.push_back(v8);
    pos.push_back(v5);
    pos.push_back(v6);
    normal.push_back(normalBot);
    normal.push_back(normalBot);
    normal.push_back(normalBot);
    normal.push_back(normalBot);
    uv.push_back(uv1);
    uv.push_back(uv2);
    uv.push_back(uv3);
    uv.push_back(uv4);

    // ¾Õ¸é
    pos.push_back(v1);
    pos.push_back(v2);
    pos.push_back(v6);
    pos.push_back(v5);
    normal.push_back(normalFront);
    normal.push_back(normalFront);
    normal.push_back(normalFront);
    normal.push_back(normalFront);
    uv.push_back(uvSide1);
    uv.push_back(uvSide2);
    uv.push_back(uvSide3);
    uv.push_back(uvSide4);

    // µÞ¸é
    pos.push_back(v3);
    pos.push_back(v4);
    pos.push_back(v8);
    pos.push_back(v7);
    normal.push_back(normalBack);
    normal.push_back(normalBack);
    normal.push_back(normalBack);
    normal.push_back(normalBack);
    uv.push_back(uvSide1);
    uv.push_back(uvSide2);
    uv.push_back(uvSide3);
    uv.push_back(uvSide4);

    // ¿ÞÂÊ
    pos.push_back(v4);
    pos.push_back(v1);
    pos.push_back(v5);
    pos.push_back(v8);
    normal.push_back(normalLeft);
    normal.push_back(normalLeft);
    normal.push_back(normalLeft);
    normal.push_back(normalLeft);
    uv.push_back(uvSide1);
    uv.push_back(uvSide2);
    uv.push_back(uvSide3);
    uv.push_back(uvSide4);

    // ¿À¸¥ÂÊ
    pos.push_back(v2);
    pos.push_back(v3);
    pos.push_back(v7);
    pos.push_back(v6);
    normal.push_back(normalRight);
    normal.push_back(normalRight);
    normal.push_back(normalRight);
    normal.push_back(normalRight);
    uv.push_back(uvSide1);
    uv.push_back(uvSide2);
    uv.push_back(uvSide3);
    uv.push_back(uvSide4);

    MeshInfo mi;
    for (size_t i = 0; i < pos.size(); i++) {
        Vertex v;
        v.pos = pos[i];
        v.normal = normal[i];
        v.uv = uv[i];
        mi.vertices.push_back(v);
    }

    mi.indices = {
        0,  1,  2,  0,  2,  3, 
        4,  5,  6,  4,  6,  7, 
        8,  9,  10, 8,  10, 11,
        12, 13, 14, 12, 14, 15,
        16, 17, 18, 16, 18, 19,
        20, 21, 22, 20, 22, 23 
    };

    return mi;
}
