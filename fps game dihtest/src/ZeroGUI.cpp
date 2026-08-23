#include "ZeroGUI.h"

namespace ZeroGUI
{
	namespace Colors
	{
		SDK::FLinearColor Text                   { 1.0f,  1.0f,  1.0f,  1.0f };
		SDK::FLinearColor Text_Shadow            { 0.0f,  0.0f,  0.0f,  0.0f };
		SDK::FLinearColor Text_Outline           { 0.0f,  0.0f,  0.0f,  0.30f };
		SDK::FLinearColor Window_Background      { 0.009f,0.009f,0.009f,1.0f  };
		SDK::FLinearColor Window_Header          { 0.10f, 0.15f, 0.84f, 1.0f  };
		SDK::FLinearColor Button_Idle            { 0.10f, 0.15f, 0.84f, 1.0f  };
		SDK::FLinearColor Button_Hovered         { 0.15f, 0.20f, 0.89f, 1.0f  };
		SDK::FLinearColor Button_Active          { 0.20f, 0.25f, 0.94f, 1.0f  };
		SDK::FLinearColor Checkbox_Idle          { 0.17f, 0.16f, 0.23f, 1.0f  };
		SDK::FLinearColor Checkbox_Hovered       { 0.22f, 0.30f, 0.72f, 1.0f  };
		SDK::FLinearColor Checkbox_Enabled       { 0.20f, 0.25f, 0.94f, 1.0f  };
		SDK::FLinearColor Combobox_Idle          { 0.17f, 0.16f, 0.23f, 1.0f  };
		SDK::FLinearColor Combobox_Hovered       { 0.17f, 0.16f, 0.23f, 1.0f  };
		SDK::FLinearColor Combobox_Elements      { 0.239f,0.42f, 0.82f, 1.0f  };
		SDK::FLinearColor Slider_Idle            { 0.17f, 0.16f, 0.23f, 1.0f  };
		SDK::FLinearColor Slider_Hovered         { 0.17f, 0.16f, 0.23f, 1.0f  };
		SDK::FLinearColor Slider_Progress        { 0.22f, 0.30f, 0.72f, 1.0f  };
		SDK::FLinearColor Slider_Button          { 0.10f, 0.15f, 0.84f, 1.0f  };
		SDK::FLinearColor ColorPicker_Background { 0.006f,0.006f,0.006f,1.0f  };
	}

	namespace PostRenderer
	{
		DrawList drawlist[128];

		void drawFilledRect(SDK::FVector2D pos, float w, float h, SDK::FLinearColor color)
		{
			for (int i = 0; i < 128; i++)
				if (drawlist[i].type == -1) { drawlist[i] = { 1, pos, {w,h}, color }; return; }
		}
		void TextLeft(const char* name, SDK::FVector2D pos, SDK::FLinearColor color, bool outline)
		{
			for (int i = 0; i < 128; i++)
				if (drawlist[i].type == -1) { drawlist[i].type=2; drawlist[i].name=name; drawlist[i].pos=pos; drawlist[i].outline=outline; drawlist[i].color=color; return; }
		}
		void TextCenter(const char* name, SDK::FVector2D pos, SDK::FLinearColor color, bool outline)
		{
			for (int i = 0; i < 128; i++)
				if (drawlist[i].type == -1) { drawlist[i].type=3; drawlist[i].name=name; drawlist[i].pos=pos; drawlist[i].outline=outline; drawlist[i].color=color; return; }
		}
		void Draw_Line(SDK::FVector2D from, SDK::FVector2D to, int thickness, SDK::FLinearColor color)
		{
			for (int i = 0; i < 128; i++)
				if (drawlist[i].type == -1) { drawlist[i].type=4; drawlist[i].from=from; drawlist[i].to=to; drawlist[i].thickness=thickness; drawlist[i].color=color; return; }
		}
	}

	SDK::UCanvas*    canvas               = nullptr;
	bool             hover_element        = false;
	SDK::FVector2D   menu_pos             = { 0, 0 };
	float            offset_x             = 0.0f;
	float            offset_y             = 0.0f;
	SDK::FVector2D   first_element_pos    = { 0, 0 };
	SDK::FVector2D   last_element_pos     = { 0, 0 };
	SDK::FVector2D   last_element_size    = { 0, 0 };
	int              current_element      = -1;
	SDK::FVector2D   current_element_pos  = { 0, 0 };
	SDK::FVector2D   current_element_size = { 0, 0 };
	int              elements_count       = 0;
	bool             sameLine             = false;
	bool             pushY                = false;
	float            pushYvalue           = 0.0f;
	SDK::FVector2D   dragPos              = { 0, 0 };
	bool             checkbox_enabled[256]= {};
	int              active_hotkey        = -1;
	bool             already_pressed      = false;
	int              active_picker        = -1;
	SDK::FLinearColor saved_color         = {};


	void SetupCanvas(SDK::UCanvas* _canvas) { canvas = _canvas; }

	SDK::FVector2D CursorPos()
	{
		POINT p; GetCursorPos(&p);
		float dpi = GetScreenScale();
		return { (float)p.x / dpi, (float)p.y / dpi };
	}

	bool MouseInZone(SDK::FVector2D pos, SDK::FVector2D size)
	{
		SDK::FVector2D cp = CursorPos();
		return cp.X > pos.X && cp.Y > pos.Y &&
		       cp.X < pos.X + size.X && cp.Y < pos.Y + size.Y;
	}

	void Draw_Cursor(bool toggle)
	{
		if (!toggle) return;
		SDK::FVector2D cp = CursorPos();
		SDK::FLinearColor c{ 0.30f, 0.30f, 0.80f, 1.0f };
		canvas->K2_DrawLine({ cp.X, cp.Y }, { cp.X+35, cp.Y+10 }, 1, c);
		int x=35, y=10;
		while (y!=30) { x-=1; if(x<15)x=15; y+=1; if(y>30)y=30;
			canvas->K2_DrawLine({ cp.X, cp.Y }, { cp.X+x, cp.Y+y }, 1, c); }
		canvas->K2_DrawLine({ cp.X,    cp.Y    }, { cp.X+15, cp.Y+30 }, 1, c);
		canvas->K2_DrawLine({ cp.X+35, cp.Y+10 }, { cp.X+15, cp.Y+30 }, 1, c);
	}

	void SameLine()      { sameLine = true; }
	void ClearFirstPos() { first_element_pos = { 0,0 }; }

	void PushNextElementY(float y, bool from_last_element)
	{
		pushY = true;
		pushYvalue = from_last_element ? last_element_pos.Y + last_element_size.Y + y : y;
	}
	void NextColumn(float x) { offset_x = x; PushNextElementY(first_element_pos.Y, false); }

	void TextLeft(const char* name, SDK::FVector2D pos, SDK::FLinearColor color, bool)
	{
		int len = (int)strlen(name)+1;
		canvas->K2_DrawText(Font::defaultFont, SDK::FString{s2wc(name),len,len}, pos,
			{0.97f,0.97f}, color, false, Colors::Text_Shadow, {pos.X+1,pos.Y+1},
			false, true, true, Colors::Text_Outline);
	}
	void TextCenter(const char* name, SDK::FVector2D pos, SDK::FLinearColor color, bool)
	{
		int len = (int)strlen(name)+1;
		canvas->K2_DrawText(Font::defaultFont, SDK::FString{s2wc(name),len,len}, pos,
			{0.97f,0.97f}, color, false, Colors::Text_Shadow, {pos.X+1,pos.Y+1},
			true, true, true, Colors::Text_Outline);
	}
	void Draw_Line(SDK::FVector2D from, SDK::FVector2D to, int thickness, SDK::FLinearColor color)
	{
		canvas->K2_DrawLine(from, to, (float)thickness, color);
	}
	void drawFilledRect(SDK::FVector2D p, float w, float h, SDK::FLinearColor color)
	{
		for (float i=0.f; i<h; i+=1.f)
			canvas->K2_DrawLine({p.X,p.Y+i},{p.X+w,p.Y+i},1.f,color);
	}
	void DrawFilledCircle(SDK::FVector2D pos, float r, SDK::FLinearColor color)
	{
		const float PI=3.14159265359f, s=0.07f;
		for (float a=0.f; a<2.f*PI; a+=s)
			Draw_Line({pos.X,pos.Y},{pos.X+cosf(a)*r,pos.Y+sinf(a)*r},1,color);
	}
	void DrawCircle(SDK::FVector2D pos, int radius, int numSides, SDK::FLinearColor color)
	{
		const float PI=3.1415927f, Step=PI*2.f/numSides;
		SDK::FVector2D V[128]; int Count=0;
		for (float a=0; a<PI*2.f; a+=Step, Count+=2)
		{
			V[Count  ]={radius*cosf(a)     +pos.X, radius*sinf(a)     +pos.Y};
			V[Count+1]={radius*cosf(a+Step)+pos.X, radius*sinf(a+Step)+pos.Y};
			Draw_Line(V[Count], V[Count+1], 1, color);
		}
	}

	bool Window(const char* name, SDK::FVector2D* pos, SDK::FVector2D size, bool isOpen)
	{
		elements_count = 0;
		if (!isOpen) return false;

		bool isHovered = MouseInZone(*pos, size);
		if (current_element != -1 && !GetAsyncKeyState(0x01)) current_element = -1;

		if (hover_element && GetAsyncKeyState(0x01)) {}
		else if ((isHovered || dragPos.X != 0) && !hover_element)
		{
			if (Input::IsMouseClicked(0, elements_count, true))
			{
				SDK::FVector2D cp = CursorPos();
				cp.X -= size.X; cp.Y -= size.Y;
				if (dragPos.X == 0) { dragPos.X = cp.X-pos->X; dragPos.Y = cp.Y-pos->Y; }
				pos->X = cp.X-dragPos.X; pos->Y = cp.Y-dragPos.Y;
			}
			else dragPos = {0,0};
		}
		else hover_element = false;

		offset_x=0; offset_y=0; menu_pos=*pos;
		first_element_pos=current_element_pos=current_element_size={0,0};

		drawFilledRect(*pos, size.X, size.Y, Colors::Window_Background);
		drawFilledRect(*pos, size.X, 25.f,   Colors::Window_Header);
		offset_y += 25.f;
		TextCenter(name, {pos->X+size.X/2, pos->Y+25.f/2}, {1,1,1,1}, false);
		return true;
	}

	void Text(const char* text, bool center, bool outline)
	{
		elements_count++;
		const float sz=25.f;
		SDK::FVector2D pad{10,10}, pos{menu_pos.X+pad.X+offset_x, menu_pos.Y+pad.Y+offset_y};
		if (sameLine) { pos.X=last_element_pos.X+last_element_size.X+pad.X; pos.Y=last_element_pos.Y; }
		if (pushY) { pos.Y=pushYvalue; pushY=false; pushYvalue=0; offset_y=pos.Y-menu_pos.Y; }
		if (!sameLine) offset_y+=sz+pad.Y;
		SDK::FVector2D tp{pos.X+5.f, pos.Y+sz/2};
		if (center) TextCenter(text,tp,{1,1,1,1},outline); else TextLeft(text,tp,{1,1,1,1},outline);
		sameLine=false; last_element_pos=pos;
		if (first_element_pos.X==0.f) first_element_pos=pos;
	}

	bool ButtonTab(const char* name, SDK::FVector2D size, bool active)
	{
		elements_count++;
		SDK::FVector2D pad{5,10}, pos{menu_pos.X+pad.X+offset_x, menu_pos.Y+pad.Y+offset_y};
		if (sameLine) { pos.X=last_element_pos.X+last_element_size.X+pad.X; pos.Y=last_element_pos.Y; }
		if (pushY) { pos.Y=pushYvalue; pushY=false; pushYvalue=0; offset_y=pos.Y-menu_pos.Y; }
		bool h=MouseInZone(pos,size);
		if      (active) drawFilledRect(pos,size.X,size.Y,Colors::Button_Active);
		else if (h)      { drawFilledRect(pos,size.X,size.Y,Colors::Button_Hovered); hover_element=true; }
		else             drawFilledRect(pos,size.X,size.Y,Colors::Button_Idle);
		if (!sameLine) offset_y+=size.Y+pad.Y;
		TextCenter(name,{pos.X+size.X/2,pos.Y+size.Y/2},{1,1,1,1},false);
		sameLine=false; last_element_pos=pos; last_element_size=size;
		if (first_element_pos.X==0.f) first_element_pos=pos;
		return h && Input::IsMouseClicked(0,elements_count,false);
	}


	bool Button(const char* name, SDK::FVector2D size)
	{
		elements_count++;
		SDK::FVector2D pad{5,10}, pos{menu_pos.X+pad.X+offset_x, menu_pos.Y+pad.Y+offset_y};
		if (sameLine) { pos.X=last_element_pos.X+last_element_size.X+pad.X; pos.Y=last_element_pos.Y; }
		if (pushY) { pos.Y=pushYvalue; pushY=false; pushYvalue=0; offset_y=pos.Y-menu_pos.Y; }
		bool h=MouseInZone(pos,size);
		if (h) { drawFilledRect(pos,size.X,size.Y,Colors::Button_Hovered); hover_element=true; }
		else     drawFilledRect(pos,size.X,size.Y,Colors::Button_Idle);
		if (!sameLine) offset_y+=size.Y+pad.Y;
		TextCenter(name,{pos.X+size.X/2,pos.Y+size.Y/2},{1,1,1,1},false);
		sameLine=false; last_element_pos=pos; last_element_size=size;
		if (first_element_pos.X==0.f) first_element_pos=pos;
		return h && Input::IsMouseClicked(0,elements_count,false);
	}

	void Checkbox(const char* name, bool* value)
	{
		elements_count++;
		const float sz=18.f;
		SDK::FVector2D pad{10,10}, pos{menu_pos.X+pad.X+offset_x, menu_pos.Y+pad.Y+offset_y};
		if (sameLine) { pos.X=last_element_pos.X+last_element_size.X+pad.X; pos.Y=last_element_pos.Y; }
		if (pushY) { pos.Y=pushYvalue; pushY=false; pushYvalue=0; offset_y=pos.Y-menu_pos.Y; }
		bool h=MouseInZone(pos,{sz,sz});
		if (h) { drawFilledRect(pos,sz,sz,Colors::Checkbox_Hovered); hover_element=true; }
		else     drawFilledRect(pos,sz,sz,Colors::Checkbox_Idle);
		if (!sameLine) offset_y+=sz+pad.Y;
		if (*value) drawFilledRect({pos.X+3,pos.Y+3},sz-6,sz-6,Colors::Checkbox_Enabled);
		TextLeft(name,{pos.X+sz+5.f,pos.Y+sz/2},{1,1,1,1},false);
		sameLine=false; last_element_pos=pos;
		if (first_element_pos.X==0.f) first_element_pos=pos;
		if (h && Input::IsMouseClicked(0,elements_count,false)) *value=!*value;
	}

	void SliderInt(const char* name, int* value, int mn, int mx)
	{
		elements_count++;
		SDK::FVector2D size{240,50}, sl{200,10}, pad{10,15};
		SDK::FVector2D pos{menu_pos.X+pad.X+offset_x, menu_pos.Y+pad.Y+offset_y};
		if (sameLine) { pos.X=last_element_pos.X+last_element_size.X+pad.X; pos.Y=last_element_pos.Y; }
		if (pushY) { pos.Y=pushYvalue; pushY=false; pushYvalue=0; offset_y=pos.Y-menu_pos.Y; }
		bool h=MouseInZone({pos.X,pos.Y+sl.Y+pad.Y},{sl.X,sl.Y});
		if (!sameLine) offset_y+=size.Y+pad.Y;
		if (h || current_element==elements_count)
		{
			if (Input::IsMouseClicked(0,elements_count,true))
			{
				current_element=elements_count;
				float cx=CursorPos().X;
				*value=(int)(((cx-pos.X)*((mx-mn)/sl.X))+mn);
				if(*value<mn)*value=mn; if(*value>mx)*value=mx;
			}
			drawFilledRect({pos.X,pos.Y+sl.Y+pad.Y},sl.X,sl.Y,Colors::Slider_Hovered);
			hover_element=true;
		}
		else drawFilledRect({pos.X,pos.Y+sl.Y+pad.Y},sl.X,sl.Y,Colors::Slider_Idle);
		drawFilledRect({pos.X,pos.Y+sl.Y+pad.Y+5.f},5.f,5.f,Colors::Slider_Progress);
		float oneP=sl.X/(float)(mx-mn);
		drawFilledRect({pos.X,pos.Y+sl.Y+pad.Y},oneP*(*value-mn),sl.Y,Colors::Slider_Progress);
		DrawFilledCircle({pos.X+oneP*(*value-mn),pos.Y+sl.Y+3.3f+pad.Y},10.f,Colors::Slider_Button);
		DrawFilledCircle({pos.X+oneP*(*value-mn),pos.Y+sl.Y+3.3f+pad.Y},5.f, Colors::Slider_Progress);
		char buf[32]; sprintf_s(buf,"%i",*value);
		TextCenter(buf,{pos.X+oneP*(*value-mn),pos.Y+sl.Y+25+pad.Y},{1,1,1,1},false);
		TextLeft(name,{pos.X+5,pos.Y+10},{1,1,1,1},false);
		sameLine=false; last_element_pos=pos; last_element_size=size;
		if (first_element_pos.X==0.f) first_element_pos=pos;
	}

	void SliderFloat(const char* name, float* value, float mn, float mx, const char* format)
	{
		elements_count++;
		SDK::FVector2D size{210,40}, sl{170,7}, adj{0,20}, pad{10,15};
		SDK::FVector2D pos{menu_pos.X+pad.X+offset_x, menu_pos.Y+pad.Y+offset_y};
		if (sameLine) { pos.X=last_element_pos.X+last_element_size.X+pad.X; pos.Y=last_element_pos.Y; }
		if (pushY) { pos.Y=pushYvalue; pushY=false; pushYvalue=0; offset_y=pos.Y-menu_pos.Y; }
		bool h=MouseInZone({pos.X,pos.Y+sl.Y+pad.Y-adj.Y},{sl.X,sl.Y+adj.Y*1.5f});
		if (!sameLine) offset_y+=size.Y+pad.Y;
		if (h || current_element==elements_count)
		{
			if (Input::IsMouseClicked(0,elements_count,true))
			{
				current_element=elements_count;
				float cx=CursorPos().X;
				*value=((cx-pos.X)*((mx-mn)/sl.X))+mn;
				if(*value<mn)*value=mn; if(*value>mx)*value=mx;
			}
			drawFilledRect({pos.X,pos.Y+sl.Y+pad.Y},sl.X,sl.Y,Colors::Slider_Hovered);
			DrawFilledCircle({pos.X,      pos.Y+pad.Y+9.3f},3.1f,Colors::Slider_Progress);
			DrawFilledCircle({pos.X+sl.X, pos.Y+pad.Y+9.3f},3.1f,Colors::Slider_Hovered);
			hover_element=true;
		}
		else
		{
			drawFilledRect({pos.X,pos.Y+sl.Y+pad.Y},sl.X,sl.Y,Colors::Slider_Idle);
			DrawFilledCircle({pos.X,      pos.Y+pad.Y+9.3f},3.1f,Colors::Slider_Progress);
			DrawFilledCircle({pos.X+sl.X, pos.Y+pad.Y+9.3f},3.1f,Colors::Slider_Idle);
		}
		TextLeft(name,{pos.X,pos.Y+5},Colors::Text,false);
		float oneP=sl.X/(mx-mn);
		drawFilledRect({pos.X,pos.Y+sl.Y+pad.Y},oneP*(*value-mn),sl.Y,Colors::Slider_Progress);
		DrawFilledCircle({pos.X+oneP*(*value-mn),pos.Y+sl.Y+2.66f+pad.Y},8.f,Colors::Slider_Button);
		DrawFilledCircle({pos.X+oneP*(*value-mn),pos.Y+sl.Y+2.66f+pad.Y},4.f,Colors::Slider_Progress);
		char buf[32]; sprintf_s(buf,format,*value);
		TextCenter(buf,{pos.X+oneP*(*value-mn),pos.Y+sl.Y+20+pad.Y},Colors::Text,false);
		sameLine=false; last_element_pos=pos; last_element_size=size;
		if (first_element_pos.X==0.f) first_element_pos=pos;
	}

	void Combobox(const char* name, SDK::FVector2D size, int* value, const char* arg, ...)
	{
		elements_count++;
		SDK::FVector2D pad{5,10}, pos{menu_pos.X+pad.X+offset_x, menu_pos.Y+pad.Y+offset_y};
		if (sameLine) { pos.X=last_element_pos.X+last_element_size.X+pad.X; pos.Y=last_element_pos.Y; }
		if (pushY) { pos.Y=pushYvalue; pushY=false; pushYvalue=0; offset_y=pos.Y-menu_pos.Y; }
		bool h=MouseInZone(pos,size);
		if (h||checkbox_enabled[elements_count]) { drawFilledRect(pos,size.X,size.Y,Colors::Combobox_Hovered); hover_element=true; }
		else drawFilledRect(pos,size.X,size.Y,Colors::Combobox_Idle);
		if (!sameLine) offset_y+=size.Y+pad.Y;
		TextLeft(name,{pos.X+size.X+5.f,pos.Y+size.Y/2},{1,1,1,1},false);
		bool h2=false;
		SDK::FVector2D ep=pos; int num=0;
		if (checkbox_enabled[elements_count]) { current_element_size.X=ep.X-5.f; current_element_size.Y=ep.Y-5.f; }
		va_list args;
		for (va_start(args,arg); arg!=NULL; arg=va_arg(args,const char*))
		{
			if (num==*value) TextCenter(arg,{pos.X+size.X/2,pos.Y+size.Y/2},{1,1,1,1},false);
			if (checkbox_enabled[elements_count])
			{
				ep.Y+=25.f; h2=MouseInZone(ep,{size.X,25.f});
				if (h2) { hover_element=true; PostRenderer::drawFilledRect(ep,size.X,25.f,Colors::Combobox_Hovered);
					if (Input::IsMouseClicked(0,elements_count,false)) { *value=num; checkbox_enabled[elements_count]=false; } }
				else PostRenderer::drawFilledRect(ep,size.X,25.f,Colors::Combobox_Idle);
				PostRenderer::TextLeft(arg,{ep.X+5.f,ep.Y+15.f},{1,1,1,1},false);
			}
			num++;
		}
		va_end(args);
		if (checkbox_enabled[elements_count]) { current_element_size.X=ep.X+5.f; current_element_size.Y=ep.Y+5.f; }
		sameLine=false; last_element_pos=pos; last_element_size=size;
		if (first_element_pos.X==0.f) first_element_pos=pos;
		if (h && Input::IsMouseClicked(0,elements_count,false)) checkbox_enabled[elements_count]=!checkbox_enabled[elements_count];
		if (!h&&!h2&&Input::IsMouseClicked(0,elements_count,false)) checkbox_enabled[elements_count]=false;
	}

	static std::string VKToString(UCHAR vk)
	{
		if (vk==VK_LBUTTON)  return "MOUSE0";
		if (vk==VK_RBUTTON)  return "MOUSE1";
		if (vk==VK_MBUTTON)  return "MBUTTON";
		if (vk==VK_XBUTTON1) return "XBUTTON1";
		if (vk==VK_XBUTTON2) return "XBUTTON2";
		UINT sc=MapVirtualKey(vk,MAPVK_VK_TO_VSC);
		switch(vk){case VK_LEFT:case VK_UP:case VK_RIGHT:case VK_DOWN:
		case VK_RCONTROL:case VK_RMENU:case VK_LWIN:case VK_RWIN:case VK_APPS:
		case VK_PRIOR:case VK_NEXT:case VK_END:case VK_HOME:
		case VK_INSERT:case VK_DELETE:case VK_DIVIDE:case VK_NUMLOCK: sc|=KF_EXTENDED; break;
		default: break; }
		CHAR sz[128]={}; GetKeyNameTextA(sc<<16,sz,128); return sz;
	}

	void Hotkey(const char* name, SDK::FVector2D size, int* key)
	{
		elements_count++;
		SDK::FVector2D pad{5,10}, pos{menu_pos.X+pad.X+offset_x, menu_pos.Y+pad.Y+offset_y};
		if (sameLine) { pos.X=last_element_pos.X+last_element_size.X+pad.X; pos.Y=last_element_pos.Y+(last_element_size.Y/2)-size.Y/2; }
		if (pushY) { pos.Y=pushYvalue; pushY=false; pushYvalue=0; offset_y=pos.Y-menu_pos.Y; }
		bool h=MouseInZone(pos,size);
		if (h) { drawFilledRect(pos,size.X,size.Y,Colors::Button_Hovered); hover_element=true; }
		else     drawFilledRect(pos,size.X,size.Y,Colors::Button_Idle);
		if (!sameLine) offset_y+=size.Y+pad.Y;
		SDK::FVector2D tp{pos.X+size.X/2, pos.Y+size.Y/2};
		if (active_hotkey==elements_count)
		{
			TextCenter("[Press Key]",tp,{1,1,1,1},false);
			if (!Input::IsAnyMouseDown()) already_pressed=false;
			if (!already_pressed) for(int c=0;c<255;c++) if(GetAsyncKeyState(c)){*key=c;active_hotkey=-1;}
		}
		else
		{
			std::string ks=VKToString((UCHAR)*key);
			TextCenter(ks.c_str(),tp,{1,1,1,1},false);
			if (h) { if(Input::IsMouseClicked(0,elements_count,false)){already_pressed=true;active_hotkey=elements_count;for(int c=0;c<255;c++)GetAsyncKeyState(c);} }
			else if(Input::IsMouseClicked(0,elements_count,false)) active_hotkey=-1;
		}
		sameLine=false; last_element_pos=pos; last_element_size=size;
		if (first_element_pos.X==0.f) first_element_pos=pos;
	}

	static bool ColorPixel(SDK::FVector2D pos, SDK::FVector2D size,
	                       SDK::FLinearColor* orig, SDK::FLinearColor color)
	{
		PostRenderer::drawFilledRect(pos,size.X,size.Y,color);
		if (orig->R==color.R&&orig->G==color.G&&orig->B==color.B)
		{
			PostRenderer::Draw_Line({pos.X,pos.Y},{pos.X+size.X-1,pos.Y},1,{0,0,0,1});
			PostRenderer::Draw_Line({pos.X,pos.Y+size.Y-1},{pos.X+size.X-1,pos.Y+size.Y-1},1,{0,0,0,1});
			PostRenderer::Draw_Line({pos.X,pos.Y},{pos.X,pos.Y+size.Y-1},1,{0,0,0,1});
			PostRenderer::Draw_Line({pos.X+size.X-1,pos.Y},{pos.X+size.X-1,pos.Y+size.Y-1},1,{0,0,0,1});
		}
		bool h=MouseInZone(pos,size);
		if(h&&Input::IsMouseClicked(0,0,false)) *orig=color;
		return true;
	}

	void ColorPicker(const char* name, SDK::FLinearColor* color)
	{
		elements_count++;
		const float sz=25.f;
		SDK::FVector2D pad{10,10}, pos{menu_pos.X+pad.X+offset_x, menu_pos.Y+pad.Y+offset_y};
		if (sameLine) { pos.X=last_element_pos.X+last_element_size.X+pad.X; pos.Y=last_element_pos.Y; }
		if (pushY) { pos.Y=pushYvalue; pushY=false; pushYvalue=0; offset_y=pos.Y-menu_pos.Y; }
		bool h=MouseInZone(pos,{sz,sz});
		if (!sameLine) offset_y+=sz+pad.Y;
		if (active_picker==elements_count)
		{
			hover_element=true;
			const float px=250,py=250;
			bool hp=MouseInZone(pos,{px,py-60});
			PostRenderer::drawFilledRect(pos,px,py-65,Colors::ColorPicker_Background);
			SDK::FVector2D ps{px/12,py/12};
#define CP(c,r,rr,gg,bb) ColorPixel({pos.X+ps.X*(c),pos.Y+ps.Y*(r)},ps,color,{rr/255.f,gg/255.f,bb/255.f,1.f})
			CP(0,0,174,235,253);CP(0,1,136,225,251);CP(0,2,108,213,250);CP(0,3,89,175,213); CP(0,4,76,151,177); CP(0,5,60,118,140);CP(0,6,43,85,100); CP(0,7,32,62,74);   CP(0,8,255,255,255);
			CP(1,0,175,205,252);CP(1,1,132,179,252);CP(1,2,90,152,250); CP(1,3,55,120,250); CP(1,4,49,105,209); CP(1,5,38,83,165); CP(1,6,28,61,120); CP(1,7,20,43,86);   CP(1,8,247,247,247);
			CP(2,0,153,139,250);CP(2,1,101,79,249); CP(2,2,64,50,230);  CP(2,3,54,38,175);  CP(2,4,39,31,144);  CP(2,5,32,25,116); CP(2,6,21,18,82);  CP(2,7,16,13,61);   CP(2,8,228,228,228);
			CP(3,0,194,144,251);CP(3,1,165,87,249); CP(3,2,142,57,239); CP(3,3,116,45,184); CP(3,4,92,37,154);  CP(3,5,73,29,121); CP(3,6,53,21,88);  CP(3,7,37,15,63);   CP(3,8,203,203,203);
			CP(4,0,224,162,197);CP(4,1,210,112,166);CP(4,2,199,62,135); CP(4,3,159,49,105); CP(4,4,132,41,89);  CP(4,5,104,32,71); CP(4,6,75,24,51);  CP(4,7,54,14,36);   CP(4,8,175,175,175);
			CP(5,0,235,175,176);CP(5,1,227,133,135);CP(5,2,219,87,88);  CP(5,3,215,50,36);  CP(5,4,187,25,7);   CP(5,5,149,20,6);  CP(5,6,107,14,4);  CP(5,7,77,9,3);     CP(5,8,144,144,144);
			CP(6,0,241,187,171);CP(6,1,234,151,126);CP(6,2,229,115,76); CP(6,3,227,82,24);  CP(6,4,190,61,15);  CP(6,5,150,48,12); CP(6,6,107,34,8);  CP(6,7,79,25,6);    CP(6,8,113,113,113);
			CP(7,0,245,207,169);CP(7,1,240,183,122);CP(7,2,236,159,74); CP(7,3,234,146,37); CP(7,4,193,111,28); CP(7,5,152,89,22); CP(7,6,110,64,16); CP(7,7,80,47,12);   CP(7,8,82,82,82);
			CP(8,0,247,218,170);CP(8,1,244,200,124);CP(8,2,241,182,77); CP(8,3,239,174,44); CP(8,4,196,137,34); CP(8,5,154,108,27);CP(8,6,111,77,19); CP(8,7,80,56,14);   CP(8,8,54,54,54);
			CP(9,0,254,243,187);CP(9,1,253,237,153);CP(9,2,253,231,117);CP(9,3,254,232,85); CP(9,4,242,212,53); CP(9,5,192,169,42);CP(9,6,138,120,30);CP(9,7,101,87,22);  CP(9,8,29,29,29);
			CP(10,0,247,243,185);CP(10,1,243,239,148);CP(10,2,239,232,111);CP(10,3,235,229,76);CP(10,4,208,200,55);CP(10,5,164,157,43);CP(10,6,118,114,31);CP(10,7,86,82,21);CP(10,8,9,9,9);
			CP(11,0,218,232,182);CP(11,1,198,221,143);CP(11,2,181,210,103);CP(11,3,154,186,76);CP(11,4,130,155,64);CP(11,5,102,121,50);CP(11,6,74,88,36);CP(11,7,54,64,26); CP(11,8,0,0,0);
#undef CP
			if (!hp && Input::IsMouseClicked(0,elements_count,false)) active_picker=-1;
		}
		else
		{
			if (h) { drawFilledRect(pos,sz,sz,Colors::Checkbox_Hovered); hover_element=true; }
			else     drawFilledRect(pos,sz,sz,Colors::Checkbox_Idle);
			drawFilledRect({pos.X+4,pos.Y+4},sz-8,sz-8,*color);
			TextLeft(name,{pos.X+sz+5.f,pos.Y+sz/2},{1,1,1,1},false);
			if (h&&Input::IsMouseClicked(0,elements_count,false)) { saved_color=*color; active_picker=elements_count; }
		}
		sameLine=false; last_element_pos=pos;
		if (first_element_pos.X==0.f) first_element_pos=pos;
	}

	void Render()
	{
		for (int i=0;i<128;i++)
		{
			auto& d=PostRenderer::drawlist[i];
			if (d.type==-1) continue;
			switch(d.type)
			{
			case 1: drawFilledRect(d.pos,d.size.X,d.size.Y,d.color); break;
			case 2: TextLeft  (d.name,d.pos,d.color,d.outline);      break;
			case 3: TextCenter(d.name,d.pos,d.color,d.outline);      break;
			case 4: Draw_Line (d.from,d.to,d.thickness,d.color);     break;
			}
			d.type=-1;
		}
	}

}
