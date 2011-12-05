#pragma once
#include "CommonHeader.h"
#include "DictionaryObject.h"
#include "ContentStream.h"
#include "ArrayObject.h"
#include "IndirectObjectReference.h"
#include "IndirectObject.h"
#include <list>
#include <fstream>

using namespace std;

/**
Class used for representing one node in Page tree.<br> 
It can be inner node or leaf node. All leaf nodes are pages itself.
Class is created from dictionary object passed to the constructor.
*/
class PageTreeNode
{
private:
  list<PageTreeNode *> kids;
  list<ContentStream *> contents;
  DictionaryObject * resources;
  DictionaryObject * fonts;
  bool isPageProcessed;
  char * pageText;
public:
  /**
  Flag if this node is leaf (has no children) or not.
  It is se by getKids() method so be sure it was called before or check "lookedForKids" property before.
  */
  bool isLeaf;
  
  /**
  Flag if getKids method was already called.
  */
  bool lookedForKids;

  /**
  Refference to the parent node - it is set by parent while creating the tree so if it was created other way 
  than the parent node will be null.
  */
  PageTreeNode * parent;
  
  /**
  dictionary the node was created from
  */
  DictionaryObject * nodeDictionary;
  
  /**
  Creates and returns (always the same) list of child page tree nodes.
  \return List of child page tree nodes.
  */
  list<PageTreeNode *> * getKids();

  /**
  Pass empty list to the method and it will add to the list all leaf nodes from the Page tree.
  \pageList List of Pages the child pages of this node will be added to.
  */
  void createPageList( list<PageTreeNode *> * pageList);
  
  /**
  Prepares page for getting text - prepares contents and resources
  */
  void processPage();
  
  /**
  Finds fonts in page resources
  \return DictionaryObject with fonts form resources.
  */
  DictionaryObject * getFonts();

  /**
  Gets resources for the page
  \return DictionaryObject with resources.
  */
  DictionaryObject * getResources();
  
  /**
  Method that writes page text to the output file
  \param file Output file the text will be written to.
  */
  void getText(FILE * file);
  
  /**
  Constructor.<br>
  Creates PageTreeNode object from node dictionary
  */
  PageTreeNode(DictionaryObject * nodeDictionary);

  /**
  Destructor.
  */
  ~PageTreeNode(void);
};
