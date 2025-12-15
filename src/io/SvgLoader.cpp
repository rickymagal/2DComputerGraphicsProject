#include "../../include/io/SvgLoader.h"
#include "../../third_party/tinywml2/tinyxml2.h"

#include <cstdlib>
#include <cstring>
#include <cctype>
#include <cstdio>
#include <string>
#include <vector>

using tinyxml2::XMLDocument;
using tinyxml2::XMLElement;

/* ===================== SvgSceneData ===================== */

SvgSceneData::SvgSceneData()
    : hasArena(false),
      hasPlayer1(false),
      hasPlayer2(false),
      arena(),
      player1Pos(),
      player1HeadRadius(0.0f),
      player2Pos(),
      player2HeadRadius(0.0f),
      obstacles() {}

/* ===================== Local helpers ===================== */

static void dbgFail(const char* path, const char* msg) {
    std::fprintf(stderr, "[SvgLoader] FAIL path='%s' : %s\n",
                 path ? path : "(null)",
                 msg ? msg : "(null)");
}

static bool parseFloatLocal(const char* s, float& out) {
    if (!s) return false;
    char* end = nullptr;
    out = std::strtof(s, &end);
    return end != s;
}

static bool strEq(const char* a, const char* b) {
    return a && b && std::strcmp(a, b) == 0;
}

static int hexVal(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    c = (char)std::tolower((unsigned char)c);
    if (c >= 'a' && c <= 'f') return 10 + (c - 'a');
    return -1;
}

static bool parseHexColor(const char* s, int& r, int& g, int& b) {
    if (!s || s[0] != '#') return false;
    if (std::strlen(s) != 7) return false;
    int r1 = hexVal(s[1]), r2 = hexVal(s[2]);
    int g1 = hexVal(s[3]), g2 = hexVal(s[4]);
    int b1 = hexVal(s[5]), b2 = hexVal(s[6]);
    if (r1 < 0 || r2 < 0 || g1 < 0 || g2 < 0 || b1 < 0 || b2 < 0) return false;
    r = r1 * 16 + r2;
    g = g1 * 16 + g2;
    b = b1 * 16 + b2;
    return true;
}

static const char* extractFillFromStyle(const char* style, char* buf, size_t bufCap) {
    if (!style || !buf || bufCap == 0) return nullptr;

    const char* p = std::strstr(style, "fill");
    while (p) {
        const char* q = p + 4;
        while (*q && std::isspace((unsigned char)*q)) ++q;
        if (*q != ':') { p = std::strstr(p + 1, "fill"); continue; }
        ++q;
        while (*q && std::isspace((unsigned char)*q)) ++q;

        size_t i = 0;
        while (*q && *q != ';' && !std::isspace((unsigned char)*q) && i + 1 < bufCap) {
            buf[i++] = *q++;
        }
        buf[i] = '\0';
        return (i > 0) ? buf : nullptr;
    }
    return nullptr;
}

static bool isBlueFillAny(const char* fill) {
    if (!fill) return false;
    if (strEq(fill, "blue")) return true;
    int r, g, b;
    if (parseHexColor(fill, r, g, b)) return (r == 0 && g == 0 && b == 255);
    return false;
}

static bool isGreenFillAny(const char* fill) {
    if (!fill) return false;
    if (strEq(fill, "green")) return true;
    int r, g, b;
    if (parseHexColor(fill, r, g, b)) return (r == 0 && g == 255 && b == 0);
    return false;
}

static bool isRedFillAny(const char* fill) {
    if (!fill) return false;
    if (strEq(fill, "red")) return true;
    int r, g, b;
    if (parseHexColor(fill, r, g, b)) return (r == 255 && g == 0 && b == 0);
    return false;
}

static bool isBlackFillAny(const char* fill) {
    if (!fill) return false;
    if (strEq(fill, "black")) return true;
    int r, g, b;
    if (parseHexColor(fill, r, g, b)) return (r == 0 && g == 0 && b == 0);
    return false;
}

struct CircleRaw {
    float cx, cy, r;
    bool hasFill;
    bool isBlue, isGreen, isRed, isBlack;
};

/* ---------- Auto-spawn for test svgs ---------- */

static void autoSpawnPlayers(SvgSceneData& out) {
    float R = out.arena.radius;
    Vec2 c = out.arena.center;

    float headR = R * 0.06f;
    float dx = R * 0.45f;

    out.player1Pos = Vec2(c.x - dx, c.y);
    out.player2Pos = Vec2(c.x + dx, c.y);
    out.player1HeadRadius = headR;
    out.player2HeadRadius = headR;

    out.hasPlayer1 = true;
    out.hasPlayer2 = true;
}

/* ===================== Fallback circle scanner ===================== */

static bool readFileAll(const std::string& path, std::string& outText) {
    std::FILE* f = std::fopen(path.c_str(), "rb");
    if (!f) return false;
    std::fseek(f, 0, SEEK_END);
    long n = std::ftell(f);
    if (n < 0) { std::fclose(f); return false; }
    std::fseek(f, 0, SEEK_SET);
    outText.resize((size_t)n);
    if (n > 0) {
        size_t rd = std::fread(&outText[0], 1, (size_t)n, f);
        if (rd != (size_t)n) { std::fclose(f); return false; }
    }
    std::fclose(f);
    return true;
}

static bool extractAttrValue(const std::string& tag, const char* key, std::string& outVal) {
    // looks for key="..."; key must be exact (e.g., cx, cy, r, fill, style)
    std::string pat = std::string(key) + "=\"";
    size_t p = tag.find(pat);
    if (p == std::string::npos) return false;
    p += pat.size();
    size_t q = tag.find('"', p);
    if (q == std::string::npos) return false;
    outVal = tag.substr(p, q - p);
    return true;
}

static bool parseCircleTag(const std::string& tag, CircleRaw& outC) {
    std::string cxS, cyS, rS;
    if (!extractAttrValue(tag, "cx", cxS)) return false;
    if (!extractAttrValue(tag, "cy", cyS)) return false;
    if (!extractAttrValue(tag, "r", rS)) return false;

    float cx = 0.0f, cy = 0.0f, rr = 0.0f;
    if (!parseFloatLocal(cxS.c_str(), cx)) return false;
    if (!parseFloatLocal(cyS.c_str(), cy)) return false;
    if (!parseFloatLocal(rS.c_str(), rr)) return false;

    std::string fillS;
    bool hasFill = extractAttrValue(tag, "fill", fillS);

    // style="...fill:...;"
    if (!hasFill) {
        std::string styleS;
        if (extractAttrValue(tag, "style", styleS)) {
            char tmp[64];
            const char* f = extractFillFromStyle(styleS.c_str(), tmp, sizeof(tmp));
            if (f) {
                fillS = f;
                hasFill = true;
            }
        }
    }

    const char* fill = hasFill ? fillS.c_str() : nullptr;

    outC.cx = cx;
    outC.cy = cy;
    outC.r  = rr;
    outC.hasFill = hasFill;
    outC.isBlue  = isBlueFillAny(fill);
    outC.isGreen = isGreenFillAny(fill);
    outC.isRed   = isRedFillAny(fill);
    outC.isBlack = isBlackFillAny(fill);
    return true;
}

static bool scanCirclesFromText(const std::string& text, std::vector<CircleRaw>& circles) {
    size_t pos = 0;
    while (true) {
        size_t p = text.find("<circle", pos);
        if (p == std::string::npos) break;
        size_t q = text.find('>', p);
        if (q == std::string::npos) break;

        std::string tag = text.substr(p, q - p + 1);

        CircleRaw c;
        if (parseCircleTag(tag, c)) {
            circles.push_back(c);
        }

        pos = q + 1;
    }
    return !circles.empty();
}

/* ===================== Public API ===================== */

bool SvgLoader::load(const std::string& path, SvgSceneData& out) {
    out = SvgSceneData();

    XMLDocument doc;
    const int rc = doc.LoadFile(path.c_str());
    if (rc != tinyxml2::XML_SUCCESS) {
        char buf[128];
        std::snprintf(buf, sizeof(buf), "LoadFile failed rc=%d", rc);
        dbgFail(path.c_str(), buf);
        return false;
    }

    const XMLElement* svg = doc.FirstChildElement("svg");

    std::vector<CircleRaw> circles;
    circles.reserve(64);

    if (svg) {
        // Normal path: traverse children (works for your original arena.svg)
        for (const XMLElement* e = svg->FirstChildElement(); e; e = e->NextSiblingElement()) {
            if (std::strcmp(e->Name(), "circle") == 0) {
                float cx = 0.0f, cy = 0.0f, rr = 0.0f;
                if (parseFloatLocal(e->Attribute("cx"), cx) &&
                    parseFloatLocal(e->Attribute("cy"), cy) &&
                    parseFloatLocal(e->Attribute("r"), rr)) {

                    char tmp[64];
                    const char* fill = e->Attribute("fill");
                    if (!fill) {
                        const char* style = e->Attribute("style");
                        if (style) fill = extractFillFromStyle(style, tmp, sizeof(tmp));
                    }

                    CircleRaw c;
                    c.cx = cx; c.cy = cy; c.r = rr;
                    c.hasFill = (fill != nullptr);
                    c.isBlue  = isBlueFillAny(fill);
                    c.isGreen = isGreenFillAny(fill);
                    c.isRed   = isRedFillAny(fill);
                    c.isBlack = isBlackFillAny(fill);

                    circles.push_back(c);
                }
            }
        }
    } else {
        // Fallback path for your test_svgs: tinywml2 doc.FirstChildElement() is broken
        std::string text;
        if (!readFileAll(path, text)) {
            dbgFail(path.c_str(), "fallback readFileAll failed");
            return false;
        }
        if (!scanCirclesFromText(text, circles)) {
            dbgFail(path.c_str(), "fallback scanCirclesFromText found no circles");
            return false;
        }
    }

    if (circles.empty()) {
        dbgFail(path.c_str(), "no circles parsed");
        return false;
    }

    // Select arena: prefer blue; else largest radius
    int arenaIdx = -1;
    for (int i = 0; i < (int)circles.size(); ++i) {
        if (circles[i].isBlue) { arenaIdx = i; break; }
    }
    if (arenaIdx < 0) {
        float bestR = -1.0f;
        for (int i = 0; i < (int)circles.size(); ++i) {
            if (circles[i].r > bestR) { bestR = circles[i].r; arenaIdx = i; }
        }
    }
    if (arenaIdx < 0) {
        dbgFail(path.c_str(), "arenaIdx < 0 after selection");
        return false;
    }

    out.arena.center = Vec2(circles[arenaIdx].cx, circles[arenaIdx].cy);
    out.arena.radius = circles[arenaIdx].r;
    out.hasArena = true;

    // Remaining circles: players by color if present, else obstacles
    for (int i = 0; i < (int)circles.size(); ++i) {
        if (i == arenaIdx) continue;
        const CircleRaw& c = circles[i];

        if (c.isGreen && !out.hasPlayer1) {
            out.player1Pos = Vec2(c.cx, c.cy);
            out.player1HeadRadius = c.r;
            out.hasPlayer1 = true;
        } else if (c.isRed && !out.hasPlayer2) {
            out.player2Pos = Vec2(c.cx, c.cy);
            out.player2HeadRadius = c.r;
            out.hasPlayer2 = true;
        } else {
            out.obstacles.emplace_back(Vec2(c.cx, c.cy), c.r);
        }
    }

    if (!out.hasPlayer1 || !out.hasPlayer2) {
        autoSpawnPlayers(out);
    }

    return true;
}
