#include "class.h"
using namespace std;

string clean(string text) {
	int i;
	for (i = 0; i < text.size() && (text[i] == ' ' || text[i] == '\t'); i++);
	text.erase(0, i);
	for (i = text.size() - 1; i >= 0 && (text[i] == ' ' || text[i] == '\t'); i--);
	text.erase(i+1);
	/*while (1) {
		int p0 = text.find("/*");
		if (p0 == string::npos) break;
		int p1 = text.find("(comment end mark)", p0 + 2);
		if (p0 == string::npos) {
			cout << "Comment across different lines is not allowed." << endl;
			text.erase(p0);
			return text;
		}
		text.erase(p0, p1 + 2 - p0);
	}*/
	return text;
}

int console() {
	ifstream ifs;
	ofstream ofs;
	string fname;
	int ln = 0;
	while (1) {
		cout << "Please enter the path of lex file:" << endl;
		getline(cin, fname);
		ifs.open(fname, ifstream::in);
		if (ifs.is_open()) break;
		cout << "File not found." << endl;
	}
	while (1) {
		cout << "Path of generated source code file:" << endl;
		getline(cin, fname);
		ofs.open(fname, ofstream::out | ofstream::trunc);
		if (ifs.is_open()) break;
		cout << "Invalid path." << endl;
	}
	REtoNFA* r2n = new REtoNFA();

	string line;
	//First part of code
	ofs << "#include<stdio.h>" << endl
		<< "#include<string.h>" << endl
		<< "#include<stdlib.h>" << endl
		<< "#define BUF_SIZE 1024" << endl
		<< "#define YY_SIZE 1024" << endl
		<< "#define DOT 16" << endl
		<< "char yy_text[YY_SIZE];" << endl
		<< "int yywrap();" << endl
		<< "int outmode = 0;int printAll=0;FILE* ofp;" << endl
		<< "void yyout(const char* str) {" << endl
		<< "if (outmode) printf(\"%s\", str);else fprintf(ofp, \"%s\", str);}" << endl
		<< "void printToken(const char* token) {" << endl
		<< "if (outmode) printf(\"<%s>\\n\", token);else fprintf(ofp, \"<%s>\\n\", token);}" << endl;
	//Definition block
	while (1) {
		if (ifs.eof()) {
			cout << "Lex file incomplete." << endl;
			return 1;
		}
		getline(ifs, line);
		ln++;
		line = clean(line);
		int p = line.find("%{");
		if (p != string::npos) {
			line.erase(0, p + 2);
			break;
		}
		if (line.length() < 3) continue;
		p = line.find("\t");
		if (p == string::npos) {
			cout << "Tab delimiter missing at line " << ln << endl;
			return 1;
		}
		int q;
		for (q = p; line[q] == '\t'; q++);
		try {
			r2n->parse(line.substr(q), line.substr(0, p));
		}
		catch (ParseException e) {
			cout << e.what() << "At line " << ln << endl;
			return 1;
		}
	}
	//ostringstream program;
	ofs << line << endl;
	//C code block
	while (1) {
		if (ifs.eof()) {
			cout << "Lex file incomplete." << endl;
			return 1;
		}
		getline(ifs, line);
		ln++;
		line = clean(line);
		if (!line.compare("%}")) break;
		ofs << line << endl;
	}
	while (1) {
		if (ifs.eof()) {
			cout << "Lex file incomplete." << endl;
			return 1;
		}
		getline(ifs, line);
		ln++;
		line = clean(line);
		if (!line.compare("%%")) break;
	}
	//RE block
	string re;
	while (1) {
		getline(ifs, line);
		ln++;
		line = clean(line);
		if (!line.compare("%%")) break;
		if (line.length() < 3) continue;
		int p = line.find("\t");
		if (p == 0) {
			r2n->appendCode(line);
			continue;
		}
		else if (p == string::npos) {
			cout << "Tab delimiter missing at line " << ln << endl;
			return 1;
		}
		int q;
		for (q = p; line[q] == '\t'; q++);
		re = line.substr(0, p);
		try {
			r2n->parseToken(re, line.substr(q));
		}
		catch (ParseException e) {
			cout << e.what() << "At line " << ln << endl;
			return 1;
		}
	}
	DFA* dfa = r2n->toDFA()->minimize();
	//Methods block
	dfa->print();
	ofs << dfa->codify() << endl;
	while (!ifs.eof()) {
		getline(ifs, line);
		ln++;
		ofs << line << endl;
	}
	//main function
	ofs << "int main() {char buf[BUF_SIZE];FILE* fp;while (1) {" << endl
		<< "printf(\"Please enter the path of file to be analyzed:\\n\");" << endl
		<< "gets_s(buf, BUF_SIZE); errno_t err = fopen_s(&fp, buf, \"rb\");" << endl
		<< "if (!err) break;else printf(\"Fail to open file.\\n\");}" << endl
		<< "printf(\"Print matched lexeme as well? (y/N)\\n\");" << endl
		<< "gets_s(buf, BUF_SIZE); if(buf[0]=='y'||buf[0]=='Y') printAll=1;" << endl
		<< "while (1) {printf(\"Print tokens in console: enter -c\\n\");" << endl
		<< "printf(\"Save tokens in file: enter file path\\n\");" << endl
		<< "gets_s(buf, BUF_SIZE);if (buf[0] == '-'&&buf[1] == 'c'&&buf[2] == '\\0') outmode = 1;" << endl
		<< "errno_t err = fopen_s(&ofp, buf, \"w\");if (err) printf(\"Invalid path.\\n\");else break;}" << endl
		<< "fseek(fp, 0, SEEK_END);int size = ftell(fp);" << endl
		<< "char* input_buf = (char*)malloc(size+1);fseek(fp, 0, SEEK_SET);" << endl
		<< "fread_s(input_buf, size, sizeof(char), size, fp); input_buf[size]=0;" << endl
		<< "yylex(input_buf);yywrap();printf(\"\\nAnalysis finished.\\n\");getchar();return 0;}" << endl;
	return 0;
}


int outmode = 0;
FILE* ofp;
void yyout(const char* token) {
	if (outmode) printf("<%s>\n", token);
	else fprintf(ofp, "<%s>\n", token);
}
int fmain() {
	char buf[1024];
	FILE* fp;
	while (1) {
		printf("Please enter the path of lex file:\n");
		gets_s(buf, 1024);
		errno_t err = fopen_s(&fp, buf, "r");
		if (!err) break;
		else printf("Fail to open file.\n");
	}
	while (1) {
		printf("Print tokens in console: enter -c\n");
		printf("Save tokens in file: enter file path\n");
		gets_s(buf, 1024);
		if (buf[0] == '-'&&buf[1] == 'c'&&buf[2] == '\0') outmode = 1;
		errno_t err = fopen_s(&ofp, buf, "w");
		if (err) printf("Invalid path.\n");
		else break;
	}
	//Get file size in bytes
	fseek(fp, 0, SEEK_END);
	int size = ftell(fp);
	char* input_buf = (char*)malloc(size);
	fseek(fp, 0, SEEK_SET);
	fread_s(input_buf, size, sizeof(char), size, fp);
	//yylex(input_buf);
	return 0;
}