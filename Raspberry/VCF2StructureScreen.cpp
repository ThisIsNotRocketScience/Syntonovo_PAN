#pragma once

#include "VCF2Structure.h"

void RenderFiltBlock(int x, int y, FilterTypes_t type, const char *txt)
{

	ImGui::SetCursorPos(ImVec2(x, y));
	ImGui::Image(gGuiResources.F[type], ImVec2(90, 45), ImVec2(0, 0), ImVec2(1, 1));

	auto A = ImGui::CalcTextSize(txt);

	ImGui::SetCursorPos(ImVec2(x + 26 / 2 - A.x / 2, y + 45 / 2 - A.y / 2));
	ImGui::TextColored(ImVec4(0, 0, 0, 1), txt);
}

void RenderFilter(int x, int y, FilterTypes_t typeA, FilterTypes_t typeB, bool Mode, const char *t1, const char *t2)
{
	ImGui::SetCursorPos(ImVec2(x, y));
	if (Mode)
	{
		ImGui::Image(gGuiResources.Fs[1], ImVec2(339, 154), ImVec2(0, 0), ImVec2(1, 1));
		RenderFiltBlock(x + 23, y + 21, typeA, t1);
		RenderFiltBlock(x + 158, y + 21, typeB, t2);
	}
	else
	{
		ImGui::Image(gGuiResources.Fs[0], ImVec2(339, 154), ImVec2(0, 0), ImVec2(1, 1));
		RenderFiltBlock(x + 99, y + 21, typeA, t1);
		RenderFiltBlock(x + 99, y + 91, typeB, t2);
	}

}
VCF2StructureScreen::VCF2StructureScreen()
{
	EnableButton(0, "Mode A", MenuEntry_Action, MenuAction_VCFAMode);
	EnableButton(1, "Mode B", MenuEntry_Action, MenuAction_VCFBMode);
	EnableButton(2, "A/B Parralel", MenuEntry_Action, MenuAction_VCFAB);
	EnableButton(4, "Mode C", MenuEntry_Action, MenuAction_VCFCMode);
	EnableButton(5, "Mode D", MenuEntry_Action, MenuAction_VCFDMode);
	EnableButton(6, "C/D Parralel", MenuEntry_Action, MenuAction_VCFCD);
	EnableButton(12, "done", MenuEntry_Action, MenuAction_CloseModal);
}

void VCF2StructureScreen::Action(int action)
{
	switch (action) {
	case MenuAction_VCFAMode: gCurrentPreset.SetFilterType(FilterVCF2A, (FilterTypes_t)((gCurrentPreset.Decode(gCurrentPreset.GetSwitch(Switch_SELVCF21A), gCurrentPreset.GetSwitch(Switch_SELVCF21B)) + 1) % __FILT_COUNT)); break;
	case MenuAction_VCFBMode: gCurrentPreset.SetFilterType(FilterVCF2B, (FilterTypes_t)((gCurrentPreset.Decode(gCurrentPreset.GetSwitch(Switch_SELVCF22A), gCurrentPreset.GetSwitch(Switch_SELVCF22B)) + 1) % __FILT_COUNT)); break;
	case MenuAction_VCFCMode: gCurrentPreset.SetFilterType(FilterVCF2C, (FilterTypes_t)((gCurrentPreset.Decode(gCurrentPreset.GetSwitch(Switch_SELVCF23A), gCurrentPreset.GetSwitch(Switch_SELVCF23B)) + 1) % __FILT_COUNT)); break;
	case MenuAction_VCFDMode: gCurrentPreset.SetFilterType(FilterVCF2D, (FilterTypes_t)((gCurrentPreset.Decode(gCurrentPreset.GetSwitch(Switch_SELVCF24A), gCurrentPreset.GetSwitch(Switch_SELVCF24B)) + 1) % __FILT_COUNT)); break;
	case MenuAction_VCFAB: gCurrentPreset.ToggleSwitch(Switch_SELVCF21SER); break;
	case MenuAction_VCFCD: gCurrentPreset.ToggleSwitch(Switch_SELVCF23SER); break;
	}
}

void VCF2StructureScreen::Render(bool active, float dt)
{
	BG = gGuiResources.VCF2;
	_screensetup_t::Render(active, dt);

	bool A1 = gCurrentPreset.GetSwitch(Switch_SELVCF21A);
	bool A2 = gCurrentPreset.GetSwitch(Switch_SELVCF21B);



	bool B1 = gCurrentPreset.GetSwitch(Switch_SELVCF22A);
	bool B2 = gCurrentPreset.GetSwitch(Switch_SELVCF22B);

	bool C1 = gCurrentPreset.GetSwitch(Switch_SELVCF23A);
	bool C2 = gCurrentPreset.GetSwitch(Switch_SELVCF23B);

	bool D1 = gCurrentPreset.GetSwitch(Switch_SELVCF24A);
	bool D2 = gCurrentPreset.GetSwitch(Switch_SELVCF24B);

	auto A = gCurrentPreset.Decode(A1, A2);
	auto B = gCurrentPreset.Decode(B1, B2);
	auto C = gCurrentPreset.Decode(C1, C2);
	auto D = gCurrentPreset.Decode(D1, D2);

	bool ModeAB = gCurrentPreset.GetSwitch(Switch_SELVCF21SER);
	bool ModeCD = gCurrentPreset.GetSwitch(Switch_SELVCF23SER);

	RenderFilter(310, 130, A, B, ModeAB, "a", "b");
	RenderFilter(310, 313, C, D, ModeCD, "c", "d");


}