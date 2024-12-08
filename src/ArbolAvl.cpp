#include <iostream>
#include <windows.h>
using namespace std;

// Clase para arbol autobalanceado (AVL)
template <typename T>
class arbolAVL {
    // Estructura del nodo del arbol AVl
    struct Node {
        T data;
        Node *left;
        Node *right;
        int height;
        // Constructor vacio
        Node (T val) : data (val), left(NULL), right(NULL), height(1) {}
    };
    // Raiz del arbol
    Node* root;
    
    // Funcion para obtener la altura del arbol
    int height (Node* node) {
        return node ? node->height : 0;
    }
    
    // Funcion para calcular el factor de balance de un nodo
    int balanceFactor (Node* node) {
        return node ? height(node->left) - height(node->right) : 0;
    }

    // Funcion para actualizar la altura de un nodo
    void updateHeight (Node* node) {
        if (node) {
            node->height = max(height(node->left), height(node->right)) + 1;
        }
    }
    // Rotacion simple a la derecha
    Node* rotateRight (Node* y) {
        Node* x = y->left;
        Node* T2 = x->right;
        x->right = y;
        y->left = T2;
        updateHeight(y);
        updateHeight(x);
        return x;
    }

    // Rotacion simple a la izquierda
    Node* rotateLeft (Node* x) {
        Node* y = x->right;
        Node* T2 = y->left;
        y->left = x;
        x->right = T2;
        updateHeight(x);
        updateHeight(y);
        return y;
    }

    // Balancear un nodo 
    Node* balance (Node* node) {
        if (!node) return node;
        updateHeight(node);
        int balanceFactorNode = balanceFactor(node);
        if (balanceFactorNode > 1) {
            if (balanceFactor(node->left) < 0) node->left = rotateLeft(node->left);
            return rotateRight(node);
        }
        if (balanceFactorNode < -1) {
            if (balanceFactor(node->right) > 0) node->right = rotateRight(node->right);
            return rotateLeft(node);
        }
        return node;
    }

    // Insertar un nodo en el arbol 
    Node* insert (Node* node, T data) {
        if (!node) return new Node(data);
        if (data < node->data) node->left = insert(node->left, data);
        else node->right = insert(node->right, data);
        return balance(node);
    }

    // Encontrar el nodo con el valor minimo
    Node* findMinValue (Node* node) { 
        Node* current = node;
        while (current->left != NULL) current = current->left;
        return current;
    }

    // Eliminar un nodo del arbol 
    Node* deleteNode (Node* root, T data) { 
        if (!root) return root;
        if (data < root->data) root->left = deleteNode(root->left, data);
        else if (data > root->data) root->right = deleteNode(root->right, data);
        else {
            if ((root->left == NULL) || (root->right == NULL)) {
                Node* temp = root->left ? root->left : root->right;
                if (!temp) {
                    temp = root;
                    root = NULL;
                } else *root = *temp;
                delete temp;
            } else {
                Node* temp = findMinValue(root->right);
                root->data = temp->data;
                root->right = deleteNode(root->right, temp->data);
            }
        }
        if (!root) return root;
        return balance(root);
    }
    // Imprimir el arbol en orden
void printInOrder(Node* root, string prefix = "", bool isLeft = true) const {
    if (root == NULL) return;
    cout << prefix << (isLeft ? "|-- " : "L-- ") << root->data << "\n";
    printInOrder(root->left, prefix + (isLeft ? "|   " : "    "), true);
    printInOrder(root->right, prefix + (isLeft ? "|   " : "    "), false);
}
  

    public: 
    // Constructor del arbol
    arbolAVL() : root (NULL) {}
    // Funcion en public para insertar nodo
    void insert (T data) {
        root = insert(root, data);
    }
    // Funcion en public para eliminar nodo
    void deleteNode (T data) {
        root = deleteNode(root, data);
    }
    // Funcion en public para imprimir el arbol
    void print() {
        printInOrder(root);
        cout<<endl;
    }
};

//Implementacion en el main

int main() {
    char seguir;
    do {
    // Configurar la consola para usar UTF-8
    SetConsoleOutputCP(CP_UTF8); // Establece la salida en UTF-8, caracteres especiales en español
    SetConsoleCP(CP_UTF8);       // Establece la entrada en UTF-8, caracteres especiales en español

    arbolAVL<int> avl;
    cout <<"\n";
    cout<< "***Programa de arbol AVL***\n";
    cout <<"\n";
    cout << "Insertando nodos en el árbol AVL:\n";
    avl.insert(40);
    cout << "Nodo 40 insertado.\n";
    avl.insert(20);
    cout << "Nodo 20 insertado.\n";
    avl.insert(60);
    cout << "Nodo 60 insertado.\n";
    avl.insert(10);
    cout << "Nodo 10 insertado.\n";
    avl.insert(30);
    cout << "Nodo 30 insertado.\n";
    avl.insert(50);
    cout << "Nodo 50 insertado.\n";
    avl.insert(70);
    cout << "Nodo 70 insertado.\n";
    avl.insert(5);
    cout << "Nodo 5 insertado.\n";
    avl.insert(15);
    cout << "Nodo 15 insertado.\n";
    avl.insert(25);
    cout << "Nodo 25 insertado.\n";

    cout << "\nÁrbol AVL después de las inserciones (en orden):\n";
    avl.print();

    cout << "\nEliminando nodo 20 del árbol AVL.\n";
    avl.deleteNode(20);

    cout << "\nÁrbol AVL después de eliminar el nodo 20 (en orden):\n";
    avl.print();
    cout<<"Programa terminado, quiere salir? (s/n).\n";
    cin >> seguir;
    } while (seguir != 's' && seguir != 'S');
    cout << "Programa terminado.\n";
    return 0;
}