#include <assert.h>
#include <stdio.h>
#include "emit.h"
#include "cool-tree.h"
#include "symtab.h"
#include <map>
#include <stack>
#include <vector>
#include <string>

enum Basicness     {Basic, NotBasic};
#define TRUE 1
#define FALSE 0

class CgenClassTable;
typedef CgenClassTable *CgenClassTableP;

class CgenNode;
typedef CgenNode *CgenNodeP;




//maps the class name (a symbol) to its CgenNode
class CgenClassTable : public SymbolTable<Symbol,CgenNode> {
private:
   std::vector<CgenNodeP> nds; //this is the meat: a list of nodes
   ostream& str;
   int stringclasstag;
   int intclasstag;
   int boolclasstag;
   int cur_tag;



// The following methods emit code for
// constants and global declarations.

   void code_global_data();
   void code_global_text();
   void code_bools(int);
   void code_select_gc();
   void code_constants();
   void code_class_nameTab();
   void code_class_objTab();
   void code_dispatch_table();
   void code_prototype_objects();
   void code_init();

// The following creates an inheritance graph from
// a list of classes.  The graph is implemented as
// a tree of `CgenNode', and class names are placed
// in the base class symbol table.

   void install_basic_classes();
   void install_class(CgenNodeP nd);
   void install_classes(Classes cs);
   void build_inheritance_tree();
   void set_relations(CgenNodeP nd);
   void set_class_tag();

public:
   Environment* env;
   CgenClassTable(Classes, ostream& str, Environment* env);
   void code();
   CgenNodeP root();
   void print_inheritance_tree();
};

// Each class corresponds to a CgenNode, records the children, the parent
class CgenNode : public class__class 
{
 private: 
   CgenNodeP parentnd;                        // Parent of class
   Basicness basic_status;                    // `Basic' if class is basic
                                             // `NotBasic' otherwise
   int tag;                                   // tag for the class(unique number)
   
public:

  std::vector<CgenNodeP> children;                  // Children of class 
  std::vector<attr_class*> attr_list;                // attributes of the class (including parents')
  std::vector<method_class *> method_list;             // method and method's parent name of this class (including parent)
  CgenNode(Class_ c,
            Basicness bstatus,
            CgenClassTableP class_table, int tag_);

   void add_child(CgenNodeP child);
   std::vector<CgenNodeP>* get_children() { return &children; }
   void set_parentnd(CgenNodeP p);
   CgenNodeP get_parentnd() { return parentnd; }
   int basic() { return (basic_status == Basic); }
   int get_tag() {return tag;}
};

class BoolConst 
{
 private: 
  int val;
 public:
  BoolConst(int);
  void code_def(ostream&, int boolclasstag);
  void code_ref(ostream&) const;
};



class Environment
{
private:
  int label_cnt;
public:
  class__class* cur_class;
  CgenClassTable* cgen_table;
  SymbolTable<Symbol, std::string> sym_table;
  int cur_exp_oft;
  ostream& str;

  Environment(Classes classes, ostream & s ):str(s)
  {
    cur_class = NULL;
    cgen_table = new CgenClassTable(classes, s, this);
    label_cnt = -1;
  }

  int get_label_cnt()
  {
    label_cnt ++;
    return label_cnt;
  }

};
