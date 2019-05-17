#ifndef XMLDOCUMENT_H
#define XMLDOCUMENT_H
#include <iostream>
#include <fstream>
#include <sstream>
#include "core/data/utility/Configuration.h"
namespace pop
{

class POP_EXPORTS XMLNode
{
public:
    XMLNode();
    XMLNode(const XMLNode & node);
    XMLNode& operator ()(const XMLNode & node);
    virtual ~XMLNode();
    class impl;
    impl *_pImpl;
    void setName(std::string name);
    void setValue(std::string value);
    XMLNode addChild(std::string name);
    bool rmChild(const XMLNode& node);
    std::string getName()const;
    std::string getValue()const;
    XMLNode getChild(std::string childname)const;
    XMLNode firstChild()const;
    /*!
    \code
    XMLDocument doc;
    doc.load("/home/vtariel/Bureau/database.xml");

    XMLNode node = doc.getChild("elements");
    for (XMLNode tool = node.firstChild(); tool; tool = tool.nextSibling())
    {
        std::cout << "Tool:";
        cout<<tool.getAttribute("pathimage")<<endl;
        std::cout << std::endl;
    }
    \endcode
        \code
<?xml version="1.0"?>
<camera IP="rtsp://193.2225/channel1" frame_rate="0" channel="0" name="toto" type="0" />
<camera IP="" frame_rate="0" channel="0" name="toto2" type="0" />
     \endcode
    */
    operator bool() const ;
//    xml_node::operator xml_node::unspecified_bool_type() const


    XMLNode nextSibling()const;
    std::string getAttribute(std::string attributename) const;
    void setAttribute(std::string name,std::string  value);
    void addAttribute(std::string name,std::string  value);
    bool rmAttribute(std::string name);
};


class POP_EXPORTS XMLDocument
{
public:
  XMLDocument();
  virtual ~XMLDocument();

  void load(std::string file);
  void loadFromString(std::string file);
//  void load(std::istream & is);
  void save(std::string file)const ;
//  void save(std::ostream file)const ;
  XMLNode getChild(std::string name);
  XMLNode addChild(std::string name);
private:
  class impl;
  impl *_pImpl;
};

}
#endif // XMLDOCUMENT_H
