//#if 0
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
//#endif

#define value(arr, i, j, width) arr[(i)*width + (j)]

void compute(float *output, float *input0, float *input1,
	int numARows, int numAColumns, int numBColumns) {

	int numBRows = numAColumns;
	int numCRows = numARows;
	int numCColumns = numBColumns;
#define A(i, j)	value(input0, i, j, numAColumns)
#define B(i, j) value(input1, i, j, numBColumns)
#define C(i, j) value(output, i, j, numCColumns)
	int ii, jj, kk;

	for (ii = 0; ii < numCRows; ++ii) {
		for (jj = 0; jj < numCColumns; ++jj) {
			float sum = 0;
			for (kk = 0; kk < numAColumns; ++kk) {
				sum += A(ii, kk) * B(kk, jj);
			}
			C(ii, jj) = sum;
		}
	}
#if 0
	for (ii = 0; ii < numCRows; ++ii) {
		for (jj = 0; jj < numCColumns; ++jj) {
			printf("%.10f", C(ii, jj));
			if (jj != numCColumns - 1) {
				printf(" ");
			}
		}
		if (ii != numCRows - 1) {
			printf("\n");
		}
	}
	printf("\n\n");
#endif
#undef A
#undef B
#undef C
}

float *createData(int height, int width) 
{
	float *data = (float *)malloc(sizeof(float) * width * height);
	int i;
	for (i = 0; i < width * height; i++) {
		data[i] = ((float)(rand() % 10) - 5) / 5.0f;
	}
	return data;
}

void writeData(	const char *file_name, 
				float *data, 
				int height,
				int width) 
{
	int ii, jj;
	FILE *handle = fopen(file_name, "wb+");
	fprintf(handle, "%d %d\n", height, width);
	for (ii = 0; ii < height; ii++) {
		for (jj = 0; jj < width; jj++) {
			fprintf(handle, "%.10f", *data++);
			if (jj != width - 1) {
				fprintf(handle, " ");
			}
		}
		if (ii != height - 1) {
			fprintf(handle, "\n");
		}
	}
	fflush(handle);
	fclose(handle);
}

float * readData(const char *file_name,
	int *height,
	int *width)
{
	int ii, jj;
	FILE *handle = fopen(file_name, "rb");
	fscanf(handle, "%d %d\n", height, width);
	//printf("height = %d width = %d\n", *height, *width);
	int rows = *height;
	int cols = *width;
	float *data = (float *)malloc(rows*cols * sizeof(float));
	assert(data != nullptr);
	float *p = data;
#if 0
	for (ii = 0; ii < rows; ii++) {
		for (jj = 0; jj < cols; jj++) {
			float d;
			fscanf(handle, "%f", &d);
			*p = d;
			p++;
			printf("%.2f ", d);
			if (jj != cols - 1) {
				printf(" ");
			}
		}
		if (ii != rows - 1) {
			printf("\n");
		}
	}
	printf("\n\n");
#else
	for (ii = 0; ii < rows; ii++) {
		for (jj = 0; jj < cols; jj++) {
			float d;
			fscanf(handle, "%f", &d);
			*p = d;
			p++;
		}
	}
#endif
	fclose(handle);
	return data;
}

void createDataset(const char *input0_file_name,
							 const char *input1_file_name,
							 const char *output_file_name,
							int numARows, 
							int numACols,
							int numBCols) 
{
	int numBRows = numACols;
	int numCRows = numARows;
	int numCCols = numBCols;

	float *input0_data = createData(numARows, numACols);
	float *input1_data = createData(numBRows, numBCols);
	float *output_data = (float *)calloc(sizeof(float), numCRows * numCCols);
	
	compute(output_data, input0_data, input1_data, numARows, numACols, numBCols);
	writeData(input0_file_name, input0_data, numARows, numACols);
	writeData(input1_file_name, input1_data, numBRows, numBCols);
	writeData(output_file_name, output_data, numCRows, numCCols);

	free(input0_data);
	free(input1_data);
	free(output_data);
}
