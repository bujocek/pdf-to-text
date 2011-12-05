#include "StdAfx.h"
#include "PageTreeNode.h"

PageTreeNode::PageTreeNode(DictionaryObject * nodeDictionary)
{
  this->nodeDictionary = nodeDictionary;
  this->lookedForKids = false;
  this->isLeaf = false;
  this->parent = null;
  this->pageText = null;
  this->resources = null;
  this->fonts = null;
  this->isPageProcessed = false;
  this->getKids();
}

PageTreeNode::~PageTreeNode(void)
{
}

inline list<PageTreeNode *> * badQuit()
{
  cerr << "\nPageTreeNode: Couldn't find kids of the node in dictionary.\n";
  return null;
}

list<PageTreeNode *> * PageTreeNode::getKids()
{
  if(!this->lookedForKids)
  {
    this->lookedForKids = true;
    PdfObject * po = this->nodeDictionary->getObject("/Kids");
    if(po == null)
    {
      this->isLeaf = true;
    }
    else
    {
      ArrayObject * ao = (ArrayObject*) po;
      if(ao == null)
        return badQuit();
      vector<PdfObject *>::iterator objectListIterator;

      for ( objectListIterator = ao->objectList.begin(); objectListIterator != ao->objectList.end();
		        objectListIterator++)
      {
        if((*objectListIterator)->objectType == PdfObject::TYPE_INDIRECT_OBJECT_REFFERENCE)
        {
          IndirectObject * io = ((IndirectObjectReference*)(*objectListIterator))->getIndirectObject();
          if(io == null)
            return badQuit();
          DictionaryObject * dictionary = (DictionaryObject*) io->getFirstObject();
          if(dictionary == null)
            return badQuit();
          PageTreeNode * ptn = new PageTreeNode(dictionary);
          ptn->parent = this;
          this->kids.push_back(ptn);
        }
        else
          return badQuit();
      }
    }
  }
  return &(this->kids);
}

void PageTreeNode::createPageList( list<PageTreeNode *> * pageList)
{
  if(this->isLeaf)
    pageList->push_back(this);
  else
  {
    list<PageTreeNode *>::iterator pageListIterator;
    for(pageListIterator = (this->getKids())->begin(); pageListIterator != (this->getKids())->end(); pageListIterator++)
      (*pageListIterator)->createPageList(pageList);
  }
}

void PageTreeNode::processPage()
{
  if(this->isPageProcessed)
    return;
  if(!this->isLeaf)
  {
    cerr << "\nPageTreeNode: Can't call process page method on non page tree node.\n";
    return;
  }
  PdfObject * contents = this->nodeDictionary->getObject("/Contents");
  ContentStream * cs;
  if(contents->objectType == PdfObject::TYPE_ARRAY)
  {
    vector<PdfObject*> * objlist = &(((ArrayObject*) contents)->objectList);
    vector<PdfObject*>::iterator oliterator;
    for(oliterator = objlist->begin(); oliterator != objlist->end(); oliterator++)
    {
      if((*oliterator)->objectType == PdfObject::TYPE_INDIRECT_OBJECT_REFFERENCE)
      {
        cs = new ContentStream(((IndirectObjectReference*)(*oliterator))->getIndirectObject(),this);
        this->contents.push_back(cs);
      }
      else
        cerr<<"\nPageTreeNode: Unexpected object found in contents array\n";
    }
    this->isPageProcessed = true;
    return;
  }
  else if(contents->objectType == PdfObject::TYPE_INDIRECT_OBJECT)
  {
    cs = new ContentStream((IndirectObject*)contents, this);
    this->contents.push_back(cs);
    this->isPageProcessed = true;
    return;
  }
  else
  {
    cerr << "\nPageTreeNode: Unsupported Content in page dictionary.\n";
    this->isPageProcessed = true;
    return;
  }
}

void PageTreeNode::getText(FILE * file)
{
  list<ContentStream*>::iterator csi;
  ContentStream * prevCsi = null;
  for(csi = this->contents.begin(); csi != this->contents.end(); csi++)
  {
    wchar_t * res = (*csi)->getText(prevCsi);
    if(res != null)
      fwrite(res,sizeof(wchar_t), wcslen(res), file);
    prevCsi = *csi;
  }
}

DictionaryObject * PageTreeNode::getResources()
{
  if(this->resources != null)
    return this->resources;
  PdfObject * res = this->nodeDictionary->getObject("/Resources");
  if(res == null)
  {
    if(this->parent == null)
    {
      cerr << "\nPageTreeNode: Couldn't find page resources.\n";
      this->resources = null;
    }
    else
    {
      this->resources = this->parent->getResources();
    }
  }
  else
  {
    if(res->objectType == PdfObject::TYPE_DICTIONARY)
      this->resources = (DictionaryObject*) res;
    else if(res->TYPE_INDIRECT_OBJECT)
    {
      IndirectObject * io = (IndirectObject*) res;
      PdfObject * po = io->getFirstObject();
      if(po == null)
      {
        cerr << "\nPageTreeNode: Couldn't get resource dictionary from indirect object.\n";
        this->resources = null;
      }
      else
      {
        if(po->objectType == PdfObject::TYPE_DICTIONARY)
          this->resources = (DictionaryObject*) po;
        else
        {
          cerr << "\nPageTreeNode: Couldn't get resource dictionary from indirect object.\n";
          this->resources = null;
        }
      }
    }
    else
    {
      cerr << "\nPageTreeNode: Unexpected object used for page resources.\n";
      this->resources = null;
    }
  }
  return this->resources;
}

DictionaryObject * PageTreeNode::getFonts()
{
  if(this->fonts != null)
    return this->fonts;
  DictionaryObject * resourcesDictionary = this->getResources();
  if(resourcesDictionary == null)
  {
    cerr<<"\nPageTreeNode: Couldn't get fonts from page resources.\n";
  }
  else
  {
    PdfObject * fontsObject = resourcesDictionary->getObject("/Font");
    
    if(fontsObject != null && fontsObject->objectType == PdfObject::TYPE_INDIRECT_OBJECT)
      fontsObject = ((IndirectObject*) fontsObject)->getFirstObject();
    
    if(fontsObject != null && fontsObject->objectType == PdfObject::TYPE_DICTIONARY)
      this->fonts = (DictionaryObject*) fontsObject;
    else
      cerr<<"\nNo fonts found in resource dictionary.\n";
  }
  return this->fonts;
}