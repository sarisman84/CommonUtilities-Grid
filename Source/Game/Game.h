#pragma once
#include <fstream>
#include "GameWorld.h"
#include "Custom/Input.h"

using namespace CommonUtilities;

class Game
{
public:
	Game();
	~Game();
	bool Init(const std::wstring& aVersion = L"", HWND aHWND = nullptr);
private:
	void InitCallBack();
	void UpdateCallBack();
	LRESULT WinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	GameWorld myGameWorld;

};
