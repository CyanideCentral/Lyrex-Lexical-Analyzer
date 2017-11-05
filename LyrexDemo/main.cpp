#include"class.h"

void test() {
	REtoNFA* rtn = new REtoNFA();
	//rtn->parseToken("p{5,}ow\\ne{2,4}r.a+\\t((c|b)*[^ab89A-Z.a-z\\)\\\\]{,9})?", "test");
	rtn->parseToken("abc{3,}", "INT");
	rtn->toDFA()->minimize()->print();
}

int main() {
	//test();
	console();
	getchar();
	return 0;
}
