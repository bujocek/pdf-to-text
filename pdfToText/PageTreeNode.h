#pragma once
#include "CommonHeader.h"
#include "DictionaryObject.h"
#include "ContentStream.h"
#include "ArrayObject.h"
#include "IndirectObjectRefference.h"
#include "IndirectObject.h"
#include <list>
#include <fstream>

using namespace std;

/**
Class used for representing one node in Page tree. It can be inner node or leaf node. All leaf nodes are pages itself.
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
  flag if getKids method was already called
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
  */
  list<PageTreeNode *> * getKids();

  /**
  Pass empty list to the method and it will add to the list all leaf nodes from the Page tree.
  */
  void createPageList( list<PageTreeNode *> * pageList);
  
  /**
  prepares page for getting text - prepares contents and resources
  */
  void processPage();
  
  /**
  finds fonts in page resources
  */
  DictionaryObject * getFonts();

  /**
  gets resources for the page
  */
  DictionaryObject * getResources();
  
  /**
  method that writes page text to the output file
  */
  void getText(wofstream& file);

  PageTreeNode(DictionaryObject * nodeDictionary);
  ~PageTreeNode(void);
};
