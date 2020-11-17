#include <stage1.h>
#include <ctime>
#include <iomanip>

//member function

Compiler::Compiler(char** argv) { // constructor
	sourceFile.open(argv[1]);
	listingFile.open(argv[2]);
	objectFile.open(argv[3]);
}

Compiler::~Compiler() { // destructor
	sourceFile.close();
	listingFile.close();
	objectFile.close();
}

void Compiler::createListingHeader() {
	time_t timeNow = time(NULL);
	listingFile << "STAGE0:  Huan Tran & Duc Huy Nguyen       " << ctime(&timeNow) << endl;
	listingFile << "LINE NO.              SOURCE STATEMENT" << endl << endl;
	//line numbers and source statements should be aligned under the headings
}

void Compiler::parser() {
	nextChar();
	//ch must be initialized to the first character of the source file
	if (nextToken() != "program")
		processError("keyword \"program\" expected");
	//a call to nextToken() has two effects
	// (1) the variable, token, is assigned the value of the next token
	// (2) the next token is read from the source file in order to make
	// the assignment. The value returned by nextToken() is also
	// the next token.
	prog();
	//parser implements the grammar rules, calling first rule
}

void Compiler::createListingTrailer() {
	//print "COMPILATION TERMINATED", " ERRORS ENCOUNTERED"
	listingFile << "\nCOMPILATION TERMINATED      " << errorCount << " ERRORS ENCOUNTERED" << endl;
}

// stage 0 production 1

void Compiler::prog() { //token should be "program"
	if (token != "program")
		processError("keyword \"program\" expected");
	progStmt();
	if (token == "const")
		consts();
	if (token == "var")
		vars();
	if (token != "begin")
		processError("keyword \"begin\" expected");
	beginEndStmt();
	if (token[0] != END_OF_FILE)
		processError("no text may follow \"end\"");
}

// stage 0 production 2

void Compiler::progStmt() { //token should be "program"
	string x;
	if (token != "program")
		processError("keyword \"program\" expected");
	x = nextToken();
	if (!isNonKeyId(token))
		processError("program name expected");
	if (nextToken() != ";")
		processError("semicolon expected");
	nextToken();
	code("program", x);
	insert(x, storeTypes::PROG_NAME, modes::CONSTANT, x, allocation::NO, 0);
}

// stage 0 production 3

void Compiler::consts() { //token should be "const"
	if (token != "const")
		processError("keyword \"const\" expected");
	if (!isNonKeyId(nextToken()))
		processError("non - keyword identifier must follow \"const\"");
	constStmts();
}

// stage 0 production 4

void Compiler::vars() { //token should be "var"
	if (token != "var")
		processError("keyword \"var\" expected");
	if (!isNonKeyId(nextToken()))
		processError("non - keyword identifier must follow \"var\"");
	varStmts();
}

// stage 0 production 5

void Compiler::beginEndStmt() { //token should be "begin"
	if (token != "begin")
		processError("keyword \"begin\" expected");
	if (nextToken() != "end")
		processError("keyword \"end\" expected");
	if (nextToken() != ".")
		processError("period expected");
	nextToken();
	code("end", ".");
}

// stage 0 production 6

void Compiler::constStmts() { //token should be NON_KEY_ID
	string x, y;
	if (!isNonKeyId(token))
		processError("non - keyword identifier expected");
	x = token;
	if (nextToken() != "=")
		processError("\"=\" expected");
	y = nextToken();
	if (y != "+" && y != "-" && y != "not" && !isNonKeyId(y) && !isBoolean(y) && !isInteger(y))
		processError("token to right of \"=\" illegal");
	if (y == "+" || y == "-")
	{
		if (!isInteger(nextToken()))
			processError("integer expected after sign");
		y = y + token;
	}
	if (y == "not")
	{
		if (!isBoolean(nextToken()))
			processError("boolean expected after \"not\"");
		if (token == "true")
			y = "false";
		else
			y = "true";
	}
	if (nextToken() != ";")
		processError("semicolon expected");
	if (whichType(y) != storeTypes::INTEGER && whichType(y) != storeTypes::BOOLEAN)
		processError("data type of token on the right - hand side must be INTEGER or BOOLEAN");
	insert(x, whichType(y), modes::CONSTANT, whichValue(y), allocation::YES, 1);
	x = nextToken();
	if (x != "begin" && x != "var" && !isNonKeyId(x))
		processError("non - keyword identifier, \"begin\", or \"var\" expected");
	if (isNonKeyId(x))
		constStmts();
}

// stage 0 production 7

void Compiler::varStmts() { //token should be NON_KEY_ID
	string x, y;
	if (!isNonKeyId(token))
		processError("non - keyword identifier expected");
	x = ids();
	if (token != ":")
		processError("\":\" expected");
	if (nextToken() != "integer" && token != "boolean")
		processError("illegal type follows \":\"");
	y = token;
	if (nextToken() != ";")
		processError("semicolon expected");

	if (y == "integer") insert(x, storeTypes::INTEGER, modes::VARIABLE, "1", allocation::YES, 1);
	else insert(x, storeTypes::BOOLEAN, modes::VARIABLE, "1", allocation::YES, 1);
	if (nextToken() != "begin" && !isNonKeyId(token))
		processError("non - keyword identifier or \"begin\" expected");
	if (isNonKeyId(token))
		varStmts();
}

// stage 0 production 8

string Compiler::ids() { //token should be NON_KEY_ID
	string temp, tempString;
	if (!isNonKeyId(token))
		processError("non - keyword identifier expected");
	tempString = token;
	temp = token;
	if (nextToken() == ",")
	{
		if (!isNonKeyId(nextToken()))
			processError("non - keyword identifier expected");
		tempString = temp + "," + ids();
	}
	return tempString;
}

void Compiler::execStmts() {      // stage 1, production 2
	if (isNonKeyId(token) || token == "read" || token == "write" || token == "begin") {
		execStmt();
		execStmts();
	}
	else if (token == "end");

	else processError("non-keyword identifier, \"read\", \"write\", or \"begin\" expected");
}

void Compiler::execStmt() {       // stage 1, production 3

	if (isNonKeyId(token)) {
		assignStmt();
	}

	else if (token == "read") {
		readStmt();
	}

	else if (token == "write") {
		writeStmt();
	}

	else processError("non-keyword id, read, or write statement expected");
}

void Compiler::assignStmt() {     // stage 1, production 4
	string secondOperand, firstOperand;
	if (!isNonKeyId(token))
		processError("non-keyword id expected");

	//Token must be already defined
	if (symbolTable.count(token) == 0) processError("reference to undefined variable");

	pushOperand(token);
	nextToken();

	if (token != ":=") processError("expected assignment operator");
	else pushOperator(":=");

	nextToken();

	if (token != "not" && token != "true" && token != "false" && token != "(" && token != "+"
		&& token != "-" && !isInteger(token) && !isNonKeyId(token))
		processError("Invalid expression:  'not', 'true', 'false', '(', '+', '-', integer, or non-keyword id expected");
	else express();

	if (token != ";") processError("\";\" expected");

	secondOperand = popOperand();
	firstOperand = popOperand();
	code(popOperator(), secondOperand, firstOperand);
}

void Compiler::readStmt() {       // stage 1, production 5
	if (token != "read") processError("expected \"read\" for read statement");

	nextToken();

	if (token != "(") processError("\"(\" expected");

	//read_list
	string temp = "";
	string idsPart = "";
	if (token != "(") processError("Expected (");
		
	nextToken();

	if (symbolTable.count(token) == 0 || isInteger(token))
	{
		if (symbolTable.count(token) == 0 && !isInteger(token))
			processError("reference to undefined variable");
		else
			processError("non-keyword identifier expected");
	}

	temp = ids();
	if (token != ")")
		processError("Expected ',' or ')'");
	else {
		for (uint i = 0; i < temp.length(); i++) {
			if (temp[i] == ',') {
				code("read", symbolTable.at(idsPart).getInternalName());
				idsPart = "";
			}
			else idsPart += temp[i];
		}
		code("read", symbolTable.at(idsPart).getInternalName());
	}

	if (nextToken() != ";") processError("Expected ;");
}

void Compiler::writeStmt() {      // stage 1, production 7
	if (token != "write")
		processError("expected \"write\" for write statement");

	nextToken();

	if (token != "(") processError("\"(\" expected");
	
	//write_list
	string temp = "";
	string idsPart = "";
	if (token != "(")
		processError("Expected (");
	nextToken();

	if (symbolTable.count(token) == 0 || isInteger(token))
	{
		if (symbolTable.count(token) == 0 && !isInteger(token))
			processError("reference to undefined variable");
		else
			processError("non-keyword identifier expected");
	}

	temp = ids();
	if (token != ")")
		processError("Expected ',' or  ')'");

	else {
		for (uint i = 0; i < temp.length(); i++)
		{
			if (temp[i] == ',') {
				code("write", symbolTable.at(idsPart).getInternalName());
				idsPart = "";
			}
			else idsPart += temp[i];
		}
		code("write", symbolTable.at(idsPart).getInternalName());
	}

	if (nextToken() != ";")
		processError("Expected ;");
}

void Compiler::express() {        // stage 1, production 9
	if (token != "not" && token != "true" && token != "false" && token != "(" && token != "+"
		&& token != "-" && !isInteger(token) && !isNonKeyId(token))
		processError("Invalid expression: not, true, false, (, +, -, non-key ID, or integer expected");

	term();

	if (token == "<>" || token == "=" || token == "<=" || token == ">=" || token == "<" || token == ">")
		expresses();
}

void Compiler::expresses() {      // stage 1, production 10
	string x = "";
	string operand1, operand2;
	if (token != "=" && token != "<>" && token != "<=" && token != ">=" && token != "<" && token != ">")
		processError("=, <>, <=, >=, <, or > expected");

	pushOperator(token);
	nextToken();

	if (token != "not" && token != "true" && token != "false" && token != "(" && token != "+"
		&& token != "-" && !isInteger(token) && !isNonKeyId(token))
		processError("Invalid expression:  'not', 'true', 'false', '(', '+', '-', integer, or non-keyword id expected");
	else term();

	operand1 = popOperand();
	operand2 = popOperand();

	code(popOperator(), operand1, operand2);

	if (token == "<>" || token == "=" || token == "<=" || token == ">=" || token == "<" || token == ">")
		expresses();
}

void Compiler::term() {           // stage 1, production 11
	if (token != "not" && token != "true" && token != "false" && token != "(" && token != "+"
		&& token != "-" && !isInteger(token) && !isNonKeyId(token))
		processError("Invalid expression: not, true, false, (, +, -, non-key ID, or integer expected");

	factor();

	if (token == "-" || token == "+" || token == "or") terms();
}

void Compiler::terms() {          // stage 1, production 12
	string x = "";
	string operand1, operand2;

	if (token != "+" && token != "-" && token != "or")
		processError("+, -, or expected");

	pushOperator(token);
	nextToken();

	if (token != "not" && token != "true" && token != "false" && token != "(" && token != "+"
		&& token != "-" && !isInteger(token) && !isNonKeyId(token))
		processError("Invalid expression: not, true, false, (, +, -, non-key ID, or integer expected");
	else factor();

	operand1 = popOperand();
	operand2 = popOperand();
	code(popOperator(), operand1, operand2);

	if (token == "+" || token == "-" || token == "or") terms();
}

void Compiler::factor() {         // stage 1, production 13
	if (token != "not" && token != "true" && token != "false" && token != "(" && token != "+"
		&& token != "-" && !isInteger(token) && !isNonKeyId(token))
		processError("Invalid expression: not, true, false, (, +, -, non-key ID, or integer expected");

	part();

	if (token == "*" || token == "div" || token == "mod" || token == "and")
		factors();

	else if (token == "=" || token == "<>" || token == "<=" || token == ">=" || token == "<" || token == ">" ||
		token == ")" || token == ";" || token == "-" || token == "+" || token == "or" || token == "begin");
	else processError("Invalid expression");
}

void Compiler::factors() {        // stage 1, production 14
	string x = "";
	string operand1, operand2;
	if (token != "*" && token != "div" && token != "mod" && token != "and")
		processError("Invalid FACTORS, *, div, mod, or and expected");

	pushOperator(token);
	nextToken();

	if (token != "not" && token != "+" && token != "-" && token != "(" && token != "true" && token != "false"
		&& !isInteger(token) && !isNonKeyId(token))
		processError("not, true, false, (, +, -, non-key ID, or integer expected");
	else part();

	operand1 = popOperand();
	operand2 = popOperand();
	code(popOperator(), operand1, operand2);
	if (token == "*" || token == "div" || token == "mod" || token == "and")
		factors();
}

void Compiler::part() {           // stage 1, production 15
	string x = "";
	if (token == "not")
	{
		nextToken();
		if (token == "(") {
			nextToken();
			if (token != "not" && token != "true" && token != "false" && token != "(" && token != "+"
				&& token != "-" && !isInteger(token) && !isNonKeyId(token))
				processError("Invalid expression:  'not', 'true', 'false', '(', '+', '-', integer, or non-keyword id expected");
			express();
			if (token != ")")
				processError(") expected");
			nextToken();
			code("not", popOperand());
		}

		else if (isBoolean(token)) {
			if (token == "true") {
				pushOperand("false");
				nextToken();
			}
			else {
				pushOperand("true");
				nextToken();
			}
		}

		else if (isNonKeyId(token)) {
			code("not", symbolTable.at(token).getInternalName());
			nextToken();
		}
	}

	else if (token == "+")
	{
		nextToken();
		if (token == "(") {
			nextToken();
			if (token != "not" && token != "true" && token != "false" && token != "(" && token != "+"
				&& token != "-" && !isInteger(token) && !isNonKeyId(token))
				processError("Invalid expression:  'not', 'true', 'false', '(', '+', '-', integer, or non-keyword id expected");
			express();
			if (token != ")")
				processError(") expected");
			nextToken();
		}
		else if (isInteger(token) || isNonKeyId(token)) {
			pushOperand(token);
			nextToken();
		}
	}

	else if (token == "-")
	{
		nextToken();
		if (token == "(") {
			nextToken();
			if (token != "not" && token != "true" && token != "false" && token != "(" && token != "+"
				&& token != "-" && !isInteger(token) && !isNonKeyId(token))
				processError("Invalid expression:  'not', 'true', 'false', '(', '+', '-', integer, or non-keyword id expected");
			express();
			if (token != ")")
				processError(") expected");
			nextToken();
			code("neg", popOperand());
		}
		else if (isInteger(token)) {
			pushOperand("-" + token);
			nextToken();
		}
		else if (isNonKeyId(token)) {
			code("neg", symbolTable.at(token).getInternalName());
			nextToken();
		}
	}

	else if (token == "(") {
		nextToken();
		if (token != "not" && token != "true" && token != "false" && token != "(" && token != "+"
			&& token != "-" && !isInteger(token) && !isNonKeyId(token))
			processError("Invalid expression:  'not', 'true', 'false', '(', '+', '-', integer, or non-keyword id expected");
		express();
		if (token != ")") processError(") expected");
		nextToken();
	}

	else if (isInteger(token) || isBoolean(token) || isNonKeyId(token)) {
		pushOperand(token);
		nextToken();
	}

	else processError("'not', '+', '-' '(', or integer, boolean, or non-keyword expected");
}

// Helper functions for the Pascallite lexicon
bool Compiler::isKeyword(string s) const { // determines if s is a keyword
	if (s == "program"
		|| s == "const"
		|| s == "var"
		|| s == "integer"
		|| s == "boolean"
		|| s == "begin"
		|| s == "end"
		|| s == "true"
		|| s == "false"
		|| s == "not"
		|| s == "mod"
		|| s == "div"
		|| s == "and"
		|| s == "or"
		|| s == "read"
		|| s == "write") {
		return true;
	}
	return false;
}

bool Compiler::isSpecialSymbol(char c) const { // determines if c is a special symbol
	if (c == '='
		|| c == ':'
		|| c == ','
		|| c == ';'
		|| c == '.'
		|| c == '+'
		|| c == '-'
		|| c == '*'
		|| c == '<'
		|| c == '>'
		|| c == '('
		|| c == ')') {
		return true;
	}
	return false;
}

bool Compiler::isNonKeyId(string s) const { // determines if s is a non_key_id
	return !(isKeyword(s) || isSpecialSymbol(s[0]) || isInteger(s));
}

bool Compiler::isInteger(string s) const { // determines if s is an integer
	try {
		stoi(s);
	}
	catch (const invalid_argument& ia) {
		return false;
	}
	return true;
}

bool Compiler::isBoolean(string s) const { // determines if s is a boolean
	if (s == "true" || s == "false") {
		return true;
	}
	return false;
}

bool Compiler::isLiteral(string s) const { // determines if s is a literal
	if (isBoolean(s) || s.front() == '+' || s.front() == '-' || isInteger(s))
		return true;

	return false;
}

// Action routines

void Compiler::insert(string externalName, storeTypes inType, modes inMode, string inValue, allocation inAlloc, int inUnits) {
//create symbol table entry for each identifier in list of external names
//Multiply inserted names are illegal

	string name;

	auto itr = externalName.begin();

	while (itr < externalName.end()) {
		name = "";
		while (itr < externalName.end() && *itr != ',') {
			name += *itr;
			++itr;
		}

		if (name != "")
		{
			if (symbolTable.count(name) > 0)
				processError("symbol " + name + " is multiply defined");
			else if (isKeyword(name))
				processError("illegal use of keyword");
			else //create table entry
			{
				if (name.at(0) < 97)
					symbolTable.insert(pair<string, SymbolTableEntry>(name.substr(0, 15),
						SymbolTableEntry(name, inType, inMode, inValue, inAlloc, inUnits)));
				else
					symbolTable.insert(pair<string, SymbolTableEntry>(name.substr(0, 15),
						SymbolTableEntry(genInternalName(inType), inType, inMode, inValue, inAlloc, inUnits)));
			}
		}

		if (itr == externalName.end()) break;
		else ++itr;
	}
}

storeTypes Compiler::whichType(string name) { //tells which data type a name has
	storeTypes dataType;
	if (isLiteral(name))
		if (isBoolean(name))
			dataType = storeTypes::BOOLEAN;
		else
			dataType = storeTypes::INTEGER;
	else //name is an identifier and hopefully a constant
		if (symbolTable.count(name) > 0)
			dataType = symbolTable.at(name).getDataType();
		else
			processError("reference to undefined constant");
	return dataType;
}

string Compiler::whichValue(string name) { //tells which value a name has
	string value;

	if (isLiteral(name)) {
		if (name == "true")
			value = "-1";
		else if (name == "false")
			value = "0";
		else value = name;
	}

	else //name is an identifier and hopefully a constant
		if (symbolTable.count(name) > 0 && symbolTable.at(name).getValue() != "")
			value = symbolTable.at(name).getValue();
		else
			processError("reference to undefined constant");
	return value;
}

void Compiler::code(string op, string operand1, string operand2) {
	if (op == "program")
		emitPrologue(operand1);
	else if (op == "end")
		emitEpilogue();
	else if (op == "read")
		emitReadCode(operand1, "");
	else if (op == "write")
		emitWriteCode(operand1, "");
	else if (op == "+") // this must be binary '+'
		emitAdditionCode(operand1, operand2);
	else if (op == "-") // this must be binary '-'
		emitSubtractionCode(operand1, operand2);
	else if (op == "neg") // this must be unary '-'
		emitNegationCode(operand1, "");
	else if (op == "not")
		emitNotCode(operand1, "");
	else if (op == "*")
		emitMultiplicationCode(operand1, operand2);
	else if (op == "div")
		emitDivisionCode(operand1, operand2);
	else if (op == "mod")
		emitModuloCode(operand1, operand2);
	else if (op == "and")
		emitAndCode(operand1, operand2);
	else if (op == "<>")
		emitInequalityCode(operand1, operand2);
	else if (op == "or")
		emitOrCode(operand1, operand2);
	else if (op == "<")
		emitLessThanCode(operand1, operand2);
	else if (op == ">")
		emitGreaterThanCode(operand1, operand2);
	else if (op == "<=")
		emitLessThanOrEqualToCode(operand1, operand2);
	else if (op == ">=")
		emitGreaterThanOrEqualToCode(operand1, operand2);
	else if (op == "=")
		emitEqualityCode(operand1, operand2);
	else if (op == ":=")
		emitAssignCode(operand1, operand2);
	else
		processError("compiler error since function code should not be called with illegal arguments");
}

void Compiler::pushOperator(string name) { // Push name onto opertorStk
	operatorStk.push(name);
}

string Compiler::popOperator() { // pop name from operandStk

	string temp;

	if (!operatorStk.empty()) {
		temp = operatorStk.top();
		operatorStk.pop();
	}

	else processError("compiler error; operator stack underflow");

	return temp;
}

void Compiler::pushOperand(string operand) { // Push name onto operandStk
	operandStk.push(operand);
}
string Compiler::popOperand() { //pop name from operandStk
	string temp;

	if (!operandStk.empty()) {
		temp = operandStk.top();
		operandStk.pop();
	}

	else processError("compiler error; operand stack underflow");

	return temp;
}

void Compiler::emit(string label, string instruction, string operands, string comment)
{
	//Turn on left justification in objectFile
	objectFile.setf(ios_base::left);

	//Output label in a field of width 8
	objectFile << setw(8) << label;

	//Output instruction in a field of width 8
	objectFile << setw(8) << instruction;

	//Output the operands in a field of width 24
	objectFile << setw(24) << operands;

	//Output the comment
	objectFile << comment << endl;
}

void Compiler::emitPrologue(string progName, string operand2)
{
	time_t timeNow = time(NULL);
	objectFile << "; Huan Tran & Duc Huy Nguyen       " << ctime(&timeNow);
	objectFile << "%INCLUDE \"Along32.inc\"\n"
		"%INCLUDE \"Macros_Along.inc\"\n\n";

	emit("SECTION", ".text");
	emit("global", "_start", "", "; program " + progName.substr(0, 15));
	objectFile << endl;
	emit("_start:");
}

void Compiler::emitEpilogue(string operand1, string operand2)
{
	emit("", "Exit", "{0}");
	objectFile << endl;
	emitStorage();
}

void Compiler::emitStorage()
{
	emit("SECTION", ".data");
	//for those entries in the symbolTable that have
				//an allocation of YES and a storage mode of CONSTANT
			//{ call emit to output a line to objectFile }

	for (auto data : symbolTable)
		if (data.second.getAlloc() == allocation::YES && data.second.getMode() == modes::CONSTANT)
			emit(data.second.getInternalName(), "dd", data.second.getValue(), "; " + data.first);
	objectFile << endl;

	emit("SECTION", ".bss");
	//for those entries in the symbolTable that have
				//an allocation of YES and a storage mode of VARIABLE
			//{ call emit to output a line to objectFile }

	for (auto data : symbolTable)
		if (data.second.getAlloc() == allocation::YES && data.second.getMode() == modes::VARIABLE)
			emit(data.second.getInternalName(), "resd", data.second.getValue(), "; " + data.first);
}

void Compiler::emitReadCode(string operand, string) {
	string name;

	auto itr = operand.begin();

	while (itr < operand.end()) {
		name = "";
		while (itr < operand.end() && *itr != ',') {
			name += *itr;
			++itr;
		}

		if (name != "") {
			if (symbolTable.count("name") == 0)
				processError("reference to undefined symbol");
			if (symbolTable.at("name").getDataType() != storeTypes::INTEGER)
				processError("can't read variables of this type");
			if (symbolTable.at("name").getMode() != modes::VARIABLE)
				processError("attempting to read to a read-only location");
			emit("", "call", "ReadInt", "; read int; value placed in eax");
			emit("", "mov", "[" + symbolTable.at("name").getInternalName() + "],eax", "; store eax at " + name);
			contentsOfAReg = name;
		}
	}
}

void Compiler::emitWriteCode(string operand, string) {
	string name;
	static bool definedStorage = false;

	auto itr = operand.begin();

	while (itr < operand.end()) {
		name = "";
		while (itr < operand.end() && *itr != ',') {
			name += *itr;
			++itr;
		}

		if (name != "") {
			if (symbolTable.count("name") == 0)
				processError("reference to undefined symbol");
			if (name != contentsOfAReg) {
				emit("", "mov", "eax,[" + symbolTable.at("name").getInternalName() + "]", "; load " + name + " in eax");
				contentsOfAReg = name;
			}
			if (symbolTable.at("name").getDataType() == storeTypes::INTEGER)
				emit("", "call", "WriteInt", "; write int in eax to standard out");
			else { //data type is BOOLEAN
				emit("", "cmp", "eax,0", "; compare to 0");
				string firstLabel = getLabel(), secondLabel = getLabel();
				emit("", "je", ".L" + firstLabel, "; jump if equal to print FALSE");
				emit("", "mov", "edx, TRUELIT", "; load address of TRUE literal in edx");
				emit("", "jmp", ".L" + secondLabel, "; unconditionally jump to .L");
				emit(firstLabel);
				emit("", "mov", "edx,FALSLIT", "; load address of FALSE literal in edx");
				emit(secondLabel);
				emit("", "call", "WriteString", "; write string to standard out");

				if (definedStorage == false) {
					definedStorage = true;
					objectFile << endl;
					emit("SECTION .data");
					emit("TRUELIT", "db", "'TRUE',0", "; literal string TRUE");
					emit("FALSELIT", "db", "'FALSE',0", "; literal string FALSE");
					objectFile << endl;
					emit("SECTION", ".text");
				} // end if
			} // end else
		}

		emit("", "call", "Clrf", "; write \\r \\n to standard out");
	} // end while
}

void Compiler::emitAssignCode(string operand1, string operand2) {         // op2 = op1
	if (symbolTable.at(operand1).getDataType() != symbolTable.at(operand2).getDataType())
		processError("incompatible types");
	if (symbolTable.at(operand2).getMode() != modes::VARIABLE)
		processError("symbol on left-hand size of assignment must have a storage mode of VARIABLE");
	if (operand1 == operand2) return;
	if (operand1 != contentsOfAReg) 
		emit("", "mov", "eax,[" + symbolTable.at(operand1).getInternalName() + "]", "; load " + operand1 + " in eax");
	emit("", "mov", "[" + symbolTable.at(operand2).getInternalName() + "],eax", "; " + operand2 + " = AReg");
	contentsOfAReg = operand2;
	
	if (operand1[0] == 'T')
		freeTemp();
}

void Compiler::emitAdditionCode(string operand1, string operand2) {       // op2 + op1
	string op1ExternalName, op2ExternalName;

	for (auto itr : symbolTable) {
		if (itr.second.getInternalName() == operand1) op1ExternalName = itr.first;
		break;
	}

	for (auto itr : symbolTable) {
		if (itr.second.getInternalName() == operand2) op2ExternalName = itr.first;
		break;
	}

	if (symbolTable.at(op1ExternalName).getDataType() != storeTypes::INTEGER 
		&& symbolTable.at(op2ExternalName).getDataType() != storeTypes::INTEGER)
		processError("illegal type");

	if (contentsOfAReg[0] == 'T' && contentsOfAReg != operand1 && contentsOfAReg != operand2)
	{
		
		emit("", "mov", "[" + contentsOfAReg + "],eax", "; deassign AReg");
		symbolTable.at(contentsOfAReg).setAlloc(allocation::YES);
		contentsOfAReg = "";
	}

	if (!contentsOfAReg.empty() && contentsOfAReg[0] != 'T' && contentsOfAReg != operand1 && contentsOfAReg != operand2)
	{
		contentsOfAReg = "";
	}

	if (contentsOfAReg != operand1 && contentsOfAReg != operand2) {
		emit("", "mov", "eax,[" + operand2 + "]", "; AReg = " + op2ExternalName);
		contentsOfAReg = operand2;
	}

	if (contentsOfAReg == operand2)
		emit("", "add", "eax,[" + operand1 + "]", "; AReg = " + op2ExternalName + " + " + op1ExternalName);
	else
		emit("", "add", "eax,[" + operand2 + "]", "; AReg = " + op1ExternalName + " + " + op2ExternalName);

	if (operand1[0] == 'T')
		freeTemp();
	if (operand2[0] == 'T')
		freeTemp();

	contentsOfAReg = getTemp();
	symbolTable.at(contentsOfAReg).setDataType(storeTypes::INTEGER);
	pushOperand(contentsOfAReg);
}

void Compiler::emitSubtractionCode(string operand1, string operand2) {    // op2 - op1
	string op1ExternalName, op2ExternalName;

	for (auto itr : symbolTable) {
		if (itr.second.getInternalName() == operand1) op1ExternalName = itr.first;
		break;
	}

	for (auto itr : symbolTable) {
		if (itr.second.getInternalName() == operand2) op2ExternalName = itr.first;
		break;
	}

	if (symbolTable.at(op1ExternalName).getDataType() != storeTypes::INTEGER
		&& symbolTable.at(op2ExternalName).getDataType() != storeTypes::INTEGER)
		processError("illegal type");

	if (contentsOfAReg[0] == 'T' && contentsOfAReg != operand2)
	{

		emit("", "mov", "[" + contentsOfAReg + "],eax", "; deassign AReg");
		symbolTable.at(contentsOfAReg).setAlloc(allocation::YES);
		contentsOfAReg = "";
	}

	if (!contentsOfAReg.empty() && contentsOfAReg[0] != 'T' && contentsOfAReg != operand2)
	{
		contentsOfAReg = "";
	}

	if (contentsOfAReg != operand2) {
		emit("", "mov", "eax,[" + operand2 + "]", "; AReg = " + op2ExternalName);
		contentsOfAReg = operand2;
	}

	if (contentsOfAReg == operand2)
		emit("", "sub", "eax,[" + operand1 + "]", "; AReg = " + op2ExternalName + " - " + op1ExternalName);

	if (operand1[0] == 'T')
		freeTemp();
	if (operand2[0] == 'T')
		freeTemp();

	contentsOfAReg = getTemp();
	symbolTable.at(contentsOfAReg).setDataType(storeTypes::INTEGER);
	pushOperand(contentsOfAReg);
}
void Compiler::emitMultiplicationCode(string operand1, string operand2) { // op2 * op1
	string op1ExternalName, op2ExternalName;

	for (auto itr : symbolTable) {
		if (itr.second.getInternalName() == operand1) op1ExternalName = itr.first;
		break;
	}

	for (auto itr : symbolTable) {
		if (itr.second.getInternalName() == operand2) op2ExternalName = itr.first;
		break;
	}

	if (symbolTable.at(op1ExternalName).getDataType() != storeTypes::INTEGER &&
		symbolTable.at(op2ExternalName).getDataType() != storeTypes::INTEGER)
		processError("illegal type");
	if (contentsOfAReg[0] == 'T' && contentsOfAReg != operand1 && contentsOfAReg != operand2) {
		emit("", "mov", "[" + contentsOfAReg + "],eax", "; deassign AReg");
		symbolTable.at(contentsOfAReg).setAlloc(allocation::YES);
		contentsOfAReg = "";
	}

	if (!contentsOfAReg.empty() && contentsOfAReg[0] != 'T' && contentsOfAReg != operand1 && contentsOfAReg != operand2)
		contentsOfAReg = "";

	if (contentsOfAReg != operand1 && contentsOfAReg != operand2) {
		emit("", "mov", "eax,[" + operand2 + "]", "; AReg = " + op2ExternalName);
		contentsOfAReg = operand2;
	}

	if (contentsOfAReg == operand2)
		emit("", "imul", "dword [" + operand1 + "]", "; AReg = " + operand2 + " * " + operand1);
	else emit("", "imul", "dword [" + operand2 + "]", "; AReg = " + operand1 + " * " + operand2);

	if (operand1[0] == 'T')
		freeTemp();
	if (operand2[0] == 'T')
		freeTemp();

	contentsOfAReg = getTemp();
	symbolTable.at(contentsOfAReg).setDataType(storeTypes::INTEGER);
	pushOperand(contentsOfAReg);
}


void Compiler::emitDivisionCode(string operand1, string operand2) { // op2 / op1
	string op1ExternalName, op2ExternalName;

	for (auto itr : symbolTable) {
		if (itr.second.getInternalName() == operand1) op1ExternalName = itr.first;
		break;
	}

	for (auto itr : symbolTable) {
		if (itr.second.getInternalName() == operand2) op2ExternalName = itr.first;
		break;
	}

	if (symbolTable.at(op1ExternalName).getDataType() != storeTypes::INTEGER &&
		symbolTable.at(op2ExternalName).getDataType() != storeTypes::INTEGER)
		processError("illegal type");
	if (contentsOfAReg[0] == 'T' && contentsOfAReg != operand2) {
		emit("", "mov", "[" + contentsOfAReg + "],eax", "; deassign AReg");
		symbolTable.at(contentsOfAReg).setAlloc(allocation::YES);
		contentsOfAReg = "";
	}

	if (!contentsOfAReg.empty() && contentsOfAReg[0] != 'T' && contentsOfAReg != operand2)
		contentsOfAReg = "";

	if (contentsOfAReg != operand2) {
		emit("", "mov", "eax,[" + operand2 + "]", "; AReg = " + op2ExternalName);
		contentsOfAReg = operand2;
	}
	emit("", "cdq", "", "; sign extend dividend from eax to edx:eax");
	emit("", "idiv", "dword [" + operand1 + "]", "; AReg = " + operand2 + " div " + operand1);

	if (operand1[0] == 'T')
		freeTemp();
	if (operand2[0] == 'T')
		freeTemp();

	contentsOfAReg = getTemp();
	symbolTable.at(contentsOfAReg).setDataType(storeTypes::INTEGER);
	pushOperand(contentsOfAReg);
}

void Compiler::emitModuloCode(string operand1, string operand2) {         // op2 % op1
	string op1ExternalName, op2ExternalName;

	for (auto itr : symbolTable) {
		if (itr.second.getInternalName() == operand1) op1ExternalName = itr.first;
		break;
	}

	for (auto itr : symbolTable) {
		if (itr.second.getInternalName() == operand2) op2ExternalName = itr.first;
		break;
	}

	if (symbolTable.at(op1ExternalName).getDataType() != storeTypes::INTEGER
		|| symbolTable.at(op2ExternalName).getDataType() != storeTypes::INTEGER)
		processError("illegal type");

	if (contentsOfAReg[0] == 'T' && contentsOfAReg != operand2)
	{
		emit("", "mov", "[" + contentsOfAReg + "],eax", "; deassign AReg");
		symbolTable.at(contentsOfAReg).setAlloc(allocation::YES);
		contentsOfAReg = "";
	}

	if (!contentsOfAReg.empty() && contentsOfAReg[0] != 'T' && contentsOfAReg != operand2)
		contentsOfAReg = "";

	if (contentsOfAReg != operand2) {
		emit("", "mov", "eax,[" + operand2 + "]", "; AReg = " + op2ExternalName);
		contentsOfAReg = operand2;
	}

	emit("", "cdq", "", "; sign extend dividend from eax to edx:eax");
	emit("", "idiv", "dword [" + operand1 + "]", "; AReg = " + operand2 + " div " + operand1);
	emit("", "xchg", "eax,edx", "; exchange quotient and remainder");

	if (operand1[0] == 'T')
		freeTemp();
	if (operand2[0] == 'T')
		freeTemp();

	contentsOfAReg = getTemp();
	symbolTable.at(contentsOfAReg).setDataType(storeTypes::INTEGER);
	pushOperand(contentsOfAReg);
}

void Compiler::emitNegationCode(string operand1, string) {           // -op1
	string op1ExternalName;

	for (auto itr : symbolTable) {
		if (itr.second.getInternalName() == operand1) op1ExternalName = itr.first;
		break;
	}

	if (symbolTable.at(op1ExternalName).getDataType() != storeTypes::INTEGER)
		processError("illegal type");

	if (contentsOfAReg[0] == 'T' && contentsOfAReg != operand1) {
		emit("", "mov", "[" + contentsOfAReg + "],eax", "; deassign AReg");
		symbolTable.at(contentsOfAReg).setAlloc(allocation::YES);
		contentsOfAReg = "";
	}

	if (!contentsOfAReg.empty() && contentsOfAReg[0] != 'T' && contentsOfAReg != operand1)
		contentsOfAReg = "";

	if (contentsOfAReg != operand1) {
		emit("", "mov", "eax,[" + operand1 + "]", "; AReg = " + op1ExternalName);
		contentsOfAReg = operand1;
	}

	emit("", "neg", "eax", "; AReg = -AReg");
	if (operand1[0] == 'T')
		freeTemp();

	contentsOfAReg = getTemp();
	symbolTable.at(contentsOfAReg).setDataType(storeTypes::INTEGER);
	pushOperand(contentsOfAReg);
}

void Compiler::emitNotCode(string operand1, string) {                // !op1
	string op1ExternalName;

	for (auto itr : symbolTable) {
		if (itr.second.getInternalName() == operand1) op1ExternalName = itr.first;
		break;
	}

	if (symbolTable.at(op1ExternalName).getDataType() != storeTypes::BOOLEAN)
		processError("illegal type");

	if (contentsOfAReg[0] == 'T' && contentsOfAReg != operand1) {
		emit("", "mov", "[" + contentsOfAReg + "],eax", "; deassign AReg");
		symbolTable.at(contentsOfAReg).setAlloc(allocation::YES);
		contentsOfAReg = "";
	}

	if (!contentsOfAReg.empty() && contentsOfAReg[0] != 'T' && contentsOfAReg != operand1)
		contentsOfAReg = "";

	if (contentsOfAReg != operand1) {
		emit("", "mov", "eax,[" + operand1 + "]", "; AReg = " + op1ExternalName);
		contentsOfAReg = operand1;
	}

	emit("", "not", "eax", "; AReg = !AReg");
	if (operand1[0] == 'T')
		freeTemp();

	contentsOfAReg = getTemp();
	symbolTable.at(contentsOfAReg).setDataType(storeTypes::INTEGER);
	pushOperand(contentsOfAReg);
}

void Compiler::emitAndCode(string operand1, string operand2) {            // op2 && op1
	string op1ExternalName, op2ExternalName;

	for (auto itr : symbolTable) {
		if (itr.second.getInternalName() == operand1) op1ExternalName = itr.first;
		break;
	}

	for (auto itr : symbolTable) {
		if (itr.second.getInternalName() == operand2) op2ExternalName = itr.first;
		break;
	}

	if (symbolTable.at(op1ExternalName).getDataType() != storeTypes::BOOLEAN &&
		symbolTable.at(op2ExternalName).getDataType() != storeTypes::BOOLEAN)
		processError("illegal type");

	if (contentsOfAReg[0] == 'T' && contentsOfAReg != operand1 && contentsOfAReg != operand2) {
		emit("", "mov", "[" + contentsOfAReg + "],eax", "; deassign AReg");
		symbolTable.at(contentsOfAReg).setAlloc(allocation::YES);
		contentsOfAReg = "";
	}

	if (!contentsOfAReg.empty() && contentsOfAReg[0] != 'T' && contentsOfAReg != operand1 && contentsOfAReg != operand2)
		contentsOfAReg = "";

	if (contentsOfAReg != operand1 && contentsOfAReg != operand2)
		emit("", "mov", "eax,[" + operand2 + "]", "; AReg = " + op2ExternalName);

	if (contentsOfAReg == operand2)
		emit("", "and", "eax,[" + operand1 + "]", "; AReg = " + op2ExternalName + " and " + op1ExternalName);
	else
		emit("", "and", "eax,[" + operand2 + "]", "; AReg = " + op1ExternalName + " and " + op2ExternalName);

	if (operand1[0] == 'T')
		freeTemp();
	if (operand2[0] == 'T')
		freeTemp();

	contentsOfAReg = getTemp();
	symbolTable.at(contentsOfAReg).setDataType(storeTypes::INTEGER);
	pushOperand(contentsOfAReg);
}

void Compiler::emitOrCode(string operand1, string operand2) {             // op2 || op1
	string op1ExternalName, op2ExternalName;

	for (auto itr : symbolTable) {
		if (itr.second.getInternalName() == operand1) op1ExternalName = itr.first;
		break;
	}

	for (auto itr : symbolTable) {
		if (itr.second.getInternalName() == operand2) op2ExternalName = itr.first;
		break;
	}

	if (symbolTable.at(op1ExternalName).getDataType() != storeTypes::BOOLEAN &&
		symbolTable.at(op2ExternalName).getDataType() != storeTypes::BOOLEAN)
		processError("illegal type");

	if (contentsOfAReg[0] == 'T' && contentsOfAReg != operand1 && contentsOfAReg != operand2) {
		emit("", "mov", "[" + contentsOfAReg + "],eax", "; deassign AReg");
		symbolTable.at(contentsOfAReg).setAlloc(allocation::YES);
		contentsOfAReg = "";
	}

	if (!contentsOfAReg.empty() && contentsOfAReg[0] != 'T' && contentsOfAReg != operand1 && contentsOfAReg != operand2)
		contentsOfAReg = "";

	if (contentsOfAReg != operand1 && contentsOfAReg != operand2)
		emit("", "mov", "eax,[" + operand2 + "]", "; AReg = " + op2ExternalName);

	if (contentsOfAReg == operand2)
		emit("", "or", "eax,[" + operand1 + "]", "; AReg = " + op2ExternalName + " or " + op1ExternalName);
	else
		emit("", "or", "eax,[" + operand2 + "]", "; AReg = " + op1ExternalName + " or " + op2ExternalName);


	if (operand1[0] == 'T')
		freeTemp();
	if (operand2[0] == 'T')
		freeTemp();

	contentsOfAReg = getTemp();
	symbolTable.at(contentsOfAReg).setDataType(storeTypes::INTEGER);
	pushOperand(contentsOfAReg);
}

void Compiler::emitEqualityCode(string operand1, string operand2) {       // op2 == op1
	string op1ExternalName, op2ExternalName;

	for (auto itr : symbolTable) {
		if (itr.second.getInternalName() == operand1) op1ExternalName = itr.first;
		break;
	}

	for (auto itr : symbolTable) {
		if (itr.second.getInternalName() == operand2) op2ExternalName = itr.first;
		break;
	}

	if (symbolTable.at(op1ExternalName).getDataType() != symbolTable.at(op2ExternalName).getDataType())
		processError("incompatible types");

	if (contentsOfAReg[0] == 'T' && contentsOfAReg != operand1 && contentsOfAReg != operand2) {
		emit("", "mov", "[" + contentsOfAReg + "],eax", "; deassign AReg");
		symbolTable.at(contentsOfAReg).setAlloc(allocation::YES);
		contentsOfAReg = "";
	}
	
	if (!contentsOfAReg.empty() && contentsOfAReg[0] != 'T' && contentsOfAReg != operand1 && contentsOfAReg != operand2)
		contentsOfAReg = "";

	if (contentsOfAReg != operand1 && contentsOfAReg != operand2)
		emit("", "mov", "eax,[" + operand2 + "]", "; AReg = " + op2ExternalName);

	if (contentsOfAReg == operand2)
		emit("", "cmp", "eax,[" + operand1 + "]", "; compare " + op2ExternalName + " and " + op1ExternalName);
	else
		emit("", "cmp", "eax,[" + operand2 + "]", "; compare " + op1ExternalName + " and " + op2ExternalName);

	string firstLabel = getLabel(), secondLabel = getLabel();
	emit("", "je", firstLabel , "; " + op1ExternalName + " = " + op2ExternalName + " then jump to set eax to TRUE");
	emit("", "mov", "eax, [FALSE]", "; else set eax to FALSE");

	if (symbolTable.count("false") == 0) {
		insert("false", storeTypes::BOOLEAN, modes::CONSTANT, "0", allocation::YES, 1);
		symbolTable.at("false").setInternalName("FALSE");
	}
	emit("", "jmp", secondLabel, "; unconditionally jump");
	emit(firstLabel);
	emit("", "mov", "eax,[TRUE]", "; set eax to TRUE");

	if (symbolTable.count("true") == 0) {
		insert("true", storeTypes::BOOLEAN, modes::CONSTANT, "-1", allocation::YES, 1);
		symbolTable.at("true").setInternalName("TRUE");
	}
	emit(secondLabel);

	if (operand1[0] == 'T')
		freeTemp();
	if (operand2[0] == 'T')
		freeTemp();

	contentsOfAReg = getTemp();
	symbolTable.at(contentsOfAReg).setDataType(storeTypes::INTEGER);
	pushOperand(contentsOfAReg);
}

void Compiler::emitInequalityCode(string operand1, string operand2) {     // op2 <> op1
	string op1ExternalName, op2ExternalName;

	for (auto itr : symbolTable) {
		if (itr.second.getInternalName() == operand1) op1ExternalName = itr.first;
		break;
	}

	for (auto itr : symbolTable) {
		if (itr.second.getInternalName() == operand2) op2ExternalName = itr.first;
		break;
	}

	if (symbolTable.at(op1ExternalName).getDataType() != symbolTable.at(op2ExternalName).getDataType())
		processError("incompatible types");

	if (contentsOfAReg[0] == 'T' && contentsOfAReg != operand1 && contentsOfAReg != operand2) {
		emit("", "mov", "[" + contentsOfAReg + "],eax", "; deassign AReg");
		symbolTable.at(contentsOfAReg).setAlloc(allocation::YES);
		contentsOfAReg = "";
	}

	if (!contentsOfAReg.empty() && contentsOfAReg[0] != 'T' && contentsOfAReg != operand1 && contentsOfAReg != operand2)
		contentsOfAReg = "";

	if (contentsOfAReg != operand1 && contentsOfAReg != operand2)
		emit("", "mov", "eax,[" + operand2 + "]", "; AReg = " + op2ExternalName);

	if (contentsOfAReg == operand2)
		emit("", "cmp", "eax,[" + operand1 + "]", "; compare " + op2ExternalName + " and " + op1ExternalName);
	else
		emit("", "cmp", "eax,[" + operand2 + "]", "; compare " + op1ExternalName + " and " + op2ExternalName);

	string firstLabel = getLabel(), secondLabel = getLabel();
	emit("", "jne", firstLabel, "; if " + op1ExternalName + " <> " + op2ExternalName + " then jump to set eax to TRUE");
	emit("", "mov", "eax, [FALSE]", "; else set eax to FALSE");

	if (symbolTable.count("false") == 0) {
		insert("false", storeTypes::BOOLEAN, modes::CONSTANT, "0", allocation::YES, 1);
		symbolTable.at("false").setInternalName("FALSE");
	}
	emit("", "jmp", secondLabel, "; unconditionally jump");
	emit(firstLabel);
	emit("", "mov", "eax,[TRUE]", "; set eax to TRUE");

	if (symbolTable.count("true") == 0) {
		insert("true", storeTypes::BOOLEAN, modes::CONSTANT, "-1", allocation::YES, 1);
		symbolTable.at("true").setInternalName("TRUE");
	}
	emit(secondLabel);

	if (operand1[0] == 'T')
		freeTemp();
	if (operand2[0] == 'T')
		freeTemp();

	contentsOfAReg = getTemp();
	symbolTable.at(contentsOfAReg).setDataType(storeTypes::INTEGER);
	pushOperand(contentsOfAReg);
}

void Compiler::emitLessThanCode(string operand1, string operand2) {       // op2 < op1
	string op1ExternalName, op2ExternalName;

	for (auto itr : symbolTable) {
		if (itr.second.getInternalName() == operand1) op1ExternalName = itr.first;
		break;
	}

	for (auto itr : symbolTable) {
		if (itr.second.getInternalName() == operand2) op2ExternalName = itr.first;
		break;
	}

	if (symbolTable.at(op1ExternalName).getDataType() != symbolTable.at(op2ExternalName).getDataType())
		processError("incompatible types");

	if (contentsOfAReg[0] == 'T' && contentsOfAReg != operand1 && contentsOfAReg != operand2) {
		emit("", "mov", "[" + contentsOfAReg + "],eax", "; deassign AReg");
		symbolTable.at(contentsOfAReg).setAlloc(allocation::YES);
		contentsOfAReg = "";
	}

	if (!contentsOfAReg.empty() && contentsOfAReg[0] != 'T' && contentsOfAReg != operand1 && contentsOfAReg != operand2)
		contentsOfAReg = "";

	if (contentsOfAReg != operand1 && contentsOfAReg != operand2)
		emit("", "mov", "eax,[" + operand2 + "]", "; AReg = " + op2ExternalName);

	if (contentsOfAReg == operand2)
		emit("", "cmp", "eax,[" + operand1 + "]", "; compare " + op2ExternalName + " and " + op1ExternalName);
	else
		emit("", "cmp", "eax,[" + operand2 + "]", "; compare " + op1ExternalName + " and " + op2ExternalName);

	string firstLabel = getLabel(), secondLabel = getLabel();

	if (contentsOfAReg == operand2)
		emit("", "jl", firstLabel, "; if " + op1ExternalName + " < " + op2ExternalName + " then jump to set eax to TRUE");
	else 
		emit("", "jl", firstLabel, "; if " + op2ExternalName + " < " + op1ExternalName + " then jump to set eax to TRUE");

	emit("", "mov", "eax, [FALSE]", "; else set eax to FALSE");

	if (symbolTable.count("false") == 0) {
		insert("false", storeTypes::BOOLEAN, modes::CONSTANT, "0", allocation::YES, 1);
		symbolTable.at("false").setInternalName("FALSE");
	}
	emit("", "jmp", secondLabel, "; unconditionally jump");
	emit(firstLabel);
	emit("", "mov", "eax,[TRUE]", "; set eax to TRUE");

	if (symbolTable.count("true") == 0) {
		insert("true", storeTypes::BOOLEAN, modes::CONSTANT, "-1", allocation::YES, 1);
		symbolTable.at("true").setInternalName("TRUE");
	}
	emit(secondLabel);

	if (operand1[0] == 'T')
		freeTemp();
	if (operand2[0] == 'T')
		freeTemp();

	contentsOfAReg = getTemp();
	symbolTable.at(contentsOfAReg).setDataType(storeTypes::INTEGER);
	pushOperand(contentsOfAReg);
}

void Compiler::emitLessThanOrEqualToCode(string operand1, string operand2) { // op2 <= op1
	string op1ExternalName, op2ExternalName;

	for (auto itr : symbolTable) {
		if (itr.second.getInternalName() == operand1) op1ExternalName = itr.first;
		break;
	}

	for (auto itr : symbolTable) {
		if (itr.second.getInternalName() == operand2) op2ExternalName = itr.first;
		break;
	}

	if (symbolTable.at(op1ExternalName).getDataType() != symbolTable.at(op2ExternalName).getDataType())
		processError("incompatible types");

	if (contentsOfAReg[0] == 'T' && contentsOfAReg != operand1 && contentsOfAReg != operand2) {
		emit("", "mov", "[" + contentsOfAReg + "],eax", "; deassign AReg");
		symbolTable.at(contentsOfAReg).setAlloc(allocation::YES);
		contentsOfAReg = "";
	}

	if (!contentsOfAReg.empty() && contentsOfAReg[0] != 'T' && contentsOfAReg != operand1 && contentsOfAReg != operand2)
		contentsOfAReg = "";

	if (contentsOfAReg != operand1 && contentsOfAReg != operand2)
		emit("", "mov", "eax,[" + operand2 + "]", "; AReg = " + op2ExternalName);

	if (contentsOfAReg == operand2)
		emit("", "cmp", "eax,[" + operand1 + "]", "; compare " + op2ExternalName + " and " + op1ExternalName);
	else
		emit("", "cmp", "eax,[" + operand2 + "]", "; compare " + op1ExternalName + " and " + op2ExternalName);

	string firstLabel = getLabel(), secondLabel = getLabel();

	if (contentsOfAReg == operand2)
		emit("", "jle", firstLabel, "; if " + op1ExternalName + " <= " + op2ExternalName + " then jump to set eax to TRUE");
	else
		emit("", "jle", firstLabel, "; if " + op2ExternalName + " <= " + op1ExternalName + " then jump to set eax to TRUE");

	emit("", "mov", "eax, [FALSE]", "; else set eax to FALSE");

	if (symbolTable.count("false") == 0) {
		insert("false", storeTypes::BOOLEAN, modes::CONSTANT, "0", allocation::YES, 1);
		symbolTable.at("false").setInternalName("FALSE");
	}
	emit("", "jmp", secondLabel, "; unconditionally jump");
	emit(firstLabel);
	emit("", "mov", "eax,[TRUE]", "; set eax to TRUE");

	if (symbolTable.count("true") == 0) {
		insert("true", storeTypes::BOOLEAN, modes::CONSTANT, "-1", allocation::YES, 1);
		symbolTable.at("true").setInternalName("TRUE");
	}
	emit(secondLabel);

	if (operand1[0] == 'T')
		freeTemp();
	if (operand2[0] == 'T')
		freeTemp();

	contentsOfAReg = getTemp();
	symbolTable.at(contentsOfAReg).setDataType(storeTypes::INTEGER);
	pushOperand(contentsOfAReg);
}

void Compiler::emitGreaterThanCode(string operand1, string operand2) {    // op2 > op1
	string op1ExternalName, op2ExternalName;

	for (auto itr : symbolTable) {
		if (itr.second.getInternalName() == operand1) op1ExternalName = itr.first;
		break;
	}

	for (auto itr : symbolTable) {
		if (itr.second.getInternalName() == operand2) op2ExternalName = itr.first;
		break;
	}

	if (symbolTable.at(op1ExternalName).getDataType() != symbolTable.at(op2ExternalName).getDataType())
		processError("incompatible types");

	if (contentsOfAReg[0] == 'T' && contentsOfAReg != operand1 && contentsOfAReg != operand2) {
		emit("", "mov", "[" + contentsOfAReg + "],eax", "; deassign AReg");
		symbolTable.at(contentsOfAReg).setAlloc(allocation::YES);
		contentsOfAReg = "";
	}

	if (!contentsOfAReg.empty() && contentsOfAReg[0] != 'T' && contentsOfAReg != operand1 && contentsOfAReg != operand2)
		contentsOfAReg = "";

	if (contentsOfAReg != operand1 && contentsOfAReg != operand2)
		emit("", "mov", "eax,[" + operand2 + "]", "; AReg = " + op2ExternalName);

	if (contentsOfAReg == operand2)
		emit("", "cmp", "eax,[" + operand1 + "]", "; compare " + op2ExternalName + " and " + op1ExternalName);
	else
		emit("", "cmp", "eax,[" + operand2 + "]", "; compare " + op1ExternalName + " and " + op2ExternalName);

	string firstLabel = getLabel(), secondLabel = getLabel();

	if (contentsOfAReg == operand2)
		emit("", "jg", firstLabel, "; if " + op1ExternalName + " > " + op2ExternalName + " then jump to set eax to TRUE");
	else
		emit("", "jg", firstLabel, "; if " + op2ExternalName + " > " + op1ExternalName + " then jump to set eax to TRUE");

	emit("", "mov", "eax, [FALSE]", "; else set eax to FALSE");

	if (symbolTable.count("false") == 0) {
		insert("false", storeTypes::BOOLEAN, modes::CONSTANT, "0", allocation::YES, 1);
		symbolTable.at("false").setInternalName("FALSE");
	}
	emit("", "jmp", secondLabel, "; unconditionally jump");
	emit(firstLabel);
	emit("", "mov", "eax,[TRUE]", "; set eax to TRUE");

	if (symbolTable.count("true") == 0) {
		insert("true", storeTypes::BOOLEAN, modes::CONSTANT, "-1", allocation::YES, 1);
		symbolTable.at("true").setInternalName("TRUE");
	}
	emit(secondLabel);

	if (operand1[0] == 'T')
		freeTemp();
	if (operand2[0] == 'T')
		freeTemp();

	contentsOfAReg = getTemp();
	symbolTable.at(contentsOfAReg).setDataType(storeTypes::INTEGER);
	pushOperand(contentsOfAReg);
}

void Compiler::emitGreaterThanOrEqualToCode(string operand1, string operand2) { // op2 >= op1
	string op1ExternalName, op2ExternalName;

	for (auto itr : symbolTable) {
		if (itr.second.getInternalName() == operand1) op1ExternalName = itr.first;
		break;
	}

	for (auto itr : symbolTable) {
		if (itr.second.getInternalName() == operand2) op2ExternalName = itr.first;
		break;
	}

	if (symbolTable.at(op1ExternalName).getDataType() != symbolTable.at(op2ExternalName).getDataType())
		processError("incompatible types");

	if (contentsOfAReg[0] == 'T' && contentsOfAReg != operand1 && contentsOfAReg != operand2) {
		emit("", "mov", "[" + contentsOfAReg + "],eax", "; deassign AReg");
		symbolTable.at(contentsOfAReg).setAlloc(allocation::YES);
		contentsOfAReg = "";
	}

	if (!contentsOfAReg.empty() && contentsOfAReg[0] != 'T' && contentsOfAReg != operand1 && contentsOfAReg != operand2)
		contentsOfAReg = "";

	if (contentsOfAReg != operand1 && contentsOfAReg != operand2)
		emit("", "mov", "eax,[" + operand2 + "]", "; AReg = " + op2ExternalName);

	if (contentsOfAReg == operand2)
		emit("", "cmp", "eax,[" + operand1 + "]", "; compare " + op2ExternalName + " and " + op1ExternalName);
	else
		emit("", "cmp", "eax,[" + operand2 + "]", "; compare " + op1ExternalName + " and " + op2ExternalName);

	string firstLabel = getLabel(), secondLabel = getLabel();

	if (contentsOfAReg == operand2)
		emit("", "jge", firstLabel, "; if " + op1ExternalName + " >= " + op2ExternalName + " then jump to set eax to TRUE");
	else
		emit("", "jge", firstLabel, "; if " + op2ExternalName + " >= " + op1ExternalName + " then jump to set eax to TRUE");

	emit("", "mov", "eax, [FALSE]", "; else set eax to FALSE");

	if (symbolTable.count("false") == 0) {
		insert("false", storeTypes::BOOLEAN, modes::CONSTANT, "0", allocation::YES, 1);
		symbolTable.at("false").setInternalName("FALSE");
	}
	emit("", "jmp", secondLabel, "; unconditionally jump");
	emit(firstLabel);
	emit("", "mov", "eax,[TRUE]", "; set eax to TRUE");

	if (symbolTable.count("true") == 0) {
		insert("true", storeTypes::BOOLEAN, modes::CONSTANT, "-1", allocation::YES, 1);
		symbolTable.at("true").setInternalName("TRUE");
	}
	emit(secondLabel);

	if (operand1[0] == 'T')
		freeTemp();
	if (operand2[0] == 'T')
		freeTemp();

	contentsOfAReg = getTemp();
	symbolTable.at(contentsOfAReg).setDataType(storeTypes::INTEGER);
	pushOperand(contentsOfAReg);
}

// lexical scanner

string Compiler::nextToken() //returns the next token or end of file marker
{
	token = "";
	while (token == "")
	{
		if (ch == '{') //process comment
		{
			while (nextChar() != END_OF_FILE && ch != '}')
			{ //empty body 
			}
			if (ch == END_OF_FILE)
				processError("unexpected end of file");
			else
				nextChar();
		}

		else if (ch == '}') processError("'}' cannot begin token");
		else if (isspace(ch)) nextChar();
		else if (isSpecialSymbol(ch)) {
			token = ch;
			nextChar();
		}
		else if (islower(ch)) {
			token = ch;
			while ((nextChar() == '_' || (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z')
				|| (ch >= '0' && ch <= '9')) && ch != END_OF_FILE)
			{
				token += ch;
			}
			if (ch == END_OF_FILE)
				processError("unexpected end of file");
		}
		else if (isdigit(ch)) {
			token = ch;
			while (nextChar() != END_OF_FILE && ch >= '0' && ch <= '9')
			{
				token += ch;
			}
			if (ch == END_OF_FILE)
				processError("unexpected end of file");
		}

		else if (ch == END_OF_FILE) token = ch;
		else processError("illegal symbol");
	}
	return token;
}

char Compiler::nextChar() //returns the next character or end of file marker
{
	sourceFile.get(ch);

	static char prevChar = '\n';

	if (sourceFile.eof()) {
		ch = END_OF_FILE;
		return ch;
	}
	else {
		if (prevChar == '\n') {
			listingFile << setw(5) << ++lineNo << '|';
		}
		listingFile << ch;
	}

	prevChar = ch;
	return ch;
}

// Other routines

string Compiler::genInternalName(storeTypes stype) const {
	string internName;

	switch (stype) {
	case storeTypes::PROG_NAME:
		internName = "P0";
		break;
	case storeTypes::INTEGER:
	{
		int countNum = 0;
		for (auto itr : symbolTable) {
			if (itr.second.getDataType() == storeTypes::INTEGER) ++countNum;
		}

		internName = "I" + to_string(countNum);
		break;
	}
	case storeTypes::BOOLEAN:
	{
		int countBool = 0;
		for (auto itr : symbolTable) {
			if (itr.second.getDataType() == storeTypes::BOOLEAN) ++countBool;
		}

		internName = "B" + to_string(countBool);
		break;
	}
	}

	return internName;
}

void Compiler::processError(string err)
{
	//Output err to listingFile
	//Call exit() to terminate program

	listingFile << endl << "Error: Line " << lineNo << ": " << err << endl;
	errorCount++;
	listingFile << "\nCOMPILATION TERMINATED      " << errorCount << " ERROR ENCOUNTERED" << endl;

	exit(-1);
}

void Compiler::freeTemp() {
	currentTempNo--;
	if (currentTempNo < -1)
		processError("compiler error, currentTempNo should be >= -1");
}

string Compiler::getTemp() {
	string temp;
	currentTempNo++;
	temp = "T" + currentTempNo;
	if (currentTempNo > maxTempNo) {
		insert(temp, storeTypes::UNKNOWN, modes::VARIABLE, "", allocation::NO, 1);
		symbolTable.at(temp).setInternalName(temp);
		maxTempNo++;
	}

	return temp;
}

string Compiler::getLabel() {
	static int labelCount = -1;
	string label;
	labelCount++;
	label = "L" + to_string(labelCount);
	return label;
}

bool Compiler::isTemporary(string s) const { // determines if s represents a temporary
	if (s[0] == 'T') return true;
	else return false;
}