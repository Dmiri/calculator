#include <iostream>
#include <string>
#include <vector>
#include <Windows.h>

using namespace std;
 
//-------------------------------------------------------

bool   is_declared(string var);
double term();
double primary();
double statement();
double expression();
double declaration();
double get_value(string s);
double define_name(string var, double val);
void   calculate();
void   set_value(string s, double d);
void   error(TCHAR err[]);

//-------------------------------------------------------
const char quit = 'q';
const char print = ';';
const char number = '8';
const char name = 'a';			//  �������  name
const char let = 'L';			//  �������  let
const string declkey = "let";	//  ��������   �����  let
//-------------------------------------------------------

int main() {
	setlocale(LC_ALL, "RU");
	try {
		define_name("pi", 3.1415926535);
		define_name("e", 2.7182818284);
		cout << "\t\tCalculator\n"
			<< "�������������� ��������� �������� ��� �������:"
			<< "\n+ �������� \t- ���������"
			<< "\n* ��������� \t^ ���������� � �������"
			<< "\n\\ ������� \t% ������� ������"
			<< "\n������ \";\" �������� ��������� ���������."
			<< "\n��������!"
			<< "\n�� ����� ������ ������������ �������� ������ ���������� ��������."
			<< "\n��������� ������ ���� ������� �� �� �������������."
			<< "\n��� �������� ����������� ��������� �������"
			<< "\n�������� ����� \"let\" ������ ��� ���������� \"=\" �������� \";\""
			<< "\n���������� ��� ������ ����� ���������:"
			<< "\n\"pi\", 3.1415926535"
			<< "\n\"e\", 2.7182818284"
			<< "\n��� ������ ������� \"q\".\n";
		calculate();
		system("pause");
		return 0;
	}
	catch (exception& e) {
		cerr << e.what() << endl;
		system("pause");
		return 1;
	}
	catch (...) {
		//error(L"exception");
		cerr << "exception \n";
		system("pause");
		return 2;
	}
}
//-------------------------------------------------------

class Token {
public:
	char kind;     //  ��� �������
	double value;  //  ��� ����� :  ��������
	string name;   //  ��� ��������
	Token(char ch) :kind(ch), value(0) { }
	Token(char ch, double val) :kind(ch), value(val) { }
	Token(char ch, string n) :kind(ch), name(n) {}
};
//-------------------------------------------------------

class Token_stream {	//������ ������ Token_stream, ����������� ������ �� ������ cin
public:
	Token_stream() :full(false), buffer(0) {} // � ������ ��� �� ������ ������� ������ Token
	void putback(Token t); //  ���������� ������ ������ Token �������
	void ignore(char c);
	Token get();       //  �������� ������ ������ Token
					   
private:
	bool full;    //  ��������� �� � ������ ������ ������ Token?
	Token buffer; //  ����� �������� ������ ������ Token,
				  //  ������������ � ����� �������� putback()
};
//-------------------------------------------------------

class Variable {
public:
	string name;
	double value;
	Variable(string n, double v) :name(n), value(v) { }
};
//-------------------------------------------------------

Token_stream ts;
vector<Variable> var_table;
//-------------------------------------------------------

//Functin for class Token_streame
//Token_stream::Token_stream() :full(false), buffer(0) {} // � ������ ��� �� ������ ������� ������ Token

void Token_stream::putback(Token t) {
	if (full) error(L"putback()  �   ������   �����");
	buffer = t;  // �������� ������ t � �����
	full = true; // ����� ������ �����
}

void Token_stream::ignore(char c) {
	if (full && c == buffer.kind) {
		full = false;
		return;
	}
	full = false;
	char ch = 0;
	while (cin >> ch)
		if (ch == c) return;
}

Token Token_stream::get() {
	if (full) { // ���� � ������ ���� �������, ������� �� ������
		full = false;
		return buffer;
	}
	char ch;
	cin >> ch; // �������� �������� �� ��, ��� �������� >> ���������� ����������� 
			// (�������, ������� �������� �� ����� ������, ������� ��������� � �.�.)
	if (ch >= 0 && ch <= 255) {
		switch (ch) {
		case print:     // ��� ������
		case quit:     // ��� ������
		case '(': case ')':
		case '+': case '-':	case '*': case '/': case '%': case '=': case '^':
			return Token(ch); // ����� ������ ������ ������������ ���� ���
		case '.':
		case '0': case '1': case '2': case '3': case '4':
		case '5': case '6': case '7': case '8': case '9': {
			cin.putback(ch);  //  ���������� ����� ������� � ����� �����
			double val;
			cin >> val;      // ��������� ����� � ��������� ������
			return Token(number, val); // ����� ������ '8' �������� "�����"
		}
		default:
			if (isalpha(ch)) {
				string s;
				s += ch;
				while (cin.get(ch) && isalnum(ch))//(isalpha(ch) || isdigit(ch)))
					s += ch;
				cin.putback(ch);
				if (s == declkey) return Token(let);
				return Token(name, s);
			}
		}
	}
	error(L" ������������   ������� ");
	return get();

}

//-------------------------------------------------------
double expression() {
	double left = term();    //  ���������   �   ���������   ����
	Token t = ts.get();      //  ��������   ���������   �������
							 //  ��   ������   ������
	while (true) {
		switch (t.kind) {
		case '+':
			left += term(); //  ���������   �   ���������   ����
			t = ts.get();
			break;
		case '-':
			left -= term(); //  ���������   �   ��������   ����
			t = ts.get();
			break;
		
		default:
			ts.putback(t);  //  ��������   ������  t  ������� �   �����   ������
			return left;    //  ����� :  ��������  +  �  �  ��� ;
							//  ����������   �����
		}
	}
}

double term() {
	double left = primary();
	Token t = ts.get(); // �������� ��������� ������� �� ������ ������
	while (true) {
		switch (t.kind) {
		case '*':
			left *= primary();
			t = ts.get();
			break;
		case '/': {
			double d = primary();
			if (d == 0) error(L"������� �� 0.");
			left /= d;
			t = ts.get();
			break;
		}
		case '%': {
			double d = primary();
			if (left != int(left))	error(L"����� �������� �����������.");
			if (d != int(d))		error(L"������ �������� �����������.");
			if (d == 0)				error(L"������� �� 0.");
			left = int(left) % int(d);
			t = ts.get();
			break;
		}
		case '^': {
			double d = primary();
			if (d != int(d))
				error(L"������ �������� �����������.");
			else {
				if (d == 0)
					left = 1;
				if (d > 0) for (double i = left; d > 1; d--) {
					left *= i; 
				}
				if (d < 0) {
					double i = left;
					left = 1;
					for (; d < 0; d++)
						left = left / i;
				}
			}
			t = ts.get();
			break;
		}
		default:
			ts.putback(t); //  ��������   ������  t  �������   �   �����   ������
			return left;
		}
	}
}

double primary() {
	Token t = ts.get();
	switch (t.kind) {
	case '(': { //  ���������   ��������  '('  ���������  ')'    
		double d = expression();
		t = ts.get();
		if (t.kind != ')') error(L"�� ������� ������.");
		return d;
	}
	case number:             //  ����������  '8'  ���   �������������   �����
		return t.value; //  ����������   ��������   �����
	case '-':
		return -primary();
	case '+':
		return primary();
	default:
		return get_value(t.name);
	}
}

void calculate() {
	try {
		while (cin) {
			cout << "> ";
			Token t = ts.get();
			while (t.kind == print)
				t = ts.get();
			if (t.kind == quit) return;
			ts.putback(t);
			cout << "=" << statement() << '\n';
		}
	}
	catch (exception& e) {
		cerr << e.what() << endl;
		ts.ignore(print);
	}
}

double statement() {
	Token t = ts.get();
	switch (t.kind) {
	case let:
		return declaration();
	default:
		ts.putback(t);
		return expression();
	}
}

//-------------------------------------------------------
double declaration() {
	//  �������������� ,  ���   ��   �����   ��������   ��������   �����  "let"
	//  ��������� : name =  ���������
	//  ����������� ���������� � ������ "name" � ��������� ���������, �������� "����������"
	Token t = ts.get();
	if (t.kind != name)
		error(L" �   ����������   ���������   ����������	name");
	string var_name = t.name;
	Token t2 = ts.get();
	if (t2.kind != '=') {
		error(L" �   ����������   ��������   ������  = ");
		cerr << var_name;
	}
	double d = expression();
	define_name(var_name, d);
	error(L"���������� ��������� �������.");
	return d;
}

double get_value(string s) {
	// ���������� �������� ���������� � ������ s
	for (unsigned int i = 0; i < var_table.size(); ++i)
		if (var_table[i].name == s)
			return var_table[i].value;
	error(L"get: �������������� ���������� ");
	cerr << s;
	return 0;
}
//  ���������   ����  (s, d)  �   ������  var_table
void set_value(string s, double d) {
	for (unsigned int i = 0; i < var_table.size(); ++i)
		if (var_table[i].name == s) {
			var_table[i].value = d;
			return;
		}
	error(L"get: �������������� ����������");
	cerr << s;
}


//  ����   ��   ����������  var  �   �������  var_table?
bool is_declared(string var) {
	for (unsigned int i = 0; i < var_table.size(); ++i)
		if (var_table[i].name == var) return true;
	return false;
}
//  ���������   ����  (var,val)  �   ������  var_table
double define_name(string var, double val) {
	if (is_declared(var)) {
		cerr << var; error(L"��������� �������������� ���������.");
	}
	var_table.push_back(Variable(var, val));
	return val;
}
//-------------------------------------------------------

/*void error(string s1, string s2)
{
	throw runtime_error(s1 + s2);
}*/

void error(TCHAR err[]) {
	MessageBox(NULL, err, L"Error!", MB_OK);
}