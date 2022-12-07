// step_func.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <ctime>

using namespace std;

typedef enum
{
	ERROR,
	OK
} Status;

typedef struct
{
	int *u;
	int t;
} u_func;

int CurrentCol = 0; // текущий столбец перебора
int N = 0, theta = 0, delta = 0;
int K = 0, H = 0, rho = 0;
u_func *U; // ступенчатая функция u(t)
int Area = 0; // площадь под графиком функции
int NumOfComb = 0; // число комбинаций. Для проерки правильности алгоритма. Должно быть типа (x+1)^x
ofstream write_file;
char filename[15];

Status axis_param_input(int *lim, int *step, int *num); // lim - theta, step - delta, num - N
void go_through_previous(int PervColCurVal);

int main()
{
	setlocale(LC_ALL, "Russian");

	float *float_dim; // размерность поля
	int dim = 0;
	int StartTime = 0, StopTime = 0;
	float Time = 0.0;

	float_dim = new float;
	cout << "Введите размерность: ";
	cin >> *float_dim;

	if ((*float_dim < 2.0) || (*float_dim - (int)*float_dim != 0))
	{
		cout << "Неверный ввод!" << endl;
		system("pause");
		return ERROR;
	}

	dim = (int)*float_dim;
	delete float_dim;

	if (axis_param_input(&theta, &delta, &N) == OK)
	{
		if (axis_param_input(&H, &rho, &K) != OK)
		{
			system("pause");
			return ERROR;
		}
	}
	else
	{
		system("pause");
		return ERROR;
	}

	// выделим память для узлов сетки поля
	U = new u_func[N];
	for (int i = 0; i < N; i++)
	{
		U[i].u = new int[1]; // число размерностей по оси y
		U[i].u[0] = 0; // сделаем все нулями изначально
		U[i].t = i * delta; // значения по оси x
	}

	sprintf(filename, "%dx%d.txt", N, K);
	write_file.open(filename);

	StartTime = clock();
	go_through_previous(0); // аргумент - начальной значение функций в первом отрезке delta
	StopTime = clock();

	write_file.close();

	cout << endl << "Число комбинаций: " << NumOfComb << endl;

	Time = (float)(StopTime - StartTime) / 1000.0;
	cout << endl << "Время работы: " << Time << " с." << endl;

	system("pause");
	return OK;
}

Status axis_param_input(int *lim, int *step, int *num)
{
	float float_lim = 0.0, float_step = 0.0, float_num = 0.0;

	cout << "Введите N: ";
	cin >> float_num;

	if (float_num - (int)float_num != 0)
	{
		cout << "Неверный ввод!" << endl;
		return ERROR;
	}

	cout << "Введите theta: ";
	cin >> float_lim;

	if (float_lim - (int)float_lim != 0)
	{
		cout << "Неверный ввод!" << endl;
		return ERROR;
	}

	if ((float_num > 0) && (float_num <= float_lim))
		float_step = float_lim / float_num;
	else
	{
		cout << "Неверный ввод!" << endl;
		return ERROR;
	}

	if (float_step - (int)float_step != 0)
	{
		cout << "Неверный ввод!" << endl;
		return ERROR;
	}
	else
	{
		*lim = (int)float_lim;
		*step = (int)float_step;
		*num = (int)float_num;
		return OK;
	}
}

// для каждого вызова функции крайний левый слолбец будет изменяться (смещаться вправо)
void go_through_previous(int PervColCurVal)
{
	for (int i = PervColCurVal; i <= K; i++)
	{
		U[CurrentCol].u[0] = i;
		// двигаемся в самый правый столбец
		if (CurrentCol < N - 1)
		{
			CurrentCol++;
			go_through_previous(U[CurrentCol].u[0]);
		}

		// когда дошли, перебираем значения
		else
		{
			Area = 0;
			U[CurrentCol].u[0] = i;
			for (int j = 0; j < N; j++)
			{
				cout << U[j].u[0];
				Area += U[j].u[0];
			}
			NumOfComb++;
			cout << " Площадь: " << Area << endl;
			write_file << Area << "\n";
		}
	}
	U[CurrentCol].u[0] = 0; // зачищаем для нового перебора на следующей итерации
	CurrentCol--;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
