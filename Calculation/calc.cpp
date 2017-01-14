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
const char name = 'a';			//  лексема  name
const char let = 'L';			//  лексема  let
const string declkey = "let";	//  ключевое   слово  let
//-------------------------------------------------------

int main() {
	setlocale(LC_ALL, "RU");
	try {
		define_name("pi", 3.1415926535);
		define_name("e", 2.7182818284);
		cout << "\t\tCalculator\n"
			<< "Поддерживаются следующие действия над числами:"
			<< "\n+ сложение \t- вычитание"
			<< "\n* умножение \t^ возведение в степень"
			<< "\n\\ деление \t% деление нацело"
			<< "\nСимвол \";\" означает окончание выражения."
			<< "\nВнимание!"
			<< "\nНа даннй момент осуществлена подержка только латинского алфавита."
			<< "\nКонстанта должна быть создана до ее использования."
			<< "\nДля создания собственной константы введите"
			<< "\nключевое слово \"let\" пробел имя переменной \"=\" значение \";\""
			<< "\nВпрограмме уже заданы такие константы:"
			<< "\n\"pi\", 3.1415926535"
			<< "\n\"e\", 2.7182818284"
			<< "\nДля выхода введите \"q\".\n";
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
	char kind;     //  вид лексемы
	double value;  //  для чисел :  значение
	string name;   //  для констант
	Token(char ch) :kind(ch), value(0) { }
	Token(char ch, double val) :kind(ch), value(val) { }
	Token(char ch, string n) :kind(ch), name(n) {}
};
//-------------------------------------------------------

class Token_stream {	//объект класса Token_stream, считывающий данные из потока cin
public:
	Token_stream() :full(false), buffer(0) {} // в буфере нет ни одного объекта класса Token
	void putback(Token t); //  возвращает объект класса Token обратно
	void ignore(char c);
	Token get();       //  получает объект класса Token
					   
private:
	bool full;    //  находится ли в буфере объект класса Token?
	Token buffer; //  здесь хранится объект класса Token,
				  //  возвращаемый в поток функцией putback()
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
//Token_stream::Token_stream() :full(false), buffer(0) {} // в буфере нет ни одного объекта класса Token

void Token_stream::putback(Token t) {
	if (full) error(L"putback()  в   полный   буфер");
	buffer = t;  // копируем объект t в буфер
	full = true; // буфер теперь полон
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
	if (full) { // если в буфере есть лексема, удаляем ее оттуда
		full = false;
		return buffer;
	}
	char ch;
	cin >> ch; // обратите внимание на то, что оператор >> пропускает разделители 
			// (пробелы, символы перехода на новую строку, символы табуляции и т.д.)
	if (ch >= 0 && ch <= 255) {
		switch (ch) {
		case print:     // для печати
		case quit:     // для выхода
		case '(': case ')':
		case '+': case '-':	case '*': case '/': case '%': case '=': case '^':
			return Token(ch); // пусть каждый символ представляет себя сам
		case '.':
		case '0': case '1': case '2': case '3': case '4':
		case '5': case '6': case '7': case '8': case '9': {
			cin.putback(ch);  //  возвращаем цифру обратно в поток ввода
			double val;
			cin >> val;      // считываем число с плавающей точкой
			return Token(number, val); // пусть символ '8' означает "число"
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
	error(L" Неправильная   лексема ");
	return get();

}

//-------------------------------------------------------
double expression() {
	double left = term();    //  считываем   и   вычисляем   Терм
	Token t = ts.get();      //  получаем   следующую   лексему
							 //  из   потока   лексем
	while (true) {
		switch (t.kind) {
		case '+':
			left += term(); //  вычисляем   и   добавляем   Терм
			t = ts.get();
			break;
		case '-':
			left -= term(); //  вычисляем   и   вычитаем   Терм
			t = ts.get();
			break;
		
		default:
			ts.putback(t);  //  помещаем   объект  t  обратно в   поток   лексем
			return left;    //  финал :  символов  +  и  –  нет ;
							//  возвращаем   ответ
		}
	}
}

double term() {
	double left = primary();
	Token t = ts.get(); // получаем следующую лексему из потока лексем
	while (true) {
		switch (t.kind) {
		case '*':
			left *= primary();
			t = ts.get();
			break;
		case '/': {
			double d = primary();
			if (d == 0) error(L"Деление на 0.");
			left /= d;
			t = ts.get();
			break;
		}
		case '%': {
			double d = primary();
			if (left != int(left))	error(L"Левое значение некорректно.");
			if (d != int(d))		error(L"Правое значение некорректно.");
			if (d == 0)				error(L"Деление на 0.");
			left = int(left) % int(d);
			t = ts.get();
			break;
		}
		case '^': {
			double d = primary();
			if (d != int(d))
				error(L"Правое значение некорректно.");
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
			ts.putback(t); //  помещаем   объект  t  обратно   в   поток   лексем
			return left;
		}
	}
}

double primary() {
	Token t = ts.get();
	switch (t.kind) {
	case '(': { //  обработка   варианта  '('  выражение  ')'    
		double d = expression();
		t = ts.get();
		if (t.kind != ')') error(L"Не закрыта скобка.");
		return d;
	}
	case number:             //  используем  '8'  для   представления   числа
		return t.value; //  возвращаем   значение   числа
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
	//  предполагается ,  что   мы   можем   выделить   ключевое   слово  "let"
	//  обработка : name =  выражение
	//  объявляется переменная с именем "name" с начальным значением, заданным "выражением"
	Token t = ts.get();
	if (t.kind != name)
		error(L" в   объявлении   ожидается   переменная	name");
	string var_name = t.name;
	Token t2 = ts.get();
	if (t2.kind != '=') {
		error(L" в   объявлении   пропущен   символ  = ");
		cerr << var_name;
	}
	double d = expression();
	define_name(var_name, d);
	error(L"Декларация выполнена успешно.");
	return d;
}

double get_value(string s) {
	// возвращает значение переменной с именем s
	for (unsigned int i = 0; i < var_table.size(); ++i)
		if (var_table[i].name == s)
			return var_table[i].value;
	error(L"get: неопределенная переменная ");
	cerr << s;
	return 0;
}
//  добавляем   пару  (s, d)  в   вектор  var_table
void set_value(string s, double d) {
	for (unsigned int i = 0; i < var_table.size(); ++i)
		if (var_table[i].name == s) {
			var_table[i].value = d;
			return;
		}
	error(L"get: неопределенная переменная");
	cerr << s;
}


//  есть   ли   переменная  var  в   векторе  var_table?
bool is_declared(string var) {
	for (unsigned int i = 0; i < var_table.size(); ++i)
		if (var_table[i].name == var) return true;
	return false;
}
//  добавляем   пару  (var,val)  в   вектор  var_table
double define_name(string var, double val) {
	if (is_declared(var)) {
		cerr << var; error(L"Повторное декларирование константы.");
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