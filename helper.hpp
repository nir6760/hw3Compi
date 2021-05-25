#ifndef _HELPER
#define _HELPER

#include <vector>
#include <string>
#include <unordered_map>
using namespace std;

/*
class Node{
    public:
    string name;
    Node(string n): name(n){}
    
};

class NUM : public Node{
    int val;

};
*/
class cut_type{
    public:
	int val;
    string type;
    string ret_type;
	string name;
	vector<string> params_type;
	vector<string> params_names;
};

#define YYSTYPE cut_type /* tells bison to use YYSTYPE as cut_type         */

class stype    //class for variable inside symbols 
{
    public:
    bool is_function;
    string name;    /* name of symbol          */
    string type;    /* type of symbol          */
    vector<string> params_type;    /* type of params          */
    vector<string> params_names;    /* type of params          */
    int offset;	 /* offset of symbol          */
    
    stype(){}

    stype(bool is_func ,const string& n, const string& t,  int of)://for variable or parameter
    is_function(is_func), name(n), type(t),  offset(of) {}

    stype(bool is_func,const string& n, const string& t,const vector<string>& p_type,const vector<string>& p_names,  int of ):
    is_function(is_func), name(n), type(t), params_type(p_type), params_names(p_names),  offset(of) {}//for function
	
	void editStype(bool is_func,const string& n, const string& t,const vector<string>& p_type,const vector<string>& p_names,  int of );//edit stype for function
};


class inside_scope{
	public:
    int curr_scope;
	vector<stype> symbols_lst;
    bool isWhileScope;
	bool isSwitchScope;

    inside_scope(int scope_num, vector<stype>& symbols, bool isWh, bool isSw ):
    curr_scope(scope_num),symbols_lst(symbols), isWhileScope(isWh), isSwitchScope(isSw) {}
    void IntializeFuncs();
    

  
};



void InitPrintFunctions();
void openScope(bool isWhile, bool isSwitch);
void closeScope(bool print);
void addFunc(const string& name, const string& ret_type
, const vector<string>& params_type, const vector<string>& params_names);
void insertVarToSymbolTable(const string& name, const string& type);
void checkMain();
void mergeVectors(vector<string>& v1, vector<string>& v2);
void checkValidTypes(const string& t1, const string& t2);
void checkValidID(const string& name);
string typeOfSym(const string& name);
void checkRetType(const string& type = "VOID");
void checkUnexpected(const string& unexp);
void isBool(const string& t);
void isIntByte(const string& t);
void checkOverFlowByte(int num);
void checkFuncDecl(const string& name, const vector<string>& params_types);
void checkValidNumeric(const string& t1, const string& t2);
void checkValidCast(const string& t1, const string& t2);
string findExpType(const string& t1, const string& t2);

void print_debug(const string& s);
#endif