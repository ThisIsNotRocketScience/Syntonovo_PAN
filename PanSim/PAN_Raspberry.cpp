#include "imgui.h"

#include "PanHeader.h"

typedef struct
{
	GuiState_t GuiState;
	int ModSelect;
} Raspberry_GuiData_t;

static Raspberry_GuiData_t Raspberry_guidata;

void Raspberry_ToState(GuiState_t state, int modselect)
{
	Raspberry_guidata.GuiState = state;
	Raspberry_guidata.ModSelect = modselect;
}

void Raspberry_RenderScreen()
{
	ImGui::Button("tadaa");
}

void Raspberry_EncoderTurn(int id, int delta)
{

}