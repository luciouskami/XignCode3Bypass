#include <Windows.h>
#include <Windowsx.h>
#include <stdio.h>

#include <iostream>
#include <memory>

#include "resource.hpp"
#include "XignCode.hpp"

#include "server.hpp"

INT APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, INT nCmdShow)
{
	AllocConsole();
	SetConsoleTitle(L"Full Bypass");
	AttachConsole(GetCurrentProcessId());
	
	FILE* pFile = nullptr;
	freopen_s(&pFile, "CON", "r", stdin);
	freopen_s(&pFile, "CON", "w", stdout);
	freopen_s(&pFile, "CON", "w", stderr);

	if (!XignCode::Initialize(L"C:\\CLOSERS"))
	{
		return 0;
	}
	
	network::server server;
	
	if (!server.initialize(38666))
	{
		return 0;
	}
	
	server.begin_listen();
	return 0;
}