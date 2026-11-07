#include <stdio.h>
#include <stdlib.h>
const int N = 512;
float Mat[N][N], MatDD[N][N], V1[N], V2[N], V3[N], V4[N]; // Declaracion de 2 matrices y 4 vectores

// BLOQUE 1

// Inicializa la matriz A y el vector b segun la semilla aleatoria
// Si f_sdd == 1, se generan valores e nla diagonal principal para asegurar que la matriz sea estrictamente diagonal dominante
// Si f_sdd == 0, se generan valores aleatorios sin garantizar la dominancia
void InitData(float A[N][N], float b[N], int f_sdd, unsigned int semilla)
{
    srand(semilla); // Inicializa el gnerador de numeros pseudoaleatorios

    for (int r = 0; r < N; r++)
    {
        float suma_fila = 0.0f; // La f al final indica que trate al valor especificamente como un float porque si no se interpretaria como un double

        for (int c = 0; c < N; c++)
        {
            A[r][c] = ((float)rand() / RAND_MAX) * 10.0f - 5.0f;
            // Valores entre -5 y 5
            // rand() devuelve un numero aleatorio entre 0 y un numero muy grande. (float) convierte ese numero entero a decimal
            // /RAND_MAX lo divide por una constante que representa el valor maximo de rand() obteniendo un valor decimal entre 0.0 y 1.0
            // *10.0f Amplia el rango posible de 0.0 a 10.0
            // -5.0f Cambia el rango posible desde -5.0 a 5.0

            if (r != c)
            {
                suma_fila += fabsf(A[r][c]);
                // fabsf() calcula el valor absoluto de un float
                // Suma los valores de la matriz excluyendo las diagonales
            }
        }
        if (f_sdd)
        {
            A[r][r] = suma_fila + 1.0f + ((float)rand() / RAND_MAX) * 5.0f;
            // Si f_ssd == 1 se asegura de que el elemento [r][r] en la diagonal sea mayor que la suma de todos los demas numeros en esa fila
            // Toma el valor de la suma de la fila, le agrega 1 y le agrega un numero positivo entre 0.0 y 5.0
        }
        b[r] = ((float)rand() / RAND_MAX) * 20.0f - 10.0f; // Llena el vector b con valores aleatorios
    }
}

// Imprime un cierto numero de elementos (numel) de un vector (vect) desde una posicion (from)
void PrintVect(float vect[N], int from, int numel)
{
    // Valida que los elementos a imprimir esten dentro del vector
    if (from + numel > N)
    {
        numel = N - from; // Corrige el valor de numel
    }
    for (int i = from; i < from + numel; i++)
    {
        printf("%f ", vect[i]);
    }
    printf("\n");
}

// Imprime un cierto numero de elementos (numel) de una fila (row) de una matriz (mat) desde una posicion (from)
void PrintRow(float mat[N][N], int row, int from, int numel)
{
    for (int c = from; c < numel + from; c++)
    {
        printf("%f ", mat[row][c]);
    }
    printf("\n");
}

// BLOQUE 2

// Determina si la matriz es Diagonal Dominante (return 1)
// Una matriz Diagonal Dominante es aquella en la que el valor absoluto de la diagonal es mas grande o igual que la suma de los valores absolutos de los otros elementos de la fila para todas las filas de la matriz
int DiagonalDom(float M[N][N])
{
    for (int r = 0; r < N; r++)
    {
        float suma_fila = 0.0f;
        for (int c = 0; c < N; c++)
        {
            if (r != c)
            {
                suma_fila += fabsf(M[r][c]);
            }
        }
        if (fabsf(M[r][r]) < suma_fila) // Si el valor de la diagonal en la columna r es menor que la suma de esa fila retorna 0
        {
            return 0;
        }
    }
    return 1;
}

// Verifica que la matriz no tenga 0 en la diagonal, si la matriz no tiene ceros en la diagonal (return 1)
int NoZerosDiag(float M[N][N])
{
    for (int r = 0; r < N; r++)
    {
        if (M[r][r] == 0.0f) // Si el valor de la diagonal es 0 retorna 0
        {
            return 0;
        }
    }
    return 1;
}

// Realizara las comprobaciones de DiagonalDom() y NoZerosDiag() y si el sistema puede resolverse mediante metodos iterativos (return 1)
int ValidateSystem(float M[N][N])
{
    return (NoZerosDiag(M) && DiagonalDom(M));
}