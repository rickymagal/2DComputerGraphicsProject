/* 
   tinyxml2.h
   Original TinyXML-2 library
   https://github.com/leethomason/tinyxml2
*/
#ifndef TINYXML2_INCLUDED
#define TINYXML2_INCLUDED

#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cstring>

#if defined(_MSC_VER)
#   pragma warning(push)
#   pragma warning(disable: 4996)
#endif

namespace tinyxml2 {

class XMLDocument;
class XMLElement;
class XMLAttribute;
class XMLText;
class XMLComment;
class XMLUnknown;
class XMLDeclaration;

enum XMLError {
    XML_SUCCESS = 0,
    XML_NO_ATTRIBUTE,
    XML_WRONG_ATTRIBUTE_TYPE,
    XML_ERROR_FILE_NOT_FOUND,
    XML_ERROR_FILE_COULD_NOT_BE_OPENED,
    XML_ERROR_FILE_READ_ERROR,
    XML_ERROR_PARSING_ELEMENT,
    XML_ERROR_PARSING_ATTRIBUTE,
    XML_ERROR_PARSING_TEXT,
    XML_ERROR_PARSING_CDATA,
    XML_ERROR_PARSING_COMMENT,
    XML_ERROR_PARSING_DECLARATION,
    XML_ERROR_PARSING_UNKNOWN,
    XML_ERROR_EMPTY_DOCUMENT,
    XML_ERROR_MISMATCHED_ELEMENT,
    XML_ERROR_PARSING,
    XML_CAN_NOT_CONVERT_TEXT,
    XML_NO_TEXT_NODE,
    XML_ELEMENT_DEPTH_EXCEEDED
};

class XMLNode {
public:
    virtual ~XMLNode();

    XMLNode* Parent() const;
    XMLNode* FirstChild() const;
    XMLElement* FirstChildElement(const char* name = 0) const;
    XMLNode* NextSibling() const;
    XMLElement* NextSiblingElement(const char* name = 0) const;

protected:
    XMLNode(XMLDocument*);
    XMLNode(const XMLNode&);

    XMLDocument* _document;
    XMLNode* _parent;
    XMLNode* _firstChild;
    XMLNode* _lastChild;
    XMLNode* _prev;
    XMLNode* _next;
};

class XMLElement : public XMLNode {
public:
    const char* Name() const;
    const char* Attribute(const char* name) const;
};

class XMLDocument : public XMLNode {
public:
    XMLDocument();
    ~XMLDocument();

    XMLError LoadFile(const char* filename);
    XMLElement* FirstChildElement(const char* name = 0) const;
};

} // namespace tinyxml2

#if defined(_MSC_VER)
#   pragma warning(pop)
#endif

#endif

