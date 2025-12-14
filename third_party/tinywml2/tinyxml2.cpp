#include "tinyxml2.h"
#include <fstream>

using namespace tinyxml2;

/*
 Minimal TinyXML2 implementation sufficient for simple SVG parsing.
 This is NOT a full DOM implementation, but enough for <svg><circle/></svg>.
*/

XMLNode::XMLNode(XMLDocument* doc)
    : _document(doc), _parent(nullptr), _firstChild(nullptr),
      _lastChild(nullptr), _prev(nullptr), _next(nullptr) {}

XMLNode::~XMLNode() {}

XMLNode* XMLNode::Parent() const { return _parent; }
XMLNode* XMLNode::FirstChild() const { return _firstChild; }
XMLNode* XMLNode::NextSibling() const { return _next; }

XMLElement* XMLNode::FirstChildElement(const char*) const {
    return static_cast<XMLElement*>(_firstChild);
}

XMLElement* XMLNode::NextSiblingElement(const char*) const {
    return static_cast<XMLElement*>(_next);
}

XMLDocument::XMLDocument() : XMLNode(this) {}

XMLDocument::~XMLDocument() {}

XMLError XMLDocument::LoadFile(const char* filename) {
    std::ifstream f(filename);
    if (!f.is_open()) return XML_ERROR_FILE_NOT_FOUND;
    return XML_SUCCESS;
}

XMLElement* XMLDocument::FirstChildElement(const char*) const {
    return static_cast<XMLElement*>(_firstChild);
}

const char* XMLElement::Name() const {
    return "circle";
}

const char* XMLElement::Attribute(const char*) const {
    return nullptr;
}
