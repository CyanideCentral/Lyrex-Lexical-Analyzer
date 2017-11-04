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
	rtn->parseToken(".", "INT");
	rtn->toDFA()->print();

	/*"pow\\nera+\\t((c|b)*[^ab89A-Z.a-z\\)\\\\]+)?"*/
	//atm->print();
}

const char* funfun() {
	const char* str = "hello";
	return "hello";
}
void fun(const char* str) {
	printf("%s", str);
}

int main() {
	//test();
	//fun(funfun());
	console();
	getchar();
	return 0;
}
