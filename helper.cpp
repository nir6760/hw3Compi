#include "helper.hpp"
#include "hw3_output.hpp"

using namespace std;
using namespace output;

extern int yylineno;
vector<inside_scope> stack_scope;
unordered_map<string, stype> symbol_table_map;
int global_offset = 0;
int number_of_while = 0;
int number_of_switch = 0;
stype* this_func = new stype();
bool was_main = false;

void checkName(const string& name);
void stype::editStype(bool is_func,const string& n, const string& t,const vector<string>& p_type,const vector<string>& p_names,  int of ){
	is_function=is_func;
	name=n;
	type=t;
	params_type=p_type;
	params_names=p_names;
	offset=of; 
}

void checkName(const string& name){ // check if the symbol allready exists
	if (symbol_table_map.find(name) != symbol_table_map.end()) {
		errorDef(yylineno, name);
		exit(1);
	}
}



void openScope(bool isWhile, bool isSwitch){ //open new scope
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

void closeScope(bool print = true) {//close scope
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
		symbol_table_map.erase(symbol.name);
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

void InitPrintFunctions() { // init print and printi
	openScope(false,false);
	addFunc("print", "VOID", {"STRING"}, {"print_val_name"});
	closeScope(false);
	addFunc("printi", "VOID", {"INT"}, {"printi_val_name"});
	closeScope(false);
	
}
void addFunc(const string& name, const string& ret_type, const vector<string>& params_type, const vector<string>& params_names) {
	checkName(name);
    this_func->editStype(true, name, ret_type, params_type, params_names, 0); 
	symbol_table_map[name] = *this_func;
    stack_scope.back().symbols_lst.push_back(*this_func);
	openScope(false,false);
    int i=0;
	for (auto& p_name : params_names) {
		i++;
		insertVar(p_name, params_type[i-1], -i);
	}
	if ("main" == name && "VOID" == ret_type && params_type.empty())
		was_main = true;
}

void insertVarToSymbolTable(const string& name, const string& type) {
	insertVar(name, type, global_offset++);
}

void insertVar(const string& name, const string& type, int offset) {

	checkName(name);
    stype var(false, name, type, offset);
	symbol_table_map[name] = var;
	stack_scope.back().symbols_lst.push_back(var);

}


void checkMain() { //check if Main was decleared
	if (!was_main) {
		errorMainMissing();
		exit(1);
	}
	closeScope(true);
	delete this_func;
}


void mergeVectors(vector<string>& v1, vector<string>& v2){//merge 2 vectors
    v1.insert(v1.end(),v2.begin(),v2.end());
}

void checkValidTypes(const string& t1, const string& t2) {
	if (t1 != t2 && !(t1 == "INT" && t2 == "BYTE")) {
		errorMismatch(yylineno);
		exit(1);
	}
}

void checkValidID(const string& name) {
	if (symbol_table_map.find(name) == symbol_table_map.end()||
	symbol_table_map[name].is_function){
		errorUndef(yylineno, name);
		exit(1);
	}
}

string typeOfSym(const string& name){ //return the type of the decleared function
	return symbol_table_map[name].type;
}

void checkRetType(const string& type) { //check if the return type is identical to what was decleared in the function
	checkValidTypes(this_func->type, type);
}

void checkUnexpected(const string& unexp) {
	if(unexp == "CONTINUE"){
		if (number_of_while == 0){
			errorUnexpectedContinue(yylineno);
			exit(1);
		}
	}
	if(unexp == "BREAK"){
		if (number_of_while == 0 && number_of_switch == 0){
			errorUnexpectedBreak(yylineno);
			exit(1);
		}
	}
}

void isBool(const string& t) { //check if type is bool
	if (t != "BOOL") {
		errorMismatch(yylineno);
		exit(1);
	}
}

void isIntByte(const string& t) { //check if type is int or byte
	if (t != "INT" && t != "BYTE"  ) {
		errorMismatch(yylineno);
		exit(1);
	}
}

void checkOverFlowByte(int num) { // check if byte is overflow
	if (num > 255) {
		errorByteTooLarge(yylineno, to_string(num));
		exit(1);
	}
}

void checkFuncDecl(const string& name, const vector<string>& params_types) {
	if (symbol_table_map.find(name) == symbol_table_map.end()||
	!symbol_table_map[name].is_function) {
		errorUndefFunc(yylineno, name);
		exit(1);
	}
	stype& f = symbol_table_map[name];
	if (f.params_type.size() != params_types.size()) {
		errorPrototypeMismatch(yylineno, name, f.params_type);
		exit(1);
	}
	for (unsigned int i =0; i < params_types.size(); ++i) {
		if (f.params_type[i] != params_types[i] && !(f.params_type[i] == "INT" && params_types[i] == "BYTE")) {
			errorPrototypeMismatch(yylineno, name, f.params_type);
			exit(1);
		}
	}

}

void checkValidNumeric(const string& t1, const string& t2) {
	if ((t1 == "INT" || t1 == "BYTE") &&
		(t2 == "INT" || t2 == "BYTE")) 
		return;
	errorMismatch(yylineno);
	exit(1);
}

void checkValidCast(const string& t1, const string& t2) { //check if the casting is valid
	if (t1 == "BYTE" && t2 == "INT") {
		errorMismatch(yylineno);
		exit(1);
	}
	
}

string findExpType(const string& t1, const string& t2) {
	if (t1 == "INT" || t2 == "INT") {
		return "INT";
	} else {
		return "BYTE";
	}

}
