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
	os << "int yylex(char* is) {int state = 0, ptr = 0, ns=0;char* zero=is;while (1) {ns=dfa[state][*is]; if (!*is) ns = state;" << endl
		<< "if (ns < 0 && dfa[state][DOT] >= 0) ns = dfa[state][DOT];" << endl
		<< "if(ns >= 0 && *is) { state = ns; yy_text[ptr++] = *is; yy_text[ptr] = 0; }" << endl
		<< "else {if (state == 0) { if (*is == '\\0') return 0; else{ printf(\"Unexepcted character at %d.\\n\", is-zero); return 1;} }" << endl
		<< "else if (dfa[state][0]<0) { printf(\"Incomplete token at %d.\\n\", is-zero); return 1; }" << endl
		<< "else {const char* t = token(state); if (*t) {" << endl
		<< "yyout(\"<\"); yyout(token(state)); yyout(\">\");" << endl
		<< "if (printAll) yyout(yy_text); yyout(\"\\n\");}" << endl
		<< "state = 0; memset(yy_text, '\\0', YY_SIZE); ptr = 0; is--;}}" << endl
		<< "if (!*is) break; is++;}return 1; }" << endl;
	return os.str();
}

void DFA::minimize() {
	vector<unordered_set<int>*>* ptt = new vector<unordered_set<int>*>;
	unordered_map<int, int> loc = unordered_map<int, int>();
	//Partition by different accepting state
	for (int i = 0; i < states->size(); i++) {
		bool fit = false;
		for (int j = 0; j < ptt->size();j++) {
			if ((*states)[*((*ptt)[j]->begin())]->code.compare((*states)[i]->code) == 0) {
				fit = true;
				(*ptt)[j]->insert(i);
				loc[i] = j;
				break;
			}
		}
		if (!fit) {
			unordered_set<int>* s = new unordered_set<int>;
			s->insert(i);
			ptt->push_back(s);
			loc[i] = ptt->size() - 1;
		}
	}
	bool min = false;
	while (!min) {
		min = true;
		int i = 0;
		while (1) {
			if (i >= ptt->size()) break;
			unordered_set<int>* s = (*ptt)[i];
			vector<unordered_set<int>*>* sub = new vector<unordered_set<int>*>;
			for (unordered_set<int>::iterator it = s->begin(); it != s->end();it++) {
				bool fit = false;
				for (int j = 0; j < ptt->size(); j++) {
					if ((*states)[*((*ptt)[j]->begin())]->code.compare((*states)[i]->code) == 0) {
						fit = true;
						(*ptt)[j]->insert(i);
						loc[i] = j;
						break;
					}
				}
				if (!fit) {
					unordered_set<int>* s = new unordered_set<int>;
					s->insert(i);
					ptt->push_back(s);
					loc[i] = ptt->size() - 1;
				}
			}
		}
	}
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

