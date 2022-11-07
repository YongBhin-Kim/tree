
/**
 * 2022-fp 수강생 20182211 김용빈 Report 1
 * 
 * 삽입 : 성공
 * 삭제 : 성공
 * 
 * 실행환경은 이 파일 단 하나입니다. 빌드하여 확인할 수 있습니다.
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
    node* left;
    node* right;
};

// Declare Functions
node* getNode(); // Used in insertBST
void insertBST(node*& T, int newKey); // BST 삽입

int height(node*& T); // Used in deleteBST
int noNodes(node*& T); // Used in deleteBST
node* maxNode(node*& T, node** stack, int &i); // Used in deleteBST
node* minNode(node*& T, node** stack, int &i); // Used in deleteBST
void deleteBST(node*& T, int deleteKey); // BST 삭제

void inorderBST(node*& T); // BST 중위순회
vector<string> split(string str); // text file 한 행을 읽기 위한 split 함수


// get Node (node init)
node* getNode() {
    node* newBSTNode = new node;
    newBSTNode->key = 0;
    newBSTNode->height = 0;
    newBSTNode->left = nullptr;
    newBSTNode->right = nullptr;
    return newBSTNode;
}

// insert BST
void insertBST(node*& T, const int newKey) {

    node* p = T;
    node* q = nullptr;
    node* stack[NODESIZE];
    int i=0;

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

    while (i > 0) { // stack[0] stack[1] .. stack[i-1]
        i--;
        if ((stack[i]->left == nullptr) && (stack[i]->right == nullptr)) // 자식 노드가 없을 때
            stack[i]->height = 0;

        else if (stack[i]->right == nullptr) // 왼쪽 자식 노드만 있을 때
            stack[i]->height = 1 + height(stack[i]->left);

        else if (stack[i]->left == nullptr) // 오른쪽 자식 노드만 있을 때
            stack[i]->height = 1 + height(stack[i]->right);

        // 모든 자식 노드가 존재할 때는 자식 노드의 height의 max값
        else
            stack[i]->height = 1 + ( ((height(stack[i]->left)) > (height(stack[i]->right))) ? (height(stack[i]->left)) : (height(stack[i]->right)) );
        printf("[%d] height : %d\n", stack[i]->key, height(stack[i]));

        stack[i] = nullptr;
    }


}

// height
int height(node*& T) {
    return T->height;
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
void deleteBST(node*& T, int deleteKey) {
    node* p = T;
    node* q = nullptr;
    node* stack[NODESIZE];

    int i=0;
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
        else if (height(p->left) > height(p->right)) p = maxNode(p->left, stack, i);
        // 높이가 같은 경우 subtree의 node 수로 결정, node 수도 같은 경우는 왼쪽 서브트리의 노드의 key의 최대값을 사용
        else {
            if (noNodes(p->left) >= noNodes(p->right)) p = maxNode(p->left, stack, i);
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

    // 삭제 후 height update
    while (i > 0) {
        i--;
        if ((stack[i]->left == nullptr) && (stack[i]->right == nullptr)) // 자식 노드가 없을 때
            stack[i]->height = 0;

        else if (stack[i]->right == nullptr) // 왼쪽 자식 노드만 있을 때
            stack[i]->height = 1 + height(stack[i]->left);

        else if (stack[i]->left == nullptr) // 오른쪽 자식 노드만 있을 때
            stack[i]->height = 1 + height(stack[i]->right);

        else // 모든 자식 노드가 존재할 때는 자식 노드의 height의 max값
            stack[i]->height = 1 + ( (height(stack[i]->left) > (height(stack[i]->right))) ? height(stack[i]->left) : height(stack[i]->right) );
        // printf("[%d] height : %d\n", stack[i]->key, height(stack[i]));

        stack[i] = nullptr;
    }


}

// inorder traversal
void inorderBST(node*& T) {
    node* p = T;
    if (p == nullptr) return;

    if (p) {
        inorderBST(p->left);
        printf("%d ", p->key);
        inorderBST(p->right);
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

// Test Function 
void test() {
    node* myNode = nullptr;

    string fp = "bst-input.txt";
    ifstream openFile(fp.data());
    if (openFile.is_open()) {
        string line;
        while (getline(openFile, line)) {

            vector<string> command = split(line);

            // i 명령일 때 insert
            if (((string)command[0].c_str()) == "i")
                insertBST(myNode, atoi(command[1].c_str()));

            // d 명령일 때 delete
            else if (((string)command[0].c_str()) == "d")
                deleteBST(myNode, atoi(command[1].c_str()));
            
            // inorder traversal
            inorderBST(myNode); cout << "" << endl;
        }
        openFile.close();
    }

}

// main Function
int main() {
    test();
}