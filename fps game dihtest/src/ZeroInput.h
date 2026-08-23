#pragma once
#include <Windows.h>

namespace ZeroGUI
{
	namespace Input
	{
		extern bool mouseDown[5];
		extern bool mouseDownAlready[256];
		extern bool keysDown[256];
		extern bool keysDownAlready[256];

		bool IsAnyMouseDown();
		bool IsMouseClicked(int button, int element_id, bool repeat);
		bool IsKeyPressed(int key, bool repeat);
		void Handle();
	}
}
