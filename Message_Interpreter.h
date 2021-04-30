#pragma once
#include <windows.h>

//MI stands for Message Interpreter
class MI
{
private:
	INPUT Input = { 0 };	
public:
	void MouseInput(int*);
	void Keyboard_Down(int*);
	void Keyboard_Up(int*);
	void CursorMove(int*);
	void MouseWheel(int*);
	void HideCursor(int*);
	void ShowCursor(int*);
};