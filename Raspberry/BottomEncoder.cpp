#include <stdio.h>
#include "FinalPanEnums.h"
#include "FinalPanHeader.h"
#include "PanPreset.h"
#include "gui.h"
#include "ModSourceScreen.h"


void bottomencoder_t::Turn(int delta)
{
	switch (style)
	{
	case MenuEntry_Toggle: gCurrentPreset.PutSwitch((SwitchEnum)target, delta > 0); break;

	case MenuEntry_MidValue:
	case MenuEntry_Percentage:
	case MenuEntry_Pitch:
	case MenuEntry_EffectParam1:
	case MenuEntry_EffectParam2:
	case MenuEntry_EffectParam3:
	case MenuEntry_FilterMix:
	case MenuEntry_RemapValue:
	case MenuEntry_Value: Parent->TweakParameterValue((OutputEnum)target, delta); break;
	default: Parent->TweakParameterValue((OutputEnum)target, delta); break;

	case MenuEntry_LedValue:
	{
		gCurrentPreset.TweakLed((LedParameter)target, delta); break;

	}

	}
}


void bottomencoder_t::SetupPosition(int id)
{
	y = 600 - ParamMasterMargin;
	x = ((1024) / 11.0f) * (id + 0.5) - ParamBoxDim/2;
	Align = align_left;

}

void bottomencoder_t::Render(bool active, float DT)
{
	if (!enabled) return;


	int x2 = x;
	int y2 = y;
	bool ActiveSet = true;
	if (Set != Parent->currentencoderset)
	{
		ActiveSet = false;
		y2 -= 100;
	}
	if (Align == align_right)
	{
		ImVec2 textsize = ImGui::CalcTextSize(title);
		x2 -= textsize.x;
	}

	ImGui::PushFont(gGuiResources.TinyFont);

	ImGui::SetCursorPos(ImVec2(x2 - ImGui::GetTextLineHeight(), y2));

	if (active)
	{
		VerticalText(title, align_left, gGuiResources.Highlight);
	}
	else
	{
		VerticalText(title, align_left);
	}
	y2 -= ParamVerticalBoxHeight;
	
	switch (style)
	{

	case MenuEntry_EnvelopeValue:
	case MenuEntry_LFOValue:
	{
		RenderBoxVertical(x, y2, Parent->GetParameterValue(target), style == BOX_REGULAR, active);
		//	char txt[400];
		//		gCurrentPreset.DescribeParam((OutputEnum)target, style, txt, 400);
		//		VerticalText(txt, align_right);
	}

	break;

	case MenuEntry_LedValue:
	{

		RenderBoxVertical(x, y2, gCurrentPreset.GetLedParameter((LedParameter)target), style == BOX_REGULAR, active);
		char txt[400];
		//gCurrentPreset.DescribeParam((OutputEnum)target, style, txt, 400);
		//VerticalText(txt, align_right);
	}

	break;
	case MenuEntry_ModMatrixValue:

	{
		int16_t modval = ((ModSourceScreen*)Parent)->GetModValue(target);
		RenderBoxVertical(x, y2, modval, BOX_MID, active);
	}
	break;
	case MenuEntry_MidValue:
	case MenuEntry_Percentage:
	case MenuEntry_Pitch:
	case MenuEntry_EffectParam1:
	case MenuEntry_EffectParam2:
	case MenuEntry_EffectParam3:
	case MenuEntry_FilterMix:
	case MenuEntry_RemapValue:
	case MenuEntry_Value:
	{

		RenderBoxVertical(x, y2, gCurrentPreset.paramvalue[target], style == MenuEntry_MidValue ? BOX_MID : BOX_REGULAR, active);

		char txt[400];

		gCurrentPreset.DescribeParam((OutputEnum)target, style, txt, 400);
	//	VerticalText(txt, align_right);
	}

	break;

	case MenuEntry_Toggle:

		int id = gCurrentPreset.GetSwitch((SwitchEnum)target) ? 1 : 0;

		if (Align == align_right)
		{
			ImGui::SetCursorPos(ImVec2(x2 - 128 - 200, y));
		}
		else
		{
			ImGui::SetCursorPos(ImVec2(x2 + 200, y));

		}
		ImGui::Image(gGuiResources.OnOff[id + (active ? 2 : 0)], ImVec2(128, 48));

		break;
	}
	ImGui::PopFont();


}



void bottomencoder_t::UpdateLed(bool active)
{
	if (enabled)
	{
		ledmode = active ? ledmode_blinkslow : ledmode_solid;
		if (active)
		{

		}
		uint16_t value = 0;
		switch (style)
		{
		case MenuEntry_LedValue:
			value = gCurrentPreset.GetLedParameter((LedParameter)target);
			break;
		default:
			value = gCurrentPreset.paramvalue[target];
			break;
		}
		LedLerp(active, value, &r, &g, &b);


	}
	else
	{
		ledmode = ledmode_off;
	}
}
