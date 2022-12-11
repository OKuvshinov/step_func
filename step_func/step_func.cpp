// step_func.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <ctime>

#define PI 3.141592
#define PRINTCOMBS 0 // переключатель для печати комбинаций в косоли

using namespace std;

typedef enum
{
	ERROR,
	OK
} Status;

typedef struct
{
	float *u;
	float t;
} u_func;

int CurrentCol = 0; // текущий столбец перебора

int N = 0;
float theta = 0, delta = 0;

int K = 0;
float H = 0, rho = 0;

u_func *U; // ступенчатая функция u(t)
float Area = 0; // площадь под графиком функции
int NumOfComb = 0; // число комбинаций. Для проерки правильности алгоритма. Должно быть типа (x+1)^x
float InitValue = 0.0; // число, с которого начинается перебор каждого столбца

ofstream write_file;
char filename[15];

float *x1, *x2;
float b1 = sqrt(2) / 2, b2 = sqrt(2) / 2;

Status axis_param_input(float *lim, float *step, int *num); // lim - theta, step - delta, num - N
void go_through_previous(float PervColCurVal);
void count_way(void); // рассчитать управление
float func1(float t_prev, float x_prev);
float func2(float t_prev, float x_prev);

int main()
{
	float q = cos(45*PI/180);
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
		cout << endl << "theta = " << theta << " delta = " << delta << " N = " << N << endl;
		if (axis_param_input(&H, &rho, &K) != OK)
		{
			system("pause");
			return ERROR;
		}
		else
		{
			cout << endl << "theta = " << H << " delta = " << rho << " N = " << K << endl;
		}
	}
	else
	{
		system("pause");
		return ERROR;
	}

	InitValue = -1.0*rho;

	// массивы координат управляемой точки
	x1 = new float[N + 1];
	x2 = new float[N + 1];

	// начальные значение управляемой точки
	x1[0] = 0.0;
	x2[0] = 0.0;

	// выделим память для узлов сетки поля
	U = new u_func[N];
	for (int i = 0; i < N; i++)
	{
		U[i].u = new float[1]; // число размерностей по оси y
		U[i].u[0] = InitValue; // сделаем все нулями изначально
		U[i].t = (float)i * delta; // значения по оси x
	}

	sprintf(filename, "%.2fx%.2f.txt", N, K);
	write_file.open(filename);

	StartTime = clock();
	go_through_previous(InitValue); // аргумент - начальной значение функций в первом отрезке delta
	StopTime = clock();

	write_file.close();

	cout << endl << "Число комбинаций: " << NumOfComb << endl;

	Time = (float)(StopTime - StartTime) / 1000.0;
	cout << endl << "Время работы: " << Time << " с." << endl;

	delete U;
	delete x1;
	delete x2;

	system("pause");
	return OK;
}

Status axis_param_input(float *lim, float *step, int *num)
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

	//if (float_lim - (int)float_lim != 0)
	//{
	//	cout << "Неверный ввод!" << endl;
	//	return ERROR;
	//}

	//if ((float_num > 0) && (float_num <= float_lim))
		float_step = float_lim / float_num;
	//else
	//{
	//	cout << "Неверный ввод!" << endl;
	//	return ERROR;
	//}

	/*if (float_step - (int)float_step != 0)
	{
		cout << "Неверный ввод!" << endl;
		return ERROR;
	}
	else
	{*/
		//*lim = (int)float_lim;
		//*step = (int)float_step;
		//*num = (int)float_num;

		*lim = float_lim;
		*step = float_step;
		*num = (int)float_num;
		return OK;
	//}
}

// для каждого вызова функции крайний левый слолбец будет изменяться (смещаться вправо)
void go_through_previous(float PervColCurVal)
{
	for (float i = PervColCurVal; i <= H; i+=2.0*rho) // перебираем значения текущего столбца
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
			//Area = 0;
			U[CurrentCol].u[0] = i;
			#if PRINTCOMBS == 1
				for (int j = 0; j < N; j++)
				{
					cout << U[j].u[0] << " ";
					//	//Area += U[j].u[0];
				}
				cout << endl << endl;
			#endif // PRINTCOMBS
			NumOfComb++;		
			count_way();
			//cout << " Площадь: " << Area << endl;
			//write_file << Area << "\n";
		}
	}
	U[CurrentCol].u[0] = InitValue; // зачищаем для нового перебора на следующей итерации
	CurrentCol--;
}

void count_way(void)
{
	// задаем новые начальные занчения
	x1[0] = 0;
	x2[0] = 0;

	//write_file << x1[0] << " " << x2[0] << endl;
	for (int i = 1; i <= N; i++)
	{
		x1[i] = x1[i - 1] + delta * func1(delta*(float)i, sqrt(pow(x1[i - 1], 2) + pow(x2[i - 1], 2))) + delta * U[i - 1].u[0] * b1;
		x2[i] = x2[i - 1] + delta * func2(delta*(float)i, sqrt(pow(x1[i - 1], 2) + pow(x2[i - 1], 2))) + delta * U[i - 1].u[0] * b2;
		//cout << x1[i] << " " << x2[i] << endl;
		//write_file << x1[i] << " " << x2[i] << endl;
	}
	write_file << endl << x1[N] << " " << x2[N];
	//cout << x1[N] << " " << x2[N] << endl;
}

float func1(float t_prev, float x_prev)
{
	return t_prev * cos(x_prev*PI/180);
}
float func2(float t_prev, float x_prev)
{
	return t_prev * sin(x_prev*PI/180);
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
