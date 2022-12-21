// step_func.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <ctime>

#include <SFML\Graphics.hpp>

#define PI 3.141592
#define PRINTCOMBS 0 // переключатель для печати комбинаций в косоли
#define WIN_W 700
#define WIN_H 700

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
float theta = 0.0, delta = 0.0;
float delta2 = 0.0;

int K = 0;
float H = 0.0, rho = 0.0;

u_func *U; // ступенчатая функция u(t)
float Area = 0; // площадь под графиком функции
unsigned long long int NumOfComb = 0; // число комбинаций. Для проерки правильности алгоритма. Должно быть типа (x+1)^x
float InitValue = 0.0; // число, с которого начинается перебор каждого столбца

ofstream write_file;
char filename[15];

float *x1, *x2;
float x1Prev = 0.0, x2Prev = 0.0; // для отрисовки линий
float b1 = sqrt(2) / 2, b2 = sqrt(2) / 2;
float delta_b1 = 0.0, delta_b2 = 0.0;

Status axis_param_input(float *lim, float *step, int *num); // lim - theta, step - delta, num - N
void go_through_previous(float PervColCurVal);
void count_way(void); // рассчитать управление
float func1(float t_prev, float x_prev);
float func2(float t_prev, float x_prev);
int InitRecountXValue = 1; // с какой точки x[N] начинаем считать новую траекторию (чтобы не с начала)

void draw_axis(void);

sf::RenderWindow window(sf::VideoMode(WIN_W, WIN_H), "Hello, World!", sf::Style::Close);
sf::CircleShape shape(2.0);
sf::Vertex line[2];
sf::Vertex MyPoint;
float OffsetX = WIN_W / 2, OffsetY = WIN_H / 2; // заранее считаем смещение (снова для ускорения)

int main()
{
	window.setFramerateLimit(60);

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
			cout << endl << "H = " << H << " rho = " << rho << " K = " << K << endl;
		}
	}
	else
	{
		system("pause");
		return ERROR;
	}

	// заранее сделаем фиксированные вычисления, чтобы быстрее считались множества достижимости
	delta2 = delta * delta;
	delta_b1 = delta * b1;
	delta_b2 = delta * b2;

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
	shape.setFillColor(sf::Color(0, 255, 0));

	StartTime = clock();
	go_through_previous(InitValue); // аргумент - начальной значение функций в первом отрезке delta
	StopTime = clock();

	write_file.close();

	draw_axis();
	window.display();

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

			#endif // PRINTCOMBS
			NumOfComb++;
			//if (NumOfComb % N == 0)
			if (i > InitValue)
			{
				InitRecountXValue = CurrentCol + 1;
			}
			count_way();
		}
	}
	U[CurrentCol].u[0] = InitValue; // зачищаем для нового перебора на следующей итерации
	InitRecountXValue--; // неизмененные шаги пересчитывать не будем
	CurrentCol--;
}

void count_way(void)
{
	//write_file << x1[0] << " " << x2[0] << endl;
	//cout << " | " << InitRecountXValue << " " << endl;
	for (int i = InitRecountXValue; i <= N; i++)
	{
		//x1[i] = x1[i - 1] + delta * func1(delta * (float)i, sqrt(x1[i - 1] * x1[i - 1] + x2[i - 1] * x2[i - 1])) + delta * U[i - 1].u[0] * b1;
		//x2[i] = x2[i - 1] + delta * func2(delta * (float)i, sqrt(x1[i - 1] * x1[i - 1] + x2[i - 1] * x2[i - 1])) + delta * U[i - 1].u[0] * b2;
		x1[i] = x1[i - 1] + delta2 * (float)i * cos(sqrt(x1[i - 1] * x1[i - 1] + x2[i - 1] * x2[i - 1]) * 0.017453) + delta_b1 * U[i - 1].u[0];
		x2[i] = x2[i - 1] + delta2 * (float)i * sin(sqrt(x1[i - 1] * x1[i - 1] + x2[i - 1] * x2[i - 1]) * 0.017453) + delta_b2 * U[i - 1].u[0];
		//cout << x1[i] << " " << x2[i] << endl;
		//write_file << x1[i] << " " << x2[i] << endl;
	}

	//shape.setPosition(WIN_W / 2 + x1[N]*5.0, WIN_H / 2 - x2[N]*5.0);
	//window.draw(shape);

	MyPoint = sf::Vertex(sf::Vector2f(OffsetX + x1[N]*5.0, OffsetY - x2[N]*5.0), sf::Color::Green);
	window.draw(&MyPoint, 1, sf::Points);

	//if (NumOfComb > 1)
	//{
	//	line[0] = sf::Vertex(sf::Vector2f(WIN_W / 2 + x1Prev * 5.0, WIN_H / 2 - x2Prev * 5.0), sf::Color::Red);
	//	line[1] = sf::Vertex(sf::Vector2f(WIN_W / 2 + x1[N] * 5.0, WIN_H / 2 - x2[N] * 5.0), sf::Color::Red);
	//	window.draw(line, 2, sf::Lines);
	//}

	//x1Prev = x1[N];
	//x2Prev = x2[N];
	
	if (NumOfComb % 1000000 == 0)
	{
		//window.display();
		cout << endl << NumOfComb;
		//write_file << endl << x1[N] << " " << x2[N];
	}
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

void draw_axis(void)
{
	int NCount = 10;

	shape.setFillColor(sf::Color(0, 0, 255));

	for (int i = WIN_W/2; i < WIN_W; i += WIN_W / (2*NCount))
	{
		shape.setPosition(i, WIN_H/2);
		window.draw(shape);
	}

	for (int i = WIN_W / 2; i > 0; i -= WIN_W / (2 * NCount))
	{
		shape.setPosition(i, WIN_H / 2);
		window.draw(shape);
	}

	for (int i = WIN_H / 2; i < WIN_H; i += WIN_H / (2 * NCount))
	{
		shape.setPosition(WIN_W/2, i);
		window.draw(shape);
	}

	for (int i = WIN_H / 2; i > 0; i -= WIN_H / (2 * NCount))
	{
		shape.setPosition(WIN_W / 2, i);
		window.draw(shape);
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
