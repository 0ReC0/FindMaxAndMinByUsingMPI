
#include <math.h>
#include <iostream>
#include "mpi.h"
#include <cstdlib>
#include <time.h>
#include <iomanip>
using namespace std;

int** InArray(int size, int** array)

{
	cout << "Enter array of " << size << "x" << size << " :" << endl;
	for (int i = 0; i < size; i++) {

		for (int j = 0; j < size; j++) {
			cout << "Enter [" << i << "][" << j << "]:";
			int element;
			cin >> element;

			array[i][j] = element;

		}

	}

	return array;
}
void GenArray(int size, int** array)

{

	for (int i = 0; i < size; i++) {


		for (int j = 0; j < size; j++) {

			array[i][j] = rand();

		}

	}


}
void OutArray(int size, int** array)

{

	for (int i = 0; i < size; i++) {

		for (int j = 0; j < size; j++) {

			cout << setw(8) << array[i][j];

		}

		cout << endl;

	}

	cout << endl;

}
double getTimeSeq(double& timeEnd,
	double& timeStart,
	int& maxvalueSeq,
	int& minvalueSeq,
	int** arr,
	int arraySize) {
	timeStart = MPI_Wtime();

	maxvalueSeq = minvalueSeq = arr[0][0];

	for (int i = 0; i < arraySize; i++) {
		for (int j = 0; j < arraySize; j++) {
			if (arr[i][j] < minvalueSeq) {
				minvalueSeq = arr[i][j];
			}
			if (maxvalueSeq < arr[i][j]) {
				maxvalueSeq = arr[i][j];
			}
		}
	}

	timeEnd = MPI_Wtime();

	return timeEnd - timeStart;
}
void TestArray(int** arr, int arraySize, int rank, int size) {
	int maxvalue = 0;
	int minvalue = 0;
	int maxvalueSeq = 0;
	int minvalueSeq = 0;
	int globalmaxvalue = 0;
	int globalminvalue = 0;

	double timeStart, timeEnd;
	double timeSeq;


	if (rank == 0) {

		timeSeq = getTimeSeq(timeEnd, timeStart, maxvalueSeq, minvalueSeq, arr, arraySize);

	}

	maxvalue = minvalue = arr[0][0];

	if (rank == 0) {
		timeStart = MPI_Wtime();
	}
	for (int i = 0; i < arraySize; i++) {
		for (int j = 0; j < arraySize; j++) {
			if (arr[i][j] < minvalue) {
				minvalue = arr[i][j];
			}
			if (maxvalue < arr[i][j]) {
				maxvalue = arr[i][j];
			}
		}
	}

	MPI_Reduce(&maxvalue, &globalmaxvalue, 1, MPI_INT, MPI_MAX, 0, MPI_COMM_WORLD);
	MPI_Reduce(&minvalue, &globalminvalue, 1, MPI_INT, MPI_MIN, 0, MPI_COMM_WORLD);

	if (rank == 0) {
		timeEnd = MPI_Wtime();
	}
	if (rank == 0)
	{
		cout << "NumThreads: " << size << "\t Array size: " << arraySize << endl
			<< "Max Seq: " << setw(10) << maxvalueSeq << "\t Max Parallel: " << globalmaxvalue << endl
			<< "Min Seq: " << setw(10) << minvalueSeq << "\t Min Parallel: " << globalminvalue << endl
			<< "timeSeq: " << setw(11) << timeSeq  << "\t timeParallel: " << setw(11)<< timeEnd - timeStart
			<<"\t speed-up: " << setw(10) << timeSeq / (timeEnd - timeStart) << "\t cost: " << setw(10) << (timeEnd - timeStart) * size
			<< "\t efficiency:" << setw(10) << timeSeq / ((timeEnd - timeStart) * size) << endl << endl;
		;
	}
	MPI_Barrier(MPI_COMM_WORLD);
}

void runControlTests(int rank, int size) {
	int sizes[3] = { 100, 500, 1000 };

	for (int arrSize : sizes)
	{
		int** array = new int* [arrSize];

		for (int i = 0; i < arrSize; i++) {

			array[i] = new int[arrSize];
		}
		if (rank == 0) {
			GenArray(arrSize, array);

			for (int i = 0; i < arrSize; i++)
			{
				MPI_Bcast(&(array[i][0]), arrSize, MPI_INT, 0, MPI_COMM_WORLD);
			}
		}
		else {
			for (int i = 0; i < arrSize; i++)
			{
				MPI_Bcast(&(array[i][0]), arrSize, MPI_INT, 0, MPI_COMM_WORLD);
			}
		}

		MPI_Barrier(MPI_COMM_WORLD);
		TestArray(array, arrSize, rank, size);
	}
}

void runAlgorithmTestWithUserInput(int rank, int size, int arrSize) {

	int** array = new int* [arrSize];

	for (int i = 0; i < arrSize; i++) {

		array[i] = new int[arrSize];
	}
	if (rank == 0) {
		InArray(arrSize, array);

		for (int i = 0; i < arrSize; i++)
		{
			MPI_Bcast(&(array[i][0]), arrSize, MPI_INT, 0, MPI_COMM_WORLD);
		}
	}
	else {
		for (int i = 0; i < arrSize; i++)
		{
			MPI_Bcast(&(array[i][0]), arrSize, MPI_INT, 0, MPI_COMM_WORLD);
		}
	}

	MPI_Barrier(MPI_COMM_WORLD);
	if (rank == 0) {
		OutArray(arrSize, array);
	}

	MPI_Barrier(MPI_COMM_WORLD);
	TestArray(array, arrSize, rank, size);
}

int main()
{
	int rank, size; // standard for each process

	MPI_Init(NULL, NULL); // initialize the MPI world
	MPI_Comm_size(MPI_COMM_WORLD, &size); // get total size of the world
	MPI_Comm_rank(MPI_COMM_WORLD, &rank); // each process gets its own rank

	MPI_Barrier(MPI_COMM_WORLD);
	runAlgorithmTestWithUserInput(rank, size, 5);

	MPI_Barrier(MPI_COMM_WORLD);
	runControlTests(rank, size);

	MPI_Finalize();
	return 0;
}
