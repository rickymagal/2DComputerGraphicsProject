#include "../../include/io/SvgLoader.h"
#include "../../include/math/Vec2.h"
#include "../../include/world/Obstacle.h"

#include <fstream>
#include <sstream>
#include <string>
#include <cstring>
#include <cstdlib>

SvgSceneData::SvgSceneData()
    : hasArena(false), hasPlayer1(false), hasPlayer2(false),
      player1Pos(0.0f, 0.0f), player1HeadRadius(0.0f),
      player2Pos(0.0f, 0.0f), player2HeadRadius(0.0f) {}

static bool parseFloatFromString(const std::string& s, float& out) {
    if (s.empty()) return false;
    char* endp = nullptr;
    out = std::strtof(s.c_str(), &endp);
    return endp != s.c_str();
}

static std::string extractAttr(const std::string& tag, const char* key) {
    std::string k = std::string(key) + "=\"";
    size_t p = tag.find(k);
    if (p == std::string::npos) return "";
    p += k.size();
    size_t q = tag.find('"', p);
    if (q == std::string::npos) return "";
    return tag.substr(p, q - p);
}

bool SvgLoader::parseFloat(const char* s, float& out) {
    if (!s) return false;
    char* endp = nullptr;
    out = std::strtof(s, &endp);
    return endp != s;
}

bool SvgLoader::parseCircleAttributes(
    const char* cxStr,
    const char* cyStr,
    const char* rStr,
    float& cx,
    float& cy,
    float& r
) {
    return parseFloat(cxStr, cx) &&
           parseFloat(cyStr, cy) &&
           parseFloat(rStr, r);
}

bool SvgLoader::isBlueFill(const char* fill) {
    return fill && (std::strcmp(fill, "blue") == 0 || std::strcmp(fill, "#0000ff") == 0);
}

bool SvgLoader::isGreenFill(const char* fill) {
    return fill && (std::strcmp(fill, "green") == 0 || std::strcmp(fill, "#00ff00") == 0);
}

bool SvgLoader::isRedFill(const char* fill) {
    return fill && (std::strcmp(fill, "red") == 0 || std::strcmp(fill, "#ff0000") == 0);
}

bool SvgLoader::isBlackFill(const char* fill) {
    return fill && (std::strcmp(fill, "black") == 0 || std::strcmp(fill, "#000000") == 0);
}

bool SvgLoader::load(const std::string& path, SvgSceneData& out) {
    std::ifstream f(path);
    if (!f.is_open()) return false;

    std::stringstream ss;
    ss << f.rdbuf();
    std::string content = ss.str();

    size_t pos = 0;
    while (true) {
        size_t c0 = content.find("<circle", pos);
        if (c0 == std::string::npos) break;

        size_t c1 = content.find('>', c0);
        if (c1 == std::string::npos) break;

        std::string tag = content.substr(c0, c1 - c0 + 1);
        pos = c1 + 1;

        std::string cxS = extractAttr(tag, "cx");
        std::string cyS = extractAttr(tag, "cy");
        std::string rS  = extractAttr(tag, "r");

        std::string fillS = extractAttr(tag, "fill");
        if (fillS.empty()) {
            std::string styleS = extractAttr(tag, "style");
            if (!styleS.empty()) {
                size_t fp = styleS.find("fill:");
                if (fp != std::string::npos) {
                    fp += 5;
                    size_t fe = styleS.find(';', fp);
                    std::string v = (fe == std::string::npos) ? styleS.substr(fp) : styleS.substr(fp, fe - fp);
                    while (!v.empty() && (v.back() == ' ' || v.back() == '\t' || v.back() == '\n' || v.back() == '\r')) v.pop_back();
                    size_t st = 0;
                    while (st < v.size() && (v[st] == ' ' || v[st] == '\t' || v[st] == '\n' || v[st] == '\r')) st++;
                    fillS = v.substr(st);
                }
            }
        }

        float cx = 0.0f, cy = 0.0f, r = 0.0f;
        if (!parseFloatFromString(cxS, cx) ||
            !parseFloatFromString(cyS, cy) ||
            !parseFloatFromString(rS, r)) {
            continue;
        }

        const char* fill = fillS.empty() ? nullptr : fillS.c_str();

        if (isBlueFill(fill)) {
            out.arena.center = Vec2(cx, cy);
            out.arena.radius = r;
            out.hasArena = true;
        } else if (isGreenFill(fill)) {
            out.player1Pos = Vec2(cx, cy);
            out.player1HeadRadius = r;
            out.hasPlayer1 = true;
        } else if (isRedFill(fill)) {
            out.player2Pos = Vec2(cx, cy);
            out.player2HeadRadius = r;
            out.hasPlayer2 = true;
        } else if (isBlackFill(fill)) {
            out.obstacles.emplace_back(Vec2(cx, cy), r);
        }
    }

    return out.hasArena && out.hasPlayer1 && out.hasPlayer2;
}
