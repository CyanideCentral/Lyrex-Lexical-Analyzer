#include"class.h"

bool isDigit(int ch) {
	return ch >= '0'&& ch <= '9';
}

int readInt(string str, int loc=0) {
	while (loc < str.length() && (str[loc] < '0' || str[loc] >= '9')) loc++;
	if (loc == str.length()) return -1;
	int s = 0;
	for (; str[loc] >= '0'&&str[loc] <= '9'; loc++) {
		s = s * 10 + str[loc] - '0';
	}
	return s;
}

int findPair(string str, int loc) {
	if (str[loc] == '\"') {
		for (loc++; str[loc]; loc++) {
			if (str[loc] == '\"') return loc;
		}
		throw new ParseException("Quote mark not paired correctly.");
	}
	else if (str[loc] == '[') {
		for (loc++; str[loc]; loc++) {
			if (str[loc] == ']') return loc;
		}
		throw new ParseException("Square braces not paired correctly.");
	}
	else if (str[loc] == '{') {
		for (loc++; str[loc]; loc++) {
			if (str[loc] == '}') return loc;
		}
		throw new ParseException("Curly braces not paired correctly.");
	}
	else if (str[loc] == '(') {
		int nest = 1;
		for (loc++; str[loc]; loc++) {
			if (str[loc] == '(') nest++;
			if (str[loc] == ')') nest--;
			if (!nest) return loc;
		}
		throw new ParseException("Square braces not paired correctly.");
	}
	else throw new ParseException("Internal error.");
	//others

}

NFA * REtoNFA::parseCharset(string re) {
	int nega = 0;
	unordered_set<char>* cset = new unordered_set<char>;
	if (re[0] == '^') {
		nega = 1;
		re = re.substr(1);
	}
	for (int i = 1; ; i++) {
		/*size is unsigned int, so size - 1 cannot be -1*/
		if (i + 1 >= re.size()) break;
		if (re[i] == '-' && re[i - 1] < re[i + 1]) {
			if (re[i - 1] < 0 || re[i + 1] < 0) throw new ParseException("Charset misusage.");
			for (char c = re[i - 1]; c <= re[i + 1]; c++) {
				cset->insert(c);
			}
			re = re.substr(0, i - 1).append(re.substr(i + 2));
			i -= 2;
		}
	}
	for (int i = 0; i < re.size(); i++) {
		cset->insert(re[i] > 0 ? re[i] : -re[i]);
	}
	/*[^...] case*/
	if (nega) {
		unordered_set<char>* whole = new unordered_set<char>;
		for (char c = '\b'; c <= '\r'; c++) whole->insert(c);
		for (char c = ' '; c <= '~'; c++) whole->insert(c);
		for (unordered_set<char>::iterator it = cset->begin(); it != cset->end(); it++) {
			whole->erase(*it);
		}
		delete cset;
		cset = whole;
	}
	return gen->paralNFA(cset);
}

/*parse RE like a{M,N}}*/
NFA * REtoNFA::parseRepeated(string re, string lm) {
	int def, ind, comma;
	NFA* head, *tail;
	/*Times determined*/
	if ((comma = lm.find(',')) == string::npos) {
		def = readInt(lm);
		ind = 0;
	}
	/*Times = 0-N*/
	else if (lm[0] == ',') {
		def = 0;
		ind = readInt(lm, 0);
	}
	/*Times = M-N*/
	else {
		def = readInt(lm, 0);
		ind = readInt(lm, comma) - def;
	}
	head = NULL;
	/*convert to aa...a */
	for (int i = 0; i < def; i++) head = gen->concNFA(head, parse(re));

	/*No upper bound*/
	if (ind < 0) tail = gen->recurNFA(parse(re));
	/*(M-N) x (eps|a)*/
	else {
		tail = NULL;
		for (int i = 0; i < ind; i++)tail = gen->concNFA(tail, gen->paralNFA(parse(re), gen->getMinNFA()));
	}

	return gen->concNFA(head, tail);
}

NFA * REtoNFA::parseEntity(string re)
{
	NFA* result;
	if (re[0] == '(') {
		return parse(re.substr(1, (int)re.size() - 2));
	}
	else if (re[0] == '[') {
		return parseCharset(re.substr(1, (int)re.size() - 2));
	}
	else if (re[0] == '{') {
		string var = re.substr(1, (int)re.size() - 2);
		if ((dict->find(var))!=dict->end()) {
			result = (*dict)[var];
			return parse(result->re);
		}
		else throw new ParseException(var.append(" is not found."));
	}
	else if (re[0] == '"') {
		return gen->getMinNFA(re.substr(1, (int)re.size() - 2));
	}
	return gen->getMinNFA(re);
}

/*Deal with escape sequences*/
string REtoNFA::preprocess(string re) {
	for (int i = 0; i + 1 < re.size(); i++) {
		if (re[i] == '\\') {
			char escape = re[i + 1];
			switch (escape) {
			case 'n':
				re[i] = '\n';
				break;
			case 'r':
				re[i] = '\r';
				break;
			case 't':
				re[i] = '\t';
				break;
			case 'f':
				re[i] = '\f';
				break;
			default:
				re[i] = escape;
				break;
			}
			re[i] = -re[i];
			re = re.substr(0, i + 1).append(re.substr(i + 2));
		}
	}
	int p;
	//dot should match all single character except new line
	string dot = "[^n]";
	dot[2] = -'\n';
	while ((p = re.find_first_of('.')) != string::npos) {
		re.replace(p, 1, dot);
	}
	return re;
}

NFA * REtoNFA::parse(string re) {
	if (re.size() == 0) return gen->getMinNFA();
	if (strchr("+*?", re[0])) throw new ParseException("Wrong usage of +*?");
	/*Position of the last char of the beginning entity*/
	int end = 0, nested = 0;
	if (strchr("({[\"", re[0])) {
		end = findPair(re, 0);
		nested = 1;
	}
	else {
		for (int i = 0; i < re.size(); i++) {
			char c = re[i];
			if (c == '\\') i++;
			//bad
			else if (strchr("([{+*?|\"", c)) {
				end = i - 1;
				break;
			}
		}
	}
	NFA* head = NULL;
	char unary = re[end + 1];
	int repeat = 0, right = 0;
	if (unary == '{') {
		right = findPair(re, end + 1);
		if (isDigit(re[end+2])||re[end+2]==',') repeat = 1;
	}
	/*unary cannot be '\0'*/
	if (repeat || (unary && strchr("+*?", unary))) {
		if (nested) {
			switch (unary) {
			case '*':
				head = gen->recurNFA(parseEntity(re.substr(0, end + 1)));
				break;
			case '+':
				head = gen->concNFA(parseEntity(re.substr(0, end + 1)), gen->recurNFA(parseEntity(re.substr(0, end + 1))));
				break;
			case '?':
				head = gen->paralNFA(gen->getMinNFA(), parse(re.substr(0, end + 1)));
				break;
			case '{':
				head = parseRepeated(re.substr(0, end + 1), re.substr(end+2, right-end-2));
			}
		}
		else {
			NFA* simple;
			if (!end) simple = NULL;
			else simple = gen->getMinNFA(re, end);
			NFA* opr = gen->getMinNFA(re.substr(end, 1));
			switch (unary) {
			case '*':
				head = gen->concNFA(simple, gen->recurNFA(opr));
				break;
			case '+':
				/*using both opr and opr* causes error*/
				head = gen->concNFA(simple, gen->concNFA(gen->getMinNFA(re.substr(end, 1)), gen->recurNFA(opr)));
				break;
			case '?':
				head = gen->concNFA(simple, gen->paralNFA(gen->getMinNFA(), opr));
				break;
			case '{':
				head = gen->concNFA(simple, parseRepeated(re.substr(end, 1), re.substr(end + 2, right - end - 2)));
			}
		}
		//{M,N} is not considered
		if (right) end = right;
		else end++;
	}
	/*{M, N} case*/
	else {
		head = parseEntity(re.substr(0, end + 1));
	}
	if (end + 1 == re.size()) return head;
	else if (re[end + 1] == '|') {
		if (end + 2 == re.size()) throw new ParseException("Wrong usage of bar");
		return gen->paralNFA(head, parse(re.substr(end + 2)));
	}
	else {
		return gen->concNFA(head, parse(re.substr(end + 1)));
	}
}

NFA * REtoNFA::parse(string re, string id)
{
	re = preprocess(re);
	NFA* atm = gen->addTerminals(parse(re));
	dict->insert(pair<string, NFA*>(id, atm));
	atm->re = re;
	return atm;
}

void REtoNFA::parseToken(string re, string code) {
	re = preprocess(re);
	NFA* atm = gen->addTerminals(parse(re));
	atm->re = re;
	tokenMap->insert(pair<int, string>(atm->end, code));
	lastState = atm->end;
	accStates->insert(atm->end);
	if (!whole) {
		NFA* init = gen->getMinNFA();
		NFA* terminal = gen->getMinNFA();
		whole = gen->concNFA(init, atm);
		whole = gen->concNFA(whole, terminal);
	}
	else {
		whole = gen->paralNFA(whole, atm);
	}
}

void REtoNFA::appendCode(string code) {
	(*tokenMap)[lastState] += code;
}

unordered_set<int>* REtoNFA::epsClosure(int stateId) {
	unordered_set<int>* set = new unordered_set<int>;
	set->insert(stateId);
	State* base = gen->getState(stateId);
	if (base->epsilon->empty()) return set;
	int size = set->size();
	while (1) {
		for (unordered_set<int>::iterator it = set->begin(); it != set->end(); it++) {
			State* branch = gen->getState(*it);
			set->insert(branch->epsilon->begin(), branch->epsilon->end());
		}
		if (size == set->size()) break;
		size = set->size();
	}
	return set;
}

unordered_set<int>* REtoNFA::epsClosure(unordered_set<int>* states) {
	unordered_set<int>* set = new unordered_set<int>;
	for (unordered_set<int>::iterator it = states->begin(); it != states->end(); it++) {
		unordered_set<int>* epsc = epsClosure(*it);
		set->insert(epsc->begin(), epsc->end());
	}
	return set;
}

unordered_set<int>* REtoNFA::move(unordered_set<int>* from, int via) {
	unordered_set<int>* set = new unordered_set<int>;
	for (unordered_set<int>::iterator it = from->begin(); it != from->end(); it++) {
		State* element = gen->getState(*it);
		if (element->edges->find(via) != element->edges->end()) set->insert(element->edges->at(via));
	}
	return set;
}

void REtoNFA::reset() {
	dict->clear();
	whole = NULL;
	tokenMap->clear();
	accStates->clear();
	gen->reset();
}

REtoNFA::REtoNFA() {
	gen = new NFA_Gen();
	whole = NULL;
	dict = new unordered_map<string, NFA*>;
	dfa = new DFA();
	accStates = new unordered_set<int>;
	tokenMap = new unordered_map<int, string>;
}

REtoNFA::~REtoNFA() {
	delete dict;
	delete gen;
	/*Do not delete dfa or nfa*/
	delete accStates;
	delete tokenMap;
}
