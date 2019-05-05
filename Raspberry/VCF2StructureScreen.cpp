#pragma once

#include "VCF2Structure.h"



void VCF2StructureScreen::Render(bool active, float dt)
{
	BG = gGuiResources.VCF2;
	_screensetup_t::Render(active, dt);

	bool A1 = gCurrentPreset.GetSwitch(Switch_SELVCF21A);
	bool A2= gCurrentPreset.GetSwitch(Switch_SELVCF21B);



	bool B1 = gCurrentPreset.GetSwitch(Switch_SELVCF22A);
	bool B2 = gCurrentPreset.GetSwitch(Switch_SELVCF22B);

	bool C1 = gCurrentPreset.GetSwitch(Switch_SELVCF23A);
	bool C2 = gCurrentPreset.GetSwitch(Switch_SELVCF23B);

	bool D1 = gCurrentPreset.GetSwitch(Switch_SELVCF24A);
	bool D2 = gCurrentPreset.GetSwitch(Switch_SELVCF24B);

	auto A = Decode(A1, A2);
	auto B = Decode(B1, B2);
	auto C = Decode(C1, C2);
	auto D = Decode(D1, D2);

	ImGui::SetCursorPos(ImVec2(100, 100));
	ImGui::Image(gGuiResources.F[A], ImVec2(90, 45), ImVec2(0, 0), ImVec2(1, 1));
	ImGui::SetCursorPos(ImVec2(100, 150));
	ImGui::Image(gGuiResources.F[B], ImVec2(90, 45), ImVec2(0, 0), ImVec2(1, 1));
	ImGui::SetCursorPos(ImVec2(100, 200));

	ImGui::Image(gGuiResources.F[C], ImVec2(90, 45), ImVec2(0, 0), ImVec2(1, 1));
	ImGui::SetCursorPos(ImVec2(100, 250));
	ImGui::Image(gGuiResources.F[D], ImVec2(90, 45), ImVec2(0, 0), ImVec2(1, 1));

}