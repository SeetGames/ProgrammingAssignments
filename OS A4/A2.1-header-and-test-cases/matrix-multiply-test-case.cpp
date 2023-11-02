#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <memory>
#include "new-coro-lib.h"
/*
	This is a rather large test case that performs
	matrix multiplication in multiple user threads.

	The way to use this program:
		matrix-multiply-test-case <row> <common dimension> <col> <max_num_thd>

	It generates two random matrices. One is a <row> by <common dimension> matrix. The other
    is 	<common dimension> by <col>. We compute the multiplication of the two and put
	it into a <row> by <col> solution matrix. The multiplication is done in two ways: one way
	would be to do all multiplication in one thread while the other way is to do the
	multiplication in several threads.

	The <max_num_thd> is the maximum number of threads you can generate to run this program.

	So far, in all my testing, the program results in printing "Correct Solution".

	The largest matrix I have tested so far is the following:
	matrix-multiply-test-case 1000 2000 1000 200
	Correct solution

	But this will take a long time so it is not recommended that you should be doing this test
	all the time.
*/


const int MAX_MATRIX_ELEMENT = (2<<8);

struct intInput
{
    int id, rows, cols, stride;
    int matrix2_rows;
    const int* matrix1;
    const int* matrix2;
    int* matrix_sol;
    intInput(const int * m1, const int * m2, int * m_sol)
        :matrix1(m1), matrix2(m2), matrix_sol(m_sol)
    {}
};

void error_msg(char *exec_filename)
{
    fprintf(stderr, "Usage: %s", exec_filename);
    fprintf(stderr, " <row> <common dimension> <col> <max_num_threads>");
    exit(-1);
}

int * make_new_matrix(int row, int col)
{
    int *new_matrix = new int[row*col];

	return new_matrix;
}

void init_new_matrix(int row, int col, int* new_matrix)
{
    int i,j;

    for(i=0; i<row; i++)
        for(j=0; j<col; j++)
            *(new_matrix + i*col + j) = rand()%MAX_MATRIX_ELEMENT;
}


void slow_multiply(	const int * matrix1, 
					const int * matrix2, 
					int * new_matrix,
					int new_row, 
					int new_col, 
					int matrix2_row)
{
    int r,c, i;

    for(r=0; r<new_row; r++)
    {
        for(c=0; c<new_col; c++)
        {
            int acc = 0;

            for(i=0; i<matrix2_row; i++)
            {
                acc += matrix1[r*matrix2_row+i]*matrix2[i*new_col + c];
            }
            new_matrix[r*new_col+c] = acc;
        }
    }
}


void *compute_mm_mult(void *data_ptr)
{
    int r,c;
    int max_rows, max_cols;
    intInput *ds_ptr = reinterpret_cast<intInput*>(data_ptr);

    r = ds_ptr->id;
    max_rows=ds_ptr->rows;
    max_cols=ds_ptr->cols;

    while(r < max_rows)
    {
        for(c=0; c<max_cols; ++c)
        {
            int acc=0;
            int i;
            for(i=0; i<ds_ptr->matrix2_rows; i++)
            {
                acc += (ds_ptr->matrix1)[r*ds_ptr->matrix2_rows+i]*(ds_ptr->matrix2)[i*max_cols+c];
                CORO::thd_yield();
            }
            (ds_ptr->matrix_sol)[r*max_cols+c]=acc;
        }
        r+=ds_ptr->stride;

    }
    delete ds_ptr;
    return nullptr;
}


void mm_multiply(	const int *m1,
					const int *m2, 
					int* m_sol,
					int max_num_threads, 
					int m_sol_rows,
					int m_sol_cols, 
					int matrix2_rows)
{
    CORO::ThreadID *thread_ids;
    int i;

    thread_ids = new CORO::ThreadID[max_num_threads];

    for(i=0; i<max_num_threads; ++i)
    {
        struct intInput *mi = new intInput(m1, m2, m_sol);
        mi->id = i;
        mi->rows = m_sol_rows;
        mi->cols = m_sol_cols;
        mi->stride = max_num_threads;
        mi->matrix2_rows = matrix2_rows;

        thread_ids[i] = CORO::new_thd(compute_mm_mult, reinterpret_cast<void*>(mi));
    }

    for(i=0; i<max_num_threads; ++i)
    {
        printf("wait thread %d \n", i);
		
        CORO::wait_thread(thread_ids[i], NULL);
    }
    delete thread_ids;
}

int matrix_compare(const int* matrix1, 
					const int* matrix2,
					int rows, 
					int cols)
{
    int i, j;
    int e = 1;
    for(i=0; i<rows; i++)
    {
        for(j=0; j<cols; j++)
        {
            if(matrix1[i*cols+j]!=matrix2[i*cols+j])
            {	
                e = 0;
                return e;
            }
        }
    }
	return e;		
}

int main(int argc, char **argv)
{
    int rows, common, cols;
    int max_num_threads;

    if(argc!=5)
        error_msg(argv[0]);

    rows=atoi(argv[1]);
    common=atoi(argv[2]);
    cols=atoi(argv[3]);
    max_num_threads=atoi(argv[4]);

    srand(time(NULL));
    int *matrix1 = make_new_matrix(rows, common);
    int *matrix2 = make_new_matrix(common, cols);
    int *single_sol = make_new_matrix(rows, cols);
    int *mm_sol = make_new_matrix(rows, cols);
	
    init_new_matrix(rows, common, matrix1);
    init_new_matrix(common, cols, matrix2);

    slow_multiply(matrix1, matrix2, single_sol, rows, cols,  common);
    CORO::thd_init();
    mm_multiply(matrix1, matrix2, mm_sol, max_num_threads, rows, cols, common);

    if(matrix_compare(single_sol, mm_sol, rows, cols)==0)
        printf("Incorrect solution\n");
    else
        printf("Correct solution\n");
}
