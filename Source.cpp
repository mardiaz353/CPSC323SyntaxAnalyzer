/*------------------
Name1: Maria Diaz
Name2: Shivam Sha

Assignment2 : Syntax Analyzer

Date of Completion: 4/40/19

------------Project Key--------------
//Production rules:

i			+		-		*		/		(		)		$
S	i = E
E	 TQ											TQ
Q			  +TQ	  -TQ								e		e
T	FR											FR
R				e		e	  *FR	  /FR				e		e
F	 i											(E)
*/

/*
S = Statement
E = Expresseion
Q = Expression Prime
T = Term
R = Term Prime
F = Factor
i = Identifier
*/
//---------------------Program Begin---------------
#include <iostream>
#include <fstream>
#include <string>
#include <cctype>
#include <vector>
#include <utility>
#include <stack>
#include <map>

using namespace std;

//List of Keywords Defined by language requirements
string KEYWORDS[20] = { "int", "float", "bool", "if", "else", "then",
"do", "while", "whileend", "do", "doend",
"for", "and", "or" , "function" };



//List of Operators and Separators Defined by Language requirments

char Ops_Seps_list[21] =
{
	'*','+','-','=','/','>','<','%',							//0-7 = Ops	
	'\'','(',')','{','}','[',']',',','.',':',';','!',' '//8-21 = Seps
};

//Finite State Machine Matrix
int FSM[6][7] = {
	/*0*/	1,2,4,0,0,0,0,
	/*1*/	1,1,4,0,0,0,0,
	/*2*/	5,2,4,0,3,0,0,
	/*3*/	5,3,4,0,3,0,0,
	/*4*/	4,4,0,4,4,4,4,
	/*5*/	0,0,0,0,0,0,0
};

enum symbols {
	//Non-Terminals
	S, E, Q, T, R, F,

	IDENTIFIER, PLUS, MINUS,
	ASTERISK, BACKSLASH, RIGHT_PARENS,
	LEFT_PARENS, DOLLAR_SIGN, EQUAL_SIGN,
	EPSILON
};

//Function Signatures -- Reference Discription Below main()
int testChar(char ch, int state);
void displayTemp(string temp);
void displayChar(char ch);

//------------------------main() Begin----------------------------------------------------------------------------------------

string oFile = "output.txt";
vector<pair<string, string> > output;

int main()
{
	map<symbols, map<symbols, int> >  table;

	//Add production rules to the table
	table[S][IDENTIFIER] = 1;//i = E
	table[E][IDENTIFIER] = 2;//TQ
	table[Q][PLUS] = 3;//+TQ
	table[Q][MINUS] = 4;//-TQ
	table[Q][RIGHT_PARENS] = 5;// epsilon
	table[Q][DOLLAR_SIGN] = 5;//epsilon
	table[T][IDENTIFIER] = 6;//FR
	table[T][RIGHT_PARENS] = 6;//FR
	table[R][PLUS] = 5;//epsilon
	table[R][MINUS] = 5;//epsilon
	table[R][BACKSLASH] = 7;//FR
	table[R][RIGHT_PARENS] = 5;//epsilon
	table[R][DOLLAR_SIGN] = 5;//epsilon
	table[F][IDENTIFIER] = 8;//i
	table[F][LEFT_PARENS] = 9;//(E)
	string tokenString = "";
	string filename;
	char  chTest = 2;
	string input = ""; //"! Find largest value between two numbers!int num1, num2, large$ if (num1 > num2){ large = num1$;} else {large = num2$;}"; //Test String
	string temp;
	char ch;
	int currentState = 0;
	int lengthofString = 0;
	//Initialize the symbols stack
	stack<symbols> symbolsStack;
	symbolsStack.push(DOLLAR_SIGN);
	symbolsStack.push(S);

	//Input Variable for to pull information from file
	cout << "What is the file name?: ";
	cin >> filename;
	ifstream inFile(filename);

	//Open File
	if (!inFile.is_open()) {
		cout << "Cannot open input file" << endl;
		return 1;
	}
	ofstream outFile(oFile);
	if (!outFile.is_open())
	{
		cout << "Cannot open output file" << endl;
		return 1;
	}

	cout << endl << endl;

	cout << "Token\t\t\tLexeme" << endl << "------------------------------------" << endl;
	outFile << "Token\t\t\tLexeme" << endl << "------------------------------------" << endl;

	cout << int(chTest) << endl;

	//Loop until there is no more available lines to read from the file.
	while (getline(inFile, input))
	{

		lengthofString = input.length(); // Get length of 'input'


		for (int i = 0; i < lengthofString; i++) //test character by character in the 'input'
		{
			ch = input[i];

			if (currentState != 4) //Comment Break if current state == 4
			{
				currentState = testChar(ch, currentState); //State Transition Call

				switch (currentState)
				{
				case 0:					 //Once Seperator || Operator is entered. Display temp and ch
					displayTemp(temp);
					displayChar(ch);
					temp = "";
					break;

				case 1:		//Continue to Concatanate So long as valid input
					temp += ch;
					break;
				case 2:
					if (ch == '.') //Grab Period if inputted
					{
						temp += ch;
						break;
					}
					temp += int(ch);
					break;
				case 3:
					if (ch == '.')
					{
						temp += ch;
						break;
					}
					temp += int(ch);
					break;

				case 4:					//Display temp Immediately Before comment block
					displayTemp(temp);
					temp = "";
					break;
				case 5:					//Invalid Inputs, Display Invalid
					displayTemp(temp);
					displayChar(ch);
					temp = "";
					break;
				}

			}
			else if (ch == '!' && currentState == 4)	//Comment break if current state == 4
			{
				currentState = FSM[currentState][2];

			}

		}
		displayTemp(temp);						//Ignore and clear temp --  Start of new line
		temp = "";
	}
	inFile.close();	//Finished Reading File -> Close File

					//_____________________Lexical Analysis Over_____________________________________________________
					//_____________________Output token lexeme list__________________________________________________
	cout << endl;
	for (int i = 0; i < output.size(); i++)
	{
		cout << output[i].first << "\t" << output[i].second << endl;
		outFile << output[i].first << "\t" << output[i].second << endl;
	}

	cout << endl << endl;
	//Create the vector that will store token strings converted to enum
	vector <pair<symbols, string> > showSymbolLexeme;//make new vector with enum symbols
	for (int i = 0; i < output.size(); i++) {
		if (output[i].first == "Identifier:\t") {
			showSymbolLexeme.push_back(pair<symbols, string>(IDENTIFIER, output[i].second));
		}
		else if (output[i].second == "+") {
			showSymbolLexeme.push_back(pair<symbols, string>(PLUS, output[i].second));
		}
		else if (output[i].second == "-") {
			showSymbolLexeme.push_back(pair<symbols, string>(MINUS, output[i].second));
		}
		else if (output[i].second == "*") {
			showSymbolLexeme.push_back(pair<symbols, string>(ASTERISK, output[i].second));
		}
		else if (output[i].second == "/") {
			showSymbolLexeme.push_back(pair<symbols, string>(BACKSLASH, output[i].second));
		}
		else if (output[i].second == "(") {
			showSymbolLexeme.push_back(pair<symbols, string>(RIGHT_PARENS, output[i].second));
		}
		else if (output[i].second == ")") {
			showSymbolLexeme.push_back(pair<symbols, string>(LEFT_PARENS, output[i].second));
		}
		else if (output[i].second == "$") {
			showSymbolLexeme.push_back(pair<symbols, string>(DOLLAR_SIGN, output[i].second));
		}
		else if (output[i].second == "=") {
			showSymbolLexeme.push_back(pair<symbols, string>(EQUAL_SIGN, output[i].second));
		}
		else {
			cout << "Error: Character does not exist in this grammar" << endl;
		}
	}
	showSymbolLexeme.push_back(pair<symbols, string>(DOLLAR_SIGN, "$")); //Put a '$' at the end of the vector that's storing the string 
	for (int i = 0; i < showSymbolLexeme.size(); i++) {
		cout << "showSymbolLexeme[" << i << "] = " << showSymbolLexeme[i].first << ", " << showSymbolLexeme[i].second << endl;
	}

	//___________________Begin Syntax Analysis___________________________________________
	
	while (symbolsStack.size() > 0) {
		for (int i = 0; i < showSymbolLexeme.size(); i++) {
			for (int j = 0; j < sizeof(symbols); j++) {
				if (showSymbolLexeme[i].first == IDENTIFIER) {
					tokenString = "Identifier";
				}
				else if (showSymbolLexeme[i].first == PLUS || showSymbolLexeme[i].first == MINUS ||
					showSymbolLexeme[i].first == ASTERISK || showSymbolLexeme[i].first == BACKSLASH
					|| showSymbolLexeme[i].first == DOLLAR_SIGN || showSymbolLexeme[i].first == EQUAL_SIGN) {
					tokenString = "Operator";
				}
				else if (showSymbolLexeme[i].first == RIGHT_PARENS || showSymbolLexeme[i].first == LEFT_PARENS) {
					tokenString = "Seperator";
				}
				else if (showSymbolLexeme[i].first == EPSILON) {
					tokenString = "e";
				}
				else {
					tokenString = "Invalid Syntax";
				}
			}


			if (symbolsStack.top() == showSymbolLexeme[i].first) {
				cout << "Token: " << tokenString << "\t ";
				cout << "Lexeme: " << showSymbolLexeme[i].second << endl;
				symbolsStack.pop();//Allow the loop to increment now
			}
			else if (symbolsStack.top() == EQUAL_SIGN) {
				cout << "Token: " << tokenString << "\t ";
				cout << "Lexeme: " << showSymbolLexeme[i].second << endl;
				symbolsStack.pop();
			}
			else {
				cout << "Rule " << table[symbolsStack.top()][showSymbolLexeme[i].first] << endl;
				switch (table[symbolsStack.top()][showSymbolLexeme[i].first]) {
				case 1: //S -> i = E
					symbolsStack.pop();
					symbolsStack.push(E);          //E
					symbolsStack.push(EQUAL_SIGN);// =
					symbolsStack.push(IDENTIFIER);// i
					cout << "<Statement> -> <identifier> = <Expression>" << endl;//Don't move onto the next element, only for lexer()
					i--;
					break;

				case 2://E -> TQ
					symbolsStack.pop();
					symbolsStack.push(Q);//Q
					symbolsStack.push(T);//T
					cout << "<Expression> -> <Term><Expression Prime>" << endl;
					i--;
					break;

				case 3://Q -> +TQ
					symbolsStack.pop();
					symbolsStack.push(Q);//Q
					symbolsStack.push(T);  //T
					symbolsStack.push(PLUS);  //+
					cout << "<Expression Prime> -> +<Term><Expression Prime>" << endl;
					i--;
					break;

				case 4://Q -> -TQ
					symbolsStack.pop();
					symbolsStack.push(Q);	 //Q
					symbolsStack.push(T);	 //T
					symbolsStack.push(MINUS);//-
					cout << "<Expression Prime> -> -<Term><Expression Prime>" << endl;
					i--;
					break;

				case 5://Q -> e
					   //With ( or $
					   //R -> e
					   //with + or -
					symbolsStack.pop();
					//symbolsStack.push(EPSILON);//e
					cout << "<Expression Prime> or <Term Prime> -> <e>" << endl;
					i--;
					break;

				case 6://T -> FR
					symbolsStack.pop();
					symbolsStack.push(R);//R
					symbolsStack.push(F);//F
					cout << "<Term> -> <Factor><Term Prime>" << endl;
					i--;
					break;

				case 7:// R -> FR
					table[R][BACKSLASH] = 7;//FR
					symbolsStack.pop();
					symbolsStack.push(R);//R
					symbolsStack.push(F);//F
					cout << "<Term Prime> -> <Factor><Term Prime>" << endl;
					i--;
					break;

				case 8:// F -> i
					symbolsStack.pop();
					symbolsStack.push(IDENTIFIER);//i
					cout << "<Factor> -> <identifier>" << endl;
					i--;
					break;

				case 9://F -> (E)
					symbolsStack.pop();
					symbolsStack.push(RIGHT_PARENS);// )
					symbolsStack.push(E);//            E
					symbolsStack.push(LEFT_PARENS);//  )
					cout << "<Factor> -> (Expression)" << endl;
					i--;
					break;

				default:
					cout << "parsing table defaulted" << endl;
					//return 0;
					break;
				}
			}
			cout << endl;
		}
		outFile.close();
	}
system("pause");
return 0;
}
	

//-------------------------main() End---------------------


/*------------------testChar() Begin----------------------
This Function is designed to navigate through the State Transition table.
Given the specific character being currently read from the file, the state will
change according to the table. Table is initialized in global scope but can be
referenced as such:
char num  '!' sep '.' ops  '\n'
new(0)			1,	2,	4,	0,	0,	0,  0
string(1)		1,	1,	4,	0,	0,	0,  0
int(2)			5,	2,	4,	0,	3,	0,  0
float(3)		5,	3,	4,	0,	3,	0,  0
comment(4)		4,	4,	0,	4,	4,	4,  4
invalid(5)		0,	0,	0,	0,	0,	0   0
*/
int testChar(char ch, int state)
{
	int currentState = state;

	if (isalpha(ch))
	{
		currentState = FSM[currentState][0];
		return currentState;
	}
	else if (isdigit(int(ch)) != 0 || ch == '$')
	{
		currentState = FSM[currentState][1];
		return currentState;
	}
	else if (ch == Ops_Seps_list[19]) //[20] == '!'
	{
		currentState = FSM[currentState][2];
		return currentState;
	}
	else if (ch == Ops_Seps_list[16]) // [16] == '.'
	{
		currentState = FSM[currentState][4];
		return currentState;
	}
	else if (isspace(ch))
	{
		currentState = FSM[currentState][6];
		return currentState;
	}
	else
	{
		int i = 0;
		for (i = 0; i < 8; i++)
		{
			if (ch == Ops_Seps_list[i] || isspace(ch))
			{
				currentState = FSM[currentState][5];
				return currentState;
			}
		}
		for (i = 8; i < 20; i++)
		{
			if (ch == Ops_Seps_list[i] || isspace(ch))
			{
				currentState = FSM[currentState][3];
				return currentState;
			}
		}
	}
}
//---------------testChar() End-----------------------------



/*-----------------displayTemp() Begin----------------------
This function is called when ever a seperator, operator, or invalid word
is entered. Once entered, displayTemp() will test the variable input and decide
whether it is a keyword, identifier, integer, float, or default to invalid
if all others are not found.
*/
void displayTemp(string temp)
{

	//Display Temp as long as Temp is not Empty
	if (temp != "")
	{
		bool is_keyword = 0;
		//----------------Test for Keyword
		for (int i = 0; i < 20; i++) {

			if (KEYWORDS[i] == temp)
			{
				//cout << "Keyword:\t=\t" << temp << endl;
				output.push_back(pair<string, string>("Keyword:\t", temp));
				return;
			}
		}
		is_keyword = 1;
		//---------------Test for Identifier
		if (isalpha(temp[0]) && is_keyword == 1)
		{
			//cout << "Identifier:\t=\t" << temp << endl;
			output.push_back(pair<string, string>("Identifier:\t", temp));
			return;
		}
		//---------------Test for Int vs Float
		int test_float = 0;
		int dot_count = 0;
		for (int i = 0; i < temp.length(); i++)
		{
			if (temp[i] == '.')
			{
				test_float = 1;
				dot_count++;

			}
			if (temp[i] == '$')
			{
				test_float = 2;
			}
		}
		if (dot_count > 1)
		{
			test_float = 2;
		}
		if (test_float == 0)		// Int
		{
			//cout << "Integer:\t=\t" << temp << endl;
			output.push_back(pair<string, string>("Integer:\t", temp));
			return;
		}
		else if (test_float == 1)	// Float
		{
			//cout << "Float:\t=\t" << temp << endl;
			output.push_back(pair<string, string>("Float:\t\t", temp));
			return;
		}
		//---------------Defualt to Invalid if all Other cases Fail
		//cout << "Invalid\t=\t" << temp << endl;
		output.push_back(pair<string, string>("Invalid:\t", temp));
		return;
	}
}
//------------------displayTemp() End-------------------------



/*---------------displayChar() Begin--------------------------
This function is called when ever a seperator, operator, or invalid word
is entered. Once entered, displayChar() will test the variable to see whether
the input was a Separator or Operator WITH EXCEPTION to special character
which inlcude: '!' and ' '(white space)*/
void displayChar(char ch)
{
	//---------------Display Separator or Operator

	string temp = string(1, ch);
	for (int i = 0; i < 8; i++)
	{
		if (ch == Ops_Seps_list[i])
		{
			//cout << "Operator\t=\t" << ch << endl;
			output.push_back(pair<string, string>("Operator:\t", temp));
		}
	}
	for (int i = 8; i < 22; i++)
	{
		if (ch == Ops_Seps_list[i] && ch != '!' && !isspace(ch))
		{
			//cout << "Separator\t=\t" << ch << endl;
			output.push_back(pair<string, string>("Separator:\t", temp));
		}
	}

}
//-------------------displayChar() End------------------------
