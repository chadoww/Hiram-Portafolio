#include <iostream>
#include <string>
using namespace std;

struct Simbolo {
    string nombre;
    string tipo;
    string ambito; 
    Simbolo *sig;
};

class TablaSimbolos {
private:
    static const int TAM = 50;
    Simbolo* tabla[TAM];

public:
    TablaSimbolos() {
        for (int i = 0; i < TAM; i++)
            tabla[i] = 0;
    }

    int hash(string s) {
        int suma = 0;
        for (int i = 0; i < s.length(); i++)
            suma += s[i];
        return suma % TAM;
    }

    bool agregar(string nombre, string tipo, string ambito) {
        int indice = hash(nombre);

        Simbolo* actual = tabla[indice];
        int duplicados = 0;

        while (actual != 0) {
            if (actual->nombre == nombre) {
                if (actual->ambito == ambito)
                    return false; // mismo ambito: no permitido
                duplicados++;
                if (duplicados >= 2)
                    return false; // maximo dos versiones
            }
            actual = actual->sig;
        }

        // Insercion al inicio
        Simbolo* nuevo = new Simbolo();
        nuevo->nombre = nombre;
        nuevo->tipo = tipo;
        nuevo->ambito = ambito;
        nuevo->sig = tabla[indice];
        tabla[indice] = nuevo;

        return true;
    }

    Simbolo* buscar(string nombre, string ambito) {
        int indice = hash(nombre);
        Simbolo* actual = tabla[indice];

        while (actual != 0) {
            if (actual->nombre == nombre &&
               (actual->ambito == ambito || actual->ambito == "global"))
                return actual;

            actual = actual->sig;
        }
        return 0;
    }

    bool eliminar(string nombre, string ambito) {
        int indice = hash(nombre);
        Simbolo* actual = tabla[indice];
        Simbolo* anterior = 0;

        while (actual != 0) {
            if (actual->nombre == nombre && actual->ambito == ambito) {
                if (anterior == 0)
                    tabla[indice] = actual->sig;
                else
                    anterior->sig = actual->sig;

                delete actual;
                return true;
            }
            anterior = actual;
            actual = actual->sig;
        }
        return false;
    }
};

int main() {
    TablaSimbolos ts;
    int opcion;

    do {
        cout << "\n===== TABLA DE SIMBOLOS =====\n";
        cout << "1. Agregar identificador\n";
        cout << "2. Buscar identificador\n";
        cout << "3. Eliminar identificador\n";
        cout << "4. Salir\n";
        cout << "Seleccione una opcion: ";
        cin >> opcion;

        if (opcion == 1) {
            string nombre, tipo, ambito;
            cin.ignore();

            cout << "Nombre del identificador: ";
            getline(cin, nombre);

            cout << "Tipo: ";
            getline(cin, tipo);

            cout << "Ambito (global/local): ";
            getline(cin, ambito);

            if (ts.agregar(nombre, tipo, ambito))
                cout << "Identificador agregado correctamente.\n";
            else
                cout << "Error: Identificador duplicado o no permitido.\n";

        } else if (opcion == 2) {
            string nombre, ambito;
            cin.ignore();

            cout << "Nombre del identificador a buscar: ";
            getline(cin, nombre);

            cout << "Ambito desde donde se busca: ";
            getline(cin, ambito);

            Simbolo* s = ts.buscar(nombre, ambito);

            if (s != 0) {
                cout << "\n--- IDENTIFICADOR ENCONTRADO ---\n";
                cout << "Nombre: " << s->nombre << endl;
                cout << "Tipo: " << s->tipo << endl;
                cout << "Ambito: " << s->ambito << endl;
            } else {
                cout << "No se encontro el identificador.\n";
            }

        } else if (opcion == 3) {
            string nombre, ambito;
            cin.ignore();

            cout << "Nombre del identificador a eliminar: ";
            getline(cin, nombre);

            cout << "Ambito del identificador: ";
            getline(cin, ambito);

            if (ts.eliminar(nombre, ambito))
                cout << "Identificador eliminado correctamente.\n";
            else
                cout << "No se pudo eliminar: no existe.\n";
        }

    } while (opcion != 4);

    cout << "Saliendo...\n";
    return 0;
}
