#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NAME_LEN 64
#define SURNAME_LEN 64

typedef struct {
    int matricula;
    char nombre[NAME_LEN];
    char apellido[SURNAME_LEN];
    float *calificaciones;
    int n_calificaciones;
    float promedio;
} Estudiante;

typedef struct Nodo {
    Estudiante data;
    struct Nodo *izq;
    struct Nodo *der;
} Nodo;

float calcular_promedio(float *arr, int n) {
    if (n <= 0) return 0.0f;
    float s = 0.0f;
    for (int i = 0; i < n; ++i) s += arr[i];
    return s / n;
}

void imprimir_estudiante(const Estudiante *e) {
    printf("Matricula: %d | %s %s | Promedio: %.2f | N califs: %d\n",
           e->matricula, e->nombre, e->apellido, e->promedio, e->n_calificaciones);
}

Nodo* crear_nodo(const Estudiante *est) {
    Nodo *n = (Nodo*) malloc(sizeof(Nodo));
    if (!n) {
        perror("malloc nodo");
        exit(EXIT_FAILURE);
    }
    n->data.matricula = est->matricula;
    strncpy(n->data.nombre, est->nombre, NAME_LEN-1);
    n->data.nombre[NAME_LEN-1] = '\0';
    strncpy(n->data.apellido, est->apellido, SURNAME_LEN-1);
    n->data.apellido[SURNAME_LEN-1] = '\0';
    n->data.n_calificaciones = est->n_calificaciones;
    if (est->n_calificaciones > 0) {
        n->data.calificaciones = (float*) malloc(sizeof(float) * est->n_calificaciones);
        if (!n->data.calificaciones) { perror("malloc califs"); exit(EXIT_FAILURE); }
        for (int i = 0; i < est->n_calificaciones; ++i) n->data.calificaciones[i] = est->calificaciones[i];
    } else {
        n->data.calificaciones = NULL;
    }
    n->data.promedio = est->promedio;
    n->izq = n->der = NULL;
    return n;
}

void liberar_estudiante_data(Estudiante *e) {
    if (e->calificaciones) free(e->calificaciones);
    e->calificaciones = NULL;
    e->n_calificaciones = 0;
}

Nodo* insertar(Nodo *root, const Estudiante *est) {
    if (!root) return crear_nodo(est);
    if (est->matricula < root->data.matricula) {
        root->izq = insertar(root->izq, est);
    } else if (est->matricula > root->data.matricula) {
        root->der = insertar(root->der, est);
    } else {
        printf("La matricula %d ya existe. No se inserto.\n", est->matricula);
    }
    return root;
}

Nodo* buscar(Nodo *root, int matricula) {
    if (!root) return NULL;
    if (matricula == root->data.matricula) return root;
    if (matricula < root->data.matricula) return buscar(root->izq, matricula);
    return buscar(root->der, matricula);
}

Nodo* min_nodo(Nodo *root) {
    if (!root) return NULL;
    while (root->izq) root = root->izq;
    return root;
}

Nodo* eliminar(Nodo *root, int matricula) {
    if (!root) return NULL;
    if (matricula < root->data.matricula) {
        root->izq = eliminar(root->izq, matricula);
    } else if (matricula > root->data.matricula) {
        root->der = eliminar(root->der, matricula);
    } else {
        if (!root->izq && !root->der) {
            liberar_estudiante_data(&root->data);
            free(root);
            return NULL;
        } else if (!root->izq) {
            Nodo *r = root->der;
            liberar_estudiante_data(&root->data);
            free(root);
            return r;
        } else if (!root->der) {
            Nodo *l = root->izq;
            liberar_estudiante_data(&root->data);
            free(root);
            return l;
        } else {
            Nodo *sucesor = min_nodo(root->der);
            liberar_estudiante_data(&root->data);
            root->data.matricula = sucesor->data.matricula;
            strncpy(root->data.nombre, sucesor->data.nombre, NAME_LEN);
            strncpy(root->data.apellido, sucesor->data.apellido, SURNAME_LEN);
            root->data.n_calificaciones = sucesor->data.n_calificaciones;
            if (sucesor->data.n_calificaciones > 0) {
                root->data.calificaciones = (float*) malloc(sizeof(float) * sucesor->data.n_calificaciones);
                for (int i = 0; i < sucesor->data.n_calificaciones; ++i)
                    root->data.calificaciones[i] = sucesor->data.calificaciones[i];
            } else {
                root->data.calificaciones = NULL;
            }
            root->data.promedio = sucesor->data.promedio;
            root->der = eliminar(root->der, sucesor->data.matricula);
        }
    }
    return root;
}

void inorder_imprimir(Nodo *root) {
    if (!root) return;
    inorder_imprimir(root->izq);
    imprimir_estudiante(&root->data);
    inorder_imprimir(root->der);
}

typedef struct {
    Estudiante *arr;
    int n;
    int cap;
} VecEst;

void vec_init(VecEst *v) {
    v->n = 0; v->cap = 8;
    v->arr = (Estudiante*) malloc(sizeof(Estudiante) * v->cap);
}

void vec_push(VecEst *v, const Estudiante *e) {
    if (v->n >= v->cap) {
        v->cap *= 2;
        v->arr = (Estudiante*) realloc(v->arr, sizeof(Estudiante) * v->cap);
    }
    v->arr[v->n] = *e;
    v->n++;
}

void vec_free(VecEst *v) {
    free(v->arr);
    v->arr = NULL;
    v->n = v->cap = 0;
}

void colectar_inorder(Nodo *root, VecEst *v) {
    if (!root) return;
    colectar_inorder(root->izq, v);
    vec_push(v, &root->data);
    colectar_inorder(root->der, v);
}

int cmp_promedio_desc(const void *a, const void *b) {
    const Estudiante *ea = (const Estudiante*) a;
    const Estudiante *eb = (const Estudiante*) b;
    if (ea->promedio < eb->promedio) return 1;
    if (ea->promedio > eb->promedio) return -1;
    return 0;
}

void reporte_orden_promedio(Nodo *root) {
    VecEst v;
    vec_init(&v);
    colectar_inorder(root, &v);
    if (v.n == 0) {
        printf("No hay estudiantes.\n");
        vec_free(&v);
        return;
    }
    qsort(v.arr, v.n, sizeof(Estudiante), cmp_promedio_desc);
    printf("Reporte por promedio (descendente):\n");
    for (int i = 0; i < v.n; ++i) {
        imprimir_estudiante(&v.arr[i]);
    }
    vec_free(&v);
}

void reporte_por_rango_promedio(Nodo *root, float umbral, int menor_o_igual) {
    if (!root) { printf("No hay estudiantes.\n"); return; }
    if (root->izq) reporte_por_rango_promedio(root->izq, umbral, menor_o_igual);
    if (menor_o_igual) {
        if (root->data.promedio < umbral) imprimir_estudiante(&root->data);
    } else {
        if (root->data.promedio >= umbral) imprimir_estudiante(&root->data);
    }
    if (root->der) reporte_por_rango_promedio(root->der, umbral, menor_o_igual);
}

void actualizar_calificacion(Nodo *root, int matricula) {
    Nodo *n = buscar(root, matricula);
    if (!n) {
        printf("No existe estudiante con matricula %d\n", matricula);
        return;
    }
    Estudiante *e = &n->data;
    if (e->n_calificaciones <= 0) {
        printf("El estudiante no tiene calificaciones registradas.\n");
        return;
    }
    printf("Estudiante encontrado: ");
    imprimir_estudiante(e);
    printf("Calificaciones actuales:\n");
    for (int i = 0; i < e->n_calificaciones; ++i) {
        printf("  [%d] %.2f\n", i, e->calificaciones[i]);
    }
    int idx;
    printf("Ingrese el indice de la calificacion a actualizar (0..%d): ", e->n_calificaciones - 1);
    if (scanf("%d", &idx) != 1) { while (getchar()!='\n'); printf("Entrada invalida.\n"); return; }
    if (idx < 0 || idx >= e->n_calificaciones) {
        printf("Indice fuera de rango.\n");
        return;
    }
    float nuevo;
    printf("Ingrese la nueva calificacion: ");
    if (scanf("%f", &nuevo) != 1) { while (getchar()!='\n'); printf("Entrada invalida.\n"); return; }
    e->calificaciones[idx] = nuevo;
    e->promedio = calcular_promedio(e->calificaciones, e->n_calificaciones);
    printf("Actualizacion completada. Nuevo promedio: %.2f\n", e->promedio);
}

void contar_menor_promedio_rec(Nodo *root, float promedio_ref, int *cont) {
    if (!root) return;
    if (root->data.promedio < promedio_ref) (*cont)++;
    contar_menor_promedio_rec(root->izq, promedio_ref, cont);
    contar_menor_promedio_rec(root->der, promedio_ref, cont);
}

void contar_menor_promedio(Nodo *root, int matricula) {
    Nodo *n = buscar(root, matricula);
    if (!n) {
        printf("No existe estudiante con matricula %d\n", matricula);
        return;
    }
    float ref = n->data.promedio;
    int cont = 0;
    contar_menor_promedio_rec(root, ref, &cont);
    printf("Promedio de referencia (matr %d) = %.2f\n", matricula, ref);
    printf("Numero de estudiantes con promedio menor que %.2f: %d\n", ref, cont);
}

void imprimir_nivel_rec(Nodo *root, int nivel_actual, int nivel_objetivo) {
    if (!root) return;
    if (nivel_actual == nivel_objetivo) {
        imprimir_estudiante(&root->data);
    } else {
        imprimir_nivel_rec(root->izq, nivel_actual + 1, nivel_objetivo);
        imprimir_nivel_rec(root->der, nivel_actual + 1, nivel_objetivo);
    }
}

void mostrar_nivel(Nodo *root, int nivel) {
    if (!root) { printf("Arbol vacio.\n"); return; }
    if (nivel < 0) { printf("Nivel invalido.\n"); return; }
    printf("Nodos en el nivel %d:\n", nivel);
    imprimir_nivel_rec(root, 0, nivel);
}

void imprimir_ascii_rec(Nodo *root, int espacio) {
    if (!root) return;
    const int INCR = 6;
    espacio += INCR;
    imprimir_ascii_rec(root->der, espacio);
    printf("\n");
    for (int i = INCR; i < espacio; ++i) putchar(' ');
    printf("%d(%.2f)\n", root->data.matricula, root->data.promedio);
    imprimir_ascii_rec(root->izq, espacio);
}

void imprimir_ascii(Nodo *root) {
    if (!root) { printf("Arbol vacio.\n"); return; }
    printf("Arbol (matricula(promedio)):\n");
    imprimir_ascii_rec(root, 0);
}

void leer_cadena_linea(char *buf, int cap) {
    int c;
    while ((c = getchar()) == '\n');
    if (c == EOF) { buf[0]='\0'; return; }
    buf[0] = (char)c;
    int i = 1;
    while (i < cap-1 && (c = getchar()) != EOF && c != '\n') {
        buf[i++] = (char)c;
    }
    buf[i] = '\0';
}

Estudiante crear_estudiante_por_input() {
    Estudiante e;
    e.calificaciones = NULL;
    e.n_calificaciones = 0;
    e.promedio = 0.0f;

    printf("Ingrese matricula (entero): ");
    while (scanf("%d", &e.matricula) != 1) {
        while (getchar()!='\n');
        printf("Entrada invalida. Ingrese matricula: ");
    }
    while (getchar()!='\n');

    printf("Ingrese nombre: ");
    leer_cadena_linea(e.nombre, NAME_LEN);

    printf("Ingrese apellido: ");
    leer_cadena_linea(e.apellido, SURNAME_LEN);

    printf("Cuantas calificaciones tiene este alumno? ");
    int n;
    while (scanf("%d", &n) != 1 || n < 0) {
        while (getchar()!='\n');
        printf("Entrada invalida. Introduzca un numero >= 0: ");
    }
    e.n_calificaciones = n;
    if (n > 0) {
        e.calificaciones = (float*) malloc(sizeof(float) * n);
        for (int i = 0; i < n; ++i) {
            float val;
            printf("  Calificacion [%d]: ", i);
            while (scanf("%f", &val) != 1) {
                while (getchar()!='\n');
                printf("Entrada invalida. Ingrese un numero: ");
            }
            e.calificaciones[i] = val;
        }
        e.promedio = calcular_promedio(e.calificaciones, n);
    } else {
        e.calificaciones = NULL;
        e.promedio = 0.0f;
    }
    while (getchar()!='\n');
    return e;
}

void liberar_arbol(Nodo *root) {
    if (!root) return;
    liberar_arbol(root->izq);
    liberar_arbol(root->der);
    liberar_estudiante_data(&root->data);
    free(root);
}

void mostrar_menu() {
    printf("\n--- MENU ADA04 - Gestion de estudiantes (ABB) ---\n");
    printf("1) Insertar nuevo estudiante\n");
    printf("2) Eliminar estudiante por matricula\n");
    printf("3) Actualizar UNA calificacion (por matricula)\n");
    printf("4) Reporte: todos los estudiantes (ordenados por matricula)\n");
    printf("5) Reporte: todos los estudiantes (ordenados por promedio desc.)\n");
    printf("6) Reporte: estudiantes con promedio < 70\n");
    printf("7) Reporte: estudiantes con promedio >= 70\n");
    printf("8) Dada una matricula, contar estudiantes con promedio menor que el suyo\n");
    printf("9) Mostrar nodos de un nivel dado\n");
    printf("10) Mostrar Arbol ASCII\n");
    printf("0) Salir\n");
    printf("-----------------------------------------------\n");
    printf("Elija una opcion: ");
}

int main() {
    Nodo *root = NULL;
    int opcion;
    do {
        mostrar_menu();
        if (scanf("%d", &opcion) != 1) {
            while (getchar()!='\n');
            printf("Entrada invalida.\n");
            continue;
        }
        switch (opcion) {
            case 1: {
                Estudiante e = crear_estudiante_por_input();
                root = insertar(root, &e);
                liberar_estudiante_data(&e);
                printf("Insercion completada.\n");
                break;
            }
            case 2: {
                int m;
                printf("Ingrese matricula a eliminar: ");
                if (scanf("%d", &m) != 1) { while (getchar()!='\n'); printf("Entrada invalida.\n"); break; }
                root = eliminar(root, m);
                printf("Si existia, la matricula %d fue eliminada.\n", m);
                break;
            }
            case 3: {
                int m;
                printf("Ingrese matricula a actualizar: ");
                if (scanf("%d", &m) != 1) { while (getchar()!='\n'); printf("Entrada invalida.\n"); break; }
                actualizar_calificacion(root, m);
                break;
            }
            case 4:
                printf("Reporte ordenado por matricula:\n");
                inorder_imprimir(root);
                break;
            case 5:
                reporte_orden_promedio(root);
                break;
            case 6:
                printf("Estudiantes con promedio < 70:\n");
                reporte_por_rango_promedio(root, 70.0f, 0);
                break;
            case 7:
                printf("Estudiantes con promedio >= 70:\n");
                reporte_por_rango_promedio(root, 70.0f, 1);
                break;
            case 8: {
                int m;
                printf("Ingrese matricula de referencia: ");
                if (scanf("%d", &m) != 1) { while (getchar()!='\n'); printf("Entrada invalida.\n"); break; }
                contar_menor_promedio(root, m);
                break;
            }
            case 9: {
                int nivel;
                printf("Ingrese nivel (0 = raiz): ");
                if (scanf("%d", &nivel) != 1) { while (getchar()!='\n'); printf("Entrada invalida.\n"); break; }
                mostrar_nivel(root, nivel);
                break;
            }
            case 10:
                imprimir_ascii(root);
                break;
            case 0:
                printf("Saliendo y liberando memoria...\n");
                break;
            default:
                printf("Opcion invalida.\n");
                break;
        }
    } while (opcion != 0);

    liberar_arbol(root);
    return 0;
}
