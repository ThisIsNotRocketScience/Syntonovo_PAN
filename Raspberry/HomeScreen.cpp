#include <stdio.h>
#include "HomeScreen.h"

extern presetnames_t presetnames;

class BankList : public _control_t
{
public:
	int x, y;
	int *bankid;
	BankList(int _x, int _y, int* _bank)
	{
		bankid = _bank;
		x = _x;
		y = _y;
		skipencodercycling = true;
	}

	virtual void Render(bool active, float dt)
	{
		ImGui::PushFont(gGuiResources.TinyFont);
		for (int i = 0; i < 8; i++)
		{
			int presetidx = (*bankid) * 8 + i;
			int Y = (ImGui::GetTextLineHeight() + ParamMasterMargin)  * i + y;
			ImGui::SetCursorPos(ImVec2(x, Y));
			ImGui::Text(presetnames.names[presetidx]);
			ImGui::GetWindowDrawList()->AddRect(ImVec2(x, Y), ImVec2(x + 300, Y + ImGui::GetTextLineHeight()), gGuiResources.Normal);
		}
		ImGui::PopFont();
	}
};



HomeScreen::HomeScreen()
{
	this->ControlsInOrder.push_back(new BankList(512-300, 100, &gPanState.BankLeft));
	this->ControlsInOrder.push_back(new BankList(512, 100, &gPanState.BankRight));
}
