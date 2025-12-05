#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int id;
    int cola;
    int transacciones;
    int tiempo;
} Cliente;

typedef struct Nodo {
    Cliente cliente;
    struct Nodo* sig;
} Nodo;

typedef struct {
    Nodo* frente;
    Nodo* final;
    int numClientes;
    int tiempoAcumulado;
    int tiempoActual;
} Cola;

void inicializarCola(Cola* c) {
    c->frente = c->final = NULL;
    c->numClientes = 0;
    c->tiempoAcumulado = 0;
    c->tiempoActual = 0;
}

int colaVacia(Cola* c) {
    return (c->frente == NULL);
}

void encolar(Cola* c, Cliente cliente) {
    Nodo* nuevo = (Nodo*)malloc(sizeof(Nodo));
    nuevo->cliente = cliente;
    nuevo->sig = NULL;
    if (c->final == NULL) {
        c->frente = c->final = nuevo;
    } else {
        c->final->sig = nuevo;
        c->final = nuevo;
    }
}

Cliente desencolar(Cola* c) {
    Nodo* temp = c->frente;
    Cliente cliente = temp->cliente;
    c->frente = c->frente->sig;
    if (c->frente == NULL) c->final = NULL;
    free(temp);
    return cliente;
}

int main() {
    Cola colas[3];
    for (int i = 0; i < 3; i++) inicializarCola(&colas[i]);

    Cliente clientesAtendidos[500];
    int totalClientes = 0;

    int n;
    printf("Ingrese el numero de clientes: ");
    scanf("%d", &n);

    // crear y asignar clientes a las colas
    for (int idCliente = 1; idCliente <= n; idCliente++) {
        int colaAsignada = rand() % 3; // cualquiera de las 3
        Cliente c;
        c.id = idCliente;
        c.cola = colaAsignada + 1;
        c.transacciones = 1 + rand() % 10; // 1-10 transacciones
        c.tiempo = 0; // se calcula al atender
        encolar(&colas[colaAsignada], c);
    }


    for (int i = 0; i < 3; i++) {
        while (!colaVacia(&colas[i])) {
            Cliente atendido = desencolar(&colas[i]);


            atendido.tiempo = atendido.transacciones + colas[i].tiempoActual;


            colas[i].tiempoActual += atendido.transacciones;

            clientesAtendidos[totalClientes++] = atendido;
            colas[i].numClientes++;
            colas[i].tiempoAcumulado += atendido.tiempo;
        }
    }

    // ordenar clientes por id
    for (int i = 0; i < totalClientes - 1; i++) {
        for (int j = i + 1; j < totalClientes; j++) {
            if (clientesAtendidos[i].id > clientesAtendidos[j].id) {
                Cliente temp = clientesAtendidos[i];
                clientesAtendidos[i] = clientesAtendidos[j];
                clientesAtendidos[j] = temp;
            }
        }
    }

    printf("\n=== LISTA DE CLIENTES ATENDIDOS ===\n");
    for (int i = 0; i < totalClientes; i++) {
        printf("Cliente %d -> Cola %d | Transacciones: %d | Tiempo que fue atendido: %d s\n",
               clientesAtendidos[i].id, clientesAtendidos[i].cola,
               clientesAtendidos[i].transacciones, clientesAtendidos[i].transacciones);
    }

    printf("\n=== CLIENTES ATENDIDOS POR COLA ===\n");
    for (int i = 1; i <= 3; i++) {
        printf("\nCola %d:\n", i);
        for (int j = 0; j < totalClientes; j++) {
            if (clientesAtendidos[j].cola == i) {
                printf("  Cliente %d | Transacciones: %d | Tiempo que duro en la cola: %d s\n",
                       clientesAtendidos[j].id,
                       clientesAtendidos[j].transacciones,
                       clientesAtendidos[j].tiempo);
            }
        }
    }

    printf("\n=== ESTADISTICAS ===\n");
    for (int i = 0; i < 3; i++) {
        printf("Cola %d: %d clientes atendidos, tiempo promedio = %.2f s\n",
               i+1, colas[i].numClientes,
               (colas[i].numClientes > 0) ? (float)colas[i].tiempoAcumulado / colas[i].numClientes : 0);//para evitar error
    }

    return 0;
}


