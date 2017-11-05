#include"class.h"
#define LINESIZE 1024
using namespace std;

void getInput() {
	string fname;
	ifstream* lexf;
	while (1) {
		cout << "Enter the name of lex file:\n";
		cin >> fname;
		lexf = new ifstream(fname, ios::in);
		if (*lexf) break;
	}
	ifstream tpl("template.txt");
	stringstream ssbuffer;
	ssbuffer << tpl.rdbuf();
	string code(ssbuffer.str());
	char buf[LINESIZE];
	while (lexf->getline(buf, LINESIZE)) {

	}
}

void test() {
	REtoNFA* rtn = new REtoNFA();
	//rtn->parseToken("p{5,}ow\\ne{2,4}r.a+\\t((c|b)*[^ab89A-Z.a-z\\)\\\\]{,9})?", "test");
	rtn->parseToken("abc{3,}", "INT");
	rtn->toDFA()->minimize()->print();

	/*"pow\\nera+\\t((c|b)*[^ab89A-Z.a-z\\)\\\\]+)?"*/
	//atm->print();
}

void fun() {
	try {
		throw ParseException("shit");
	}
	catch (...) {
		cout << "fuck";
	}
}

int main() {
	//test();
	//fun();
	console();
	getchar();
	return 0;
}
