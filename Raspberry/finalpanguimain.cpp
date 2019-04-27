#include "bcm_host.h"

#include "EGL/egl.h"
#include "EGL/eglext.h"

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#include <stdio.h>
#include <assert.h>

#include "../PanSim/FinalPanEnums.h"
#include "../PanSim/FinalPanHeader.h"

#include "../libs/imgui-master/imgui.h"
#include "imgui_impl_es2.h"


extern void FinalPan_WindowFrame();
extern void FinalPan_LoadResources();
extern void FinalPan_SetupLeds();

/*
inline int sprintf_s(char* buffer, size_t sizeOfBuffer, const char* format, ...)
{
	va_list ap;
	va_start(ap, format);
	int result = vsnprintf(buffer, sizeOfBuffer, format, ap);
	va_end(ap);
	return result;
}

template<size_t sizeOfBuffer>
inline int sprintf_s(char(&buffer)[sizeOfBuffer], const char* format, ...)
{
	va_list ap;
	va_start(ap, format);
	int result = vsnprintf(buffer, sizeOfBuffer, format, ap);
	va_end(ap);
	return result;
}
*/
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

	dispman_element = vc_dispmanx_element_add(dispman_update, dispman_display,
		0/*layer*/, &dst_rect, 0/*src*/,
		&src_rect, DISPMANX_PROTECTION_NONE, 0 /*alpha*/, 0/*clamp*/, (DISPMANX_TRANSFORM_T)0/*transform*/);

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

	
	while (!terminate)
	{

		glViewport(0, 0, 1024,600);
		glClear(GL_COLOR_BUFFER_BIT);


		ImGui_ImlES_NewFrame();
		ImGui::SetNextWindowPos(ImVec2(0, 0));
		FinalPan_WindowFrame();
		FinalPan_SetupLeds();
		ImGui::Render();
		ImGui_ImlES_RenderDrawLists(ImGui::GetDrawData());


		eglSwapBuffers(state->display, state->surface);
	}
	exit_func();
	return 0;
}