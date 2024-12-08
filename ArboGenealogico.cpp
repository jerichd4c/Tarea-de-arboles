#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <windows.h>

using namespace std;

// Clase para nodo (familiar) del arbol

struct Node {
    string name;
    Node* parent;
    Node* sibling;
    Node** children;
    int numChildren;
    bool printed;

    Node(string n) : name(n), parent(nullptr), sibling(nullptr), children(nullptr), numChildren(0), printed(false) {}
};

// Funcion para añadir un hijo a un nodo

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

// Funcion para añadir un hermano a un nodo

void addSibling(Node* node, Node* sibling) {
    while (node->sibling != nullptr) {
        node = node->sibling;
    }
    node->sibling = sibling; // Añadir el nuevo hermano al final de la lista de hermanos
}

// Funcion para agregar datos al .CSV

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

    // Orden del encabezado: name, parent, sibling
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

        // Añadir hijo
        if (!parentName.empty()) {
            for (int i = 0; i < nodeCount; ++i) {
                if (nodes[i]->name == parentName) {
                    addChild(nodes[i], newNode);
                    break;
                }
            }
        }

        // Añadir hermano
        if (!siblingName.empty()) {
            for (int i = 0; i < nodeCount; ++i) {
                if (nodes[i]->name == siblingName) {
                    addSibling(nodes[i], newNode);
                    break;
                }
            }
        }

        // Establecer la raíz
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

// Funcion para asegurarse de que en el arbol no hayan nodos repetidos

void resetPrintedFlags(Node* node) {
    if (node) {
        node->printed = false;
        for (int i = 0; i < node->numChildren; ++i) {
            resetPrintedFlags(node->children[i]);
        }
        if (node->sibling) {
            resetPrintedFlags(node->sibling);
        }
    }
}

// Funcion para imprimir el arbol

void printTree(Node* root, int depth = 0, bool isSibling = false, string prefix = "") {
    if (root && !root->printed) {
        root->printed = true; // Marcar el nodo como impreso
        cout << prefix;
        if (isSibling) {
            cout << "|-- ";
        } else {
            cout << "+-- ";
        }
        cout << root->name << endl;

        string childPrefix = prefix + (isSibling ? "|   " : "    ");
        // Imprimir todos los hijos primero
        for (int i = 0; i < root->numChildren; ++i) {
            printTree(root->children[i], depth + 1, false, childPrefix);
        }

        // Luego imprimir los hermanos
        if (root->sibling) {
            printTree(root->sibling, depth, true, prefix);
        }
    }
}

// Funcion para encontrar un nodo y posteriormente agregarlo

Node* findNode(Node* root, const string& name) {
    if (!root) return nullptr;
    if (root->name == name) return root;
    Node* found = nullptr;
    for (int i = 0; i < root->numChildren && !found; ++i) {
        found = findNode(root->children[i], name);
    }
    if (!found && root->sibling) {
        found = findNode(root->sibling, name);
    }
    return found;
}

// Funcion para modificar un nodo del arbol

void modifyFamilyMember(Node* root, const string& oldName, const string& newName) {
    Node* node = findNode(root, oldName);
    if (node) {
        node->name = newName;
    }
}

// Funcion para agregar el nodo

void addFamilyMember(Node* root, const string& name, const string& parentName, const string& siblingName) {
    Node* newNode = new Node(name);

    // Añadir hijo
    if (!parentName.empty()) {
        Node* parent = findNode(root, parentName);
        if (parent) {
            addChild(parent, newNode);
        }
    }

    // Añadir hermano
    if (!siblingName.empty()) {
        Node* sibling = findNode(root, siblingName);
        if (sibling) {
            addSibling(sibling, newNode);
        }
    }

    if (parentName.empty() && siblingName.empty()) {
        // Si el nodo no tiene padres ni hermanos, es una nueva raíz
        root = newNode;
    }
}

// Funcion para mostrar el menu

void menu(Node* root) {
    int option = 0;
    while (option != 4) {
        cout << "1. Imprimir árbol" << endl;
        cout << "2. Agregar familiar" << endl;
        cout << "3. Modificar familiar" << endl;
        cout << "4. Salir" << endl;
        cout << "Seleccione una opción: ";
        cin >> option;

        switch (option) {
        case 1:
            resetPrintedFlags(root); // Reseteamos las banderas antes de imprimir
            printTree(root);
            break;
        case 2: {
            string name, parentName, siblingName;
            cout << "Ingrese el nombre del nuevo familiar: ";
            cin >> name;
            cout << "Ingrese el nombre del padre (dejar en blanco si no aplica): ";
            cin >> parentName;
            cout << "Ingrese el nombre del hermano (dejar en blanco si no aplica): ";
            cin >> siblingName;
            addFamilyMember(root, name, parentName, siblingName);
            break;
        }
        case 3: {
            string oldName, newName;
            cout << "Ingrese el nombre actual del familiar: ";
            cin >> oldName;
            cout << "Ingrese el nuevo nombre del familiar: ";
            cin >> newName;
            modifyFamilyMember(root, oldName, newName);
            break;
        }
        case 4:
            cout << "Saliendo..." << endl;
            break;
        default:
            cout << "Opción inválida." << endl;
            break;
        }
    }
}

// Implementacion en el main

int main() {

    // Configurar la consola para usar UTF-8
    SetConsoleOutputCP(CP_UTF8); // Establece la salida en UTF-8, caracteres especiales en español
    SetConsoleCP(CP_UTF8);       // Establece la entrada en UTF-8, caracteres especiales en español
    
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