// step_func.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>

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

Status axis_param_input(int *lim, int *step, int *num); // lim - theta, step - delta, num - N

int main()
{
	setlocale(LC_ALL, "Russian");

	int N = 0, theta = 0, delta = 0;
	int K = 0, H = 0, rho = 0;
	u_func *U; // ступенчатая функция u(t)
	float *float_dim; // размерность поля
	int dim = 0;
	int Area = 0; // площадь под графиком функции

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

	U = new u_func[N];

	for (int delta_i = 0; delta_i < N; delta_i++)
	{
		U[delta_i].t = delta_i * delta; // значения функции по оси t
		U[delta_i].u = new int[dim - 1]; // массив значений функции в каждой размерности для этого значения времени t
		for (int u_i_j = 0; u_i_j < dim - 1; u_i_j++)
		{
			cout << "Введите значение функции (число клеток от " << -1*K << " до " << K << "): ";
			cin >> U[delta_i].u[u_i_j];
			if ((-1*K <= U[delta_i].u[u_i_j]) && (U[delta_i].u[u_i_j] <= K))
				Area += delta * abs(rho*U[delta_i].u[u_i_j]);
			else
			{
				cout << "Неверный ввод!" << endl;
				system("pause");
				return ERROR;
			}
		}
	}

	cout << "Площадь равна: " << Area << endl;
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

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
