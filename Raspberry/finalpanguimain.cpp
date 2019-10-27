#include <vector>
#ifndef WIN32
#include "bcm_host.h"

#include "EGL/egl.h"
#include "EGL/eglext.h"

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <unistd.h>			//Used for UART
#include <termios.h>		//Used for UART
#include <pthread.h>



#include <stdio.h>
#include <assert.h>

#include "FinalPanEnums.h"
#include "FinalPanHeader.h"
#include "PanPreset.h"

#include "../libs/imgui-master/imgui.h"
#include "imgui_impl_es2.h"

int uart0_filestream = -1;
#include <fcntl.h>			//Used for UART



extern void FinalPan_WindowFrame(float DT);
extern void FinalPan_LoadResources();
extern void FinalPan_SetupLeds();
extern void FinalPan_Pause(bool pause);
//void FinalPan_SetupDefaultPreset();

extern PanPreset_t gCurrentPreset;
extern PanState_t gPanState;

typedef struct
{
	uint32_t screen_width;
	uint32_t screen_height;
	// OpenGL|ES objects
	DISPMANX_DISPLAY_HANDLE_T dispman_display;
	DISPMANX_ELEMENT_HANDLE_T dispman_element;
	EGLDisplay display;
	EGLSurface surface;
	EGLContext context;
} APP_STATE_T;

static void init_ogl();
static void redraw_scene();
static void exit_func(void);
static volatile bool terminate;
static APP_STATE_T _state, *state = &_state;

#define check() assert(glGetError() == 0)


static void init_ogl()
{
	int32_t success = 0;
	EGLBoolean result;
	EGLint num_config;

	static EGL_DISPMANX_WINDOW_T nativewindow;

	DISPMANX_ELEMENT_HANDLE_T dispman_element;
	DISPMANX_DISPLAY_HANDLE_T dispman_display;
	DISPMANX_UPDATE_HANDLE_T dispman_update;
	VC_RECT_T dst_rect;
	VC_RECT_T src_rect;

	static const EGLint attribute_list[] =
	{
		EGL_RED_SIZE, 8,
		EGL_GREEN_SIZE, 8,
		EGL_BLUE_SIZE, 8,
		EGL_ALPHA_SIZE, 8,
		EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
		EGL_NONE
	};

	static const EGLint context_attributes[] =
	{
		EGL_CONTEXT_CLIENT_VERSION, 2,
		EGL_NONE
	};
	EGLConfig config;

	// get an EGL display connection
	state->display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	assert(state->display != EGL_NO_DISPLAY);
	check();

	// initialize the EGL display connection
	result = eglInitialize(state->display, NULL, NULL);
	assert(EGL_FALSE != result);
	check();

	// get an appropriate EGL frame buffer configuration
	result = eglChooseConfig(state->display, attribute_list, &config, 1, &num_config);
	assert(EGL_FALSE != result);
	check();

	// get an appropriate EGL frame buffer configuration
	result = eglBindAPI(EGL_OPENGL_ES_API);
	assert(EGL_FALSE != result);
	check();

	// create an EGL rendering context
	state->context = eglCreateContext(state->display, config, EGL_NO_CONTEXT, context_attributes);
	assert(state->context != EGL_NO_CONTEXT);
	check();

	// create an EGL window surface
	success = graphics_get_display_size(0 /* LCD */, &state->screen_width, &state->screen_height);
	assert(success >= 0);

	dst_rect.x = 0;
	dst_rect.y = 0;
	dst_rect.width = state->screen_width;
	dst_rect.height = state->screen_height;

	src_rect.x = 0;
	src_rect.y = 0;
	src_rect.width = state->screen_width << 16;
	src_rect.height = state->screen_height << 16;

	dispman_display = vc_dispmanx_display_open(0 /* LCD */);
	dispman_update = vc_dispmanx_update_start(0);

	VC_DISPMANX_ALPHA_T alpha = { DISPMANX_FLAGS_ALPHA_FIXED_ALL_PIXELS,255,0 };

	dispman_element = vc_dispmanx_element_add(dispman_update, dispman_display,
		0/*layer*/, &dst_rect, 0/*src*/,
		&src_rect, DISPMANX_PROTECTION_NONE, &alpha /*alpha*/, 0/*clamp*/, (DISPMANX_TRANSFORM_T)0/*transform*/);

	nativewindow.element = dispman_element;
	nativewindow.width = state->screen_width;
	nativewindow.height = state->screen_height;
	vc_dispmanx_update_submit_sync(dispman_update);

	check();

	state->surface = eglCreateWindowSurface(state->display, config, &nativewindow, NULL);
	assert(state->surface != EGL_NO_SURFACE);
	check();

	// connect the context to the surface
	result = eglMakeCurrent(state->display, state->surface, state->surface, state->context);
	assert(EGL_FALSE != result);
	check();

	// Set background color and clear buffers
	glClearColor(0.15f, 0.25f, 0.35f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	check();

	printf("OpenGL set up a window size %dx%d\n", state->screen_width, &state->screen_height);


}


static void exit_func(void)
// Function to be passed to atexit().
{
	DISPMANX_UPDATE_HANDLE_T dispman_update;
	int s;
	// clear screen
	glClear(GL_COLOR_BUFFER_BIT);
	eglSwapBuffers(state->display, state->surface);

	eglDestroySurface(state->display, state->surface);

	dispman_update = vc_dispmanx_update_start(0);
	s = vc_dispmanx_element_remove(dispman_update, state->dispman_element);
	vc_dispmanx_update_submit_sync(dispman_update);
	s = vc_dispmanx_display_close(state->dispman_display);
	
	// Release OpenGL resources
	eglMakeCurrent(state->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
	eglDestroyContext(state->display, state->context);
	eglTerminate(state->display);

	// release texture buffers
	
} // exit_func()


extern void ByteReceived(unsigned char byte)
{
	printf("%c", byte);
}

#include "serial.h"
#include "serial.hpp"
#include "serial.cpp"
#include "sync/uart.h"
#include "sync/uart.cpp"

Serial S;

uart_t uart;
sync_state_t rpi_sync;

static pthread_mutex_t serial_critical_section_mutex;

#define SERIAL_ENTER_CRITICAL_SECTION() pthread_mutex_lock(&serial_critical_section_mutex);
#define SERIAL_LEAVE_CRITICAL_SECTION() pthread_mutex_unlock(&serial_critical_section_mutex);

std::vector<uint8_t> in_queue;

int waitfor_level = 60;

static void checkqueue()
{
	int do_call = 0;
	SERIAL_ENTER_CRITICAL_SECTION();
	if (in_queue.size() >= waitfor_level) do_call = 1;
	SERIAL_LEAVE_CRITICAL_SECTION();
	if (do_call) {
		uart.config.read_complete(0, uart.config.read_complete_data);
	}
}

static uint8_t serial_peek()
{
	SERIAL_ENTER_CRITICAL_SECTION();
	uint8_t b = in_queue.front();
	SERIAL_LEAVE_CRITICAL_SECTION();
	return b;
}

static void serial_read(uint8_t* data, int count)
{
	SERIAL_ENTER_CRITICAL_SECTION();
	for (int i = 0; i < count; i++) {
		data[i] = in_queue.front();
		in_queue.erase(in_queue.begin());
	}
	SERIAL_LEAVE_CRITICAL_SECTION();
}

static int serial_write(uint8_t* data, int count)
{
	SERIAL_ENTER_CRITICAL_SECTION();
	//printf("w ");
	for (int i = 0; i < count; i++) {
		//printf("%x ", data[i]);
		S.Put(data[i]);
	}
	//printf("\n");
	SERIAL_LEAVE_CRITICAL_SECTION();

	uart.config.write_complete(0, uart.config.write_complete_data);

	return count;
}

static void serial_waitfor(int level)
{
	SERIAL_ENTER_CRITICAL_SECTION();
	waitfor_level = level;
	SERIAL_LEAVE_CRITICAL_SECTION();
	checkqueue();
}

pthread_t serial_thread;

void* SerialThread(void*)
{
	while (1) {
		uint8_t b = S.Blocking_get();
		SERIAL_ENTER_CRITICAL_SECTION();
		//printf("%x ", b);
		in_queue.push_back(b);
		SERIAL_LEAVE_CRITICAL_SECTION();
		checkqueue();
	}

	return 0;
}


presetnames_t  presetnames = {0};
uint8_t mod_values[128] = {0};

void sync_data_func(int addr, uint8_t* data)
{
	//printf("sync_data(%x, %x %x %x %x)\n", addr, data[0], data[1], data[2], data[3]);
	if (addr & 3) return;
	if (addr >= 0 && addr < (int)sizeof(gCurrentPreset)) {
		*(uint32_t*)&((uint8_t*)&gCurrentPreset)[addr] = *(uint32_t*)data;
	}
	if (addr >= 0x1000000 && addr < 0x1000000 + (int)sizeof(presetnames)) {
		addr -= 0x1000000;
		*(uint32_t*)&((uint8_t*)&presetnames)[addr] = *(uint32_t*)data;
	}
	if (addr >= 0x2000000 && addr < 0x2000000 + (int)sizeof(mod_values)) {
		addr -= 0x2000000;
		//printf("mod %x: %x\n", addr, *(uint32_t*)data);
		*(uint32_t*)&((uint8_t*)&mod_values)[addr] = *(uint32_t*)data;
	}
}

#define CMD_PAD_ZERO		(0x11)
#define CMD_CALIBRATE		(0x12)
#define CMD_PRESET_LOAD		(0x21)
#define CMD_PRESET_STORE	(0x22)

#define OOB_UI_PAUSE		(0x41)
#define OOB_UI_CONTINUE		(0x42)
#define OOB_BUTTON_DOWN		(0x50)
#define OOB_BUTTON_UP		(0x51)
#define OOB_ENCODER_CCW		(0x56)
#define OOB_ENCODER_CW		(0x57)
#define OOB_ENCODER_DOWN	(0x58)
#define OOB_ENCODER_UP		(0x59)
#define OOB_SWITCH_CHANGE	(0x5F)

void cmd_pad_zero()
{
	sync_oob_word(&rpi_sync, CMD_PAD_ZERO, 0, 0);
}

void cmd_calibrate()
{
	sync_oob_word(&rpi_sync, CMD_CALIBRATE, 0, 0);
}

void cmd_preset_load(int presetid)
{
	if (presetid < 0 || presetid >= 256) return;
	uint8_t data[4] = {0};
	data[0] = presetid;
	sync_oob_word(&rpi_sync, CMD_PRESET_LOAD, *(uint32_t*)data, 0);
}

void cmd_preset_save(int presetid)
{
	if (presetid < 0 || presetid >= 256) return;
	uint8_t data[4] = {0};
	data[0] = presetid;
	sync_oob_word(&rpi_sync, CMD_PRESET_STORE, *(uint32_t*)data, 0);
}

#include "FinalPanEnums.h"

//void LedEncoderButtonPress(FinalEncoderEnum Button);
//void LedEncoderButtonLeft(FinalEncoderEnum Button);
//void LedEncoderButtonRight(FinalEncoderEnum Button);
//void LedButtonPressed(FinalLedButtonEnum button);

int sync_phase = 0;
void sync_complete(int status);

template <typename T>
class DiffSyncer
{
public:
	DiffSyncer(T* data, uint32_t address) : _data(data), _address(address), _resend_full(true)
	{
		_dummy = 0;
	}

	void reset()
	{
		_resend_full = true;
	}

	int run()
	{
		if (_resend_full) {
		//printf("%x - %x @ %x\n", 0, sizeof(T), _address);
			_resend_full = false;
			memcpy(&_prev, _data, sizeof(T));
			sync_block(&rpi_sync, (uint8_t*)&_prev, _address, sizeof(T), sync_complete);
			return 0;
		}

		for (int i = 0; i < sizeof(T); i += 4) {
			if ( *(uint32_t*)&((uint8_t*)_data)[i] != *(uint32_t*)&((uint8_t*)&_prev)[i] )
			{
				int j = i + 4;
				for (; j < sizeof(T); j += 4) {
					if ( *(uint32_t*)&((uint8_t*)_data)[j] == *(uint32_t*)&((uint8_t*)&_prev)[j] ) {
						break;
					}
				}
		//printf("%x - %x (max %x) @ %x\n", i, j, sizeof(T), _address);
				memcpy(&((uint8_t*)&_prev)[i], &((uint8_t*)_data)[i], j - i);
				sync_block(&rpi_sync, &((uint8_t*)&_prev)[i], _address + i, j - i, sync_complete);
				return 0;
			}
		}

		return 1;
	}
private:
	T _prev;
	uint32_t _dummy; // don't move it, it's for padding end of _prev
	T* _data;
	uint32_t _address;
	bool _resend_full;
};

volatile int sync_running = 1;
DiffSyncer<PanLedState_t> ledsync(&gPanState.s, 0x1000000);
DiffSyncer<PanPreset_t> presetsync(&gCurrentPreset, 0x0);

#undef SHOWSYNCPRINTF

int sync_oobdata_func(uint8_t cmd, uint32_t data)
{
	switch (cmd) {
	case OOB_UI_PAUSE:

		FinalPan_Pause(true);
//#ifdef SHOWSYNCPRINTF
		printf("OOB_UI_PAUSE\n");
//#endif
		ledsync.reset();
		presetsync.reset();
		sync_running = 1;
		break;
	case OOB_UI_CONTINUE:
		FinalPan_Pause(false);
//#ifdef SHOWSYNCPRINTF
		printf("OOB_UI_CONTINUE\n");
//#endif
		//FinalPan_SetupDefaultPreset();
		ledsync.reset();
		presetsync.reset();
		sync_running = 0;
		break;
	case OOB_BUTTON_DOWN:
#ifdef SHOWSYNCPRINTF
		printf("button down %d\n", data);
#endif
		LedButtonPressed((FinalLedButtonEnum)data);
		break;
	case OOB_BUTTON_UP:
#ifdef SHOWSYNCPRINTF
		printf("button up %d\n", data);
#endif
		break;
	case OOB_ENCODER_CCW:
#ifdef SHOWSYNCPRINTF
		printf("encoder dec %d %d\n", data & 0xff, data >> 8);
#endif
		LedEncoderButtonLeft((FinalEncoderEnum)(data & 0xFF), data>>8);
		break;
	case OOB_ENCODER_CW:
#ifdef SHOWSYNCPRINTF
		printf("encoder inc %d %d\n", data & 0xff, data >> 8);
#endif
		LedEncoderButtonRight((FinalEncoderEnum)(data & 0xFF), data>>8);
		break;
	case OOB_ENCODER_DOWN:
#ifdef SHOWSYNCPRINTF
		printf("encoder down %d\n", data);
#endif
		LedEncoderButtonPress((FinalEncoderEnum)data);
		break;
	case OOB_ENCODER_UP:
#ifdef SHOWSYNCPRINTF
		printf("encoder up %d\n", data);
#endif
		break;
	case OOB_SWITCH_CHANGE:
#ifdef SHOWSYNCPRINTF
		//printf("switch change %d\n", data);
#endif
		break;
	default:
#ifdef SHOWSYNCPRINTF
		printf("sync_oobdata(%d, %x)\n", cmd, data);
#endif
		break;
	}

	return 0;
}


void sync_complete(int status)
{
	sync_running = 1;
	if (status == 1) {
		sync_phase = 0;
		// sending is resumed by OOB_UI_CONTINUE triggering the timer
		return;
	}

	for (int i = 0; i < 2; i++) {
		
		switch (sync_phase) {
		case 0:
			if (ledsync.run() == 0) {
				return;
			}
			break;
		case 1:
			if (presetsync.run() == 0) {
				return;
			}
			break;
		}
		
		sync_phase++;
		if (sync_phase == 2) sync_phase = 0;
	}

	sync_running = 0;
}

int setupSerial(void)
{
	uart.rx.waitfor = serial_waitfor;
	uart.rx.peek = serial_peek;
	uart.rx.read = serial_read;
	uart.tx.write = serial_write;
	sync_init(&rpi_sync, &uart, sync_data_func, sync_oobdata_func);

	S.Connect("/dev/ttyAMA0", 500000);

	pthread_create(&serial_thread, NULL, SerialThread, 0);

	static int tmp = 0;
	sync_block(&rpi_sync, (uint8_t*)&tmp, 1, 1, sync_complete);


	return 0;
}





#include <sys/time.h>

long long current_timestamp() {
	struct timeval te;
	gettimeofday(&te, NULL); // get current time
	long long milliseconds = te.tv_sec * 1000LL + te.tv_usec / 1000; // calculate milliseconds
																	 // printf("milliseconds: %lld\n", milliseconds);
	return milliseconds;
}

int main()
{
	bcm_host_init();
	memset(state, 0, sizeof(*state));
	init_ogl();
	printf("starting ImGui\n");
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui_ImlES_Init();
	ImGui::StyleColorsLight();

	ImGui::GetStyle().ItemInnerSpacing = ImVec2(5, 5);;
	ImGui::GetStyle().ItemSpacing = ImVec2(5, 5);
	ImGui::GetStyle().FramePadding = ImVec2(5, 5);
	ImGui::GetStyle().Colors[ImGuiCol_WindowBg] = ImVec4(1.0f, 1.0f, 1.0f, .800f);


	printf("done starting ImGui\n");


	ImVec4 clear_color = ImVec4(1.0f / 255.0f, 58.0f / 255.0f, 66.0f / 255.0f, 1.00f);
	glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);

	printf("Loading Pan resources\n");

	FinalPan_LoadResources();
	printf("done loading Pan resources\n");

	printf("opening serial thread\n");
	setupSerial();
	

	auto t = current_timestamp();
	auto lasttick1 = t;
	auto lasttick100 = t;
	while (!terminate)
	{
		if (t - lasttick100 > 200) {
			lasttick100 = t;
			uart.config.timer_tick(uart.config.timer_tick_data);
		}
		if (t - lasttick1 > 1) {
			lasttick1 = t;
			if (!sync_running) sync_complete(0);
		}
		
		glViewport(0, 0, 1023,599);
		glClear(GL_COLOR_BUFFER_BIT);


		ImGui_ImlES_NewFrame();
		
		ImGui::SetNextWindowPos(ImVec2(0, 0));
		auto nt = current_timestamp();
		auto diff = nt - t;
		t = nt;

		FinalPan_WindowFrame(diff * 0.001);
		FinalPan_SetupLeds();
		ImGui::Render();
		ImGui_ImlES_RenderDrawLists(ImGui::GetDrawData());


		eglSwapBuffers(state->display, state->surface);
	}
	exit_func();
	return 0;
}


#endif

