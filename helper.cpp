#include "helper.hpp"
#include "hw3_output.hpp"

using namespace std;
using namespace output;
#define BYTE_EDGE 255
extern int yylineno;
vector<inside_scope> stack_scope;
unordered_map<string, stype> symbol_tabl_hash;
int global_offset = 0;
int number_of_while = 0;
int number_of_switch = 0;
stype* this_func = new stype();
bool was_main = false;

void checkName(const string& name);
void helperInsertVar(const string& name, const string& type, int offset);

// edit stype for the current function
void stype::editStype(bool is_func,const string& n, const string& t,const vector<string>& p_type,const vector<string>& p_names,  int of ){
	is_function=is_func;
	name=n;
	type=t;
	params_type=p_type;
	params_names=p_names;
	offset=of; 
}

// check if the symbol allready exists
void checkName(const string& name){ 
	if (symbol_tabl_hash.find(name) != symbol_tabl_hash.end()) {
		errorDef(yylineno, name);
		delete this_func;
		exit(1);
	}
}


//open new scope
void openScope(bool isWhile, bool isSwitch){ 
	vector<stype> empty_vec;
    inside_scope sc(global_offset, empty_vec, isWhile, isSwitch);
    if(isWhile){
        ++number_of_while;
    }
	if(isSwitch){
		++number_of_switch;
	}
    stack_scope.push_back(sc);

}

//close scope and print
void closeScope(bool print = true) {
	inside_scope& sc = stack_scope.back();
	if(print){
		endScope();
	}
	for (auto& symbol : sc.symbols_lst) {
		if (print){
			if (symbol.is_function){
				printID(symbol.name, symbol.offset, makeFunctionType(symbol.type, symbol.params_type));
			} 
			else{
				printID(symbol.name, symbol.offset, symbol.type);
			}				
		}
		symbol_tabl_hash.erase(symbol.name);
	}
	global_offset = sc.curr_scope;
	if (sc.isWhileScope){
		--number_of_while;
	}
	if (sc.isSwitchScope){
		--number_of_switch;
	}
	stack_scope.pop_back();
}

// init print and print and global scope
void InitPrintFunctions() { 
	openScope(false,false);
	addFunc("print", "VOID", {"STRING"}, {"print_val_name"});
	closeScope(false);
	addFunc("printi", "VOID", {"INT"}, {"printi_val_name"});
	closeScope(false);
	
}

//add function to symbol table and open scope for it
void addFunc(const string& name, const string& ret_type, const vector<string>& params_type, const vector<string>& params_names) {
	checkName(name);
    this_func->editStype(true, name, ret_type, params_type, params_names, 0); 
	symbol_tabl_hash[name] = *this_func;
    stack_scope.back().symbols_lst.push_back(*this_func);
	openScope(false,false);
    int i=0;
	for (auto& p_name : params_names) {
		i++;
		helperInsertVar(p_name, params_type[i-1], -i);
	}
	if ("main" == name && "VOID" == ret_type && params_type.empty())
		was_main = true;
}

// insert var to symbol table
void insertVarToSymbolTable(const string& name, const string& type) {
	helperInsertVar(name, type, global_offset++);
}

// helper for insert var to symbol table
void helperInsertVar(const string& name, const string& type, int offset) {

	checkName(name);
    stype var(false, name, type, offset);
	symbol_tabl_hash[name] = var;
	stack_scope.back().symbols_lst.push_back(var);

}

//check if Main was valid decleared and close the global scope
void checkMain() { 
	if (!was_main) {
		errorMainMissing();
		delete this_func;
		exit(1);
	}
	closeScope(true);
	delete this_func;
}


//merge 2 vectors to the first one
void mergeVectors(vector<string>& v1, vector<string>& v2){
    v1.insert(v1.end(),v2.begin(),v2.end());
}

//check if types are valid in the action
void checkValidTypes(const string& t1, const string& t2) { 
	if (t1 != t2 && !(t1 == "INT" && t2 == "BYTE")) {
		errorMismatch(yylineno);
		delete this_func;
		exit(1);
	}
}

// check if id was not define
void checkValidID(const string& name) {
	if (symbol_tabl_hash.find(name) == symbol_tabl_hash.end()||
	symbol_tabl_hash[name].is_function){
		errorUndef(yylineno, name);
		delete this_func;
		exit(1);
	}
}

//return the type of the decleared function
string typeOfSym(const string& name){ 
	return symbol_tabl_hash[name].type;
}

//check if the return type is identical to what was decleared in the function
void checkRetType(const string& type) { 
	checkValidTypes(this_func->type, type);
}

// check for unexpected break or continue
void checkUnexpected(const string& unexp) {
	if(unexp == "CONTINUE"){
		if (number_of_while == 0){
			errorUnexpectedContinue(yylineno);
			delete this_func;
			exit(1);
		}
	}
	if(unexp == "BREAK"){
		if (number_of_while == 0 && number_of_switch == 0){
			errorUnexpectedBreak(yylineno);
			delete this_func;
			exit(1);
		}
	}
}

//check if type is bool
void isBool(const string& t) { 
	if (t != "BOOL") {
		errorMismatch(yylineno);
		delete this_func;
		exit(1);
	}
}

void isIntByte(const string& t) { //check if type is int or byte
	if (t != "INT" && t != "BYTE"  ) {
		errorMismatch(yylineno);
		delete this_func;
		exit(1);
	}
}

// check if byte is overflow
void checkOverFlowByte(int num) { 
	if (num > BYTE_EDGE) {
		errorByteTooLarge(yylineno, to_string(num));
		delete this_func;
		exit(1);
	}
}

// check if function was decleared and if its valid call
void checkFuncDecl(const string& name, const vector<string>& params_types) { 
	if (symbol_tabl_hash.find(name) == symbol_tabl_hash.end()||
	!symbol_tabl_hash[name].is_function) {
		errorUndefFunc(yylineno, name);
		delete this_func;
		exit(1);
	}
	stype& f = symbol_tabl_hash[name];
	if (f.params_type.size() != params_types.size()) {
		errorPrototypeMismatch(yylineno, name, f.params_type);
		delete this_func;
		exit(1);
	}
	for (unsigned int i =0; i < params_types.size(); ++i) {
		if (f.params_type[i] != params_types[i] && !(f.params_type[i] == "INT" && params_types[i] == "BYTE")) {
			errorPrototypeMismatch(yylineno, name, f.params_type);
			delete this_func;
			exit(1);
		}
	}

}

// find the if they both numerics
void checkValidNumeric(const string& t1, const string& t2) {
	if ((t1 == "INT" || t1 == "BYTE") &&
		(t2 == "INT" || t2 == "BYTE")) 
		return;
	errorMismatch(yylineno);
	delete this_func;
	exit(1);
}

//check if the casting is valid
void checkValidCast(const string& t1, const string& t2) { 
	if (t1 == "BYTE" && t2 == "INT") {
		errorMismatch(yylineno);
		delete this_func;
		exit(1);
	}
	
}

// find the currect type of the exp
string findExpType(const string& t1, const string& t2) { 
	if (t1 == "INT" || t2 == "INT") {
		return "INT";
	} else {
		return "BYTE";
	}

}
