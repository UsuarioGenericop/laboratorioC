#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Declaracion de constantes globales
#define N 512
#define MaxIter 10
#define Tol 0.00001f
#define Seed 42

// Declaracion de matrices y vectores globales
float Mat[N][N], MatDD[N][N], V1[N], V2[N], V3[N], V4[N], b[N], x[N];

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
        printf("%f\t", vect[i]);
    }
    printf("\n");
}

// Imprime un cierto numero de elementos (numel) de una fila (row) de una matriz (mat) desde una posicion (from)
void PrintRow(float mat[N][N], int row, int from, int numel)
{
    // Valida que la fila a imprimir este dentro de la matriz
    if (row < 0 || row >= N)
        return;
    if (from + numel > N)
    {
        numel = N - from; // Corrige el valor de numel
    }
    for (int c = from; c < numel + from; c++)
    {
        printf("%f\t", mat[row][c]);
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

// BLOQUE 3

// Calcula el prodcuto entre la matriz (M) y el vector (vec) dejando el resultado en el vector (vectres)
void Matrix_x_Vector(float M[N][N], float vect[N], float vectres[N])
{
    for (int r = 0; r < N; r++)
    {
        float total_fila = 0.0f;
        for (int c = 0; c < N; c++)
        {
            total_fila += M[r][c] * vect[c];
        }
        vectres[r] = total_fila;
    }
}

// Calcula la norma 2 de la diferencia entre dos vectores v1 y v2 retornando la longitud del vector resultante
float Norm2(float v1[N], float v2[N])
{
    float sumaCuadrados = 0.0f;
    for (int i = 0; i < N; i++)
    {
        float diff = v1[i] - v2[i];
        sumaCuadrados += diff * diff;
    }
    return sqrtf(sumaCuadrados);
}

// BLOQUE 4

// Metodo iterativo de Jacobi que retorna el numero de iteraciones realizadas hasta alcanzar la tolerancia o -1 si se llega al numero maximo de iteraciones
int Jacobi(float A[N][N], float b[N], float x[N], float tol, int max_iter)
{
    float resultados[N];
    for (int iteraciones = 0; iteraciones < max_iter; iteraciones++) // Bucle para la cantidad de iteraciones
    {
        for (int r = 0; r < N; r++)
        {
            float suma_fila = 0.0f;
            for (int c = 0; c < N; c++)
            {
                if (r != c)
                {
                    suma_fila += A[r][c] * x[c]; // Suma el producto de la matriz [r][c] entre el vector [c] excluyendo las diagonales
                }
            }
            resultados[r] = (b[r] - suma_fila) / A[r][r]; // Despeja Xn restandole la suma de la fila al resultado y dividiendolo por el valor de la diagonal
        }

        float error = Norm2(resultados, x); // Se mide el cambio entre el intento k-1 y k porque a medida que esta diferencia tiende a 0 el intento k tiende a la respuesta correcta
        for (int i = 0; i < N; i++)
        {
            x[i] = resultados[i]; // Se cambian los valores del vector de prueba por los resultados para la siguiente iteracion
        }
        if (error < tol)
        {
            return iteraciones + 1; // Si el error es menor a la tolerancia se termina la funcion (return iteraciones +1) se suma 1 a las iteraciones porque la variable empezo en 0
        }
    }
    return -1; // Si el bucle termina con la cantidad de iteraciones (return -1)
}

// Metodo iterativo de GaussSeidel que retorna el numero de iteraciones realizadas hasta alacancar la tolerancia o -1 si se llega al numero maximo de iteraciones
int GaussSeidel(float A[N][N], float b[N], float x[N], float tol, int max_iter)
{
    for (int iteraciones = 0; iteraciones < max_iter; iteraciones++) // Bucle para la cantidad de iteraciones
    {
        float sumaCuadradosError = 0.0f;
        for (int r = 0; r < N; r++)
        {
            float suma_fila = 0.0f;
            for (int c = 0; c < N; c++)
            {
                if (r != c)
                {
                    suma_fila += A[r][c] * x[c]; // Suma el producto de la matriz [r][c] entre el vector [c] excluyendo las diagonales
                }
            }
            float x_antiguo = x[r];              // Guarda el valor de x[r] antes de cambiarlo
            x[r] = (b[r] - suma_fila) / A[r][r]; // Despeja Xn restandole la suma de la fila al resultado y dividiendolo por el valor de la diagonal
            float diferencia = x[r] - x_antiguo; // Se calcula el error con el cuadrado de la diferencia del valor que antigio de x y el nuevo valor de x
            sumaCuadradosError += diferencia * diferencia;
        }
        float error = sqrtf(sumaCuadradosError); // No se puede calcular el error con la funcion Norm2 porque esta requere 2 vectores, el ejercicio especifica que para la funcion GaussSeidel solo debe haber un vector de solucion x
        if (error < tol)
        {
            return iteraciones + 1; // Si el error es menor a la tolerancia se termina la funcion (return iteraciones +1) se suma 1 a las iteraciones porque la variable empezo en 0
        }
    }
    return -1; // Si el bluce termina con la cantidad de iteraciones (return -1)
}

int main()
{
    float vectorCalidad[N]; // Vector auxiliar para el calculo de la calidad
    int iter = 0;           // Variable para guardar el numero de iteraciones
    float residuo = 0;      // Varible que guarda la calidad de la solucion

    // Sistema de ecuaciones f_sdd == 1
    InitData(Mat, b, 1, Seed); // Crea el sistema de ecuaciones

    printf("==================================================\n");
    printf("  Elementos iniciales (f_sdd == 1)\n");
    printf("==================================================\n");

    // Muestra por pantalla los 10 primeros elementos del vector b
    printf("10 primeros elementos del vector: \n");
    PrintVect(b, 0, 10);
    // Muestra los 10 primeros elementos de las filas 3 y 4 de la matriz Mat
    printf("10 primeros elementos de de las filas 3 y cuatro de la matriz: \n");
    PrintRow(Mat, 3, 0, 10);
    PrintRow(Mat, 4, 0, 10);

    printf("\n==================================================\n");
    printf("  Jacobi\n");
    printf("==================================================\n");
    if (ValidateSystem(Mat)) // Se asegura de que el sistema puede resolverse mediante metodos iterativos
    {
        printf("El sistema puede resolverse por metodos iterativos\n");
    }
    else
    {
        printf("El sistema no puede resolverse por metodos iterativos\n");
    }

    // Inicializa el vector x a 0
    for (int i = 0; i < N; i++)
    {
        x[i] = 0;
    }
    // Solucion con Jacobi
    iter = Jacobi(Mat, b, x, Tol, MaxIter);
    // Calcula la calidad de la solucion
    Matrix_x_Vector(Mat, x, vectorCalidad);
    residuo = Norm2(vectorCalidad, b);
    // Muestra los reslutados
    printf("--------------------------------------------------\n");
    printf("  Iteraciones\n\n");
    printf("Iteraciones realizadas: %d\n", iter);
    printf("--------------------------------------------------\n");
    printf("  Calidad de la solucion\n\n");
    printf("Calidad de la solucion (Residuo): %f\n", residuo);
    printf("--------------------------------------------------\n");
    printf("  Elementos de la solucion\n\n");
    printf("Primeros 5 elementos de la solucion x: ");
    PrintVect(x, 0, 5);

    // Solucion con Gauss-Seidel
    printf("\n\n==================================================\n");
    printf("  Gauss-Seidel\n");
    printf("==================================================\n");

    if (ValidateSystem(Mat)) // Se asegura de que el sistema puede resolverse mediante metodos iterativos
    {
        printf("El sistema puede resolverse por metodos iterativos\n");
    }
    else
    {
        printf("El sistema no puede resolverse por metodos iterativos\n");
    }
    // Inicializa el vector x a 0
    for (int i = 0; i < N; i++)
    {
        x[i] = 0;
    }

    // Solucion con Gauss-Seidel
    iter = GaussSeidel(Mat, b, x, Tol, MaxIter);
    // Calcula la calidad de la solucion
    Matrix_x_Vector(Mat, x, vectorCalidad);
    residuo = Norm2(vectorCalidad, b);

    // Muestra los resultados
    printf("--------------------------------------------------\n");
    printf("  Iteraciones\n\n");
    printf("Iteraciones realizadas: %d\n", iter);
    printf("--------------------------------------------------\n");
    printf("  Calidad de la solucion\n\n");
    printf("Calidad de la solucion (Residuo): %f\n", residuo);
    printf("--------------------------------------------------\n");
    printf("  Elementos de la solucion\n\n");
    printf("Primeros 5 elementos de la solucion x: ");
    PrintVect(x, 0, 5);

    // Sistema de ecuaciones f_sdd == 0
    InitData(Mat, b, 0, Seed); // Crea el sistema de ecuaciones

    printf("\n\n==================================================\n");
    printf("  Elementos iniciales (f_sdd == 0)\n");
    printf("==================================================\n");

    // Muestra por pantalla los 10 primeros elementos del vector b
    printf("10 primeros elementos del vector: \n");
    PrintVect(b, 0, 10);
    // Muestra los 10 primeros elementos de las filas 3 y 4 de la matriz Mat
    printf("10 primeros elementos de de las filas 3 y cuatro de la matriz: \n");
    PrintRow(Mat, 3, 0, 10);
    PrintRow(Mat, 4, 0, 10);

    printf("\n==================================================\n");
    printf("  Jacobi\n");
    printf("==================================================\n");
    if (ValidateSystem(Mat)) // Se asegura de que el sistema puede resolverse mediante metodos iterativos
    {
        printf("El sistema puede resolverse por metodos iterativos\n");
        // Solucion con Jacobi
        iter = Jacobi(Mat, b, x, Tol, MaxIter);
        // Calcula la calidad de la solucion
        Matrix_x_Vector(Mat, x, vectorCalidad);
        residuo = Norm2(vectorCalidad, b);
        // Muestra los reslutados
        printf("--------------------------------------------------\n");
        printf("  Iteraciones\n\n");
        printf("Iteraciones realizadas: %d\n", iter);
        printf("--------------------------------------------------\n");
        printf("  Calidad de la solucion\n\n");
        printf("Calidad de la solucion (Residuo): %f\n", residuo);
        printf("--------------------------------------------------\n");
        printf("  Elementos de la solucion\n\n");
        printf("Primeros 5 elementos de la solucion x: ");
        PrintVect(x, 0, 5);
    }
    else
    {
        printf("El sistema no puede resolverse por metodos iterativos\n");
    }

    // Inicializa el vector x a 0
    for (int i = 0; i < N; i++)
    {
        x[i] = 0;
    }

    // Solucion con Gauss-Seidel
    printf("\n\n==================================================\n");
    printf("  Gauss-Seidel\n");
    printf("==================================================\n");

    if (ValidateSystem(Mat)) // Se asegura de que el sistema puede resolverse mediante metodos iterativos
    {
        printf("El sistema puede resolverse por metodos iterativos\n");
        // Solucion con Gauss-Seidel
        iter = GaussSeidel(Mat, b, x, Tol, MaxIter);
        // Calcula la calidad de la solucion
        Matrix_x_Vector(Mat, x, vectorCalidad);
        residuo = Norm2(vectorCalidad, b);

        // Muestra los resultados
        printf("--------------------------------------------------\n");
        printf("  Iteraciones\n\n");
        printf("Iteraciones realizadas: %d\n", iter);
        printf("--------------------------------------------------\n");
        printf("  Calidad de la solucion\n\n");
        printf("Calidad de la solucion (Residuo): %f\n", residuo);
        printf("--------------------------------------------------\n");
        printf("  Elementos de la solucion\n\n");
        printf("Primeros 5 elementos de la solucion x: ");
        PrintVect(x, 0, 5);
    }
    else
    {
        printf("El sistema no puede resolverse por metodos iterativos\n");
    }
    // Inicializa el vector x a 0
    for (int i = 0; i < N; i++)
    {
        x[i] = 0;
    }

    return 0;
}