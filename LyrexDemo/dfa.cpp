#include"class.h"

DState::DState() {
	nstates = new unordered_set<int>;
	edges = new unordered_map<int, int>;
}

DState::~DState() {
	delete nstates;
	delete edges;
}

DState * DFA::newDState() {
	DState* ds = new DState;
	states->push_back(ds);
	ds->id = states->size() - 1;
	return ds;
}

void DFA::print() {
	//cout << tokenMap->begin()->first << endl;
	for (int i = 0; i < states->size(); i++) {
		DState* cur = (*states)[i];
		printf("State %d (", cur->id);
		for (unordered_set<int>::iterator it = cur->nstates->begin(); it != cur->nstates->end(); it++) {
			cout << *it << " ";
		}
		printf(") edges:");
		for (unordered_map<int, int>::iterator it = cur->edges->begin(); it != cur->edges->end(); it++) {
			printf("%c: %d; ", it->first, it->second);
		}
		cout << "code: " << cur->code;
		cout << endl;
	}
}

string DFA::codify() {
	ostringstream os;
	//Code called for each token
	os << "const char* token(int id){switch(id){" << endl;
	for (int i = 0; i < states->size(); i++) {
		DState* ds = (*states)[i];
		if (ds->code.length()) os << "case " << ds->id << ": " << ds->code << "break;" << endl;
	}
	os << "} return "";}";
	//DFA array
	os << "int dfa[][128]={";
	for (int i = 0; i < states->size(); i++) {
		os << "{";
		DState* ds = (*states)[i];
		os << (ds->code.length() ? 1 : -1) << ", ";
		for (int ch = 1; ch < 128; ch++) {
			os << (ds->edges->find(ch) == ds->edges->end() ? -1 : ds->edges->at(ch));
			os << (ch == 127 ? "}" : ", ");
		}
		os << (i + 1 == states->size() ? "};" : ", ") << endl;
	}
	//yylex function
	os << "int yylex(char* is) {int state = 0, ptr = 0, ns=0;while (*is) {ns=dfa[state][*is];" << endl
		<< "if(ns >= 0) { state = ns; yy_text[ptr++] = *is; yy_text[ptr] = 0; }" << endl
		<< "else {if (state == 0) { if (*is == '\\0') return 0; else return 1; }" << endl
		<< "else if (dfa[state][0]<0) { return 1; }" << endl
		<< "else {const char* t = token(state); if (*t) {" << endl
		<< "yyout(\"<\"); yyout(token(state)); yyout(\">\");" << endl
		<< "if (printAll) yyout(yy_text); yyout(\"\\n\");}" << endl
		<< "state = 0; memset(yy_text, '\\0', YY_SIZE); ptr = 0; is--;}}" << endl
		<< "if (!*is) break; is++;}return 1; }" << endl;
	return os.str();
}

DFA::DFA() {
	states = new vector<DState*>;
}

DFA::~DFA() {
	delete states;
}

DFA * REtoNFA::toDFA() {
	DState* head = dfa->newDState();
	head->nstates = epsClosure(whole->start);
	int index = 0;
	unordered_set<int>* chars = new unordered_set<int>;
	while (1) {
		DState* cur = (*dfa->states)[index];
		for (unordered_set<int>::iterator it = cur->nstates->begin(); it != cur->nstates->end(); it++) {
			for (unordered_map<int, int>::iterator itp = gen->getState(*it)->edges->begin(); itp != gen->getState(*it)->edges->end(); itp++) {
				chars->insert(itp->first);
			}
		}
		for (unordered_map<int, int>::iterator it = cur->edges->begin(); it != cur->edges->end(); it++) {
			chars->insert(it->first);
		}
		for (unordered_set<int>::iterator it = chars->begin(); it != chars->end(); it++) {
			unordered_set<int>* target = epsClosure(move(cur->nstates, *it));
			if (target->empty()) continue;
			bool isNew = true;
			for (int i = 0; i < dfa->states->size(); i++) {
				if (isEqual((*dfa->states)[i]->nstates, target)) {
					isNew = false;
					cur->edges->insert(pair<int, int>(*it, i));
					break;
				}
			}
			if (isNew) {
				DState* created = dfa->newDState();
				created->nstates->insert(target->begin(), target->end());
				cur->edges->insert(pair<int, int>(*it, created->id));
				for (unordered_set<int>::iterator it = target->begin(); it != target->end(); it++) {
					if (tokenMap->find(*it) != tokenMap->end()) {
						created->code = tokenMap->at(*it);
						break;
					}
					else created->code = "";
				}
			}
			delete target;
		}
		index++;
		if (index >= dfa->states->size()) break;
	}
	return dfa;
}

