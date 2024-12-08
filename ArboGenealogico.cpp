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

// Variable global para el arbol

string filename="arbolGenealogico.csv";

// Prototipo de funciones

void saveTreeToCSV(Node* root, const string& filename);
void countNodes(Node* node, int& count);
void collectNodes(Node* node, Node** nodes, int& index);

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

// Función para añadir un hermano a un nodo
void addSibling(Node* node, Node* sibling) {
    while (node->sibling != nullptr) {
        node = node->sibling;
    }
    node->sibling = sibling; // Añadir el nuevo hermano al final de la lista de hermanos
    sibling->sibling = nullptr; // Asegurar que el nuevo hermano no tenga un siguiente hermano
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

void modifyFamilyMember(Node* root, int id, const string& newName) {
    Node* node = findNodeById(root, id);
    if (node) {
        node->name = newName;

        // Guardar el árbol actualizado en el archivo CSV
        saveTreeToCSV(root, filename);
        
        cout << "Familiar modificado exitosamente." << endl;
    } else {
        cout << "Familiar con el ID proporcionado no encontrado." << endl;
    }
}

// Funcion para agregar un nuevo familiar

void addFamilyMember(Node*& root, int id, const string& name, const string& parentName, const string& siblingName) {
    cout << "Agregando familiar con ID: " << id << ", Nombre: " << name << endl;
    if (findNodeById(root, id)) {
        cout << "ID ya está en uso. Por favor, elija otro ID." << endl;
        return;
    }

    Node* parent = nullptr;
    Node* sibling = nullptr;

    // Validar que el padre exista si se proporciona un nombre de padre
    if (!parentName.empty()) {
        parent = findNodeByName(root, parentName);
        if (!parent) {
            cout << "Error: El padre especificado no existe." << endl;
            return;
        }
    }

    // Validar que el hermano exista si se proporciona un nombre de hermano
    if (!siblingName.empty()) {
        sibling = findNodeByName(root, siblingName);
        if (!sibling) {
            cout << "Error: El hermano especificado no existe." << endl;
            return;
        }

        // Validar que el hermano tenga el mismo padre especificado, si se proporciona un nombre de padre
        if (parent && sibling->parent != parent) {
            cout << "Error: El hermano especificado no tiene el mismo padre que el proporcionado." << endl;
            return;
        }
    }

    Node* newNode = new Node(id, name);

    // Añadir hijo
    if (parent) {
        cout << "Añadiendo como hijo de: " << parent->name << endl;
        addChild(parent, newNode);
    }

    // Añadir hermano
    if (sibling) {
        cout << "Añadiendo como hermano de: " << sibling->name << endl;
        addSibling(sibling, newNode);
    }

    // Establecer la raíz si no hay padre ni hermano
    if (!parent && !sibling) {
        cout << "Estableciendo raíz: " << name << endl;
        root = newNode;
    }

    // Verificar la estructura del árbol antes de guardar
    cout << "Estructura del árbol antes de guardar:" << endl;
    resetPrintedFlags(root); // Reseteamos las banderas antes de imprimir
    printTree(root);

    // Guardar el árbol actualizado en el archivo CSV
    saveTreeToCSV(root, filename);

    cout << "Familiar agregado exitosamente." << endl;
}

// Función auxiliar para escribir nodo en el archivo

void writeNode(ofstream& file, Node* node) {
    if (node) {
        string parentName = (node->parent) ? node->parent->name : "";
        string siblingName = (node->sibling) ? node->sibling->name : "";
        file << node->id << ";" << node->name << ";" << parentName << ";" << siblingName << "\n";

        for (int i = 0; i < node->numChildren; ++i) {
            writeNode(file, node->children[i]);
        }
    }
}

// Función para guardar el árbol en un archivo CSV ordenado por ID
void saveTreeToCSV(Node* root, const string& filename) {
    cout << "Guardando árbol en archivo CSV..." << endl;
    ofstream file(filename);
    if (!file.is_open()) {
        cerr << "Error al abrir el archivo." << endl;
        return;
    }

    // Escribir encabezados
    file << "id;name;parent;sibling\n";

    // Resetear banderas antes de contar nodos y recopilarlos
    resetPrintedFlags(root);

    // Contar todos los nodos
    int nodeCount = 0;
    countNodes(root, nodeCount);
    cout << "Número de nodos en el árbol: " << nodeCount << endl;

    // Recopilar todos los nodos en un arreglo dinámico
    Node** nodes = new Node*[nodeCount];
    int index = 0;
    resetPrintedFlags(root);  // Resetear banderas antes de recopilar
    collectNodes(root, nodes, index);

    // Ordenar los nodos por ID usando bubble sort
    for (int i = 0; i < nodeCount - 1; ++i) {
        for (int j = 0; j < nodeCount - i - 1; ++j) {
            if (nodes[j]->id > nodes[j + 1]->id) {
                Node* temp = nodes[j];
                nodes[j] = nodes[j + 1];
                nodes[j + 1] = temp;
            }
        }
    }

    // Escribir los nodos en el archivo
    for (int i = 0; i < nodeCount; ++i) {
        Node* node = nodes[i];
        string parentName = (node->parent) ? node->parent->name : "";
        string siblingName = (node->sibling) ? node->sibling->name : "";
        file << node->id << ";" << node->name << ";" << parentName << ";" << siblingName << "\n";
        cout << "Escribiendo nodo - ID: " << node->id << ", Nombre: " << node->name << ", Hermano: " << siblingName << endl;
    }

    // Liberar memoria
    delete[] nodes;

    file.close();
    cout << "Árbol guardado exitosamente." << endl;
}

// Función auxiliar para contar todos los nodos

void countNodes(Node* node, int& count) {
    if (node && !node->printed) {
        count++;
        node->printed = true;
        for (int i = 0; i < node->numChildren; ++i) {
            countNodes(node->children[i], count);
        }
    }
    if (node->sibling) {
        countNodes(node->sibling, count);
    }
}

// Función auxiliar para recopilar nodos en un arreglo dinámico sin duplicados
void collectNodes(Node* node, Node** nodes, int& index) {
    if (node && !node->printed) {
        nodes[index++] = node;
        node->printed = true;
        for (int i = 0; i < node->numChildren; ++i) {
            collectNodes(node->children[i], nodes, index);
        }
    }
    if (node->sibling) {
        collectNodes(node->sibling, nodes, index);
    }
}

// Funcion para mostrar el menu

void menu(Node* root) {
    int option = 0;
    while (option != 4) {
        cout << "1. Imprimir árbol" << endl;
        cout << "2. Agregar familiar" << endl;
        cout << "3. Modificar nombre del familiar" << endl; // Cambiar el texto para reflejar que solo se puede modificar el nombre
        cout << "4. Salir" << endl;
        cout << "Seleccione una opción: ";
        cin >> option;

        switch (option) {
        case 1:
            resetPrintedFlags(root); // Se resetea las banderas antes de imprimir
            printTree(root);
            break;
        case 2: {
            int id;
            string name, parentName, siblingName;
            cout << "Ingrese el ID del nuevo familiar: ";
            cin >> id;
            cout << "Ingrese el nombre del nuevo familiar: ";
            cin >> name;
            cout << "Ingrese el nombre del padre: ";
            cin.ignore(); // Para evitar problemas al leer nombres con espacios
            getline(cin, parentName);
            cout << "Ingrese el nombre del hermano (dejar en blanco si es hijo unico): ";
            getline(cin, siblingName);
            addFamilyMember(root, id, name, parentName, siblingName);
            break;
        }
        case 3: {
            int id;
            string newName;
            cout << "Ingrese el ID del familiar a modificar: ";
            cin >> id;
            cout << "Ingrese el nuevo nombre del familiar: ";
            cin.ignore(); // Para evitar problemas al leer nombres con espacios
            getline(cin, newName);
            modifyFamilyMember(root, id, newName); // Solo modificamos el nombre, no el ID
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
    
    Node* root = buildTreeFromCSV(filename);

    if (root) {
        menu(root);
    } else {
        cerr << "No se pudo construir el árbol genealógico." << endl;
    }

    return 0;
}