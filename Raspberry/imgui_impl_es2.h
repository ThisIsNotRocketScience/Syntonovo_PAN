#pragma once


IMGUI_API bool        ImGui_ImlES_Init();
IMGUI_API void        ImGui_ImlES_Shutdown();
IMGUI_API void        ImGui_ImlES_NewFrame();
IMGUI_API bool        ImGui_ImlES_ProcessEvent();

// Use if you want to reset your rendering device without losing ImGui state.
IMGUI_API void        ImGui_ImlES_InvalidateDeviceObjects();
IMGUI_API bool        ImGui_ImlES_CreateDeviceObjects();
IMGUI_API void		  ImGui_ImlES_RenderDrawLists(ImDrawData* draw_data);
