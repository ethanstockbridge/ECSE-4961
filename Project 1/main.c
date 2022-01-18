// General C Imports
#include <stdint.h>
#include <stdio.h>

#define N 100

//test function for normal c matrix multiplication
void int_matrix_multiplication(int matrixA[N][N], int matrixB[N][N], int matrixC[N][N])
{
	int i,j,k;
	for (i = 0; i < N; i++)
	{
		for (j = 0; j < N; j++)
		{
			matrixC[i][j] = 0;
			for (k = 0; k < N; k++)
			{
				matrixC[i][j] += matrixA[i][k]*matrixB[k][j];
			}
		}
	}
}

//print function for printing matricies (int)
void printMatrix(int matrix[N][N])
{
	printf("Matrix:\r\n");
	for(int i=0;i<N;i++)
	{
		for(int j=0; j<N; j++)
		{
			printf("%i ", matrix[i][j]);
		}
		printf("\r\n");
	}
	printf("\r\n\r\n");
}


int main()
{
	int matrixA[N][N];
	int matrixB[N][N];
	int matrixC[N][N];
	//load matricies
	for(int i=0;i<N;i++)
	{
		for(int j=0; j<N; j++)
		{
			matrixA[i][j] = i;
			matrixB[i][j] = j;
		}
	}
	//print loaded matricies
	printMatrix(matrixA);
	printMatrix(matrixB);
	//multiply with normal c matrix multiplication
	int_matrix_multiplication(matrixA,matrixB,matrixC);
	//print result
	printMatrix(matrixC);
}