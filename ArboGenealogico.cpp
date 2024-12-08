#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <windows.h>

using namespace std;

// Clase para nodo (familiar) del arbol

struct Node {
    int id;
    string name;
    Node* parent;
    Node* sibling;
    Node** children;
    int numChildren;
    bool printed;

    Node(int id, string n) : id(id), name(n), parent(nullptr), sibling(nullptr), children(nullptr), numChildren(0), printed(false) {}
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
    string headers[4];
    int headerIndex = 0;
    while (headerIndex < 4 && getline(headerStream, headers[headerIndex], ';')) {
        headerIndex++;
    }

    // Orden del encabezado: id, name, parent, sibling
    if (headerIndex < 4 || headers[0] != "id" || headers[1] != "name" || headers[2] != "parent" || headers[3] != "sibling") {
        cerr << "Encabezados del archivo CSV inválidos." << endl;
        file.close();
        return nullptr;
    }

    Node* root = nullptr;
    Node** nodes = new Node*[100]; // Arreglo para almacenar nodos (ajustar tamaño según necesidad)
    int nodeCount = 0;

    while (getline(file, line)) {
        stringstream ss(line);
        string idStr, name, parentName, siblingName;
        getline(ss, idStr, ';');
        getline(ss, name, ';');
        getline(ss, parentName, ';');
        getline(ss, siblingName, ';');

        int id = stoi(idStr);
        Node* newNode = new Node(id, name);

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
        // Mostrar el nombre y el ID del nodo
        cout << root->name << " (ID: " << root->id << ")" << endl;

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

Node* findNodeById(Node* root, int id) {
    if (!root) return nullptr;
    if (root->id == id) return root;
    Node* found = nullptr;
    for (int i = 0; i < root->numChildren && !found; ++i) {
        found = findNodeById(root->children[i], id);
    }
    if (!found && root->sibling) {
        found = findNodeById(root->sibling, id);
    }
    return found;
}

// Funcion para encontrar un nodo por su nombre

Node* findNodeByName(Node* root, const string& name) {
    if (!root) return nullptr;
    if (root->name == name) return root;
    Node* found = nullptr;
    for (int i = 0; i < root->numChildren && !found; ++i) {
        found = findNodeByName(root->children[i], name);
    }
    if (!found && root->sibling) {
        found = findNodeByName(root->sibling, name);
    }
    return found;
}

// Funcion para modificar un nodo del arbol

void modifyFamilyMember(Node* root, int oldId, int newId, const string& newName) {
    Node* node = findNodeById(root, oldId);
    if (node) {
        // Verificar si el nuevo ID ya está en uso
        if (newId != oldId && findNodeById(root, newId)) {
            cout << "ID ya está en uso. Por favor, elija otro ID." << endl;
            return;
        }
        // Modificar el nodo con el nuevo ID y nombre
        node->id = newId;
        node->name = newName;
    } else {
        cout << "Familiar con el ID proporcionado no encontrado." << endl;
    }
}

// Funcion para agregar el nodo

void addFamilyMember(Node* root, int id, const string& name, const string& parentName, const string& siblingName) {
    if (findNodeById(root, id)) {
        cout << "ID ya está en uso. Por favor, elija otro ID." << endl;
        return;
    }

    Node* newNode = new Node(id, name);

    // Añadir hijo
    if (!parentName.empty()) {
        Node* parent = findNodeByName(root, parentName);
        if (parent) {
            addChild(parent, newNode);
        }
    }

    // Añadir hermano
    if (!siblingName.empty()) {
        Node* sibling = findNodeByName(root, siblingName);
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
            int id;
            string name, parentName, siblingName;
            cout << "Ingrese el ID del nuevo familiar: ";
            cin >> id;
            cout << "Ingrese el nombre del nuevo familiar: ";
            cin >> name;
            cout << "Ingrese el nombre del padre (dejar en blanco si no aplica): ";
            cin.ignore(); // Para evitar problemas al leer nombres con espacios
            getline(cin, parentName);
            cout << "Ingrese el nombre del hermano (dejar en blanco si no aplica): ";
            getline(cin, siblingName);
            addFamilyMember(root, id, name, parentName, siblingName);
            break;
        }
        case 3: {
            int oldId, newId;
            string newName;
            cout << "Ingrese el ID actual del familiar: ";
            cin >> oldId;
            cout << "Ingrese el nuevo ID del familiar: ";
            cin >> newId;
            cout << "Ingrese el nuevo nombre del familiar: ";
            cin.ignore(); // Para evitar problemas al leer nombres con espacios
            getline(cin, newName);
            modifyFamilyMember(root, oldId, newId, newName);
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