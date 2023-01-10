#include <iostream>
#include <string>
#include <fstream>
#include <queue>
#define RED "\033[31m" 
#define RESET "\033[0m"

using namespace std;

struct rootInfo {
	int priority;
	string actionName;
	bool black;
	rootInfo(int priority, string actionName, bool black=false) : priority(priority), actionName(actionName), black(black) {}
};

bool equalKeys(rootInfo* key1, rootInfo* key2) {
	if (!key1 or !key2) return false;
	if (key1->priority == key2->priority and key1->actionName == key2->actionName) return true;
	return false;
}

bool compare(rootInfo* key1, rootInfo* key2) {
	if (!key1) return false;
	if (key2->priority < key1->priority) return true;
	else if (key2->priority == key1->priority) {
		if (key2->actionName > key1->actionName) return true;
		else return false;
	}
	else return false;
}

class Node;

void printNode(ostream& os,Node* node);

int findHeight(Node* root);

void formattedPrint(ostream& os, Node* root);

class Node{
public:
	rootInfo* keys[3];
	Node* pointers[4], * parent;
	Node() {
		for (int i=0; i < 3; i++) keys[i] = nullptr;
		for (int i=0; i < 4; i++) pointers[i] = nullptr;
		parent = nullptr;
	}
	bool isFull() {
		for (int i = 0; i < 3; i++) {
			if (keys[i] == nullptr) return false;
		}
		return true;
	}
	bool isLeaf() {
		for (int i = 0; i < 4; i++) if (pointers[i] != nullptr) return false;
		return true;
	}
	bool has1Key() {
		return (keys[1] and !keys[0] and !keys[2]);
	}
	bool has2Keys() {
		return(keys[1] and ((keys[2] and !keys[0]) or (!keys[2] and keys[0])));
	}
	bool isEmpty() {
		if (!keys[0] and !keys[1] and !keys[2]) return true;
		return false;
	}
	friend ostream& operator<<(ostream& os, Node* root) {
		int i = 0, cnt = 1, lastcnt = 0, cnt2 = 0;;
		queue <Node*> q;
		q.push(root);
		cout << "Level-order: " << endl;
		while (!q.empty()) {
			os << "Level " << i << ": "; i++;
			cnt2 = 0; lastcnt = 0;
			while (cnt) {
				Node* n = q.front(); q.pop(); cnt--;
				if (n) printNode(os,n);
				else os << "null";
				if(n) 
				for (int i = 0; i < 4; i++) {
					if (n->pointers[i]) lastcnt++;
					q.push(n->pointers[i]);
					cnt2++;
				}
				if (cnt) os << ", ";

			}
			os << endl;
			if (!lastcnt) break;
			cnt = cnt2;
		}
		cout << "Formatirano: " << endl;
		formattedPrint(os, root);
		return os;
	}
};

rootInfo* maxInSubtree(Node *p) {
	while (!p->isLeaf()) {
		if (p->pointers[3]) p = p->pointers[3];
		else p = p->pointers[2];
	}
	if (p->keys[2]) return p->keys[2];
	else return p->keys[1];
}

rootInfo* minInSubtree(Node *p) {
	while (!p->isLeaf()) {
		if (p->pointers[0]) p = p->pointers[0];
		else p = p->pointers[1];
	}
	if (p->keys[0]) return p->keys[0];
	else return p->keys[1];
}

void printNode(ostream& os, Node* node) {
	if (node) {
		os << "[";
		for (int i = 0; i < 3; i++) {
			if (node->keys[i] == nullptr) os << "x";
			else {
				if (!node->keys[i]->black) os << RED << node->keys[i]->priority << node->keys[i]->actionName[0] << RESET;
				else os << node->keys[i]->priority << node->keys[i]->actionName[0];
			}
			if(i!=2) os << "|";
		}
		os << "]";
	}
}

int findHeight(Node* root) {
	queue <Node*> q; q.push(root); q.push(nullptr);
	int height = 0;
	while (!q.empty()) {
		Node* node = q.front(); q.pop();
		if (!node) {
			height++;
			if (q.empty()) break;
			q.push(nullptr);
		}
		else for (int i = 0; i < 4; i++) if (node->pointers[i]) q.push(node->pointers[i]);
	}
	return height;
}

void formattedPrint(ostream& os, Node* root) {
	queue <Node*> q; q.push(root);
	int height = findHeight(root), razmak = 3 * 50* pow(height, 2), razmak2, i = 0;
	while (!q.empty()) {
		razmak2 = (razmak) / 4;
		razmak = (razmak) / 8;
		int rootSpace = (razmak/8 + razmak /8 + razmak/4 + 11 );
		for (int k = 0; k < ((i) ? razmak: rootSpace); k++) os << " ";
		for (int j = 0; j < pow(4, i); j++) {
			Node* temp = q.front(); q.pop();
			if (temp) {
				printNode(os, temp);
				for (Node* p : temp->pointers) q.push(p);
			}
			else {
				os << "NULLPTR"; q.push(nullptr); q.push(nullptr); q.push(nullptr); q.push(nullptr);
			}
			for (int s = 0; s < razmak2; s++) os << " ";
		}
		for (int t = 0; t < (height - i); t++) os << endl;
		if (i == height - 1) break;
		i++;
	}
}

void insert(Node*& root,rootInfo* key, bool propagation=false, Node *parent=nullptr, Node *left=nullptr,Node *right=nullptr) {
	if (!propagation) {
		if (!root) {
			root = new Node(); key->black = true;
			root->keys[1] = key;
			return;
		}
		Node* p = root, * q = nullptr;
		while (p) {
			q = p;
			Node* next = nullptr;
			for (int i = 3; i >= 0; i--)
				if (p->pointers[i]) { next = p->pointers[i]; break; }
			for (int i = 0; i < 3; i++)
				if (compare(p->keys[i], key)) { next = p->pointers[i]; break; }
			p = next;
		}
		if (!q->isFull()) {
			if (q->keys[0]) {
				if (compare(q->keys[0], key)) {
					q->keys[2] = q->keys[1]; q->keys[2]->black = false;
					q->keys[1] = q->keys[0]; q->keys[1]->black = true;
					q->keys[0] = key;
				}
				else if (compare(q->keys[1], key)) {
					q->keys[2] = q->keys[1]; q->keys[2]->black = false;
					q->keys[1] = key; key->black = true;
				}
				else q->keys[2] = key;
			}
			else if (q->keys[2]) {
				if (!compare(q->keys[2], key)) {
					q->keys[0] = q->keys[1]; q->keys[0]->black = false;
					q->keys[1] = q->keys[2]; q->keys[2]->black = true;
					q->keys[2] = key;
				}
				else if (!compare(q->keys[1], key)) {
					q->keys[0] = q->keys[1]; q->keys[0]->black = false;
					q->keys[1] = key; key->black = true;
				}
				else q->keys[0] = key;
			}
			else {
				if (compare(q->keys[1], key)) q->keys[0] = key;
				else q->keys[2] = key;
			}

		}
		else {
			rootInfo* prop = q->keys[1]; prop->black = true;
			Node* left = new Node(), * right = new Node();
			left->keys[1] = q->keys[0]; left->keys[1]->black = true;
			right->keys[1] = q->keys[2]; right->keys[1]->black = true;
			if (compare(prop, key)) {
				if (compare(left->keys[1], key)) left->keys[0] = key;
				else left->keys[2] = key;
			}
			else {
				if (compare(right->keys[1], key)) right->keys[0] = key;
				else right->keys[2] = key;
			}
			if (!q->parent) {
				root = new Node;
				root->keys[1] = prop;
				root->pointers[1] = left; root->pointers[2] = right;
				left->parent = root; right->parent = root;
				delete q;
			}
			else {
				Node* par = q->parent;
				if (par->isFull()) insert(root, prop, true, par, left, right);
				else {
					left->parent = par; right->parent = par;
					if (par->keys[0]) {
						if (compare(par->keys[0], prop)) {
							par->keys[2] = par->keys[1]; par->keys[2]->black = false;
							par->keys[1] = par->keys[0]; par->keys[1]->black = true;
							par->keys[0] = prop; prop->black = false;
							par->pointers[3] = par->pointers[2]; par->pointers[2] = par->pointers[1];
							par->pointers[0] = left; par->pointers[1] = right;
						}
						else if (compare(par->keys[1], prop)) {
							par->keys[2] = par->keys[1]; par->keys[2]->black = false;
							par->keys[1] = prop; prop->black = true;
							par->pointers[3] = par->pointers[2];
							par->pointers[1] = left; par->pointers[2] = right;
						}
						else {
							par->keys[2] = prop; prop->black = false;
							par->pointers[2] = left; par->pointers[3] = right;
						}
					}
					else if (par->keys[2]) {
						if (compare(par->keys[1], prop)) {
							par->keys[0] = prop; prop->black = false;
							par->pointers[0] = left; par->pointers[1] = right;
						}
						else if (compare(par->keys[2], prop)) {
							par->keys[0] = par->keys[1]; par->keys[0]->black = false;
							par->keys[1] = prop; prop->black = true;
							par->pointers[0] = par->pointers[1];
							par->pointers[1] = left; par->pointers[2] = right;
						}
						else {
							par->keys[0] = par->keys[1]; par->keys[0]->black = false;
							par->keys[1] = par->keys[2]; par->keys[1]->black = true;
							par->keys[2] = prop; prop->black = false;
							par->pointers[0] = par->pointers[1]; par->pointers[1] = par->pointers[2];
							par->pointers[2] = left; par->pointers[3] = right;
						}
					}
					else {
						if (compare(par->keys[1], prop)) {
							par->keys[0] = prop; prop->black = false;
							par->pointers[0] = left; par->pointers[1] = right;
						}
						else {
							par->keys[2] = prop; prop->black = false;
							par->pointers[2] = left; par->pointers[3] = right;
						}
					}
				}
			}
		}
	}
	else {
		rootInfo* prop = parent->keys[1];
		Node* newLeft = new Node, * newRight = new Node;
		if (compare(parent->keys[0], key)) {
			newLeft->keys[0] = key; key->black = false;
			newLeft->keys[1] = parent->keys[0]; newLeft->keys[1]->black = true;
			newLeft->pointers[0] = left; newLeft->pointers[1] = right; newLeft->pointers[2] = parent->pointers[1]; parent->pointers[1]->parent = newLeft;
			newRight->keys[1] = parent->keys[2]; newRight->keys[1]->black = true;
			newRight->pointers[1] = parent->pointers[2]; newRight->pointers[2] = parent->pointers[3];
			if(parent->pointers[2]) parent->pointers[2]->parent = newRight; if (parent->pointers[3]) parent->pointers[3]->parent = newRight;
			left->parent = newLeft; right->parent = newLeft;
		}
		else if (compare(prop, key)) {
			newLeft->keys[2] = key; key->black = false;
			newLeft->keys[1] = parent->keys[0]; newLeft->keys[1]->black = true;
			newLeft->pointers[2] = left; newLeft->pointers[3] = right; newLeft->pointers[1] = parent->pointers[0]; parent->pointers[0]->parent = newLeft;
			newRight->keys[1] = parent->keys[2]; newRight->keys[1]->black = true;
			newRight->pointers[1] = parent->pointers[2]; newRight->pointers[2] = parent->pointers[3];
			if (parent->pointers[2]) parent->pointers[2]->parent = newRight; if (parent->pointers[3]) parent->pointers[3]->parent = newRight;
			left->parent = newLeft; right->parent = newLeft;
		}
		else if (compare(parent->keys[2], key)) {
			newRight->keys[1] = parent->keys[2]; newRight->keys[1]->black = true;
			newRight->keys[0] = key; key->black = false;
			newRight->pointers[0] = left; newRight->pointers[1] = right; newRight->pointers[2] = parent->pointers[3]; parent->pointers[3]->parent = newRight;
			newLeft->keys[1] = parent->keys[0]; newLeft->keys[1]->black = true;
			newLeft->pointers[1] = parent->pointers[0]; newLeft->pointers[2] = parent->pointers[1];
			if (parent->pointers[0]) parent->pointers[0]->parent = newLeft; if (parent->pointers[1]) parent->pointers[1]->parent = newLeft;
			left->parent = newRight; right->parent = newRight;
		}
		else {
			newRight->keys[1] = parent->keys[2]; newRight->keys[1]->black = true;
			newRight->keys[2] = key; key->black = false;
			newRight->pointers[2] = left; newRight->pointers[3] = right; newRight->pointers[1] = parent->pointers[2]; parent->pointers[2]->parent = newRight;
			newLeft->keys[1] = parent->keys[0]; newLeft->keys[1]->black = true;
			newLeft->pointers[1] = parent->pointers[0]; newLeft->pointers[2] = parent->pointers[1];
			if (parent->pointers[0]) parent->pointers[0]->parent = newLeft; if (parent->pointers[1]) parent->pointers[1]->parent = newLeft;
			left->parent = newRight; right->parent = newRight;
		}
		Node* parent2 = parent->parent;
		if (!parent2) {
			root = new Node;
			root->keys[1] = prop; prop->black = true;
			root->pointers[1] = newLeft; root->pointers[2] = newRight;
			newLeft->parent = root; newRight->parent = root;
		}
		else if (parent2->isFull()) insert(root, prop, true, parent2, newLeft, newRight);
		else {
			newLeft->parent = parent2; newRight->parent = parent2;
			if (parent2->keys[0]) {
				if (compare(parent2->keys[0], prop)) {
					parent2->keys[2] = parent2->keys[1]; parent2->keys[2]->black = false;
					parent2->keys[1] = parent2->keys[0]; parent2->keys[1]->black = true;
					parent2->keys[0] = prop; prop->black = false;
					parent2->pointers[3] = parent2->pointers[2]; parent2->pointers[2] = parent2->pointers[1];
					parent2->pointers[0] = newLeft; parent2->pointers[1] = newRight;
				}
				else if (compare(parent2->keys[1], prop)) {
					parent2->keys[2] = parent2->keys[1]; parent2->keys[2]->black = false;
					parent2->keys[1] = prop; prop->black = true;
					parent2->pointers[3] = parent2->pointers[2];
					parent2->pointers[1] = newLeft; parent2->pointers[2] = newRight;
				}
				else {
					parent2->keys[2] = prop; prop->black = false;
					parent2->pointers[2] = newLeft; parent2->pointers[3] = newRight;
				}
			}
			else if (parent2->keys[2]) {
				if (compare(parent2->keys[1], prop)) {
					parent2->keys[0] = prop; prop->black = false;
					parent2->pointers[0] = newLeft; parent2->pointers[1] = newRight;
				}
				else if (compare(parent2->keys[2], prop)) {
					parent2->keys[0] = parent2->keys[1]; parent2->keys[0]->black = false;
					parent2->keys[1] = prop; prop->black = true;
					parent2->pointers[0] = parent2->pointers[1];
					parent2->pointers[1] = newLeft; parent2->pointers[2] = newRight;
				}
				else {
					parent2->keys[0] = parent2->keys[1]; parent2->keys[0]->black = false;
					parent2->keys[1] = parent2->keys[2]; parent2->keys[1]->black = true;
					parent2->keys[2] = prop; prop->black = false;
					parent2->pointers[0] = parent2->pointers[1]; parent2->pointers[1] = parent2->pointers[2];
					parent2->pointers[2] = newLeft; parent2->pointers[3] = newRight;
				}
			}
			else {
				if (compare(parent2->keys[1], prop)) {
					parent2->keys[0] = prop; prop->black = false;
					parent2->pointers[0] = newLeft; parent2->pointers[1] = newRight;
				}
				else {
					parent2->keys[2] = prop; prop->black = false;
					parent2->pointers[2] = newLeft; parent2->pointers[3] = newRight;
				}
			}
		}
	}
}

rootInfo* searchByPriority(Node* root, int priority) {
	if (!root) return nullptr;
	Node* p = root, * q = nullptr;
	while (p) {
		q = p;
		Node *next = nullptr;
		for (int i = 3; i >= 0; i--)if (p->pointers[i]) { next = p->pointers[i]; break; }
		bool nadjenq = false;
		for (int i = 0; i < 3; i++) {
			if(p->keys[i])
				if (priority <= p->keys[i]->priority) {
					next = p->pointers[i];
					break;
				}
		}
		for (int i = 0; i < 3; i++) {
			if (p->keys[i]) {
				if (p->keys[i]->priority == priority) {
					if (!p->pointers[i]) { nadjenq = true; break; }
					else {
						if (p->pointers[i]->keys[2]) {if(p->pointers[i]->keys[2]->priority != priority) nadjenq = true; break; }
						else if(p->pointers[i]->keys[1] and p->pointers[i]->keys[1]->priority != priority) { nadjenq = true; break; }
					}
					break;
				}
			}
		}

		if (nadjenq) break;
		p = next;
	}
	for (int i = 0; i < 3; i++) {
		if (q->keys[i])
			if (q->keys[i]->priority == priority) return q->keys[i];
	}
	return nullptr;
}

int numOfActions(Node* root, int priority) {
	queue <Node*> q;
	q.push(root);
	int counter = 0;
	while (!q.empty()) {
		Node* node = q.front(); q.pop();
		for (int i = 0; i < 3; i++) if (node->keys[i] and node->keys[i]->priority == priority) counter++;
		for (int i = 0; i < 4; i++) if (node->pointers[i]) q.push(node->pointers[i]);
	}
	return counter;
}

Node* treeFromFile() {
	string file;
	cout << "Unesite ime datoteke(sa .txt ekstenzijom): ";
	cin >> file;
	ifstream dat;
	dat.open(file);
	if (!dat.is_open()) { cout << "Nepostojeca datoteka.\n"; return nullptr; }
	Node* root = nullptr;
	string line;
	do{
		getline(dat, line);
		if (line == "") break;
		int i = 0, lastSpace = 0;
		for (char c : line) {
			if (c == ' ') lastSpace = i;
			i++;
		}
		char help[256];
		for (int t = 0; t < lastSpace; t++) {
			help[t] = line[t];
		}
		help[lastSpace] = '\0';
		string akcija = help;
		char prio[10]; int j = 0;
		prio[0] = '\0';
		for (int t = lastSpace + 1; t < line.size(); t++) {
			prio[j++] = line[t]; prio[j] = '\0';
		}
		int priority = stoi(prio);
		rootInfo* key = new rootInfo(priority, akcija);
		insert(root, key);
	} while (line != "");
	dat.close();
	return root;
}

int findBinaryHeight(Node* root) {
	queue <Node*> q; queue <int> q2;
	int height = 0;
	q.push(root); q.push(nullptr); q2.push(1); q2.push(1);
	while (!q.empty()) {
		Node* node = q.front(); q.pop();
		int index = q2.front(); q2.pop();
		if (!node) {
			height++;
			if (q.empty()) break;
			q.push(nullptr); q2.push(1);
		}
		else {
			if (index == 1) {
				if (node->keys[0]) {
					q.push(node); q2.push(0);
				}
				else {
					if (node->pointers[1]) { q.push(node->pointers[1]); q2.push(1); }
				}
				if (node->keys[2]) {
					q.push(node); q2.push(2);
				}
				else {
					if (node->pointers[2]) { q.push(node->pointers[2]); q2.push(1); }
				}
			}
			else{
				if (node->pointers[index]) { q.push(node->pointers[index]); q2.push(1); }
				if (node->pointers[index + 1]) { q.push(node->pointers[index + 1]); q2.push(1); }
			}
		}
	}
	return height;
}

void printAsBinary(Node * root){
	queue <Node*> q; queue <int> q2;
	q.push(root); q2.push(1);
	int height = findBinaryHeight(root), razmak = 20 * height, razmak2,i=0;
	while (!q.empty()) {
		razmak2 = razmak;
		razmak = (razmak - (height - i)) / 2;
		for (int k = 0; k < razmak; k++) cout << " ";
		for (int j = 0; j < pow(2, i); j++) {
			Node* temp = q.front(); q.pop();
			int index = q2.front(); q2.pop();
			if (temp) {
				if (!(index == 1)) cout << RED;
				cout << temp->keys[index]->priority << temp->keys[index]->actionName[0];
				if (!(index == 1)) cout << RESET;
				if (index == 1) {
					if (temp->keys[0]) { q.push(temp); q2.push(0); }
					else { q.push(temp->pointers[1]); q2.push(1); }
					if (temp->keys[2]) { q.push(temp); q2.push(2); }
					else { q.push(temp->pointers[2]); q2.push(1); }
				}
				else {
					q.push(temp->pointers[index]); q2.push(1);
					q.push(temp->pointers[index + 1]); q2.push(1);
				}
			}
			else {
				cout << "XXX"; q.push(nullptr); q.push(nullptr); q2.push(1); q2.push(1);
			}
			for (int s = 0; s < razmak2; s++) cout << " ";
		}
		for (int t = 0; t < (height - i); t++) cout << endl;
		if (i == height - 1) break;
		i++;
	}
}

void fix(Node*& root, Node* p) {
	while (p->isEmpty()) {
		Node* parent = p->parent;
		Node* brother = nullptr; bool realBrother = true; bool left = true; bool uLevom = true; bool blackParent = false;
		if (parent->pointers[0] == p) {
			brother = parent->pointers[1];
		}
		else if (parent->pointers[1] == p) {
			if (parent->keys[0]) { brother = parent->pointers[0]; left = false; }
			else { brother = parent->pointers[2]; realBrother = false; }
			if (!parent->keys[0] and !parent->keys[2]) blackParent = true;
		}
		else if (parent->pointers[2] == p) {
			if (parent->keys[2]) { brother = parent->pointers[3]; }
			else { brother = parent->pointers[1]; realBrother = false; }
			if (!parent->keys[0] and !parent->keys[2]) blackParent = true;
			uLevom = false; left = false;
		}
		else {
			brother = parent->pointers[2]; left = false;
			uLevom = false;
		}
		Node* preliven = nullptr;
		if (realBrother) { 
			if (brother->isFull()) {
				if (left and uLevom) {
					p->keys[1] = parent->keys[0]; p->keys[1]->black = true;
					p->keys[2] = brother->keys[0];
					parent->keys[0] = brother->keys[1]; parent->keys[0]->black = false;
					brother->keys[1] = brother->keys[2]; brother->keys[1]->black = true;
					brother->keys[2] = nullptr; brother->keys[0] = nullptr;
					brother->pointers[1] = brother->pointers[2]; brother->pointers[2] = brother->pointers[3]; brother->pointers[3] = nullptr;
				}
				else if (!left and uLevom) {
					p->keys[1] = parent->keys[0]; p->keys[1]->black = true;
					p->keys[0] = brother->keys[2];
					parent->keys[0] = brother->keys[1]; parent->keys[0]->black = false;
					brother->keys[1] = brother->keys[0]; brother->keys[1]->black = true;
					brother->keys[2] = nullptr; brother->keys[0] = nullptr;
					brother->pointers[2] = brother->pointers[1]; brother->pointers[1] = brother->pointers[0]; brother->pointers[0] = nullptr;
				}
				else if (left and !uLevom) {
					p->keys[1] = parent->keys[2]; p->keys[1]->black = true;
					p->keys[2] = brother->keys[0];
					parent->keys[2] = brother->keys[1]; parent->keys[2]->black = false;
					brother->keys[1] = brother->keys[2]; brother->keys[1]->black = true;
					brother->keys[2] = nullptr; brother->keys[0] = nullptr;
					brother->pointers[1] = brother->pointers[2]; brother->pointers[2] = brother->pointers[3]; brother->pointers[3] = nullptr;
				}
				else {
					p->keys[0] = brother->keys[2];
					p->keys[1] = parent->keys[2]; p->keys[1]->black = true;
					parent->keys[2] = brother->keys[1]; parent->keys[2]->black = false;
					brother->keys[1] = brother->keys[0]; brother->keys[1]->black = true;
					brother->keys[2] = nullptr; brother->keys[0] = nullptr;
					brother->pointers[2] = brother->pointers[1]; brother->pointers[1] = brother->pointers[0]; brother->pointers[0] = nullptr;
				}
			}
			else if (brother->has2Keys()) {
				if (left and uLevom) {
					p->keys[1] = parent->keys[0]; p->keys[1]->black = true;
					if (brother->keys[0]) {
						parent->keys[0] = brother->keys[0];
						brother->keys[0] = nullptr;
					}
					else {
						parent->keys[0] = brother->keys[1]; parent->keys[0]->black = false;
						brother->keys[1] = brother->keys[2]; brother->keys[1]->black = true;
						brother->keys[2] = nullptr;
						brother->pointers[0] = brother->pointers[1]; brother->pointers[1] = brother->pointers[2]; brother->pointers[2] = nullptr;
					}
				}
				else if (!left and uLevom) {
					p->keys[1] = parent->keys[0]; p->keys[1]->black = true;
					if (brother->keys[2]) {
						parent->keys[0] = brother->keys[2];
						brother->keys[2] = nullptr;
					}
					else {
						parent->keys[0] = brother->keys[1]; parent->keys[0]->black = false;
						brother->keys[1] = brother->keys[0]; brother->keys[1]->black = true;
						brother->keys[0] = nullptr;
						brother->pointers[2] = brother->pointers[1]; brother->pointers[1] = brother->pointers[0]; brother->pointers[0] = nullptr;
					}
				}
				else if (left and !uLevom) {
					p->keys[1] = parent->keys[2]; p->keys[1]->black = true;
					if (brother->keys[0]) {
						parent->keys[2] = brother->keys[0];
						brother->keys[0] = nullptr;
					}
					else {
						parent->keys[2] = brother->keys[1]; parent->keys[2]->black = false;
						brother->keys[1] = brother->keys[2]; brother->keys[1]->black = true;
						brother->keys[2] = nullptr;
						brother->pointers[1] = brother->pointers[2]; brother->pointers[2] = brother->pointers[3]; brother->pointers[3] = nullptr;
					}
				}
				else {
					p->keys[1] = parent->keys[2]; p->keys[1]->black = true;
					if (brother->keys[2]) {
						parent->keys[2] = brother->keys[2];
						brother->keys[2] = nullptr;
					}
					else {
						parent->keys[2] = brother->keys[1]; parent->keys[2]->black = false;
						brother->keys[1] = brother->keys[0]; brother->keys[1]->black = true;
						brother->keys[0] = nullptr;
						brother->pointers[2] = brother->pointers[1]; brother->pointers[1] = brother->pointers[0]; brother->pointers[0] = nullptr;
					}
				}
			}
			else {
				if (left and uLevom) {
					brother->keys[2] = brother->keys[1]; brother->keys[2]->black = false;
					brother->keys[1] = parent->keys[0]; brother->keys[1]->black = true;
					parent->keys[0] = nullptr;
					parent->pointers[0] = nullptr;
					delete p;
					preliven = brother;
				}
				else if (!left and uLevom) {
					p->keys[1] = parent->keys[0]; p->keys[1]->black = true;
					p->keys[0] = brother->keys[1]; p->keys[0]->black = false;
					brother->keys[1] = nullptr;
					parent->keys[0] = nullptr;
					parent->pointers[0] = nullptr;
					delete brother;
					preliven = p;
				}
				else if (left and !uLevom) {
					p->keys[1] = parent->keys[2]; p->keys[1]->black = true;
					p->keys[2] = brother->keys[1]; p->keys[2]->black = false;
					brother->keys[1] = nullptr;
					parent->keys[2] = nullptr;
					parent->pointers[3] = nullptr;
					delete brother;
					preliven = p;
				}
				else {
					brother->keys[0] = brother->keys[1]; brother->keys[0]->black = false;
					brother->keys[1] = parent->keys[2]; brother->keys[1]->black = true;
					parent->keys[2] = nullptr;
					parent->pointers[3] = nullptr;
					delete p;
					preliven = brother;
				}
			}
		}
		else if (blackParent) {
			if (left) {
				if (brother->has1Key()) {
					p->keys[1] = parent->keys[1]; parent->keys[1] = nullptr; parent->pointers[2] = nullptr;
					p->keys[2] = brother->keys[1]; p->keys[2]->black = false; brother->keys[1] = nullptr;
					p->pointers[2] = brother->pointers[1]; p->pointers[3] = brother->pointers[2];
					if (brother->pointers[1]) brother->pointers[1]->parent = p;
					if (brother->pointers[2]) brother->pointers[2]->parent = p;
					delete brother;
					preliven = p;
				}
				else if (brother->has2Keys()) {
					p->keys[1] = parent->keys[1];
					if (brother->keys[0]) { parent->keys[1] = brother->keys[0]; parent->keys[1]->black = true; brother->keys[0] = nullptr; }
					else { 
						parent->keys[1] = brother->keys[1]; brother->keys[1] = brother->keys[2]; brother->keys[1]->black = true; brother->keys[2] = nullptr; 
						brother->pointers[1] = brother->pointers[2]; brother->pointers[2] = brother->pointers[3]; brother->pointers[3] = nullptr;
					}
				}
				else {
					p->keys[1] = parent->keys[1]; p->keys[2] = brother->keys[0]; brother->keys[0] = nullptr;
					parent->keys[1] = brother->keys[1];
					brother->keys[1] = brother->keys[2]; brother->keys[1]->black = true; brother->keys[2] = nullptr;
					brother->pointers[1] = brother->pointers[2]; brother->pointers[2] = brother->pointers[3]; brother->pointers[3] = nullptr;
				}
			}
			else {
				if (brother->has1Key()) {
					brother->keys[0] = brother->keys[1]; brother->keys[0]->black = false;
					brother->keys[1] = parent->keys[1]; parent->keys[1] = nullptr; parent->pointers[2] = nullptr;
					brother->pointers[0] = brother->pointers[1]; brother->pointers[1] = brother->pointers[2]; brother->pointers[2] = p->pointers[1];
					if (p->pointers[1]) p->pointers[1]->parent = brother;
					delete p;
					preliven = brother;
				}
				else if (brother->has2Keys()) {
					p->keys[1] = parent->keys[1];
					if (brother->keys[2]) { parent->keys[1] = brother->keys[2]; parent->keys[1]->black = true; brother->keys[2] = nullptr; }
					else { 
						parent->keys[1] = brother->keys[1]; brother->keys[1] = brother->keys[0]; brother->keys[1]->black = true; brother->keys[0] = nullptr;
						brother->pointers[2] = brother->pointers[1]; brother->pointers[1] = brother->pointers[0]; brother->pointers[0] = nullptr;
					}
				}
				else {
					p->keys[1] = parent->keys[1];
					p->keys[0] = brother->keys[2]; brother->keys[2] = nullptr;
					parent->keys[1] = brother->keys[1];
					brother->keys[1] = brother->keys[0]; brother->keys[1]->black = true;
					brother->keys[0] = nullptr;
					brother->pointers[2] = brother->pointers[1]; brother->pointers[1] = brother->pointers[0]; brother->pointers[0] = nullptr;
				}
			}
		}
		else {
			if (left) {
				if (brother->isFull()) {
					p->keys[1] = parent->keys[1]; parent->pointers[0] = parent->pointers[1];
					p->keys[2] = brother->keys[0];
					parent->keys[0] = brother->keys[1]; parent->keys[0]->black = false;
					parent->keys[1] = parent->keys[2]; parent->keys[1]->black = true;
					parent->keys[2] = nullptr;
					parent->pointers[1] = parent->pointers[2]; parent->pointers[2] = parent->pointers[3]; parent->pointers[3] = nullptr;
					brother->keys[1] = brother->keys[2]; brother->keys[1]->black = true;
					brother->keys[0] = nullptr; brother->keys[2] = nullptr;
				}
				else if (brother->has2Keys()) {
					p->keys[1] = parent->keys[1]; parent->keys[1] = parent->keys[2]; parent->keys[1]->black = true; parent->keys[2] = nullptr;
					parent->pointers[0] = parent->pointers[1]; parent->pointers[1] = parent->pointers[2]; parent->pointers[2] = parent->pointers[3];
					parent->pointers[3] = nullptr;
					if (brother->keys[0]) {
						parent->keys[0] = brother->keys[0]; brother->keys[0] = nullptr;
					}
					else {
						parent->keys[0] = brother->keys[1]; parent->keys[0]->black = false;
						brother->keys[1] = brother->keys[2]; brother->keys[1]->black = true;
						brother->keys[2] = nullptr;
					}
				}
				else {
					brother->keys[2] = brother->keys[1]; brother->keys[2]->black = false;
					brother->keys[1] = parent->keys[1];
					parent->keys[1] = parent->keys[2]; parent->keys[1]->black = true; parent->keys[2] = nullptr;
					parent->pointers[1] = parent->pointers[2]; parent->pointers[2] = parent->pointers[3]; parent->pointers[3] = nullptr;
					delete p;
				}
			}
			else {
				if (brother->isFull()) {
					p->keys[1] = parent->keys[1]; parent->keys[1] = parent->keys[0]; parent->keys[1]->black = true; parent->keys[0] = nullptr;
					parent->pointers[3] = parent->pointers[2]; parent->pointers[2] = parent->pointers[1]; parent->pointers[1] = parent->pointers[0];
					parent->pointers[0] = nullptr;
					p->keys[0] = brother->keys[2]; parent->keys[2] = brother->keys[1]; parent->keys[2]->black = false; brother->keys[1] = brother->keys[0];
					brother->keys[1]->black = true; brother->keys[0] = nullptr; brother->keys[2] = nullptr;
				}
				else if (brother->has2Keys()) {
					p->keys[1] = parent->keys[1]; parent->keys[1] = parent->keys[0];
					parent->keys[1]->black = true; parent->keys[0] = nullptr;
					parent->pointers[3] = parent->pointers[2]; parent->pointers[2] = parent->pointers[1]; parent->pointers[1] = parent->pointers[0];
					parent->pointers[0] = nullptr;
					if (brother->keys[2]) {
						parent->keys[2] = brother->keys[2]; brother->keys[2] = nullptr;
					}
					else {
						parent->keys[2] = brother->keys[1]; parent->keys[2]->black = false;
						brother->keys[1] = brother->keys[0]; brother->keys[1]->black = true;
						brother->keys[0] = nullptr;
					}
				}
				else {
					brother->keys[0] = brother->keys[1]; brother->keys[0]->black = false;
					brother->keys[1] = parent->keys[1];
					parent->keys[1] = parent->keys[0]; parent->keys[1]->black = true;
					parent->keys[0] = nullptr;
					parent->pointers[2] = parent->pointers[1]; parent->pointers[1] = parent->pointers[0];
					parent->pointers[0] = nullptr;
					delete p;
				}
			}
		}
		if (parent == root and root->isEmpty()) {
			root = preliven;
			root->parent = nullptr;
			break;
		}
		p = parent;
	}
}

rootInfo* deleteKey(Node*& root, rootInfo* key, bool& nadjen) {
	Node* p = root;
	while (p) {
		Node* next = nullptr; bool nadjen = false;
		for (int i = 3; i >= 0; i--) if (p->pointers[i]) { next = p->pointers[i]; break; }
		for (int i = 0; i < 3; i++) {
			if (p->keys[i] and equalKeys(p->keys[i], key)) { nadjen = true; break; delete key; key = p->keys[i]; }
		}
		if (nadjen) break;
		for (int i = 0; i < 3; i++) {
			if (compare(p->keys[i], key)) { next = p->pointers[i]; break; }
		}
		p = next;
	}
	if (!p) { cout << "Nije nadjena akcija." << endl; delete key;  nadjen = false; return nullptr; }
	if (!p->isLeaf()) {
		int pos = 0;
		for (int i = 0; i < 3; i++) if (p->keys[i] and equalKeys(p->keys[i],key)) { pos = i; break; }
		Node* q = p->pointers[pos + 1];
		while (!q->isLeaf()) {
			for (int i = 0; i < 3; i++) if (q->pointers[i]) { q = q->pointers[i]; break; }
		}
		if (q->keys[0]) {
			if (equalKeys(key,p->keys[0])) { swap(p->keys[0], q->keys[0]);}
			else if (equalKeys(key,p->keys[1])) { swap(p->keys[1], q->keys[0]); p->keys[1]->black = true; }
			else { swap(p->keys[2], q->keys[0]); }
			key = q->keys[0];
		}
		else {
			if (equalKeys(key, p->keys[0])) { swap(p->keys[0], q->keys[1]); p->keys[0]->black = false; }
			else if (equalKeys(key,p->keys[1])) { swap(p->keys[1], q->keys[1]); p->keys[1]->black = true; }
			else { swap(p->keys[2], q->keys[1]); p->keys[2]->black = false; }
			key = q->keys[1];
		}
		p = q;
	}
	if (p->isFull()) {
			if (equalKeys(key,p->keys[0])) p->keys[0] = nullptr;
			else if (equalKeys(key,p->keys[2])) p->keys[2] = nullptr;
			else { p->keys[1] = p->keys[2]; p->keys[2] = nullptr; p->keys[1]->black = true; }
		}
	else if (p->has2Keys()) {
		if (equalKeys(key, p->keys[1])) {
			p->keys[1] = p->keys[0] ? p->keys[0] : p->keys[2]; p->keys[1]->black = true;
			if (p->keys[0]) p->keys[0] = nullptr;
			else p->keys[2] = nullptr;
		}
		else if (equalKeys(key, p->keys[0])) {
			p->keys[0] = nullptr;
		}
		else p->keys[2] = nullptr;
	}
	else {
		Node* parent = p->parent;
		if (!parent) { delete root; root = nullptr;}
		p->keys[1] = nullptr;
		fix(root, p);
	}
	return key;
}

void deleteNode(Node* node) {
	for (int i = 0; i < 3; i++) delete node->keys[i];
	delete node;
}

void deleteTree(Node* root) {
	if (!root) return;
	queue <Node*> q; q.push(root);
	while (!q.empty()) {
		Node* node = q.front(); q.pop();
		if (node) {
			for (int i = 0; i < 4; i++) q.push(node->pointers[i]);
			deleteNode(node);
		}
	}
}

rootInfo* findMaxPriority(Node* root) {
	rootInfo* key = new rootInfo(0, "");
	while (!(root->isLeaf())) {
		if (root->pointers[3]) root = root->pointers[3];
		else root = root->pointers[2];
	}
	if (root->keys[2]) *key = *root->keys[2];
	else *key = *root->keys[1];
	return key;
}

void actionPriorityChange(Node*& root) {
	string actionName; int priority;
	cout << "Unesite naziv akcije kojoj menjate prioritet: "; cin.ignore(); getline(cin, actionName);
	cout << "Unesite prioritet akcije kojoj menjate prioritet: "; cin >> priority;
	rootInfo* key = new rootInfo(priority, actionName);
	Node* p = root,*q=nullptr;
	while (p) {
		q = p;
		Node* next = nullptr; bool nadjen = false;
		for (int i = 3; i >= 0; i--) if (p->pointers[i]) { next = p->pointers[i]; break; }
		for (int i = 0; i < 3; i++) {
			if (compare(p->keys[i], key)) { next = p->pointers[i]; break; }
		}
		for (int i = 0; i < 3;i++) {
			if (equalKeys(p->keys[i], key)) { delete key; key = p->keys[i]; nadjen = true; break; }
		}
		if (nadjen) break;
		p = next;
	}
	if (!p) { cout << "Data akcija nije nadjena." << endl; return; }
	int newPriority; cout << "Unesite njen nov prioritet: "; cin >> newPriority;
	rootInfo* newKey = new rootInfo(newPriority, key->actionName); bool ok1 = true;
	if (key == p->keys[0]) {
		if (!compare(p->keys[1], newKey)) ok1 = false;
		if (p->pointers[0]) if (compare(maxInSubtree(p->pointers[0]), newKey)) ok1 = false;
		if (p->pointers[1]) if (!compare(minInSubtree(p->pointers[1]), newKey)) ok1 = false;
	}
	else if (key == p->keys[1]) {
		if(p->keys[2]) if (!compare(p->keys[2], newKey)) ok1 = false;
		if(p->keys[0]) if (compare(p->keys[0], newKey)) ok1 = false;
		if (p->pointers[1]) if (compare(maxInSubtree(p->pointers[1]), newKey)) ok1 = false;
		if (p->pointers[2]) if (!compare(minInSubtree(p->pointers[2]), newKey)) ok1 = false;
	}
	else {
		if (compare(p->keys[1], newKey)) ok1 = false;
		if (p->pointers[2]) if (compare(maxInSubtree(p->pointers[2]), newKey)) ok1 = false;
		if (p->pointers[3]) if (!compare(minInSubtree(p->pointers[3]), newKey)) ok1 = false;
	}
	if (p->parent) {
		if (p == p->parent->pointers[0]) {
			if (!compare(p->parent->keys[0],newKey)) ok1 = false;
		}
		else if (p == p->parent->pointers[1]) {
			if(p->parent->keys[0]) if (compare(p->parent->keys[0], newKey)) ok1 = false;
			if (!compare(p->parent->keys[1], newKey)) ok1 = false;
		}
		else if (p == p->parent->pointers[2]) {
			if (compare(p->parent->keys[1], newKey)) ok1 = false;
			if(p->parent->keys[2]) if (!compare(p->parent->keys[2], newKey)) ok1 = false;
		}
		else {
			if (compare(p->parent->keys[2], newKey)) ok1 = false;
		}
	}
	if (ok1) { key->priority = newKey->priority; }
	else
	{
		rootInfo* helper = new rootInfo(key->priority, key->actionName); bool nadjen = true;
		helper = deleteKey(root, helper, nadjen); helper->priority = newPriority; helper->black = false;
		insert(root, helper);
	}
	cout << "Uspesno zamenjen prioritet." << endl;
	delete newKey;
}

int main() {
	Node* root = nullptr;
	const string meni = "0.Izlaz\n1.Umetanje nove akcije\n2.Pretrazivanje stabla po prioritetu akcije\n3.Brisanje proizvoljne akcije\n4.Brisanje akcije najveceg prioriteta\n5.Ispis 2-3-4\n6.Ispis u obliku binarnog crveno-crnog\n7.Izmena prioriteta akcije\n8.Pronalazenje ukupnog broja akcija sa zadatim prioritetom\n9.Formiranje stabla iz datoteke\n10. Brisanje stabla\n11.Ponovni ispis menija\n";
	int input = -1;
	cout << meni;
	do {
		cout << "Unesite broj zeljene akcije: ";
		cin >> input;
		switch (input) {
		case 0: break;
		case 1: {
			string actionName; int priority;
			cout << "Unesite ime akcije: "; cin >> actionName;
			cout << "Unesite prioritet akcije: "; cin >> priority;
			rootInfo* key = new rootInfo(priority, actionName);
			insert(root, key); cout << "Uspesno ubacena akcija." << endl;
		} break;
		case 2: {
			if (root) {
				int priority;
				cout << "Unesite prioritet akcije za koji se vrsi pretraga: "; cin >> priority;
				rootInfo* key = searchByPriority(root, priority);
				if (key) cout << "Nadjena je akcija " << key->actionName << " sa prioritetom " << key->priority << endl;
				else cout << "Nije nadjena akcija sa prioritetom " << priority << endl;
			}
			else {
				cout << "Stablo ne postoji." << endl;
			}
		} break;
		case 3: {
			if (root) {
				string naziv; int priority;
				cout << "Unesite naziv akcije za brisanje: "; cin.ignore(); getline(cin, naziv);
				cout << "Unesite prioritet akcije za brisanje: "; cin >> priority;
				rootInfo* key = new rootInfo(priority, naziv); bool nadjen = 1;
				key = deleteKey(root, key, nadjen); delete key; if(nadjen) cout << "Kljuc uspesno obrisan." << endl;
			}
			else cout << "Stablo ne postoji." << endl;
		}break;
		case 4: {
			if(!root) cout << "Stablo ne postoji." << endl;
			else {
				rootInfo* key = findMaxPriority(root); bool nadjen = true;
				key = deleteKey(root, key, nadjen); cout << "Akcija koja je obrisana je " << key->actionName << " sa prioritetom " << key->priority << endl;
				delete key;
			}
		}break;
		case 5: {
			if (root) cout << root;
			else cout << "Stablo ne postoji." << endl;
		} break;
		case 6: {
			if (!root) cout << "Stablo ne postoji." << endl;
			else {
				printAsBinary(root);
			}
		} break;
		case 7: {
			if(!root) cout << "Stablo ne postoji." << endl;
			else {
				actionPriorityChange(root);
			}
		} break;
		case 8: {
			if (!root) cout << "Stablo ne postoji." << endl;
			else {
				int priority;
				cout << "Unesite prioritet za koji se trazi broj akcija: "; cin >> priority;
				int number = numOfActions(root,priority);
				cout << "Broj akcija sa prioritem " << priority << " je " << number << endl;
			}
		} break;
		case 9: {
			Node *root2 = treeFromFile();
			if (root2) { 
				cout << "Uspesno formirano stablo." << endl;
				if(root) deleteTree(root);
				root = root2;
			}
			else {
				cout << "Greska pri formiranju stabla.(Prazna datoteka ili datoteka ne postoji)" << endl;
			}
		}break;
		case 10: {
			if (!root) { cout << "Stablo ne postoji." << endl; }
			else {
				deleteTree(root);
				root = nullptr;
				cout << "Stablo uspesno obrisano." << endl;
			}
		} break;
		case 11: {
			cout << meni;
		} break;
		default: cout << "Broj je van opsega." << endl;
		}

	} while (input);
	if (root) {
		deleteTree(root); root = nullptr;
	}
	return 0;
}