#include "matrix.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

// Include SSE intrinsics
#if defined(_MSC_VER)
#include <intrin.h>
#elif defined(__GNUC__) && (defined(__x86_64__) || defined(__i386__))
#include <immintrin.h>
#include <x86intrin.h>
#endif

/* Below are some intel intrinsics that might be useful
 * void _mm256_storeu_pd (double * mem_addr, __m256d a)
 * __m256d _mm256_set1_pd (double a)
 * __m256d _mm256_set_pd (double e3, double e2, double e1, double e0)
 * __m256d _mm256_loadu_pd (double const * mem_addr)
 * __m256d _mm256_add_pd (__m256d a, __m256d b)
 * __m256d _mm256_sub_pd (__m256d a, __m256d b)
 * __m256d _mm256_fmadd_pd (__m256d a, __m256d b, __m256d c)
 * __m256d _mm256_mul_pd (__m256d a, __m256d b)
 * __m256d _mm256_cmp_pd (__m256d a, __m256d b, const int imm8)
 * __m256d _mm256_and_pd (__m256d a, __m256d b)
 * __m256d _mm256_max_pd (__m256d a, __m256d b)
*/

/* Generates a random double between low and high */
double rand_double(double low, double high) {
    double range = (high - low);
    double div = RAND_MAX / range;
    return low + (rand() / div);
}

/* Generates a random matrix */
void rand_matrix(matrix *result, unsigned int seed, double low, double high) {
    srand(seed);
    for (int i = 0; i < result->rows; i++) {
        for (int j = 0; j < result->cols; j++) {
            set(result, i, j, rand_double(low, high));
        }
    }
}

/*
 * Returns the double value of the matrix at the given row and column.
 * You may assume `row` and `col` are valid. Note that the matrix is in row-major order.
 */
double get(matrix *mat, int row, int col) {
    // Task 1.1 TODO
    int index = row*mat->cols + col;
    return (mat->data)[index];
}

/*
 * Sets the value at the given row and column to val. You may assume `row` and
 * `col` are valid. Note that the matrix is in row-major order.
 */
void set(matrix *mat, int row, int col, double val) {
    // Task 1.1 TODO
    int index = row*mat->cols + col;
    (mat->data)[index] = val;
}

/*
 * Allocates space for a matrix struct pointed to by the double pointer mat with
 * `rows` rows and `cols` columns. You should also allocate memory for the data array
 * and initialize all entries to be zeros. `parent` should be set to NULL to indicate that
 * this matrix is not a slice. You should also set `ref_cnt` to 1.
 * You should return -1 if either `rows` or `cols` or both have invalid values. Return -2 if any
 * call to allocate memory in this function fails.
 * Return 0 upon success.
 */
int allocate_matrix(matrix **mat, int rows, int cols) {
    // Task 1.2 TODO
    // HINTS: Follow these steps.
    // 1. Check if the dimensions are valid. Return -1 if either dimension is not positive.
    // 2. Allocate space for the new matrix struct. Return -2 if allocating memory failed.
    // 3. Allocate space for the matrix data, initializing all entries to be 0. Return -2 if allocating memory failed.
    // 4. Set the number of rows and columns in the matrix struct according to the arguments provided.
    // 5. Set the `parent` field to NULL, since this matrix was not created from a slice.
    // 6. Set the `ref_cnt` field to 1.
    // 7. Store the address of the allocated matrix struct at the location `mat` is pointing at.
    // 8. Return 0 upon success.
    if(rows <= 0 || cols <= 0){
	return -1;
    }
    matrix *new_mat = (matrix*)malloc(sizeof(matrix));
    if(new_mat == NULL){
	return -2;
    }
    int new_dim = rows*cols;
    double *new_data = (double*)calloc(new_dim,sizeof(double));
    if(new_data == NULL){
	return -2;
    }
    new_mat->parent = NULL;
    new_mat->ref_cnt = 1;
    new_mat->rows = rows;
    new_mat->cols = cols;
    new_mat->data = new_data;
    *mat = new_mat;
    return 0;

}

/*
 * You need to make sure that you only free `mat->data` if `mat` is not a slice and has no existing slices,
 * or that you free `mat->parent->data` if `mat` is the last existing slice of its parent matrix and its parent
 * matrix has no other references (including itself).
 */
void deallocate_matrix(matrix *mat) {
    // Task 1.3 TODO
    // HINTS: Follow these steps.
    // 1. If the matrix pointer `mat` is NULL, return.
    // 2. If `mat` has no parent: decrement its `ref_cnt` field by 1. If the `ref_cnt` field becomes 0, then free `mat` and its `data` field.
    // 3. Otherwise, recursively call `deallocate_matrix` on `mat`'s parent, then free `mat`.
    if(mat == NULL){
	return;
    }
    if(mat->parent == NULL){
	mat->ref_cnt -= 1;
	if(mat->ref_cnt==0){
	    free(mat->data);
	    free(mat);
	}
    }
    else{
        deallocate_matrix(mat->parent);
        free(mat);
    }



}

/*
 * Allocates space for a matrix struct pointed to by `mat` with `rows` rows and `cols` columns.
 * Its data should point to the `offset`th entry of `from`'s data (you do not need to allocate memory)
 * for the data field. `parent` should be set to `from` to indicate this matrix is a slice of `from`
 * and the reference counter for `from` should be incremented. Lastly, do not forget to set the
 * matrix's row and column values as well.
 * You should return -1 if either `rows` or `cols` or both have invalid values. Return -2 if any
 * call to allocate memory in this function fails.
 * Return 0 upon success.
 * NOTE: Here we're allocating a matrix struct that refers to already allocated data, so
 * there is no need to allocate space for matrix data.
 */
int allocate_matrix_ref(matrix **mat, matrix *from, int offset, int rows, int cols) {
    // Task 1.4 TODO
    // HINTS: Follow these steps.
    // 1. Check if the dimensions are valid. Return -1 if either dimension is not positive.
    // 2. Allocate space for the new matrix struct. Return -2 if allocating memory failed.
    // 3. Set the `data` field of the new struct to be the `data` field of the `from` struct plus `offset`.
    // 4. Set the number of rows and columns in the new struct according to the arguments provided.
    // 5. Set the `parent` field of the new struct to the `from` struct pointer.
    // 6. Increment the `ref_cnt` field of the `from` struct by 1.
    // 7. Store the address of the allocated matrix struct at the location `mat` is pointing at.
    // 8. Return 0 upon success.
    if(rows<=0 || cols<=0){
	return -1;
    }
    matrix *new_mat = malloc(sizeof(matrix));
    if(new_mat == NULL){
	return -2;
    }
    new_mat->data = from->data + offset;
    new_mat->rows = rows;
    new_mat->cols = cols;
    new_mat->parent = from;
    from->ref_cnt += 1;
    *mat = new_mat;
    return 0;
}

/*
 * Sets all entries in mat to val. Note that the matrix is in row-major order.
 */
void fill_matrix(matrix *mat, double val) {
    // Task 1.5 TODO
    int matrix_size = mat->rows * mat->cols;
    #pragma omp parallel
    {
	#pragma omp for
        for(int i = 0; i < matrix_size; i++){
	    mat->data[i] = val;
        }
    }
}

/*
 * Store the result of taking the absolute value element-wise to `result`.
 * Return 0 upon success.
 * Note that the matrix is in row-major order.
 */
int abs_matrix(matrix *result, matrix *mat) {
    // Task 1.5 TODO
    int matrix_size = mat->rows * mat->cols;
    #pragma omp parallel
    {
	#pragma omp for
        for(int i = 0; i< matrix_size;i++){
	    result->data[i] = fabs(mat->data[i]);
        }
    }
    return 0;
}

/*
 * (OPTIONAL)
 * Store the result of element-wise negating mat's entries to `result`.
 * Return 0 upon success.
 * Note that the matrix is in row-major order.
 */
int neg_matrix(matrix *result, matrix *mat) {
    // Task 1.5 TODO
    return 0;
}

/*
 * Store the result of adding mat1 and mat2 to `result`.
 * Return 0 upon success.
 * You may assume `mat1` and `mat2` have the same dimensions.
 * Note that the matrix is in row-major order.
 */
int add_matrix(matrix *result, matrix *mat1, matrix *mat2) {
    // Task 1.5 TODO
    int matrix_size = mat1->rows * mat1->cols;
    #pragma omp parallel
    {
	#pragma omp for
        for(int i = 0; i<matrix_size;i++){
	    result->data[i] = mat1->data[i] + mat2->data[i];	    
        }
    }
    return 0;
}

/*
 * (OPTIONAL)
 * Store the result of subtracting mat2 from mat1 to `result`.
 * Return 0 upon success.
 * You may assume `mat1` and `mat2` have the same dimensions.
 * Note that the matrix is in row-major order.
 */
int sub_matrix(matrix *result, matrix *mat1, matrix *mat2) {
    // Task 1.5 TODO
    return 0;
}

/*
 * Store the result of multiplying mat1 and mat2 to `result`.
 * Return 0 upon success.
 * Remember that matrix multiplication is not the same as multiplying individual elements.
 * You may assume `mat1`'s number of columns is equal to `mat2`'s number of rows.
 * Note that the matrix is in row-major order.
 */
int mul_matrix(matrix *result, matrix *mat1, matrix *mat2) {
    // Task 1.6 TODO
    int cols1 = mat1->cols;
    int cols = mat2->cols;
    int rows = mat1->rows;
    //transpose second matrix
    double mat1_array[mat1->rows][mat1->cols];
    double mat2_array[mat2->cols][mat2->rows];
#pragma omp parallel
    {
#pragma omp for
    for(int i = 0; i < mat1->rows;i++){
	//printf("MAT1 Row ");
	//printf("%d",i);
	//printf(" ");
	for(int j = 0; j < mat1->cols;j++){
	    mat1_array[i][j] = mat1->data[j+i*mat1->cols];
	    //printf("%f",mat1_array[i][j]);
	    //printf(" ");
	}
    }
    }
#pragma omp parallel
    {
#pragma omp for
	for(int i = 0; i < mat2->cols;i++){
	    //printf("MAT2 Row ");
	    //printf("%d",i);
	    //printf(" ");
	    for(int j = 0; j<mat2->rows;j++){
		mat2_array[i][j] = mat2->data[i+j*mat2->cols];
		//printf("%f",mat2_array[i][j]);
		//printf(" ");
	    }
	}
    }
#pragma omp parallel
    {
#pragma omp for 
        for(int y = 0; y < mat1->rows;y++){
	    double current_products[4];
	    for(int x = 0; x<mat2->cols;x++){
		double dot_prod = 0;
		int current = 0;
		while(current + 4 < mat1->cols){
		    __m256d m1 = _mm256_loadu_pd(mat1_array[y] + current);
		    __m256d m2 = _mm256_loadu_pd(mat2_array[x] + current);
		    __m256d products = _mm256_mul_pd(m1,m2);
		    _mm256_storeu_pd(current_products,products);
		    dot_prod += (current_products[0] + current_products[1] + current_products[2] + current_products[3]);
		    //printf("simd used");
		    current += 4;
		}
		while(current < mat1->cols){
		    dot_prod += mat1_array[y][current] * mat2_array[x][current];
		    current++;
		}
		//printf("%f",dot_prod);
	        result->data[x+y*mat2->cols] = dot_prod;
	}
    }
    }
    return 0;
}

/*
 * Store the result of raising mat to the (pow)th power to `result`.
 * Return 0 upon success.
 * Remember that pow is defined with matrix multiplication, not element-wise multiplication.
 * You may assume `mat` is a square matrix and `pow` is a non-negative integer.
 * Note that the matrix is in row-major order.
 */
int pow_matrix(matrix *result, matrix *mat, int pow) {
    // Task 1.6 TODO
    matrix *temp_mat;
    allocate_matrix(&temp_mat,result->cols,result->rows);
    fill_matrix(temp_mat,0);
    fill_matrix(result,0);
#pragma omp parallel
    {
#pragma omp for
    for(int i = 0; i < result->cols; i++){
	result->data[i+i*result->cols] = 1;
    }
    }
    if(pow == 0){
	return 0;
    }
    int current = 1;
#pragma omp parallel
    {
#pragma omp for
    for(int i = 0; i < mat->rows*mat->cols;i++){
	result->data[i] = mat->data[i];
    }
    }
    while(current * 2 <= pow){
	current *= 2;
	mul_matrix(temp_mat,result,result);
#pragma omp parallel
    {
	#pragma omp for
	for(int i = 0; i<mat->rows*mat->cols;i++){
	    result->data[i] = temp_mat->data[i];
	}
    }
    }
    for(int i = current;i<pow;i++){
	mul_matrix(temp_mat,mat,result);
#pragma omp parallel
	{
#pragma omp for
	for(int j = 0; j < mat->rows * mat-> cols;j++){
	    result->data[j] = temp_mat->data[j];
	}
	}
    }
    free(temp_mat);
    return 0;
}
