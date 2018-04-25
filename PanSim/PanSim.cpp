#include <Windows.h>
#pragma comment(lib, "winmm.lib")


#include "imgui.h"
#include "imgui_impl_sdl_gl3.h"
#include <stdio.h>
#include <GL/gl3w.h>    
#include <SDL.h>

#include "../libs/lodepng-master/lodepng.h"

#include  "PanHeader.h"



SDL_Surface* load_PNG(const char* filename)
{
	std::vector<unsigned char> buffer, image;
	lodepng::load_file(buffer, filename); //load the image file with given filename
	unsigned w, h;
	unsigned error = lodepng::decode(image, w, h, buffer); //decode the png

	if (error)
	{
		//std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;
		return 0;
	}

	Uint32 rmask, gmask, bmask, amask;
	/* SDL interprets each pixel as a 32-bit number, so our masks must depend
	on the endianness (byte order) of the machine */
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	rmask = 0xff000000;
	gmask = 0x00ff0000;
	bmask = 0x0000ff00;
	amask = 0x000000ff;
#else
	rmask = 0x000000ff;
	gmask = 0x0000ff00;
	bmask = 0x00ff0000;
	amask = 0xff000000;
#endif

	//avoid too large window size by downscaling large image
	unsigned jump = 1;
	//if(w / 1024 >= jump) jump = w / 1024 + 1;
	//if(h / 1024 >= jump) jump = h / 1024 + 1;

	SDL_Surface *dest = SDL_CreateRGBSurface(0, w, h, 32,
		rmask, gmask, bmask, amask);
	if (dest == NULL) {
		fprintf(stderr, "CreateRGBSurface failed: %s\n", SDL_GetError());
		return 0;
	}

	if (SDL_MUSTLOCK(dest)) { SDL_LockSurface(dest); }

	//plot the pixels of the PNG file
	for (unsigned y = 0; y + jump - 1 < h; y += jump)
		for (unsigned x = 0; x + jump - 1 < w; x += jump)
		{
			//get RGBA components
			Uint32 r = image[4 * y * w + 4 * x + 0]; //red
			Uint32 g = image[4 * y * w + 4 * x + 1]; //green
			Uint32 b = image[4 * y * w + 4 * x + 2]; //blue
			Uint32 a = image[4 * y * w + 4 * x + 3]; //alpha

													 //make translucency visible by placing checkerboard pattern behind image
													 //int checkerColor = 191 + 64 * (((x / 16) % 2) == ((y / 16) % 2));
													 //r = (a * r + (255 - a) * checkerColor) / 255;
													 //g = (a * g + (255 - a) * checkerColor) / 255;
													 //b = (a * b + (255 - a) * checkerColor) / 255;

													 //give the color value to the pixel of the screenbuffer
			Uint32* bufp = (Uint32 *)dest->pixels + (y * dest->pitch / 4) / jump + (x / jump);
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
			*bufp = 0x01000000 * r + 65536 * g + 256 * b + a;
#else
			*bufp = 0x01000000 * a + 65536 * b + 256 * g + r;
#endif
		}

	if (SDL_MUSTLOCK(dest)) { SDL_UnlockSurface(dest); }
	return dest;
}

void SetupIcon(SDL_Window *window)
{

	SDL_SetWindowIcon(window, load_PNG("favicon-32x32.png"));

}
#include "SerialPort.h"

CSerialPort DSPSerial;
CSerialPort UISerial;


void GetSerialPorts(int port, int uiport)
{
	try
	{
		if (port > 0) DSPSerial.Open(port, 115200UL);
	}
	catch (CSerialException e)
	{

	}
	
	if (DSPSerial.IsOpen())
	{
		unsigned char bytes[8] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
		DSPSerial.Write(bytes, 8);
	}

	try
	{
		if (uiport > 0)
		{
			UISerial.Open(uiport, 1000000UL);
			UISerial.Setup(100000, 100000);
		
			
		}	
	}
	catch (CSerialException e)
	{

	}
	
}

void CloseSerialPorts()
{
	if (DSPSerial.IsOpen()) DSPSerial.Close();
	if (UISerial.IsOpen()) UISerial.Close();
}

uint32_t ConvertDat(uint32_t d)
{
	unsigned char d1 = d & 127;
	unsigned char d2 = (d >> 7) & 127;
	unsigned char d3 = (d >> 14) & 127;
	unsigned char d4 = (d >> 21) & 127;

	return d1 + (d2 << 8) + (d3 << 16) + (d4 << 24);
}

void UISendCommand(unsigned char command, uint32_t data)
{
	if (UISerial.IsOpen() == false) return;
	uint8_t buffer[10];
	uint32_t D = ConvertDat(data);
	buffer[0] = command;
	buffer[1] = (D & 127);
	buffer[2] = (((D >> 8) & 127));
	buffer[3] = (((D >> 16) & 127));
	buffer[4] = (((D >> 24) & 127));
	UISerial.Write(buffer, 5);
}

void UIWriteLed(int idx, int value)
{	
	uint32_t D = (idx << 16) + (255-value);
	UISendCommand(0x83, D);
}

void WriteKnob(int id, uint32_t value)
{
	if (DSPSerial.IsOpen() == false) return;
	char b[4];
	b[0] = (id>> 0) & 0xFF;
	b[1] = (id >> 8) & 0xFF;
	b[2] = (value >> 8) & 0xFF;
	b[3] = (value >> 0) & 0xFF;
	DSPSerial.Write(b, 4);
}

void WriteWithSubKnob(int id, int subid, uint32_t value)
{
	id |= subid << 8;
	if (DSPSerial.IsOpen() == false) return;
	char b[4];
	b[0] = (id >> 0) & 0xFF;
	b[1] = (id >> 8) & 0xFF;
	b[2] = (value >> 8) & 0xFF;
	b[3] = (value >> 0) & 0xFF;

	DSPSerial.Write(b, 4);
}

void WriteSwitch(int id, int state)
{
	//TODO
}

void WriteSyncLfo(uint8_t* paramids)
{
	//TODO
}

typedef struct
{
	uint16_t paramid;
	uint16_t value;
} setpara_t;

void set(setpara_t& para)
{
	if (DSPSerial.IsOpen() == false) return;
	char b[4];

	//b[0] = 1;
	b[0] = (para.paramid >> 0) & 0xFF;
	b[1] = (para.paramid >> 8) & 0xFF;
	b[2] = (para.value >> 8) & 0xFF;
	b[3] = (para.value >> 0) & 0xFF;

	DSPSerial.Write(b, 4);
}

void note_on(int noteid, int notevel)
{
	setpara_t setpara;
	setpara.paramid = 0x02fc;
	setpara.value = noteid | (notevel << 8);
	set(setpara);
}

void note_off(int noteid, int notevel)
{
	setpara_t setpara;
	setpara.paramid = 0x01fc;
	setpara.value = noteid | (notevel << 8);
	set(setpara);
}


static bool MyKnob(const char* label, float* p_value, float v_min, float v_max, const char*desc)
{
	ImGuiIO& io = ImGui::GetIO();
	ImGuiStyle& style = ImGui::GetStyle();

	float radius_outer = 20.0f;
	ImVec2 pos = ImGui::GetCursorScreenPos();
	ImVec2 center = ImVec2(pos.x + radius_outer, pos.y + radius_outer);
	float line_height = ImGui::GetTextLineHeight();
	ImDrawList* draw_list = ImGui::GetWindowDrawList();

	float ANGLE_MIN = 3.141592f * 0.75f;
	float ANGLE_MAX = 3.141592f * 2.25f;

	ImGui::InvisibleButton(label, ImVec2(radius_outer * 2, radius_outer * 2 + line_height + style.ItemInnerSpacing.y));
	bool value_changed = false;
	bool is_active = ImGui::IsItemActive();
	bool is_hovered = ImGui::IsItemActive();
	if (is_active && io.MouseDelta.x != 0.0f)
	{
		float step = (v_max - v_min) / 200.0f;
		*p_value += io.MouseDelta.x * step;
		if (*p_value < v_min) *p_value = v_min;
		if (*p_value > v_max) *p_value = v_max;
		value_changed = true;
	}

	float t = (*p_value - v_min) / (v_max - v_min);
	float angle = ANGLE_MIN + (ANGLE_MAX - ANGLE_MIN) * t;
	float angle_cos = cosf(angle), angle_sin = sinf(angle);
	float radius_inner = radius_outer*0.40f;
	draw_list->AddCircleFilled(center, radius_outer, ImGui::GetColorU32(ImGuiCol_FrameBg), 16);
	draw_list->AddLine(ImVec2(center.x + angle_cos*radius_inner, center.y + angle_sin*radius_inner), ImVec2(center.x + angle_cos*(radius_outer - 2), center.y + angle_sin*(radius_outer - 2)), ImGui::GetColorU32(ImGuiCol_SliderGrabActive), 2.0f);
	draw_list->AddCircleFilled(center, radius_inner, ImGui::GetColorU32(is_active ? ImGuiCol_FrameBgActive : is_hovered ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg), 16);
	auto R = ImGui::CalcTextSize(desc);
	draw_list->AddText(ImVec2(center.x - R.x/2, pos.y - line_height - style.ItemInnerSpacing.y), ImGui::GetColorU32(ImGuiCol_Text), desc);

	if (is_active || is_hovered)
	{
		ImGui::SetNextWindowPos(ImVec2(pos.x - style.WindowPadding.x, pos.y - line_height - style.ItemInnerSpacing.y - style.WindowPadding.y));
		ImGui::BeginTooltip();
		ImGui::Text("%.3f", *p_value);
		ImGui::EndTooltip();
	}

	return value_changed;
}

static bool MyEncoder(const char* label, float* p_value, int* p_delta)
{
	ImGuiIO& io = ImGui::GetIO();
	ImGuiStyle& style = ImGui::GetStyle();

	float radius_outer = 20.0f;
	ImVec2 pos = ImGui::GetCursorScreenPos();
	ImVec2 center = ImVec2(pos.x + radius_outer, pos.y + radius_outer);
	float line_height = ImGui::GetTextLineHeight();
	ImDrawList* draw_list = ImGui::GetWindowDrawList();

	ImGui::InvisibleButton(label, ImVec2(radius_outer * 2, radius_outer * 2 + line_height + style.ItemInnerSpacing.y));
	bool value_changed = false;
	bool is_active = ImGui::IsItemActive();
	bool is_hovered = ImGui::IsItemActive();
	if (is_active && io.MouseDelta.x != 0.0f)
	{
		float step = 0.5;
		*p_value += io.MouseDelta.x * step;

		if (*p_value <= -1) {
			*p_delta = (int)ceil(*p_value);
			*p_value -= *p_delta;
			value_changed = true;
		}
		else if (*p_value >= 1) {
			*p_delta = (int)floor(*p_value);
			*p_value -= *p_delta;
			value_changed = true;
		}
	}
	else
	{
		*p_value = 0;
		*p_delta = 0;
	}

	float radius_inner = radius_outer * 0.40f;
	draw_list->AddCircleFilled(center, radius_outer, ImGui::GetColorU32(ImGuiCol_FrameBg), 16);
	draw_list->AddCircleFilled(center, radius_inner, ImGui::GetColorU32(is_active ? ImGuiCol_FrameBgActive : is_hovered ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg), 16);
	auto R = ImGui::CalcTextSize(label);
	draw_list->AddText(ImVec2(center.x - R.x / 2, pos.y - line_height - style.ItemInnerSpacing.y), ImGui::GetColorU32(ImGuiCol_Text), label);

	//if (is_active || is_hovered)
	//{
	//	ImGui::SetNextWindowPos(ImVec2(pos.x - style.WindowPadding.x, pos.y - line_height - style.ItemInnerSpacing.y - style.WindowPadding.y));
	//	ImGui::BeginTooltip();
	//	ImGui::Text("%.3f", *p_value);
	//	ImGui::EndTooltip();
	//}

	return value_changed;
}


static bool MySlider(const char* label, float* p_value, float v_min, float v_max)
{
	ImGuiIO& io = ImGui::GetIO();
	ImGuiStyle& style = ImGui::GetStyle();

	float radius_outer = 20.0f;
	ImVec2 pos = ImGui::GetCursorScreenPos();
	ImVec2 center = ImVec2(pos.x + radius_outer, pos.y + radius_outer);
	float line_height = ImGui::GetTextLineHeight();
	ImDrawList* draw_list = ImGui::GetWindowDrawList();

	float POS_MIN = 5;
	float POS_MAX = 35;

	ImGui::InvisibleButton(label, ImVec2(radius_outer * 2, radius_outer * 4 + line_height + style.ItemInnerSpacing.y));
	bool value_changed = false;
	bool is_active = ImGui::IsItemActive();
	bool is_hovered = ImGui::IsItemActive();
	if (is_active && io.MouseDelta.y != 0.0f)
	{
		float step = (v_max - v_min) / 200.0f;
		*p_value -= io.MouseDelta.y * step;
		if (*p_value < v_min) *p_value = v_min;
		if (*p_value > v_max) *p_value = v_max;
		value_changed = true;
	}

	float t = ((*p_value - v_min) / (v_max - v_min))-0.5;
	t *= 2;
	auto R = ImGui::CalcTextSize(label);
	draw_list->AddText(ImVec2(center.x - R.x / 2, pos.y - line_height - style.ItemInnerSpacing.y), ImGui::GetColorU32(ImGuiCol_Text), label);
	draw_list->AddLine(ImVec2(center.x , center.y -  40), ImVec2(center.x , center.y + 40), ImGui::GetColorU32(ImGuiCol_ButtonHovered), 4);
	draw_list->AddLine(ImVec2(center.x - 4, center.y - t * 40), ImVec2(center.x + 4, center.y - t * 40), ImGui::GetColorU32(ImGuiCol_Text), 4);
	if (is_active || is_hovered)
	{
		ImGui::SetNextWindowPos(ImVec2(pos.x - style.WindowPadding.x, pos.y - line_height - style.ItemInnerSpacing.y - style.WindowPadding.y));
		ImGui::BeginTooltip();
		ImGui::Text("%.3f", *p_value);
		ImGui::EndTooltip();
	}

	return value_changed;
}

bool LedButton(const char* label, bool* v)
{
	
	ImGuiIO& io = ImGui::GetIO();
	ImGuiStyle& style = ImGui::GetStyle();

	float radius_outer = 10.0f;
	ImVec2 pos = ImGui::GetCursorScreenPos();
	ImVec2 center = ImVec2(pos.x + radius_outer, pos.y + radius_outer);
	float line_height = ImGui::GetTextLineHeight();
	ImDrawList* draw_list = ImGui::GetWindowDrawList();


	bool pressed = ImGui::InvisibleButton(label, ImVec2(radius_outer * 2, radius_outer * 2 + line_height + style.ItemInnerSpacing.y));
	bool is_active = ImGui::IsItemActive();
	bool is_hovered = ImGui::IsItemHovered();


	draw_list->AddCircleFilled(center, radius_outer, ImGui::GetColorU32(is_active ? ImGuiCol_FrameBgActive : is_hovered ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg), 16);
	auto R = ImGui::CalcTextSize(label);
	draw_list->AddText(ImVec2(center.x - R.x / 2, pos.y - line_height - style.ItemInnerSpacing.y), ImGui::GetColorU32(ImGuiCol_Text), label);

	//if (pressed) *v = !*v;
	if (*v)
	{
		draw_list->AddCircleFilled(center, radius_outer,IM_COL32(255,255,0,255) , 16);

	}

	return pressed;
}

bool ImLed(const char* label, bool* v)
{

	ImGuiIO& io = ImGui::GetIO();
	ImGuiStyle& style = ImGui::GetStyle();

	float radius_outer = 5.0f;
	ImVec2 pos = ImGui::GetCursorScreenPos();
	ImVec2 center = ImVec2(pos.x + radius_outer, pos.y + radius_outer);
	float line_height = ImGui::GetTextLineHeight();
	ImDrawList* draw_list = ImGui::GetWindowDrawList();


	bool is_active = ImGui::IsItemActive();
	bool is_hovered = ImGui::IsItemHovered();


	draw_list->AddCircleFilled(center, radius_outer, IM_COL32(0,0,0,255), 16);
	auto R = ImGui::CalcTextSize(label);
	draw_list->AddText(ImVec2(center.x - R.x / 2, pos.y - line_height - style.ItemInnerSpacing.y), ImGui::GetColorU32(ImGuiCol_Text), label);

	
	if (*v)
	{
		draw_list->AddCircleFilled(center, radius_outer, IM_COL32(255, 255, 0, 255), 16);

	}

	return false;
}
void on_short_message(const unsigned char &ch_msg, const unsigned char &data1, const unsigned char &data2, const unsigned char &data3)
{
	unsigned char type = ch_msg & 0xf0;

	// channel voice message
	switch (type) {
	case 0x80:
		// note off
		note_off(data1, data2);
		break;
	case 0x90:
		if (data2 > 0)
		{
			note_on(data1, data2);
		}
		else
		{
			note_off(data1, data2);
		}
		// note on
		break;
	case 0xA0:
		// polyphonic key pressure
		break;
	case 0xB0:
		// control change
		break;
	case 0xC0:
		// program change
		break;
	case 0xD0:
		// channel pressure
		break;
	case 0xE0:
		// pitch bend
		break;
	default:
		break;
	}
	_tprintf_s(_T("wMsg=MIM_DATA, ch_msg=%02x, data1=%02x, data2=%02x, data3=%02x\n"), ch_msg, data1, data2, data3);
}
void CALLBACK MidiInProc(HMIDIIN hMidiIn, UINT wMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2)
{
	unsigned char ch_msg, data1, data2, data3;
	switch (wMsg) {
	case MIM_OPEN:
		printf("wMsg=MIM_OPEN\n");
		break;
	case MIM_CLOSE:
		printf("wMsg=MIM_CLOSE\n");
		break;
	case MIM_DATA:
		printf("wMsg=MIM_DATA, dwInstance=%08x, dwParam1=%08x, dwParam2=%08x\n", dwInstance, dwParam1, dwParam2);

		ch_msg = (unsigned char)(0xFF & dwParam1);
		data1 = (unsigned char)(0xFF & (dwParam1 >> 8));
		data2 = (unsigned char)(0xFF & (dwParam1 >> 16));
		data3 = (unsigned char)(0xFF & (dwParam1 >> 24));
		on_short_message(ch_msg, data1, data2, data3);

		break;
	case MIM_LONGDATA:
		printf("wMsg=MIM_LONGDATA\n");
		break;
	case MIM_ERROR:
		printf("wMsg=MIM_ERROR\n");
		break;
	case MIM_LONGERROR:
		printf("wMsg=MIM_LONGERROR\n");
		break;
	case MIM_MOREDATA:
		printf("wMsg=MIM_MOREDATA\n");
		break;
	default:
		printf("wMsg = unknown\n");
		break;
	}
	return;
}


int T = 0;
int SerialStatus = 0;
int SerialCounter = 0;
uint32_t SerialData = 0;

int FindKnobIDX(int in)
{
	for (int i = 0; i < __KNOB_COUNT; i++)
	{
		if (Knobs[i].frontpanelidx == in)
		{
			return i;
		}
	}
	return -1;
}
int FindButtonIDX(int in)
{
	for (int i = 0; i < __LEDBUTTON_COUNT; i++)
	{
		if (Buttons[i].fpid == in)
		{
			return i;
		}
	}
	return -1;
}
void DoCommand(unsigned char comm, uint32_t data)
{
	switch (comm)
	{
	case 0x81:
	{
		uint16_t idx = data >> 8;
		uint16_t val = data & 0xff;
		int N = FindButtonIDX(idx);
		if (N > -1)
		{
			Buttons[N].value = ((val == 0) ? true : false);
		}
		printf("incoming button: %d, %d\n",idx, val);
		
		UIWriteLed(idx, val);
	}
	break;
	case 0x82:
	{
		uint16_t idx = data >> 16;
		uint16_t val = data & 0xffff;
		int TargetIDX = FindKnobIDX(idx);
		if (TargetIDX > -1)
		{
			Knobs[TargetIDX].value = val/1023.0f;
		//	printf("incoming pot: %d, %d\n", idx, val);
		}
	}
	break;
	case 0x90:
		printf("0x90: %d\n", data);
		break;
	case 0x91:
		printf("0x91: %d\n", data);
		break;
	}
}

int HandleSerial(unsigned char inb)
{
	if ((inb & 0x80) == 0x80)
	{
		SerialStatus = inb;
		SerialCounter = 4;
		SerialData = 0;
		return 1;
	}
	else
	{
		if (SerialCounter > 0)
		{
			SerialCounter--;
			SerialData += inb << ((3-SerialCounter) * 7);
			if (SerialCounter == 0)
			{
				DoCommand(SerialStatus, SerialData);
			}
			return 1;
		}

	}
	return 0;
}


int main(int argc, char** argv)
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0)
	{
		printf("Error: %s\n", SDL_GetError());
		return -1;
	}

	HMIDIIN hMidiDevice[255] = { 0 };
	DWORD nMidiPort = 0;
	UINT nMidiDeviceNum;
	MMRESULT rv;

	int dspport = 0;
	int uiport = 0;
	if (argc > 2)
	{
		dspport = atoi(argv[1]);
		uiport = atoi(argv[2]);
	}
	else
	{
		printf("please use PanSim.exe <dspportnum> <uiportnum> where params are integer! COM11: 11\n");
	}

	if (midiInGetNumDevs() > 0)
	{
		for (int i = 0; i < midiInGetNumDevs(); i++)
		{
			rv = midiInOpen(&hMidiDevice[i], i, (DWORD)(void*)MidiInProc, 0, CALLBACK_FUNCTION);
			if (rv != MMSYSERR_NOERROR) {
				fprintf(stderr, "midiInOpen() failed...rv=%d", rv);
				return -1;
			}
			midiInStart(hMidiDevice[i]);
		}
	}
	// Setup window
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
	SDL_DisplayMode current;
	SDL_GetCurrentDisplayMode(0, &current);
	SDL_Window *window = SDL_CreateWindow("Synton PAN Simulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 2000, 720, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	SetupIcon(window);
	SDL_GLContext glcontext = SDL_GL_CreateContext(window);
	SDL_GL_SetSwapInterval(1); // Enable vsync
	gl3wInit();

	// Setup ImGui binding
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
	ImGui_ImplSdlGL3_Init(window);

	// Setup style
	ImGui::StyleColorsLight();
	
	ImVec4 clear_color = ImVec4(1.0f / 255.0f, 58.0f / 255.0f, 66.0f / 255.0f, 1.00f);

	// racing turqoise: 1,58,66
	// haringmuts geel: 255, 234,0 

#define RACINGGREEN IM_COL32(1, 58, 66, 255)

	GLint BGtexture=0;
	//glGenTextures(&BGtexture, 1);

	ImTextureID BG = (ImTextureID)BGtexture;

	bool done = false;
	int32_t clkcolor = ImColor::HSV(0, 1, 1);
	int32_t tickcolor = ImColor::HSV(0.25, 1, 1);;// IM_COL32(0, 128, 255, 255);
	int32_t beatcolor = ImColor::HSV(0.5, 1, 1);
	int32_t loopcolor = ImColor::HSV(0.75, 1, 1);

	int32_t clklinecolor = ImColor::HSV(0, 1, 1,0.5);
	int32_t ticklinecolor = ImColor::HSV(0.25, 1, 1, 0.5);;// IM_COL32(0, 128, 255, 255);
	int32_t beatlinecolor = ImColor::HSV(0.5, 1, 1, 0.5);
	int32_t looplinecolor = ImColor::HSV(0.75, 1, 1, 0.5);


	int32_t gatecolor = ImColor::HSV(0, 0, 0.3);
	int32_t accentcolor = ImColor::HSV(0, 0, 1);

	ImFont* pFont = io.Fonts->AddFontFromFileTTF("ProggyTiny.ttf", 8.0f);
	ImFont* pFontBold = io.Fonts->AddFontFromFileTTF("ProggyTiny.ttf", 10.0f);

	unsigned char * pixels;
	int width, height, bytes_per_pixels;

	
	ImGui::GetStyle().ItemInnerSpacing = ImVec2(5, 5);;
	ImGui::GetStyle().ItemSpacing = ImVec2(5, 5);
	ImGui::GetStyle().FramePadding = ImVec2(5, 5);
	ImGui::GetStyle().Colors[ImGuiCol_WindowBg] = ImVec4(1.0f, 1.0f, 1.0f, .800f);

	static bool parameters = true;
	GetSerialPorts(dspport,uiport);

	Teensy_Reset();
	Raspberry_Reset();
	Teensy_InitPreset();

	setpara_t para;
	para.paramid = 0xfcfe;
	para.value = 3;
	set(para);
	unsigned char buffer[100];
	while (!done)
	{
		int bytecount = 0;
		int handledbytes = 0;
		if (UISerial.IsOpen())
		{
			int byteswaiting = UISerial.BytesWaiting();
			bytecount += byteswaiting;
			while (byteswaiting > 0)
			{
				int r = __min(100, byteswaiting);
				byteswaiting -= r;
				UISerial.Read(buffer, r);
				for (int i = 0; i < r; i++)
				{
					handledbytes += HandleSerial(buffer[i]);
				}
			}
		}
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			ImGui_ImplSdlGL3_ProcessEvent(&event);
			if (event.type == SDL_QUIT)
				done = true;
		}
		ImGui_ImplSdlGL3_NewFrame(window);

		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("PanSim Windows"))
			{
				ImGui::MenuItem("Pan Parameters", NULL, &parameters);
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}
		
		if (parameters)
		{
			ImGui::PushFont(pFontBold);
			{
				ImGui::Begin("Pan Parameters", &parameters, ImGuiWindowFlags_AlwaysAutoResize);
				ImGui::PushFont(pFont);
				if (BG) ImGui::Image(BG, ImVec2(2534, 1183));
				ImGui::LabelText("l1","%d left %d bytes this frame.. %d handled", bytecount - handledbytes, bytecount, handledbytes);
				ImVec2 pos = ImGui::GetCursorScreenPos();
				float xscalefac = 60;
				float yscalefac = 60;
				for (int i = 0; i < __KNOB_COUNT; i++)
				{
					ImGui::SetCursorScreenPos(ImVec2(pos.x + Knobs[i].x * xscalefac, pos.y + Knobs[i].y * yscalefac));
					if (Knobs[i].isslider==1)
					{
						if (MySlider(Knobs[i].name, &Knobs[i].value, 0, 1))
						{
							Teensy_KnobChanged(Knobs[i].id, uint32_t(floor((Knobs[i].value*65535.0))));
						}
					}
					else
					{
						if (MyKnob(Knobs[i].name, &Knobs[i].value, 0, 1, Knobs[i].label))
						{
							Teensy_KnobChanged(Knobs[i].id, uint32_t(floor((Knobs[i].value*65535.0))));
						}
					}
				}

				for (int i = 0; i < __LEDBUTTON_COUNT; i++)
				{
					ImGui::SetCursorScreenPos(ImVec2(pos.x + Buttons[i].x * xscalefac, pos.y + Buttons[i].y * yscalefac));
					if (LedButton(Buttons[i].name, &Buttons[i].value))
					{
						Teensy_ButtonPressed(Buttons[i].id, Buttons[i].value);

					}
				}
				for (int i = 0; i < __LED_COUNT; i++)
				{
					ImGui::SetCursorScreenPos(ImVec2(pos.x + Leds[i].x * xscalefac, pos.y + Leds[i].y * yscalefac));
					ImLed(Leds[i].name, &Leds[i].value);
				}
				for (int i = 0; i < __ENCODER_COUNT; i++)
				{
					ImGui::SetCursorScreenPos(ImVec2(pos.x + Encoders[i].x * xscalefac, pos.y + Encoders[i].y * yscalefac));
					if (MyEncoder(Encoders[i].name, &Encoders[i].pos, &Encoders[i].delta))
					{
						Raspberry_EncoderTurn(Encoders[i].id, Encoders[i].delta);
					}
				}

				ImGui::SetCursorScreenPos(ImVec2(pos.x + TheScreen.x * xscalefac, pos.y + TheScreen.y * yscalefac));
				ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0, 0, 0, 255));

				ImGui::BeginChild("screen", ImVec2(xscalefac * TheScreen.width, yscalefac * TheScreen.height), true);
				Raspberry_RenderScreen();
				ImGui::EndChild();
				ImGui::PopStyleColor();
				ImGui::PopFont();


				ImGui::End();
				ImGui::PopFont();
			}
		}

	

		glViewport(0, 0, (int)ImGui::GetIO().DisplaySize.x, (int)ImGui::GetIO().DisplaySize.y);
		glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT);
		ImGui::Render();
		ImGui_ImplSdlGL3_RenderDrawData(ImGui::GetDrawData());
		//ImGui::PopFont();
		SDL_GL_SwapWindow(window);
		SDL_Delay(0);
	}

	for (int i = 0; i < midiInGetNumDevs(); i++)
	{
		if (hMidiDevice[i])
		{
			midiInStop(hMidiDevice[i]);
			midiInClose(hMidiDevice[i]);
		}
	}
	ImGui_ImplSdlGL3_Shutdown();
	ImGui::DestroyContext();
	CloseSerialPorts();
	SDL_GL_DeleteContext(glcontext);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}
