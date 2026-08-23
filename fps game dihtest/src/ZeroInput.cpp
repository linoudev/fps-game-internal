#include "ZeroInput.h"

namespace ZeroGUI
{
	namespace Input
	{
		bool mouseDown[5]        = {};
		bool mouseDownAlready[256] = {};
		bool keysDown[256]       = {};
		bool keysDownAlready[256]= {};

		bool IsAnyMouseDown()
		{
			return mouseDown[0] || mouseDown[1] || mouseDown[2] || mouseDown[3] || mouseDown[4];
		}

		bool IsMouseClicked(int button, int element_id, bool repeat)
		{
			if (mouseDown[button])
			{
				if (!mouseDownAlready[element_id]) { mouseDownAlready[element_id] = true; return true; }
				if (repeat) return true;
			}
			else mouseDownAlready[element_id] = false;
			return false;
		}

		bool IsKeyPressed(int key, bool repeat)
		{
			if (keysDown[key])
			{
				if (!keysDownAlready[key]) { keysDownAlready[key] = true; return true; }
				if (repeat) return true;
			}
			else keysDownAlready[key] = false;
			return false;
		}

		void Handle()
		{
			mouseDown[0] = (GetAsyncKeyState(0x01) != 0);
		}
	}
}
