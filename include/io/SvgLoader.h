#ifndef IO_SVG_LOADER_H
#define IO_SVG_LOADER_H

#include <string>
#include <vector>

#include "../math/Vec2.h"
#include "../world/Arena.h"
#include "../world/Obstacle.h"

struct SvgSceneData {
    bool hasArena;
    bool hasPlayer1;
    bool hasPlayer2;

    Arena arena;
    Vec2 player1Pos;
    float player1HeadRadius;

    Vec2 player2Pos;
    float player2HeadRadius;

    std::vector<Obstacle> obstacles;

    SvgSceneData();
};

class SvgLoader {
public:
    static bool load(const std::string& path, SvgSceneData& out);

private:
    static bool parseFloat(const char* s, float& out);
    static bool parseCircleAttributes(
        const char* cxStr,
        const char* cyStr,
        const char* rStr,
        float& cx,
        float& cy,
        float& r
    );

    static bool isBlueFill(const char* fill);
    static bool isGreenFill(const char* fill);
    static bool isRedFill(const char* fill);
    static bool isBlackFill(const char* fill);
};

#endif

