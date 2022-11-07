
/**
 * ============================================================
 * 2022-fp 수강생 20182211 김용빈 Report 2
 * 
 * [AVL Tree]
 * 삽입 : 
 * 삭제 : 
 * 
 * 실행환경은 이 파일 단 하나입니다. 빌드하여 확인할 수 있습니다.
 * ============================================================
*/

#define NODESIZE 10000

#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

// node Class
class node {
public:
    int key;
    int height;
    int bf;
    node* left;
    node* right;
};

// Declare Functions
node* getNode(); // Used in insertBST
void insertBST(node*& T, const int newKey, node** stack, int &i); // BST 삽입

int height(node*& T); // Used in deleteBST
int noNodes(node*& T); // Used in deleteBST
node* maxNode(node*& T, node** stack, int &i); // Used in deleteBST
node* minNode(node*& T, node** stack, int &i); // Used in deleteBST
void deleteBST(node*& T, int deleteKey, node** stack, int &i); // BST 삭제

void inorderAVL(node*& T); // BST 중위순회
vector<string> split(string str); // text file 한 행을 읽기 위한 split 함수

void checkBalance(node*& T, const int newKey, string &rotationType, node*& p, node*& q, node** stack, int &i);

// get Node (node init)
node* getNode() {
    node* newAVLNode = new node;
    newAVLNode->key = 0;
    newAVLNode->height = 0;
    newAVLNode->bf = 0;
    newAVLNode->left = nullptr;
    newAVLNode->right = nullptr;
    return newAVLNode;
}

// insert BST
void insertBST(node*& T, const int newKey, node** stack, int &i) {
    i = 0;
    node* p = T;
    node* q = nullptr;

    while (p != nullptr) {
        // key already exists case
        if (newKey == p->key) {
            printf("i %d : The key already exists\n", newKey);
            return;
        }
        q = p;
        stack[i] = q; i++;

        if (newKey < p->key)
            p = p->left;
        else
            p = p->right; 
    } // --> p = null

    node* newNode = getNode();
    newNode->key = newKey;

    if (T == nullptr)
        T = newNode;
    else if (newKey < q->key)
        q->left = newNode;
    else
        q->right = newNode;
    
    int j = i;
    while (j > 0) { // stack[0] stack[1] .. stack[i-1]
        j--;
        if ((stack[j]->left == nullptr) && (stack[j]->right == nullptr)) // 자식 노드가 없을 때
            stack[j]->height = 0;

        else if (stack[j]->right == nullptr) // 왼쪽 자식 노드만 있을 때
            stack[j]->height = 1 + height(stack[j]->left);

        else if (stack[j]->left == nullptr) // 오른쪽 자식 노드만 있을 때
            stack[j]->height = 1 + height(stack[j]->right);

        // 모든 자식 노드가 존재할 때는 자식 노드의 height의 max값
        else
            stack[j]->height = height(stack[j]);
            
    }
}

// height
int height(node*& T) {
    if (T == nullptr)
        return -1;
    if ( (T->left == nullptr) && (T->right == nullptr) )
        return 0;
    if ( (T->left != nullptr) && (T->right == nullptr) )
        return 1 + T->left->height;
    if ( (T->left == nullptr) && (T->right != nullptr) )
        return 1 + T->right->height;
    if ( (T->left != nullptr) && (T->right != nullptr) )
        return 1+max(T->left->height, T->right->height);
    return 0;
}

// noNodes
int noNodes(node*& T) {
    node* p = T;
    int cnt = 0;
    if (p == nullptr) return 0;
    if (p) {
        cnt = 1 + noNodes(p->left) + noNodes(p->right);
    }
    return cnt;
}

// maxNode
node* maxNode(node*& T, node** stack, int &i) {
    node* p = T;
    while (p->right != nullptr) {
        stack[i] = p; i++;
        p = p->right;
    }
    return p;
}

// minNode
node* minNode(node*& T, node** stack, int &i) {
    node* p = T;
    while (p->left != nullptr) {
        stack[i] = p; i++;
        p = p->left;
    }
    return p;
}

// delete BST
void deleteBST(node*& T, int deleteKey, node** stack, int &i) {
    node* p = T;
    node* q = nullptr;
    // node* stack[NODESIZE];

    i = 0;
    // 하위 노드로 계속 찾아가다가 deleteKey를 찾거나 끝까지 못찾으면 종료
    while ((p != nullptr) && (deleteKey != p->key)) {
        q = p;
        stack[i] = q; i++;

        if (deleteKey < p->key) p = p->left;
        else p = p->right;
    }

    // deleteKey not found case
    if (p == nullptr) {
        printf("d %d : The key does not exist\n", deleteKey);
        return;
    }

    // deleteKey found, case of degree 2 -> p와 q의 위치를 이동시키고 case of degree 1, case of degree 0 으로 해결
    if ((p->left != nullptr) && (p->right != nullptr)) {
        node* tmpNode;
        stack[i] = p; i++;
        tmpNode = p; // tmpNode <- deleteKey (now p)

        // Balance를 고려하여 높이가 높은 서브트리의 노드로 대체
        // p <- 오른쪽 서브트리 노드의 key의 최소를 갖는 노드
        if (height(p->left) < height(p->right)) p = minNode(p->right, stack, i);
        // p <- 왼쪽 서브트리 노드의 key의 최대를 갖는 노드
        else if (height(p->left) > height(p->right)) {
            p = maxNode(p->left, stack, i);
        }
        // 높이가 같은 경우 subtree의 node 수로 결정, node 수도 같은 경우는 왼쪽 서브트리의 노드의 key의 최대값을 사용
        else {
            if (noNodes(p->left) > noNodes(p->right))  {
                p = maxNode(p->left, stack, i); // yb
            }
            else  p = minNode(p->right, stack, i);
        }

        // trick : 결정한 노드의 key값을 deleteKey가 key값인 노드의 key값으로 대체
        tmpNode->key = p->key;
        q = stack[i-1];

    }

    // 여기서 주목할 점은 degree 2 trick 처리 후 degree 1 or degree 0 의 case로 처리하기 위해 else if 가 아닌 if문을 사용한 것이다.
    // deleteKey found, case of degree 0
    if ((p->left == nullptr) && (p->right == nullptr)) {
        // case of root
        if (q == nullptr) T = nullptr;
        // q의 왼쪽 자식이 p
        else if (q->left == p) q->left = nullptr;
        // q의 오른쪽 자식이 p
        else q->right =nullptr;
    }
    // deleteKey found, case of degree 1
    else {
        if (p->left != nullptr) {
            if (q == nullptr) T = T->left;
            else if (q->left == p) q->left = p->left;
            else q->right= p->left;
        }
        else {
            if (q == nullptr) T = T->right;
            else if (q->left == p) q->left = p->right;
            else q->right = p->right;
        }
    }

    int j = i;
    // 삭제 후 height update
    while (j > 0) {
        j--;
        // printf("%d ", stack[i]->key);
        if ((stack[j]->left == nullptr) && (stack[j]->right == nullptr)) { // 자식 노드가 없을 때
            stack[j]->height = 0;
        }

        else if (stack[j]->right == nullptr) // 왼쪽 자식 노드만 있을 때
            stack[j]->height = 1 + height(stack[j]->left);

        else if (stack[j]->left == nullptr) // 오른쪽 자식 노드만 있을 때
            stack[j]->height = 1 + height(stack[j]->right);

        else // 모든 자식 노드가 존재할 때는 자식 노드의 height의 max값
            stack[j]->height = height(stack[j]);
        // stack[i] = nullptr;
        // printf("<%d %d> ", stack[j]->key, stack[j]->height);
    }


}

// inorder traversal
void inorderAVL(node*& T) {
    node* p = T;
    if (p == nullptr) return;

    if (p) {
        inorderAVL(p->left);
        printf("(%d, %d) ", p->key, p->bf);
        inorderAVL(p->right);
    }

}

// File read split
vector<string> split(string str)
{
	vector<string> result;
	string tmp;

	for (int i=0; !(str[i] == 13 || str[i] == '\n' || str[i] == 0); i++) { // LF (Line Feed)
		if (str[i] == ' ') {
			result.push_back(tmp);
			tmp.clear();
			continue;
		}
		tmp.push_back(str[i]);
	}
	result.push_back(tmp);
	return result;
}

// =====================

// checkBalacne
void checkBalance(node*& T, const int newKey, string &rotationType, node*& p, node*& q, node** stack, int &i) {
    int key = newKey;
    p = T;
    q = nullptr;
    node* x = nullptr;
    node* f = nullptr;

    // bf 갱신
    q = nullptr;

    while (i > 0) { // stack[0] stack[1] .. stack[i-1]
        i--;
        q = stack[i];
        // printf("%d ", q->key);
        // stack[i] = nullptr;
        if ((q->left == nullptr) && (q->right == nullptr)) // 자식 노드가 없을 때
            q->bf = 0;

        else if (q->right == nullptr) { // 왼쪽 자식 노드만 있을 때
            // q->bf = 1+ q->left->height;
            q->bf = 1+ height(q->left);
        }

        else if (q->left == nullptr) { // 오른쪽 자식 노드만 있을 때
            // q->bf = -1 - q->right->height;
            q->bf = -1 - height(q->right);
        }

        // 모든 자식 노드가 존재할 때 : bf = (왼쪽 자식 노드의 height) - (오른쪽 자식 노드의 height)
        else {
            q->bf = height(q->left) - height(q->right);
        }

        // printf("[%d %d] ", q->key, q->bf);
        if ( (1 < q->bf) || (q->bf < -1) ) {
            if (x == nullptr) {
                x = q;
                p = x;
                if (i > 0) {
                    f = stack[i-1];
                }
            }
        }
    }

    if ( x == nullptr ) { // q is null or parent of p
        rotationType = "NO";
    }
    // L
    else {
        if ( x->bf == 2 ) {
            if ( x->left->bf < 0 ) { // LL
                rotationType = "LR";
            }
            // else if ( x->left->bf == -1 ) { // LR
            else {
                rotationType = "LL";
            }
        }
    // R
        if ( x->bf == -2 ) {
            if ( x->right->bf > 0 ) { // RL
                rotationType = "RL";
            }
            // else if ( x->right->bf == -1 ) { // RR
            else {
                rotationType = "RR";
            }
        }
    }
    q = nullptr;
    q = f;
}

void rotateTree(node*& T, string &rotationType, node*& p, node*& q) {
    int i = 0; 
    node* stack[NODESIZE];
    cout  << rotationType << " ";
    node* bftmp;
    if ( rotationType == "NO" )
        return;

    if ( rotationType == "LL" ) {
        node *tmp = p->left;
        p->left = tmp->right;
        tmp->right = p;

        p->bf = 0;
        tmp->right->height = height(tmp->right);
        tmp->right->bf = 0; // yb
        tmp->height = height(tmp);
        tmp->bf = height(tmp->left) - height(tmp->right);

        if (q == nullptr) { // case of p is root
            T = tmp;
        }

        else { // case of height of T >= 3 

            if (q->key > tmp->key) {
                q->left = tmp;
            }
            else {
                q->right = tmp;
            }
            bftmp = T;

            while (bftmp != tmp) {
                stack[i] = bftmp; i++;

                if (tmp->key < bftmp->key) {
                    bftmp = bftmp->left;    
                }
                else {
                    bftmp = bftmp->right;
                }
            }

            while(i > 0) {
                i--;
                stack[i]->height = height(stack[i]);
                stack[i]->bf = height(stack[i]->left) - height(stack[i]->right);
            }
        }

    }

    else if ( rotationType == "RR" ) {

        node *tmp = p->right;
        p->right = tmp->left;
        tmp->left = p;

        p->bf = 0;
        tmp->left->height = height(tmp->left);
        tmp->left->bf = height(tmp->left->left)-height(tmp->left->right);
        tmp->height = height(tmp);
        tmp->bf = height(tmp->left) - height(tmp->right);


        if (q == nullptr) { // case of p is root
            T = tmp;

        }

        else { // case of height of T >= 3  // 66 = 0, 81 = 0, 105--, 50유지, 
            i = 0;
            if ( tmp->key < q->key )
                q->left = tmp;
            else 
                q->right = tmp;

            bftmp = T;

            while (bftmp != tmp) {
                stack[i] = bftmp; i++;

                if (tmp->key < bftmp->key) {
                    bftmp = bftmp->left;    
                }
                else {
                    bftmp = bftmp->right;
                }
            }
            while(i > 0) {
                i--;
                stack[i]->height = height(stack[i]);
                stack[i]->bf = height(stack[i]->left) - height(stack[i]->right);
            }
            
        }
    }

    else if ( rotationType == "LR" ) {
        // 부분 RR
        node *tmp2 = p->left;
        node *tmp = tmp2->right;
        tmp2->right = tmp->left;
        tmp->left = tmp2;
        p->left = tmp; // tmp = 2 p = 3 tmp2 = 1
        
        // LL
        p->left = tmp->right;
        tmp->right = p;

        tmp->left->height = height(tmp->left);
        tmp->left->bf = height(tmp->left->left)-height(tmp->left->right);

        tmp->right->height = height(tmp->right);
        tmp->right->bf = height(tmp->right->left)-height(tmp->right->right);
        tmp->height = height(tmp);
        tmp->bf = height(tmp->left) - height(tmp->right);



        // bf, height update
        if (q == nullptr) { // case of p is root
            T = tmp;
        }
        else { // case of height of T >= 3  // 66 = 0, 81 = 0, 105--, 50유지, 
            i = 0;
            if ( tmp->key < q->key )
                q->left = tmp;
            else 
                q->right = tmp;

            bftmp = T;

            while (bftmp != tmp) {
                stack[i] = bftmp; i++;

                if (tmp->key < bftmp->key) {
                    bftmp = bftmp->left;    
                }
                else {
                    bftmp = bftmp->right;
                }
            }
            while(i > 0) {
                i--;
                stack[i]->height = height(stack[i]);
                stack[i]->bf = height(stack[i]->left) - height(stack[i]->right);
            }
            
        }
    }

    else if ( rotationType == "RL" ) {
        // 부분 LL
        node *tmp2 = p->right;

        node *tmp = tmp2->left;
        tmp2->left = tmp->right;
        tmp->right = tmp2;
        p->right = tmp; // tmp = 2 p = 3 tmp2 = 1
        
        // RR
        p->right = tmp->left;
        tmp->left = p;


        tmp->left->height = height(tmp->left);
        tmp->left->bf = height(tmp->left->left)-height(tmp->left->right);
        tmp->right->height = height(tmp->right);
        tmp->right->bf = height(tmp->right->left)-height(tmp->right->right);
        tmp->height = height(tmp);
        tmp->bf = height(tmp->left) - height(tmp->right);


        // bf, height update
        if (q == nullptr) { // case of p is root
            T = tmp;
        }
        else { // case of height of T >= 3  // 66 = 0, 81 = 0, 105--, 50유지, 
            i = 0;
            if ( tmp->key < q->key )
                q->left = tmp;
            else 
                q->right = tmp;

            bftmp = T;

            while (bftmp != tmp) {
                stack[i] = bftmp; i++;

                if (tmp->key < bftmp->key) {
                    bftmp = bftmp->left;    
                }
                else {
                    bftmp = bftmp->right;
                }
            }
            while(i > 0) {
                i--;
                stack[i]->height = height(stack[i]);
                stack[i]->bf = height(stack[i]->left) - height(stack[i]->right);
            }
            
        }
    }

}

// insert AVL
void insertAVL(node*& T, const int newKey) {
    node* stack[NODESIZE];
    string rotationType;
    node* p;
    node* q;
    int i = 0;
    insertBST(T, newKey, stack, i);
    checkBalance(T, newKey, rotationType, p, q, stack, i);
    rotateTree(T, rotationType, p, q);

}

// delete AVL
void deleteAVL(node*& T, const int deleteKey) {
    string rotationType;
    node* p;
    node* q;
    node* stack[NODESIZE];
    int i=0;
    deleteBST(T, deleteKey, stack, i);
    checkBalance(T, deleteKey, rotationType, p, q, stack, i);
    rotateTree(T, rotationType, p, q);

}

// Test Function 
void test() {
    node* myNode = nullptr;
    node* rootNode = nullptr;
    int newKey;
    int deleteKey;

    // string fp = "avl-input.txt";
    string fp = "test.txt";
    ifstream openFile(fp.data());
    if (openFile.is_open()) {
        string line;
        while (getline(openFile, line)) {

            vector<string> command = split(line);

            // i 명령일 때 insert
            if (((string)command[0].c_str()) == "i") {
                newKey = atoi(command[1].c_str());
                insertAVL(myNode, newKey);
            }

            // d 명령일 때 delete
            else if (((string)command[0].c_str()) == "d") {
                deleteKey = atoi(command[1].c_str());
                deleteAVL(myNode, deleteKey);
            }
            
            // inorder traversal
            inorderAVL(myNode); 
            cout << "" << endl;
        }
        openFile.close();
    }

}

// main Function
int main() {
    test();
}