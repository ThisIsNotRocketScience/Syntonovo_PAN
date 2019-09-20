#include <stdio.h>
#include "FinalPanEnums.h"
#include "FinalPanHeader.h"
#include "PanPreset.h"
#include "gui.h"
#include "ModSourceScreen.h"

extern int DecodeCurrentEffect();

void bottomencoder_t::Turn(int delta)
{
	if (Set != Parent->currentencoderset) return;
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

int GetEncoderX(int id)
{
	return 512 + (id - 5.0)* ((1024) / 12.0f);
	//return ((1024) / 11.0f) * (id + 0.5);
}

void bottomencoder_t::SetupPosition(int id)
{
	EncoderID = id;
	y = 600 - ParamMasterMargin;
	x = GetEncoderX(id);
	Align = align_left;
}


extern char EffectChipStrings[8][4][24];


void TextHorizontal(float x, float y, char *txt, bool active, alignment_t align = align_left, font_size fontsize = font_small)
{
	bool nopop = false;
	switch (fontsize)
	{
	case font_medium: ImGui::PushFont(gGuiResources.MediumFont); break;
	case font_large: ImGui::PushFont(gGuiResources.BigFont); break;
	case font_small: ImGui::PushFont(gGuiResources.SmallFont); break;
	default:
		nopop = true;
	}
	
	float x2 = x;
	
	switch (align)
	{
	case align_right:
	{
		ImVec2 textsize = ImGui::CalcTextSize(txt);
		x2 -= textsize.x;
	}
	break;
	case align_center:
	{
		ImVec2 textsize = ImGui::CalcTextSize(txt);
		x2 -= textsize.x / 2;
	}
	break;
	}

	ImGui::SetCursorPos(ImVec2(x2, y));
	ImGui::Text(txt);
	if (!nopop) ImGui::PopFont();
}


void bottomencoder_t::Render(bool active, float DT)
{
	if (!enabled) return;
	
	int x2 = x;
	int y2 = y;
	int textjump =  (((EncoderID % 2) == 0) ? -30 : 0);
	bool ActiveSet = true;

	char finaltitle[300];
	snprintf(finaltitle, 300, "%s" ,title);
	switch (style)
	{
	case MenuEntry_Value:
		switch (target)
		{
		case Output_VCA_FXPOT1:
			snprintf(finaltitle, 300, "FX: %s", EffectChipStrings[DecodeCurrentEffect()][1]);
			break;
		case Output_VCA_FXPOT2:
			snprintf(finaltitle, 300, "FX: %s", EffectChipStrings[DecodeCurrentEffect()][2]);
			break;
		case Output_VCA_FXPOT3:
			snprintf(finaltitle, 300, "FX: %s", EffectChipStrings[DecodeCurrentEffect()][3]);
			break;
		}
		break;
	}

	if (Set != Parent->currentencoderset)
	{
		ActiveSet = false;
		y2 -= 100;
	}
	else
	{
		if (textjump != 0)
		{
			ImGui::GetWindowDrawList()->AddLine(ImVec2(x2, y2 - 90), ImVec2(x2, y2 - 90 + textjump), active ? gGuiResources.Highlight : gGuiResources.Normal, 2.0);
		}
	}

	if (Align == align_right)
	{
		ImVec2 textsize = ImGui::CalcTextSize(finaltitle);
		x2 -= textsize.x;
	}
	
	ImGui::PushFont(gGuiResources.TinyFont);

	ImGui::SetCursorPos(ImVec2(x2 - ImGui::GetTextLineHeight() - ParamBoxDim / 2, y2-120 + textjump ));

	
	if (active)
	{
		TextHorizontal(x2 , y2 - 120 + textjump, finaltitle, false, align_center, font_tiny);
		// VerticalText(finaltitle, align_left, gGuiResources.Highlight);
	}
	else
	{
		TextHorizontal(x2 , y2 - 120 + textjump, finaltitle, false, align_center, font_tiny);
		// ImGui::Text(finaltitle);
		// VerticalText(finaltitle, align_left);
	}

	y2 -= ParamVerticalBoxHeight;

	switch (style)
	{
	case MenuEntry_Ghosted:
		RenderBoxVertical(x, y2, Parent->GetParameterValue(target, Set), BOX_GHOST, active);
		break;
	case MenuEntry_EnvelopeValue:
	case MenuEntry_LFOValue:
	{
		RenderBoxVertical(x, y2, Parent->GetParameterValue(target, Set), BOX_REGULAR, active);
		//	char txt[400];
		//		gCurrentPreset.DescribeParam((OutputEnum)target, style, txt, 400);
		//		VerticalText(txt, align_right);
	}

	break;

	case MenuEntry_LedValue:
	{

		RenderBoxVertical(x, y2, gCurrentPreset.GetLedParameter((LedParameter)target), BOX_REGULAR, active);
		//char txt[400];
		//gCurrentPreset.DescribeParam((OutputEnum)target, style, txt, 400);
		//VerticalText(txt, align_right);
	}

	break;
	case MenuEntry_ModMatrixValue:

	{
		int16_t modval = Parent->GetParameterValue(target, Set);
		RenderBoxVertical(x, y2, modval, BOX_MOD, active);
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

		if (IsDirectOutput)
		{
			ImTextureID t = 0;
			switch(ModSourceCount)
			{
			case 0: break;
			case 1:t = gGuiResources.Plugs[Plugs_OnePlug]; break;
			case 2:t = gGuiResources.Plugs[Plugs_TwoPlug]; break;
			default: t = gGuiResources.Plugs[Plugs_ManyPlug]; break;
			}

			if (t)
			{
				ImGui::SetCursorPos(ImVec2(x - ImGui::GetTextLineHeight()-19, y2+8));
				ImGui::Image(t, ImVec2(22, 76));
			}
		}

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
