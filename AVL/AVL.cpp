
/**
 * ============================================================
 * 2022-fp 수강생 20182211 김용빈 Report 2
 * 
 * [AVL Tree]
 * 삽입 : 성공
 * 삭제 : 성공
 * 
 * 실행환경은 이 파일 단 하나입니다. 빌드하여 확인할 수 있습니다.
 * ============================================================
*/

#define NODESIZE 10000

#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

// AVL Node Class 선언
class node {
public:
    int key;
    int height;
    int bf;
    node* left;
    node* right;
};

// Declare Functions
// BST 삽입
node* getNode(); // Used in insertBST
void insertBST(node*& T, const int newKey, node** stack, int &i); // BST 삽입

// BST 삭제
int height(node*& T); // Used in deleteBST
int noNodes(node*& T); // Used in deleteBST
node* maxNode(node*& T, node** stack, int &i); // Used in deleteBST
node* minNode(node*& T, node** stack, int &i); // Used in deleteBST
void deleteBST(node*& T, int deleteKey, node** stack, int &i); // BST 삭제

// 출력, 문자열 함수
void inorderAVL(node*& T); // AVL 중위순회
vector<string> split(string str); // text file 한 행을 읽기 위한 split 함수

// AVL 삽입, 삭제
void checkBalance(node*& T, const int newKey, string &rotationType, node*& p, node*& q, node** stack, int &i); // AVL트리의 BF를 재조정, rotationType을 판단하는 함수
void rotateTree(node*& T, string &rotationType, node*& p, node*& q); // checkBalance가 판단한 rotationType에 따라 트리를 회전, BF를 갱신하는 함수
void insertAVL(node*& T, const int newKey); // AVL 삽입
void deleteAVL(node*& T, const int deleteKey); // AVL 삭제


// get Node (node init)
node* getNode() {
    node* newAVLNode = new node;
    newAVLNode->key = 0;
    newAVLNode->height = 0;
    newAVLNode->bf = 0; // Balance Factor : want to {-1, 0, 1}
    newAVLNode->left = nullptr;
    newAVLNode->right = nullptr;
    return newAVLNode;
}

// insert BST
void insertBST(node*& T, const int newKey, node** stack, int &i) {
    i = 0;
    node* p = T;
    node* q = nullptr;

    // q는 p의 부모, p->key가 만일 newKey이면 이미 key가 존재하는 경우, 아닐 시 삽입 가능
    // stack에 p의 경로가 담겨있는데, 나중에 checkBalance에서 BF를 수정하기 위해 매개변수로 받아준다.
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
    while (j > 0) { // stack[0] stack[1] .. stack[j-1]
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
    if (T == nullptr) // T가 그 자체로 비어있는 경우
        return -1;
    if ( (T->left == nullptr) && (T->right == nullptr) ) // T의 자식이 없는 경우
        return 0;
    if ( (T->left != nullptr) && (T->right == nullptr) ) // T의 왼쪽 자식만 존재하는 경우
        return 1 + T->left->height;
    if ( (T->left == nullptr) && (T->right != nullptr) ) // T의 오른쪽 자식만 존재하는 경우
        return 1 + T->right->height;
    if ( (T->left != nullptr) && (T->right != nullptr) ) // T의 자식이 양쪽 다 존재하는 경우 1 + max(왼, 오)
        return 1+max(T->left->height, T->right->height);
    return 0;
}

// noNodes
int noNodes(node*& T) {
    // T의 하위 노드의 개수
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
    // T의 하위 노드중 key값이 최대인 노드를 찾아서 반환, 그리고 그 경로를 stack에 담는다.
    node* p = T;
    while (p->right != nullptr) {
        stack[i] = p; i++;
        p = p->right;
    }
    return p;
}

// minNode
node* minNode(node*& T, node** stack, int &i) {
    // T의 하위 노드중 key값이 최소인 노드를 찾아서 반환, 그리고 그 경로를 stack에 담는다.
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

    i = 0;
    // insertBST와 마찬가지로, 매개변수로 받은 stack에 q의 경로를 저장한다.
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
        if ((stack[j]->left == nullptr) && (stack[j]->right == nullptr)) // 자식 노드가 없을 때
            stack[j]->height = 0;

        else if (stack[j]->right == nullptr) // 왼쪽 자식 노드만 있을 때
            stack[j]->height = 1 + height(stack[j]->left);

        else if (stack[j]->left == nullptr) // 오른쪽 자식 노드만 있을 때
            stack[j]->height = 1 + height(stack[j]->right);

        else // 모든 자식 노드가 존재할 때는 자식 노드의 height의 max값
            stack[j]->height = height(stack[j]);
    }


}

// inorder traversal
void inorderAVL(node*& T) {
    node* p = T;
    if (p == nullptr) return;

    // 왼쪽 자식 -> 부모 -> 오른쪽 자식 의 순서로 탐색
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

// checkBalacne
void checkBalance(node*& T, const int newKey, string &rotationType, node*& p, node*& q, node** stack, int &i) {
    int key = newKey;
    p = T;
    q = nullptr;
    node* x = nullptr;
    node* f = nullptr;

    // bf 갱신
    q = nullptr;

    // stack에 담긴 (삽입 혹은 삭제 시 저장해둔) 노드들을 따라서 bf를 갱신한다.
    while (i > 0) { // stack[0] stack[1] .. stack[i-1]
        i--;
        q = stack[i];
        if ((q->left == nullptr) && (q->right == nullptr)) // 자식 노드가 없을 때
            q->bf = 0;

        else if (q->right == nullptr) // 왼쪽 자식 노드만 있을 때
            q->bf = 1+ height(q->left);

        else if (q->left == nullptr) // 오른쪽 자식 노드만 있을 때
            q->bf = -1 - height(q->right);

        // 모든 자식 노드가 존재할 때 : bf = (왼쪽 자식 노드의 height) - (오른쪽 자식 노드의 height)
        else
            q->bf = height(q->left) - height(q->right);

        // bf가 -1 미만 혹은 1 초과인 경우
        if ( (1 < q->bf) || (q->bf < -1) ) {
            // x == null이라는 뜻은, 처음 나왔다는 의미
            if (x == nullptr) {
                x = q;
                p = x;
                // 루트 노드가 아니라면, p의 부모를 f에 저장해두었다가, 나중에 q에 다시 집어넣을 것임
                if (i > 0) {
                    f = stack[i-1];
                }
            }
        }
    }

    // bf가 -1 이상 1 이하인 경우, 회전할 필요 없음
    if ( x == nullptr ) { 
        rotationType = "NO";
    }
    // LL or LR case
    else {
        if ( x->bf == 2 ) {
            if ( x->left->bf < 0 ) // LR
                rotationType = "LR";
            else { // LL
                rotationType = "LL";
            }
        }
    // RR or RL case
        if ( x->bf == -2 ) {
            if ( x->right->bf > 0 ) // RL
                rotationType = "RL";
            else // RR
                rotationType = "RR";
        }
    }
    q = nullptr;
    q = f;
}

// rotateTree
void rotateTree(node*& T, string &rotationType, node*& p, node*& q) {
    int i = 0; 
    node* stack[NODESIZE];
    node* bftmp; // BF 회전이 있다면 그 후 BF 갱신을 위해 선언

    // 회전이 필요 없는 경우
    if ( rotationType == "NO" )
        return;

    // case : LL rotation
    if ( rotationType == "LL" ) {
        node *tmp = p->left;
        p->left = tmp->right;
        tmp->right = p;

        // tmp 즉 회전 이후 가운데로 올라온 노드를 중심으로 BF를 갱신해준다. 왼쪽 노드와 서브트리는 유지하고 자신과 오른쪽 노드를 갱신한다.
        tmp->right->height = height(tmp->right);
        tmp->right->bf = 0;
        tmp->height = height(tmp);
        tmp->bf = height(tmp->left) - height(tmp->right);

        // case of p is root (height of T is 2)
        if (q == nullptr) 
            T = tmp;

        else { // case of height of T >= 3 

            // 회전 이후 서브트리의 루트노드가 된 tmp를 기존 부모와 엮어준다.
            if (q->key > tmp->key)
                q->left = tmp;
            else
                q->right = tmp;
            
            // 루트부터 시작해서 bftmp가 tmp까지 하위로 내려가면서 stack에 저장한 후, height와 BF를 갱신한다.
            bftmp = T;

            while (bftmp != tmp) {
                stack[i] = bftmp; i++;

                if (tmp->key < bftmp->key)
                    bftmp = bftmp->left;    
                else
                    bftmp = bftmp->right;
            }

            while(i > 0) {
                i--;
                stack[i]->height = height(stack[i]);
                stack[i]->bf = height(stack[i]->left) - height(stack[i]->right);
            }
        }

    }

    // case : RR rotation
    else if ( rotationType == "RR" ) {

        node *tmp = p->right;
        p->right = tmp->left;
        tmp->left = p;

        // tmp 즉 회전 이후 가운데로 올라온 노드를 중심으로 BF를 갱신해준다. 오른쪽 노드와 서브트리는 유지하고 자신과 왼쪽 노드를 갱신한다.
        tmp->left->height = height(tmp->left);
        tmp->left->bf = height(tmp->left->left)-height(tmp->left->right);
        tmp->height = height(tmp);
        tmp->bf = height(tmp->left) - height(tmp->right);

        // case of p is root (height of T is 2)
        if (q == nullptr)
            T = tmp;

        else { // case of height of T >= 3 
            // 회전 이후 서브트리의 루트노드가 된 tmp를 기존 부모와 엮어준다.
            if ( tmp->key < q->key )
                q->left = tmp;
            else 
                q->right = tmp;

            // 루트부터 시작해서 bftmp가 tmp까지 하위로 내려가면서 stack에 저장한 후, height와 BF를 갱신한다.
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
        p->left = tmp;
        
        // LL
        p->left = tmp->right;
        tmp->right = p;

        // LR 회전이 끝난 후 회전축의 맨 위로 올라온 tmp를 기준으로 본인과 왼,오른쪽 자식 모두 height와 BF를 갱신한다.
        tmp->left->height = height(tmp->left);
        tmp->left->bf = height(tmp->left->left)-height(tmp->left->right);

        tmp->right->height = height(tmp->right);
        tmp->right->bf = height(tmp->right->left)-height(tmp->right->right);
        tmp->height = height(tmp);
        tmp->bf = height(tmp->left) - height(tmp->right);



        // case of p is root (height of T is 2)
        if (q == nullptr)
            T = tmp;

        else { // case of height of T >= 3 
            // 회전 이후 서브트리의 루트노드가 된 tmp를 기존 부모와 엮어준다.
            if ( tmp->key < q->key )
                q->left = tmp;
            else 
                q->right = tmp;

            // 루트부터 시작해서 bftmp가 tmp까지 하위로 내려가면서 stack에 저장한 후, height와 BF를 갱신한다.
            bftmp = T;
            while (bftmp != tmp) {
                stack[i] = bftmp; i++;

                if (tmp->key < bftmp->key)
                    bftmp = bftmp->left;    
                else
                    bftmp = bftmp->right;
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
        p->right = tmp;
        
        // RR
        p->right = tmp->left;
        tmp->left = p;

        // RL 회전이 끝난 후 회전축의 맨 위로 올라온 tmp를 기준으로 본인과 왼,오른쪽 자식 모두 height와 BF를 갱신한다.
        tmp->left->height = height(tmp->left);
        tmp->left->bf = height(tmp->left->left)-height(tmp->left->right);
        tmp->right->height = height(tmp->right);
        tmp->right->bf = height(tmp->right->left)-height(tmp->right->right);
        tmp->height = height(tmp);
        tmp->bf = height(tmp->left) - height(tmp->right);


        // case of p is root (height of T is 2)
        if (q == nullptr)
            T = tmp;
        else { // case of height of T >= 3 
            // 회전 이후 서브트리의 루트노드가 된 tmp를 기존 부모와 엮어준다.
            if ( tmp->key < q->key )
                q->left = tmp;
            else 
                q->right = tmp;

            // 루트부터 시작해서 bftmp가 tmp까지 하위로 내려가면서 stack에 저장한 후, height와 BF를 갱신한다.
            bftmp = T;
            while (bftmp != tmp) {
                stack[i] = bftmp; i++;

                if (tmp->key < bftmp->key)
                    bftmp = bftmp->left;    
                else
                    bftmp = bftmp->right;
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

    insertBST(T, newKey, stack, i); // BST 삽입
    checkBalance(T, newKey, rotationType, p, q, stack, i); // 삽입 후 BF 갱신
    cout << rotationType << " "; // rotationType 출력
    rotateTree(T, rotationType, p, q); // 나온 타입에 따라 Tree rotate

}

// delete AVL
void deleteAVL(node*& T, const int deleteKey) {
    node* stack[NODESIZE];
    string rotationType;
    node* p;
    node* q;
    int i=0;
    deleteBST(T, deleteKey, stack, i); // BST 삭제
    checkBalance(T, deleteKey, rotationType, p, q, stack, i); // 삭제 후 BF 갱신
    cout << rotationType << " "; // rotationType 출력
    rotateTree(T, rotationType, p, q); // 나온 타입에 따라 Tree rotate

}

// main Function
int main() {
    node* myNode = nullptr;
    node* rootNode = nullptr;
    int newKey;
    int deleteKey;

    string fp = "avl-input.txt"; // 주어진 입력 텍스트 파일
    ifstream openFile(fp.data());
    if (openFile.is_open()) {
        string line;
        while (getline(openFile, line)) {

            vector<string> command = split(line);

            // i 명령일 때 insert
            if (((string)command[0].c_str()) == "i") {
                newKey = atoi(command[1].c_str());
                insertAVL(myNode, newKey); // AVL 삽입
            }

            // d 명령일 때 delete
            else if (((string)command[0].c_str()) == "d") {
                deleteKey = atoi(command[1].c_str());
                deleteAVL(myNode, deleteKey); // AVL 삭제
            }
            
            // inorder traversal
            inorderAVL(myNode); 
            cout << "" << endl;
        }
        openFile.close();
    }

}