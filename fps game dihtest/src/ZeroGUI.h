#pragma once
#include "ZeroInput.h"
#include "Render.h"
#include "../CppSDK/SDK/Engine_classes.hpp"
#include <Windows.h>
#include <string>
#include <cstdarg>
#include <cstring>
#include <cmath>
#include <algorithm>

static wchar_t* s2wc(const char* c)
{
	const size_t cSize = strlen(c) + 1;
	wchar_t* wc = new wchar_t[cSize];
	mbstowcs_s(nullptr, wc, cSize, c, cSize - 1);
	return wc;
}

static float GetScreenScale()
{
	HDC hdc = GetDC(NULL);
	int dpi  = GetDeviceCaps(hdc, LOGPIXELSX);
	ReleaseDC(NULL, hdc);
	return (float)dpi / 96.0f;
}


namespace ZeroGUI
{
	namespace Colors
	{
		extern SDK::FLinearColor Text;
		extern SDK::FLinearColor Text_Shadow;
		extern SDK::FLinearColor Text_Outline;
		extern SDK::FLinearColor Window_Background;
		extern SDK::FLinearColor Window_Header;
		extern SDK::FLinearColor Button_Idle;
		extern SDK::FLinearColor Button_Hovered;
		extern SDK::FLinearColor Button_Active;
		extern SDK::FLinearColor Checkbox_Idle;
		extern SDK::FLinearColor Checkbox_Hovered;
		extern SDK::FLinearColor Checkbox_Enabled;
		extern SDK::FLinearColor Combobox_Idle;
		extern SDK::FLinearColor Combobox_Hovered;
		extern SDK::FLinearColor Combobox_Elements;
		extern SDK::FLinearColor Slider_Idle;
		extern SDK::FLinearColor Slider_Hovered;
		extern SDK::FLinearColor Slider_Progress;
		extern SDK::FLinearColor Slider_Button;
		extern SDK::FLinearColor ColorPicker_Background;
	}

	namespace PostRenderer
	{
		struct DrawList
		{
			int  type = -1;
			SDK::FVector2D    pos;
			SDK::FVector2D    size;
			SDK::FLinearColor color;
			const char*       name;
			bool              outline;
			SDK::FVector2D    from;
			SDK::FVector2D    to;
			int               thickness;
		};

		extern DrawList drawlist[128];

		void drawFilledRect(SDK::FVector2D pos, float w, float h, SDK::FLinearColor color);
		void TextLeft  (const char* name, SDK::FVector2D pos, SDK::FLinearColor color, bool outline);
		void TextCenter(const char* name, SDK::FVector2D pos, SDK::FLinearColor color, bool outline);
		void Draw_Line (SDK::FVector2D from, SDK::FVector2D to, int thickness, SDK::FLinearColor color);
	}

	extern SDK::UCanvas*   canvas;
	extern bool            hover_element;
	extern SDK::FVector2D  menu_pos;
	extern float           offset_x;
	extern float           offset_y;
	extern SDK::FVector2D  first_element_pos;
	extern SDK::FVector2D  last_element_pos;
	extern SDK::FVector2D  last_element_size;
	extern int             current_element;
	extern SDK::FVector2D  current_element_pos;
	extern SDK::FVector2D  current_element_size;
	extern int             elements_count;
	extern bool            sameLine;
	extern bool            pushY;
	extern float           pushYvalue;
	extern SDK::FVector2D  dragPos;
	extern bool            checkbox_enabled[256];
	extern int             active_hotkey;
	extern bool            already_pressed;
	extern int             active_picker;
	extern SDK::FLinearColor saved_color;

	void SetupCanvas(SDK::UCanvas* _canvas);

	SDK::FVector2D CursorPos();
	bool           MouseInZone(SDK::FVector2D pos, SDK::FVector2D size);

	void Draw_Cursor(bool toggle);

	void SameLine();
	void ClearFirstPos();
	void PushNextElementY(float y, bool from_last_element = true);
	void NextColumn(float x);

	void TextLeft  (const char* name, SDK::FVector2D pos, SDK::FLinearColor color, bool outline);
	void TextCenter(const char* name, SDK::FVector2D pos, SDK::FLinearColor color, bool outline);
	void Draw_Line (SDK::FVector2D from, SDK::FVector2D to, int thickness, SDK::FLinearColor color);
	void drawFilledRect(SDK::FVector2D pos, float w, float h, SDK::FLinearColor color);
	void DrawFilledCircle(SDK::FVector2D pos, float r, SDK::FLinearColor color);
	void DrawCircle(SDK::FVector2D pos, int radius, int numSides, SDK::FLinearColor color);

	bool Window   (const char* name, SDK::FVector2D* pos, SDK::FVector2D size, bool isOpen);
	void Text     (const char* text, bool center = false, bool outline = false);
	bool ButtonTab(const char* name, SDK::FVector2D size, bool active);
	bool Button   (const char* name, SDK::FVector2D size);
	void Checkbox (const char* name, bool* value);
	void SliderInt(const char* name, int*   value, int   min, int   max);
	void SliderFloat(const char* name, float* value, float min, float max, const char* format = "%.0f");
	void Combobox (const char* name, SDK::FVector2D size, int* value, const char* arg, ...);
	void Hotkey   (const char* name, SDK::FVector2D size, int* key);
	void ColorPicker(const char* name, SDK::FLinearColor* color);

	void Render();

}
