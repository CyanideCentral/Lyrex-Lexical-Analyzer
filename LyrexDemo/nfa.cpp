#include "class.h"

State::State(int assignedId, int position) {
	id = assignedId;
	pos = position;
	epsilon = new unordered_set<int>;
	edges = new unordered_map<int, int>;
}

State::~State() {
	delete epsilon;
	delete edges;
}

bool isEqual(unordered_set<int>* a, unordered_set<int>* b) {
	if (!a&&!b) return true;
	if (!a || !b) return false;
	if (a->size() != b->size()) return false;
	for (unordered_set<int>::iterator it = a->begin(); it != a->end(); it++) {
		if (b->find(*it) == b->end()) return false;
	}
	return true;
}

NFA::NFA() {
	states = new list<State*>;
}

NFA::~NFA() {
	delete states;
}

void NFA::print() {
	printf("NFA\n");
	for (list<State*>::iterator it = states->begin(); it != states->end(); it++) {
		printf("State %d\t epsilon: ", (*it)->id);
		for (unordered_set<int>::iterator iter = (*it)->epsilon->begin(); iter != (*it)->epsilon->end(); iter++) {
			printf("%d ", *iter);
		}
		if ((*it)->epsilon->size() == 0) printf("/");
		for (unordered_map<int, int>::iterator iter = (*it)->edges->begin(); iter != (*it)->edges->end(); iter++) {
			printf("\t%c: %d ", (*iter).first, (*iter).second);
		}
		printf("\n");
	}
}

State * NFA_Gen::newState(int position) {
	State* s = new State(cur_num, position);
	statemap->insert(pair<int, State*>(cur_num, s));
	cur_num++;
	return s;
}

State * NFA_Gen::getState(int stateId)
{
	return (*statemap)[stateId];
}

NFA * NFA_Gen::getMinNFA(string str, int len)
{
	NFA* atm = new NFA();
	State* first = newState();
	atm->states->push_back(first);
	atm->start = first->id;
	for (int i = 0; i < len&&str[i]; i++) {
		State* next = newState();
		atm->states->push_front(next);
		first->edges->insert(pair<int, int>(str[i]>0 ? str[i] : -str[i], next->id));
		first = next;
	}
	atm->end = first->id;
	return atm;
}

NFA * NFA_Gen::getMinNFA(string str)
{
	return getMinNFA(str, str.size());
}

NFA * NFA_Gen::getMinNFA()
{
	return getMinNFA("", 0);
}

NFA * NFA_Gen::concNFA(NFA * from, NFA * to) {
	if (!from&&!to) return getMinNFA();
	if (!from) return to;
	if (!to) return from;
	getState(from->end)->epsilon->insert(to->start);
	from->states->insert(from->states->begin(), to->states->begin(), to->states->end());
	from->end = to->end;
	delete to;
	return from;
}

NFA * NFA_Gen::paralNFA(NFA * anfa, NFA * bnfa)
{
	list<NFA*>* nfal = new list<NFA*>{ anfa, bnfa };
	return paralNFA(nfal);
}

NFA * NFA_Gen::paralNFA(list<NFA*>* nfalist)
{
	list<NFA*>::iterator it = nfalist->begin();
	NFA* first = *it;
	while (++it != nfalist->end()) {
		getState(first->start)->epsilon->insert((*it)->start);
		getState((*it)->end)->epsilon->insert(first->end);
		first->states->insert(first->states->end(), (*it)->states->begin(), (*it)->states->end());
		delete *it;
	}
	return first;
}

NFA * NFA_Gen::paralNFA(unordered_set<char>* charset)
{
	NFA* atm = getMinNFA();
	State* target = newState();
	atm->end = target->id;
	atm->states->push_front(target);
	unordered_set<char>::iterator it = charset->begin();
	while (it!=charset->end()) {
		getState(atm->start)->edges->insert(pair<char, int>(*it++, target->id));
	}
	return atm;
}

NFA * NFA_Gen::recurNFA(NFA * item)
{
	State* first = newState(NORMAL_STATE);
	first->epsilon->insert(item->start);
	getState(item->end)->epsilon->insert(first->id);
	item->start = item->end = first->id;
	item->states->push_front(first);
	return item;
}

NFA * NFA_Gen::addTerminals(NFA * partial)
{
	getState(partial->start)->pos = START_STATE;
	if (partial->end != partial->start) getState(partial->end)->pos = ACCEPTING_STATE;
	else {
		State* term = newState(ACCEPTING_STATE);
		getState(partial->start)->epsilon->insert(term->id);
		partial->states->push_front(term);
		partial->end = term->id;
	}
	return partial;
}

void NFA_Gen::reset() {
	statemap->clear();
	cur_num = 1;
}

NFA_Gen::NFA_Gen() {
	statemap = new unordered_map<int, State*>;
}

NFA_Gen::~NFA_Gen() {
	delete statemap;
}
