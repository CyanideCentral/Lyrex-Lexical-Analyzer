#pragma once
#include<string>
#include<iostream>
#include<fstream>
#include<sstream>
#include<list>
#include<vector>
#include<unordered_set>
#include<unordered_map>

#define START_STATE -1
#define NORMAL_STATE 0
#define ACCEPTING_STATE 1

using namespace std;

int console();

bool isDigit(int ch);
string clean(string text);
int readInt(string str, int loc);
bool isEqual(unordered_set<int>* a, unordered_set<int>* b);

class State {
private:
public:
	int id;
	unordered_set<int>* epsilon;
	/*key: character; value: target state id*/
	unordered_map<int, int>* edges;
	int pos;
	State(int assignedId, int position = NORMAL_STATE);
	~State();
	
};

class NFA {
private:
public:
	int start;
	int end;
	string re;
	list<State*>* states;
	NFA();
	~NFA();
	void print();
};

class NFA_Gen {
private:
	int cur_num = 1;
	unordered_map<int, State*>* statemap;
public:
	NFA_Gen();
	~NFA_Gen();
	State* newState(int position = NORMAL_STATE);
	NFA* getMinNFA(string str, int len);
	NFA* getMinNFA(string str);
	NFA* getMinNFA();
	NFA* concNFA(NFA* from, NFA* to);
	NFA* paralNFA(NFA* anfa, NFA* bnfa);
	NFA* paralNFA(list<NFA*>* nfalist);
	NFA* paralNFA(unordered_set<char>* charset);
	NFA* recurNFA(NFA* item);
	State* getState(int stateId);
	NFA* addTerminals(NFA* partial);
	void reset();
};

class ParseException :public exception {
private:
	string info;
public:
	virtual const char* what() {
		//return "Failed parsing RE to NFA. Please check your lex file.";
		return info.c_str();
	}
	ParseException(string prompt) {
		info = prompt;
	}
};

/*Find the position of the matching brace of the given character*/
int findPair(string str, int loc);

class DState {
private:
public:
	int id;
	unordered_set<int>* nstates;
	unordered_map<int, int>* edges;
	string code;
	DState();
	~DState();
};

class DFA {
private:
public:
	vector<DState*>* states;
	
	DFA();
	~DFA();
	DState* newDState();
	void print();
	string codify();
};

class REtoNFA {
private:
	/*mapping of identifier to corresponding NFA*/
	unordered_map<string, NFA*>* dict;
	/*mapping of state id to token function code*/
	unordered_set<int>* accStates;
	unordered_map<int, string>* tokenMap;
	NFA_Gen* gen;
	NFA* whole;
	DFA* dfa;
	NFA* parseCharset(string re);
	NFA* parseRepeated(string re, string lm);
	NFA* parseEntity(string re);
	string preprocess(string re);
	int lastState;

	unordered_set<int>* epsClosure(int stateId);
	unordered_set<int>* REtoNFA::epsClosure(unordered_set<int>* states);
	unordered_set<int>* move(unordered_set<int>* from, int via);
public:
	REtoNFA();
	~REtoNFA();
	NFA* parse(string re);
	NFA* parse(string re, string id);
	void parseToken(string re, string code);
	void appendCode(string code);
	void reset();
	DFA* toDFA();
};

