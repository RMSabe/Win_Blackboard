/*
	Mini Blackboard Application for Microsoft Windows systems.

	Author: Rafael Sabe
	Email: rafaelmsabe@gmail.com

	Use arrow keys to draw on the blackboard
	Use "esc" or "backspace" to clear the blackboard
*/

#ifndef UNICODE
#define UNICODE
#endif

#include <windows.h>
#include <GL/gl.h>

#define WNDCLASS_NAME (L"MY_WNDCLASS")
#define WINDOW_NAME (L"My Little Blackboard")
#define WINDOW_XPOS 50
#define WINDOW_YPOS 50
#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

#define RUNTIME_STATUS_IDLE (-1L)
#define RUNTIME_STATUS_INIT 0L
#define RUNTIME_STATUS_UPDATEGRAPH 1L

#define GRAPH_UPDATE_UP 1UL
#define GRAPH_UPDATE_DOWN 2UL
#define GRAPH_UPDATE_LEFT 4UL
#define GRAPH_UPDATE_RIGHT 8UL

HINSTANCE runtimeInstance;
LONG runtimeStatus = -1l;

ULONG graphUpdate = 0ul;

HWND mainWindow;
HDC mainWindowDC;
HGLRC mainWindowRC;

VOID WINAPI initialize(VOID);
VOID WINAPI terminate(VOID);

VOID WINAPI registerWndClass(VOID);
VOID WINAPI createMainWindow(VOID);

BOOL WINAPI runtimeContinue(VOID);
LRESULT CALLBACK windowProcedure(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

VOID WINAPI runtimeProcedure(VOID);
VOID WINAPI updateGraphProcedure(VOID);

VOID WINAPI clearScreen(VOID);

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, INT nCmdShow)
{
	runtimeInstance = hInstance;
	initialize();
	while(runtimeContinue()) runtimeProcedure();
	terminate();
	return 0;
}

VOID WINAPI initialize(VOID)
{
	registerWndClass();
	createMainWindow();

	mainWindowRC = wglCreateContext(mainWindowDC);
	wglMakeCurrent(mainWindowDC, mainWindowRC);

	runtimeStatus = RUNTIME_STATUS_INIT;
	return;
}

VOID WINAPI terminate(VOID)
{
	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(mainWindowRC);

	ReleaseDC(mainWindow, mainWindowDC);
	DestroyWindow(mainWindow);

	UnregisterClass(WNDCLASS_NAME, runtimeInstance);
	return;
}

VOID WINAPI registerWndClass(VOID)
{
	WNDCLASS wndclass;
	ZeroMemory(&wndclass, sizeof(WNDCLASS));
	wndclass.hInstance = runtimeInstance;
	wndclass.lpszClassName = WNDCLASS_NAME;
	wndclass.lpfnWndProc = windowProcedure;
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	RegisterClass(&wndclass);
	return;
}

VOID WINAPI createMainWindow(VOID)
{
	UINT windowStyle = (WS_CAPTION | WS_VISIBLE | WS_SYSMENU | WS_OVERLAPPED);
	RECT rect = {
		.left = WINDOW_XPOS,
		.top = WINDOW_YPOS,
		.right = WINDOW_XPOS + WINDOW_WIDTH,
		.bottom = WINDOW_YPOS + WINDOW_HEIGHT
	};

	AdjustWindowRect(&rect, windowStyle, FALSE);
	INT xpos = rect.left;
	INT ypos = rect.top;
	INT width = rect.right - rect.left;
	INT height = rect.bottom - rect.top;

	mainWindow = CreateWindow(WNDCLASS_NAME, WINDOW_NAME, windowStyle, xpos, ypos, width, height, NULL, NULL, runtimeInstance, NULL);
	mainWindowDC = GetDC(mainWindow);

	INT pixelFormat;
	PIXELFORMATDESCRIPTOR pfd;
	ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));

	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = (PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER);
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.iLayerType = PFD_MAIN_PLANE;
	pfd.cColorBits = 24;
	pfd.cDepthBits = 16;

	pixelFormat = ChoosePixelFormat(mainWindowDC, &pfd);
	SetPixelFormat(mainWindowDC, pixelFormat, &pfd);

	return;
}

BOOL WINAPI runtimeContinue(VOID)
{
	MSG msg;
	while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		if(msg.message == WM_QUIT) return FALSE;

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return TRUE;
}

LRESULT CALLBACK windowProcedure(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
		case WM_CLOSE:
			PostQuitMessage(0);
			return 0;

		case WM_KEYDOWN:
			switch(wParam)
			{
				case VK_UP:
					graphUpdate |= GRAPH_UPDATE_UP;
					runtimeStatus = RUNTIME_STATUS_UPDATEGRAPH;
					return 0;

				case VK_DOWN:
					graphUpdate |= GRAPH_UPDATE_DOWN;
					runtimeStatus = RUNTIME_STATUS_UPDATEGRAPH;
					return 0;

				case VK_LEFT:
					graphUpdate |= GRAPH_UPDATE_LEFT;
					runtimeStatus = RUNTIME_STATUS_UPDATEGRAPH;
					return 0;

				case VK_RIGHT:
					graphUpdate |= GRAPH_UPDATE_RIGHT;
					runtimeStatus = RUNTIME_STATUS_UPDATEGRAPH;
					return 0;

				case VK_BACK:
				case VK_ESCAPE:

					clearScreen();
					return 0;
			}
			break;

		case WM_KEYUP:
			if(runtimeStatus == RUNTIME_STATUS_UPDATEGRAPH)
			{
				switch(wParam)
				{
					case VK_UP:
						graphUpdate &= ~GRAPH_UPDATE_UP;
						if(graphUpdate == 0ul) runtimeStatus = RUNTIME_STATUS_IDLE;
						return 0;

					case VK_DOWN:
						graphUpdate &= ~GRAPH_UPDATE_DOWN;
						if(graphUpdate == 0ul) runtimeStatus = RUNTIME_STATUS_IDLE;
						return 0;

					case VK_LEFT:
						graphUpdate &= ~GRAPH_UPDATE_LEFT;
						if(graphUpdate == 0ul) runtimeStatus = RUNTIME_STATUS_IDLE;
						return 0;

					case VK_RIGHT:
						graphUpdate &= ~GRAPH_UPDATE_RIGHT;
						if(graphUpdate == 0ul) runtimeStatus = RUNTIME_STATUS_IDLE;
						return 0;
				}
			}
			break;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

VOID WINAPI runtimeProcedure(VOID)
{
	switch(runtimeStatus)
	{
		case RUNTIME_STATUS_IDLE:
			Sleep(10);
			break;

		case RUNTIME_STATUS_INIT:
			ShowWindow(mainWindow, SW_SHOW);
			clearScreen();
			glPointSize(5.0f);
			glColor3f(1.0f, 1.0f, 1.0f);

			runtimeStatus = RUNTIME_STATUS_IDLE;
			break;

		case RUNTIME_STATUS_UPDATEGRAPH:
			updateGraphProcedure();
			SwapBuffers(mainWindowDC);
			break;
	}

	return;
}

VOID WINAPI updateGraphProcedure(VOID)
{
	static FLOAT x = 0.0f;
	static FLOAT y = 0.0f;

	if((graphUpdate & GRAPH_UPDATE_UP) && (y < 1.0f)) y += 0.01f;

	if((graphUpdate & GRAPH_UPDATE_DOWN) && (y > -1.0f)) y -= 0.01f;

	if((graphUpdate & GRAPH_UPDATE_LEFT) && (x > -1.0f)) x -= 0.005f;

	if((graphUpdate & GRAPH_UPDATE_RIGHT) && (x < 1.0f)) x += 0.005f;

	glBegin(GL_POINTS);
	glVertex2f(x, y);
	glEnd();

	Sleep(1);
	return;
}

VOID WINAPI clearScreen(VOID)
{
	glColor3f(0.0f, 0.0f, 0.0f);

	glBegin(GL_QUADS);
	glVertex2f(-1.0f, 1.0f);
	glVertex2f(1.0f, 1.0f);
	glVertex2f(1.0f, -1.0f);
	glVertex2f(-1.0f, -1.0f);
	glEnd();

	SwapBuffers(mainWindowDC);

	glBegin(GL_QUADS);
	glVertex2f(-1.0f, 1.0f);
	glVertex2f(1.0f, 1.0f);
	glVertex2f(1.0f, -1.0f);
	glVertex2f(-1.0f, -1.0f);
	glEnd();

	SwapBuffers(mainWindowDC);

	glColor3f(1.0f, 1.0f, 1.0f);
	return;
}
