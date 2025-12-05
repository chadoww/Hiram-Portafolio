#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 2048
#define MAX_COL 30
#define MAX_LEN 256

typedef struct Pelicula {
    char columnas[MAX_COL][MAX_LEN];  
    struct Pelicula *siguiente;
} Pelicula;

int contieneTexto(const char *texto, const char *buscado);
Pelicula* cargarDesdeCSV(const char *archivo);
void mostrarPelicula(Pelicula *p);
void buscarPorTitulo(Pelicula *lista, const char *titulo);
void buscarPorID(Pelicula *lista, const char *id);
void actualizarPelicula(Pelicula *lista, const char *titulo);
void insertarPelicula(Pelicula **lista);
void eliminarPelicula(Pelicula **lista, const char *titulo);
void guardarEnCSV(Pelicula *lista, const char *archivo);
void liberarLista(Pelicula *lista);

int contieneTexto(const char *texto, const char *buscado) {
    char t[MAX_LEN], b[MAX_LEN];
    strncpy(t, texto, sizeof(t));
    strncpy(b, buscado, sizeof(b));
    t[sizeof(t)-1] = '\0'; 
    b[sizeof(b)-1] = '\0';
    for(int i = 0; t[i]; i++) t[i] = tolower((unsigned char)t[i]);
    for(int i = 0; b[i]; i++) b[i] = tolower((unsigned char)b[i]);
    return strstr(t, b) != NULL;
}

Pelicula* cargarDesdeCSV(const char *archivo) {
    FILE *f = fopen(archivo, "r");
    if (!f) { 
        printf("Error al abrir archivo %s\n", archivo); 
        return NULL; 
    }

    char linea[MAX_LINE];

    fgets(linea, sizeof(linea), f); 

    Pelicula *lista = NULL, *ultimo = NULL;
    while (fgets(linea, sizeof(linea), f)) {
        Pelicula *p = malloc(sizeof(Pelicula));
        if (!p) { 
            fclose(f); 
            return lista; 
        }
        p->siguiente = NULL;

        char *token = strtok(linea, ",\n");
        int col = 0;
        while (token && col < MAX_COL) {
            strncpy(p->columnas[col], token, MAX_LEN - 1);
            p->columnas[col][MAX_LEN - 1] = '\0';
            token = strtok(NULL, ",\n");
            col++;
        }
        for (int i = col; i < MAX_COL; i++) p->columnas[i][0] = '\0';

        if (lista == NULL) lista = p;
        else ultimo->siguiente = p;
        ultimo = p;
    }
    fclose(f);
    return lista;
}

void mostrarPelicula(Pelicula *p) {
    if (!p) return;
    printf("ID: %s | Titulo: %s | Fecha: %s\n", p->columnas[0], p->columnas[1], p->columnas[5]);
}

void buscarPorTitulo(Pelicula *lista, const char *titulo) {
    int encontrado = 0;
    while (lista) {
        if (contieneTexto(lista->columnas[1], titulo)) {
            mostrarPelicula(lista);
            encontrado = 1;
        }
        lista = lista->siguiente;
    }
    if (!encontrado) printf("No se encontro ninguna pelicula con el titulo '%s'\n", titulo);
}

void buscarPorID(Pelicula *lista, const char *id) {
    int encontrado = 0;
    while (lista) {
        if (strcmp(lista->columnas[0], id) == 0) {
            mostrarPelicula(lista);
            encontrado = 1;
            break; 
        }
        lista = lista->siguiente;
    }
    if (!encontrado) printf("No se encontro ninguna pelicula con el ID '%s'\n", id);
}

void actualizarPelicula(Pelicula *lista, const char *titulo) {
    while (lista) {
        if (contieneTexto(lista->columnas[1], titulo)) {
            mostrarPelicula(lista);
            printf("Modificar:\n1. ID\n2. Titulo\n3. Fecha\n> ");

            char linea[20]; 
            int opc;
            fgets(linea, sizeof(linea), stdin);
            opc = atoi(linea);

            switch (opc) {
                case 1:
                    printf("Nuevo ID: ");
                    fgets(lista->columnas[0], MAX_LEN, stdin);
                    lista->columnas[0][strcspn(lista->columnas[0], "\n")] = 0; 
                    break;
                case 2:
                    printf("Nuevo Titulo: ");
                    fgets(lista->columnas[1], MAX_LEN, stdin);
                    lista->columnas[1][strcspn(lista->columnas[1], "\n")] = 0; 
                    break;
                case 3:
                    printf("Nueva Fecha: ");
                    fgets(lista->columnas[5], MAX_LEN, stdin);
                    lista->columnas[5][strcspn(lista->columnas[5], "\n")] = 0; 
                    break;
                default:
                    printf("Opcion invalida.\n");
                    return;
            }
            printf("Pelicula actualizada.\n");
            return;
        }
        lista = lista->siguiente;
    }
    printf("No se encontro la pelicula '%s'\n", titulo);
}

void insertarPelicula(Pelicula **lista) {
    Pelicula *p = malloc(sizeof(Pelicula));
    if (!p) { 
        printf("Error de memoria\n"); 
        return; 
    }
    p->siguiente = *lista;
    *lista = p;

    printf("ID: "); 
    fgets(p->columnas[0], MAX_LEN, stdin); 
    p->columnas[0][strcspn(p->columnas[0], "\n")] = 0;
    
    printf("Titulo: "); 
    fgets(p->columnas[1], MAX_LEN, stdin); 
    p->columnas[1][strcspn(p->columnas[1], "\n")] = 0;
    
    printf("Fecha: "); 
    fgets(p->columnas[5], MAX_LEN, stdin); 
    p->columnas[5][strcspn(p->columnas[5], "\n")] = 0;

    for (int i = 2; i < MAX_COL; i++) {
        if (i != 5) {
            p->columnas[i][0] = '\0';
        }
    }

    printf("Pelicula insertada.\n");
}

void eliminarPelicula(Pelicula **lista, const char *titulo) {
    Pelicula *actual = *lista, *ant = NULL;
    while (actual) {
        if (contieneTexto(actual->columnas[1], titulo)) {
            mostrarPelicula(actual);
            printf("Desea eliminar esta pelicula? (si/no): ");
            char resp[10]; 
            fgets(resp, sizeof(resp), stdin);
            resp[strcspn(resp, "\n")] = 0;
            if (strcmp(resp, "si") == 0) {
                if (ant) ant->siguiente = actual->siguiente;
                else *lista = actual->siguiente;
                free(actual);
                printf("Pelicula eliminada.\n");
                return;
            } else { 
                printf("Operacion cancelada.\n"); 
                return; 
            }
        }
        ant = actual; 
        actual = actual->siguiente;
    }
    printf("No se encontro pelicula '%s'\n", titulo);
}

void guardarEnCSV(Pelicula *lista, const char *archivo) {
    FILE *f = fopen(archivo, "w");
    if (!f) { 
        printf("Error al guardar archivo\n"); 
        return; 
    }
    fprintf(f, "id,title,vote_average,vote_count,status,release_date,revenue,runtime,adult,backdrop_path,budget,homepage,imdb_id,original_language,original_title,overview,popularity,poster_path,tagline,genres,production_companies,production_countries,spoken_languages,keywords\n");
    
    while (lista) {
        for (int i = 0; i < MAX_COL; i++) {
            fprintf(f, "%s", lista->columnas[i]);
            if (i < MAX_COL - 1) {
                fprintf(f, ",");
            } else {
                fprintf(f, "\n");
            }
        }
        lista = lista->siguiente;
    }
    fclose(f);
    printf("Cambios guardados exitosamente.\n");
}

void liberarLista(Pelicula *lista) {
    Pelicula *tmp;
    while (lista) { 
        tmp = lista; 
        lista = lista->siguiente; 
        free(tmp);
    }
}

int main() {
    Pelicula *lista = cargarDesdeCSV("Movies.csv");
    if (!lista) return 1;

    char linea[100];
    int opcion;
    char texto[100];

    do {
        printf("\n===== MENU =====\n");
        printf("1. Buscar pelicula por titulo\n");
        printf("2. Buscar pelicula por ID\n");
        printf("3. Insertar nueva pelicula\n");
        printf("4. Eliminar pelicula\n");
        printf("5. Actualizar pelicula\n");
        printf("0. Salir\n> ");

        if (fgets(linea, sizeof(linea), stdin)) {
            opcion = atoi(linea);
        } else {
            opcion = -1;
        }

        switch (opcion) {
            case 1:
                printf("Titulo a buscar: ");
                fgets(texto, sizeof(texto), stdin); 
                texto[strcspn(texto, "\n")] = 0;
                buscarPorTitulo(lista, texto); 
                break;
            case 2:
                printf("ID a buscar: ");
                fgets(texto, sizeof(texto), stdin); 
                texto[strcspn(texto, "\n")] = 0;
                buscarPorID(lista, texto); 
                break;
            case 3:
                insertarPelicula(&lista);
                guardarEnCSV(lista, "Movies.csv"); 
                break;
            case 4:
                printf("Titulo a eliminar: ");
                fgets(texto, sizeof(texto), stdin); 
                texto[strcspn(texto, "\n")] = 0;
                eliminarPelicula(&lista, texto);
                guardarEnCSV(lista, "Movies.csv"); 
                break;
            case 5:
                printf("Titulo a actualizar: ");
                fgets(texto, sizeof(texto), stdin); 
                texto[strcspn(texto, "\n")] = 0;
                actualizarPelicula(lista, texto);
                guardarEnCSV(lista, "Movies.csv"); 
                break;
            case 0: 
                liberarLista(lista); 
                printf("Saliendo...\n");
                break;
            default: 
                printf("Opcion invalida\n");
        }
    } while (opcion != 0);

    return 0;
}