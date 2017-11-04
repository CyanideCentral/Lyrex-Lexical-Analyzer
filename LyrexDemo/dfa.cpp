#include"class.h"

DState::DState() {
	nstates = new unordered_set<int>;
	edges = new unordered_map<int, int>;
}

DState::~DState() {
	delete nstates;
	delete edges;
}

void DFA::generateCode(string path) {
}

DState * DFA::newDState() {
	DState* ds = new DState;
	states->push_back(ds);
	ds->id = states->size() - 1;
	return ds;
}

void DFA::print() {
	cout << tokenMap->begin()->first << endl;
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
	string sw = "";
}

DFA::DFA() {
	states = new vector<DState*>;
	tokenMap = new unordered_map<int, string>;
}

DFA::~DFA() {
	delete states;
	delete tokenMap;
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
					if (dfa->tokenMap->find(*it) != dfa->tokenMap->end()) {
						created->code = dfa->tokenMap->at(*it);
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

