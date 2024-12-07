#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

using namespace std;

struct Node {
    string name;
    Node* parent;
    Node* sibling;
    Node** children;
    int numChildren;

    Node(string n) : name(n), parent(nullptr), sibling(nullptr), children(nullptr), numChildren(0) {}
};

void addChild(Node* parent, Node* child) {
    if (parent->numChildren == 0) {
        parent->children = new Node*[1];
    } else {
        Node** newChildren = new Node*[parent->numChildren + 1];
        for (int i = 0; i < parent->numChildren; ++i) {
            newChildren[i] = parent->children[i];
        }
        delete[] parent->children;
        parent->children = newChildren;
    }
    parent->children[parent->numChildren++] = child;
    child->parent = parent;
}

void addSibling(Node* node, Node* sibling) {
    while (node->sibling != nullptr) {
        node = node->sibling;
    }
    node->sibling = sibling;
}

Node* buildTreeFromCSV(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error al abrir el archivo." << endl;
        return nullptr;
    }

    string line;
    getline(file, line); // Leer headers

    // Validar encabezados
    stringstream headerStream(line);
    string headers[3];
    int headerIndex = 0;
    while (headerIndex < 3 && getline(headerStream, headers[headerIndex], ';')) {
        headerIndex++;
    }

    // Esperamos encabezados en el siguiente orden: name, parent, sibling
    if (headerIndex < 3 || headers[0] != "name" || headers[1] != "parent" || headers[2] != "sibling") {
        cerr << "Encabezados del archivo CSV inválidos." << endl;
        file.close();
        return nullptr;
    }

    Node* root = nullptr;
    Node** nodes = new Node*[100]; // Arreglo para almacenar nodos (ajustar tamaño según necesidad)
    int nodeCount = 0;

    while (getline(file, line)) {
        stringstream ss(line);
        string name, parentName, siblingName;
        getline(ss, name, ';');
        getline(ss, parentName, ';');
        getline(ss, siblingName, ';');

        Node* newNode = new Node(name);

        if (!parentName.empty()) {
            for (int i = 0; i < nodeCount; ++i) {
                if (nodes[i]->name == parentName) {
                    addChild(nodes[i], newNode);
                    break;
                }
            }
        }

        if (!siblingName.empty()) {
            for (int i = 0; i < nodeCount; ++i) {
                if (nodes[i]->name == siblingName) {
                    addSibling(nodes[i], newNode);
                    break;
                }
            }
        }

        if (nodeCount == 0) {
            root = newNode;
        }

        nodes[nodeCount++] = newNode;

        // Aumentar el tamaño del arreglo si es necesario
        if (nodeCount % 100 == 0) {
            Node** temp = new Node*[nodeCount + 100];
            for (int i = 0; i < nodeCount; ++i) {
                temp[i] = nodes[i];
            }
            delete[] nodes;
            nodes = temp;
        }
    }

    delete[] nodes; // Liberar memoria
    file.close();
    return root;
}

void printTree(Node* root, int depth = 0) {
    if (root) {
        for (int i = 0; i < depth; ++i) cout << "  ";
        cout << root->name << endl;
        for (int i = 0; i < root->numChildren; ++i) {
            printTree(root->children[i], depth + 1);
        }
        if (root->sibling) {
            printTree(root->sibling, depth);
        }
    }
}

void menu(Node* root) {
    int option = 0;
    while (option != 2) {
        cout << "1. Imprimir árbol" << endl;
        cout << "2. Salir" << endl;
        cout << "Seleccione una opción: ";
        cin >> option;

        switch (option) {
        case 1:
            printTree(root);
            break;
        case 2:
            cout << "Saliendo..." << endl;
            break;
        default:
            cout << "Opción inválida." << endl;
            break;
        }
    }
}

int main() {
    string filename;
    cout << "Ingrese el nombre del archivo CSV: ";
    cin >> filename;

    Node* root = buildTreeFromCSV(filename);

    if (root) {
        menu(root);
    } else {
        cerr << "No se pudo construir el árbol genealógico." << endl;
    }

    return 0;
}
