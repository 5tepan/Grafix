#include <windows.h>
#include <windowsx.h>
#include <tchar.h>
#include <math.h>

TCHAR const CLASS_NAME[] = _T("MainWndClass");
TCHAR const WINDOW_TITLE[] = _T("My first window");

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
int MainLoop();
bool RegisterWndClass(HINSTANCE hInstance);
HWND CreateMainWindow(HINSTANCE hInstance);

enum
{
	ANIMATION_TIMER_ID = 1
};

UINT_PTR g_timerId = 0;

double g_jumpStep = 0;
const long minHeight = 0;
const long maxHeight = 200;
long speedUpF = 1;
long speedUpS = 2;
long speedUpA = 3;

int WINAPI WinMain(
	HINSTANCE hInstance,
	HINSTANCE /*hPrevInstance*/,
	LPSTR /*lpCmdLine*/,
	int nCmdShow)
{
	// Регистрируем класс главного окна
	if (!RegisterWndClass(hInstance))
	{
		return 1;
	}

	// Создаем главное окно приложения
	HWND hMainWindow = CreateMainWindow(hInstance);
	if (hMainWindow == NULL)
	{
		return 1;
	}

	// Показываем главное окно приложения
	ShowWindow(hMainWindow, nCmdShow);
	UpdateWindow(hMainWindow);

	// Запускаем цикл выборки сообщений, пока не получим 
	// сигнал о завершении приложения
	return MainLoop();
}

HWND CreateMainWindow(HINSTANCE hInstance)
{
	HWND hMainWindow = CreateWindowEx(
		0,
		CLASS_NAME,
		WINDOW_TITLE,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT,
		NULL,
		NULL,
		hInstance,
		NULL);

	return hMainWindow;
}

int MainLoop()
{
	MSG msg;
	BOOL res;
	while ((res = GetMessage(&msg, NULL, 0, 0)) != 0)
	{
		if (res == -1)
		{
			// произошла ошибка - нужно обработать ее и, вероятно,
			// завершить работу приложения
		}
		else
		{
			// Если это сообщение о нажатии виртуальной клавиши,
			// то добавляем в очередь сообщений сообщения, несущие информацию о
			// коде вводимого пользователем символа
			TranslateMessage(&msg);
			// передаем сообщение в соответствующую оконную процедуру
			DispatchMessage(&msg);
		}
	}

	// сюда мы попадем только в том случае извлечения сообщения WM_QUIT
	// msg.wParam содержит код возврата, помещенный при помощи функции PostQuitMessage()
	return msg.wParam;
}

BOOL OnCreate(HWND hwnd, LPCREATESTRUCT /*lpCreateStruct*/)
{
	g_timerId = SetTimer(hwnd, ANIMATION_TIMER_ID, 20, NULL);

	return (g_timerId != 0);
}
void Animate(HWND hwnd)
{
	//// определяем промежуток времени (в миллисекундах) с момента предыдущего кадра
	//DWORD currentTick = GetTickCount();

	// Объявляем всю клиентскую область окна недействительной и перерисовываем окно
	InvalidateRect(hwnd, NULL, TRUE);
	UpdateWindow(hwnd);
}
void OnDestroy(HWND hWnd/*hWnd*/)
{
	// уничтожаем ранее созданный таймер
	KillTimer(hWnd, ANIMATION_TIMER_ID);

	PostQuitMessage(0);
}

void OnTimer(HWND hwnd, UINT id)
{
	// Обрабатываем сообщение от таймера
	switch (id)
	{
	case ANIMATION_TIMER_ID:
		Animate(hwnd);
		break;
	}
}

void OnPaint(HWND hwnd)
{
	PAINTSTRUCT ps;
	HDC dc = BeginPaint(hwnd, &ps);

	// создаем красное перо
	HPEN pen = CreatePen(PS_SOLID, 2, RGB(255, 0, 0));
	HPEN oldPen = SelectPen(dc, pen);

	// Создаем кисть и выбираем ее в контексте устройства
	LOGBRUSH brushInfo;
	brushInfo.lbStyle = BS_SOLID;
	brushInfo.lbColor = RGB(255, 255, 255);
	brushInfo.lbHatch = 0;
	HBRUSH brush = CreateBrushIndirect(&brushInfo);

	LOGBRUSH brushInfoF;
	brushInfoF.lbStyle = BS_SOLID;
	brushInfoF.lbColor = RGB(255, 255, 0);
	brushInfoF.lbHatch = 0;
	HBRUSH brushF = CreateBrushIndirect(&brushInfoF);

	LOGBRUSH brushInfoS;
	brushInfoS.lbStyle = BS_SOLID;
	brushInfoS.lbColor = RGB(100, 100, 100);
	brushInfoS.lbHatch = 0;
	HBRUSH brushS = CreateBrushIndirect(&brushInfoS);

	LOGBRUSH brushInfoA;
	brushInfoA.lbStyle = BS_SOLID;
	brushInfoA.lbColor = RGB(0, 255, 255);
	brushInfoA.lbHatch = 0;
	HBRUSH brushA = CreateBrushIndirect(&brushInfoA);

	// рисуем 
	int x = 10, y = maxHeight + 10, l = 50, h = 100, w = 15;
	g_jumpStep += 1;
	long up;
	// Ф
	double tF = sqrt(2 * maxHeight / speedUpF);
	double g_jumpStepF = fmod(g_jumpStep, tF);
	if ((double)fmod(g_jumpStep, tF * 2) == g_jumpStepF)
	{
		up = 1;
		y = maxHeight + 10;
	}
	else
	{
		up = -1;
		y = 10;
	}
	y -= speedUpF * g_jumpStepF * g_jumpStepF / 2 * up;
	HBRUSH oldBrush = SelectBrush(dc, brushF);
	POINT ptF[8] = { POINT{x + l, y}, POINT{x, y}, POINT{x, y + h / 2}, POINT{x + l / 2 - w / 2, y + h / 2},
		POINT{x + l / 2 - w / 2, y + h }, POINT{x + l / 2 + w / 2, y + h}, POINT{x + l / 2 + w / 2, y + h / 2}, POINT{x + l, y + h / 2} };
	Polygon(dc, ptF, sizeof(ptF) / sizeof(ptF[0]));

	SelectBrush(dc, brush);
	POINT ptF1[4] = { POINT{x + l - w, y + w}, POINT{x + l - w, y + w + w}, POINT{x + l - w - w / 2, y + w + w} , POINT{x + l - w - w / 2, y + w } };
	Polygon(dc, ptF1, sizeof(ptF1) / sizeof(ptF1[0]));
	POINT ptF2[4] = { POINT{x + w, y + w}, POINT{x + w, y + w + w}, POINT{x + w + w / 2, y + w + w} , POINT{x + w + w / 2, y + w } };
	Polygon(dc, ptF2, sizeof(ptF2) / sizeof(ptF2[0]));
	// С
	x = x + l + 20;
	double tS = sqrt(2 * maxHeight / speedUpS);
	double g_jumpStepS = fmod(g_jumpStep, tS);
	if ((double)fmod(g_jumpStep, tS * 2) == g_jumpStepS)
	{
		up = 1;
		y = maxHeight + 10;
	}
	else
	{
		up = -1;
		y = 10;
	}
	y -= speedUpS * g_jumpStepS * g_jumpStepS / 2 * up;
	SelectBrush(dc, brushS);
	POINT ptS[8] = { POINT{x + l, y}, POINT{x, y}, POINT{x, y + h}, POINT{x + l, y + h},
		POINT{x + l, y + h - w}, POINT{x + w, y + h - w}, POINT{x + w, y + w}, POINT{x + l, y + w} };
	Polygon(dc, ptS, sizeof(ptS) / sizeof(ptS[0]));

	// A
	x = x + l + 20;
	double tA = sqrt(2 * maxHeight / speedUpA);
	double g_jumpStepA = fmod(g_jumpStep, tA);
	if ((double)fmod(g_jumpStep, tA * 2) == g_jumpStepA)
	{
		up = 1;
		y = maxHeight + 10;
	}
	else
	{
		up = -1;
		y = 10;
	}
	y -= speedUpA * g_jumpStepA * g_jumpStepA / 2 * up;
	SelectBrush(dc, brushA);
	POINT ptA[7] = { POINT{x + l / 2, y}, POINT{x, y + h}, POINT{x + w, y + h}, POINT{x + l / 2 - w / 2, y + h * 2 / 3},
		POINT{x + l / 2 + w / 2, y + h * 2 / 3}, POINT{x + l - w, y + h}, POINT{x + l , y + h} };
	Polygon(dc, ptA, sizeof(ptA) / sizeof(ptA[0]));
	SelectBrush(dc, brush);
	POINT ptA1[3] = { POINT{x + l / 2, y + h * 1 / 3}, POINT{x + l / 2 - w / 2, y + h * 2 / 3 - w }, POINT{x + l / 2 + w / 2, y + h * 2 / 3 - w} };
	Polygon(dc, ptA1, sizeof(ptA1) / sizeof(ptA1[0]));
	// Восстанавливаем ранее выбранное перо и удаляем наше
	SelectPen(dc, oldPen);
	DeletePen(pen);

//задание 2
	x = 10;
	y = 500;
	w = 200;
	Rectangle(dc, x, y, x + w, y + w);//стена
	Rectangle(dc, x + w / 4, y + w / 4, x + w / 2, y + w / 2);//окно
	Rectangle(dc, x + w * 3 / 4, y + w / 4, x + w, y + w);//дверь
	POINT p1[3] = { POINT{x - 5, y + 5 }, POINT{x + w / 2, y - w }, POINT{x + w + 5 , y + 5} };//крыша
	Polyline(dc, p1, sizeof(p1) / sizeof(p1[0]));
	POINT p2[4] = { POINT{x + w * 3 / 4, y - w * 1 / 2 }, POINT{x + w * 3 / 4, y - w }, POINT{x + w * 4 / 5 ,y - w}, POINT{x + w * 4 / 5 ,y - w * 1 / 2 + 10} };//труба
	Polyline(dc, p2, sizeof(p2) / sizeof(p2[0]));

	int i;//забор
	for (i = 0; i < 10; i++)
	{
		POINT p3[4] = { POINT{x + w + i * 10, y + w}, POINT{x + w + i * 10, y + w / 4},POINT{x + w + i * 10 + 5, y + w / 4 - 5}, POINT{x + w + (i + 1) * 10, y + w / 4} };
		Polyline(dc, p3, sizeof(p3) / sizeof(p3[0]));
	}
	// Восстанавливаем ранее выбранную кисть и удаляем нашу
	SelectBrush(dc, oldBrush);
	DeleteBrush(brush);
	DeleteBrush(brushF);
	DeleteBrush(brushS);
	DeleteBrush(brushA);

	EndPaint(hwnd, &ps);
}

LRESULT CALLBACK WindowProc(
	HWND hwnd,
	UINT uMsg,
	WPARAM wParam,
	LPARAM lParam)
{
	switch (uMsg)
	{
		HANDLE_MSG(hwnd, WM_DESTROY, OnDestroy);
		HANDLE_MSG(hwnd, WM_PAINT, OnPaint);
		HANDLE_MSG(hwnd, WM_TIMER, OnTimer);
		HANDLE_MSG(hwnd, WM_CREATE, OnCreate);
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

bool RegisterWndClass(HINSTANCE hInstance)
{
	WNDCLASSEX wndClass =
	{
		sizeof(wndClass),	//UINT cbSize;
		CS_HREDRAW | CS_VREDRAW, //UINT style;
		&WindowProc,		//WNDPROC lpfnWndProc;
		0,					//int cbClsExtra;
		0,					//int cbWndExtra;
		hInstance,			//HINSTANCE hInstance;
		NULL,				//HICON hIcon;
		LoadCursor(NULL, IDC_ARROW), //HCURSOR hCursor;
		(HBRUSH)(COLOR_BTNFACE + 1), //HBRUSH hbrBackground;
		NULL,				//LPCTSTR lpszMenuName;
		CLASS_NAME,			//LPCTSTR lpszClassName;
		NULL,				//HICON hIconSm;
	};

	return RegisterClassEx(&wndClass) != FALSE;
}
