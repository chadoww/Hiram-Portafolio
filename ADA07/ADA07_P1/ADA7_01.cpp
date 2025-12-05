#include <iostream>
#include <string>
using namespace std;

struct Estudiante {
    int matricula;
    string nombre;
    string carrera;
    bool activo;
    bool borrado;
};

class HashTable {
private:
    static const int TAM = 100;
    Estudiante tabla[TAM];

public:
    HashTable() {
        for (int i = 0; i < TAM; i++) {
            tabla[i].activo = false;
            tabla[i].borrado = false;
        }
    }

    int hash(int key) {
        return key % TAM;
    }

    bool agregar(int matricula, string nombre, string carrera) {
        int indice = hash(matricula);
        int i = 0;

        while (i < TAM) {
            int pos = (indice + i*i) % TAM;

            if (!tabla[pos].activo || tabla[pos].borrado) {
                tabla[pos].matricula = matricula;
                tabla[pos].nombre = nombre;
                tabla[pos].carrera = carrera;
                tabla[pos].activo = true;
                tabla[pos].borrado = false;
                return true;
            }
            i++;
        }
        return false; 
    }

    Estudiante* buscar(int matricula) {
        int indice = hash(matricula);
        int i = 0;

        while (i < TAM) {
            int pos = (indice + i*i) % TAM;

            if (tabla[pos].activo && tabla[pos].matricula == matricula)
                return &tabla[pos];

            if (!tabla[pos].activo && !tabla[pos].borrado)
                return 0;

            i++;
        }
        return 0;
    }

    bool eliminar(int matricula) {
        Estudiante* est = buscar(matricula);
        if (est != 0) {
            est->activo = false;
            est->borrado = true;
            return true;
        }
        return false;
    }
};

int main() {
    HashTable h;
    int opcion;

    do {
        cout << "\n===== MENU =====\n";
        cout << "1. Agregar estudiante\n";
        cout << "2. Buscar estudiante\n";
        cout << "3. Eliminar estudiante\n";
        cout << "4. Salir\n";
        cout << "Seleccione una opcion: ";
        cin >> opcion;

        if (opcion == 1) {
            int matricula;
            string nombre, carrera;

            cout << "Ingrese matricula: ";
            cin >> matricula;
            cin.ignore();

            cout << "Ingrese nombre: ";
            getline(cin, nombre);

            cout << "Ingrese carrera: ";
            getline(cin, carrera);

            if (h.agregar(matricula, nombre, carrera))
                cout << "Estudiante agregado correctamente.\n";
            else
                cout << "Error: Tabla llena. No se pudo insertar.\n";

        } else if (opcion == 2) {
            int matricula;
            cout << "Ingrese matricula a buscar: ";
            cin >> matricula;

            Estudiante* e = h.buscar(matricula);
            if (e != 0) {
                cout << "\n--- ESTUDIANTE ENCONTRADO ---\n";
                cout << "Matricula: " << e->matricula << endl;
                cout << "Nombre: " << e->nombre << endl;
                cout << "Carrera: " << e->carrera << endl;
            } else {
                cout << "No existe estudiante con esa matricula.\n";
            }

        } else if (opcion == 3) {
            int matricula;
            cout << "Ingrese matricula a eliminar: ";
            cin >> matricula;

            if (h.eliminar(matricula))
                cout << "Estudiante eliminado correctamente.\n";
            else
                cout << "No se encontro la matricula para eliminar.\n";
        }

    } while (opcion != 4);

    cout << "Programa terminado\n";
    return 0;
}
