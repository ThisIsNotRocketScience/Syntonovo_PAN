#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h> 
#include <bcm_host.h>
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include "../libs/imgui-master/imgui.h"
#include "imgui_impl_es2.h"
#include "../PanSim/PanHeader.h"

// Add our helper for creating OpenGL shaders and buffers
#include "gl_helper.h"


#include <tftgl.h>
extern "C"
{
	extern unsigned int tftglInitEgl(unsigned int flags);
	extern unsigned int tftglInitDisplay(unsigned int flags);
}


uint32_t vc_image_ptr;
DISPMANX_RESOURCE_HANDLE_T resource;
VC_RECT_T dst_rect;
VC_RECT_T src_rect;
VC_DISPMANX_ALPHA_T alpha = { DISPMANX_FLAGS_ALPHA_FIXED_ALL_PIXELS, 255, /*alpha 0->255*/  0 };

void Setup(int width, int height, DISPMANX_MODEINFO_T *info, DISPMANX_DISPLAY_HANDLE_T display)
{
	vc_dispmanx_rect_set(&dst_rect, 0, 0, width, height);
	resource = vc_dispmanx_resource_create(VC_IMAGE_RGBA32, width, height, &vc_image_ptr);
	vc_dispmanx_rect_set(&src_rect, 0, 0, width << 16, height << 16);
	vc_dispmanx_rect_set(&dst_rect,
		(info->width - width) / 2,
		(info->height - height) / 2,
		width,
		height);


	DISPMANX_UPDATE_HANDLE_T update = vc_dispmanx_update_start(0);




	DISPMANX_ELEMENT_HANDLE_T element =
		vc_dispmanx_element_add(update,
			display,
			3, // layer
			&dst_rect,
			resource,
			&src_rect,
			DISPMANX_PROTECTION_NONE,
			&alpha,
			NULL, // clamp
			DISPMANX_NO_ROTATE);

	int result = vc_dispmanx_update_submit_sync(update);


}

void
showSnapshot(
	DISPMANX_DISPLAY_HANDLE_T display,
	DISPMANX_MODEINFO_T *info,
	VC_IMAGE_TYPE_T type,
	int width,
	int height,
	int pitch,
	void *buffer)
{


	int result = vc_dispmanx_resource_write_data(resource,
		type,
		pitch,
		buffer,
		&dst_rect);

	//    sleep(10);

	  //  update = vc_dispmanx_update_start(0);
	   // result = vc_dispmanx_element_remove(update, element);
	  //  result = vc_dispmanx_update_submit_sync(update);

	  //  result = vc_dispmanx_resource_delete(resource);

}

unsigned char thepix[800 * 480 * 4];




void
fullscreen(
	DISPMANX_DISPLAY_HANDLE_T display,
	DISPMANX_MODEINFO_T *info)
{
	int width = 800;
	int height = 480;

	VC_IMAGE_TYPE_T type = VC_IMAGE_RGBA32;
	int pitch = ALIGN_UP(width * 4, 32);

	glReadPixels(0, 0, 800, 480, GL_RGBA, GL_UNSIGNED_BYTE, thepix);


	//  printf("displaying snapshot %dx%d\n", width, height);
	showSnapshot(display, info, type, width, height, pitch, thepix);

}

unsigned int SetupEGL(unsigned int flags) {

	unsigned int res;
	res = tftglInitEgl(flags);
	if (res != TFTGL_OK) return res;
	return TFTGL_OK;
}

const unsigned int gTextureSize = 1024;

GLuint       m_hTexture[2];  // Texture handles
GLuint       m_hFBO[2];      // Handles for Frame Buffer Objects

void MakeFrameBuffer()
{
	glGenTextures(2, m_hTexture);

	for (int Index = 0; Index < 2; Index++)
	{
		// Bind and configure each texture
		glBindTexture(GL_TEXTURE_2D, m_hTexture[Index]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, gTextureSize, gTextureSize, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}

	glGenFramebuffers(2, m_hFBO);

	for (int Index = 0; Index < 2; Index++)
	{
		// Attach each texture to the first color buffer of an FBO and clear it
		glBindFramebuffer(GL_FRAMEBUFFER, m_hFBO[Index]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_hTexture[Index], 0);
		glClear(GL_COLOR_BUFFER_BIT);
	}

}
ImVec4 clear_color = ImVec4(1.0f / 255.0f, 58.0f / 255.0f, 66.0f / 255.0f, 1.00f);

void UseFrameBuffer()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_hFBO[0]);
	glViewport(0, 0, gTextureSize, gTextureSize);
	glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
	glClear(GL_COLOR_BUFFER_BIT);
	
}
void UseNormal()
{
	glBindFramebuffer(GL_FRAMEBUFFER,0);
	glViewport(0, 0, 800, 480);

}

#define STRINGIFY(x) #x
static const char* vertexShaderCode = STRINGIFY(
	attribute vec3 pos;
	varying vec2 v_uv;
	void main() 
	{
		v_uv = (pos.yx) * 0.5 + vec2(0.5);
		v_uv.x *=480.0/1024.0;
		v_uv.y *=800.0/1024.0;
		v_uv.y += 224.0/1024.0;
		gl_Position = vec4(pos.x, pos.y, pos.z, 1.0);
	}
);

static const char* fragmentShaderCode = STRINGIFY(
	varying vec2 v_uv;
	uniform sampler2D u_texture;
	void main() {
		gl_FragColor = texture2D(u_texture, v_uv);
	
	}
);

char *VERTEX_SHADER_SRC =
"#version 100\n"
"attribute vec4 a_position;\n"
"varying vec2 v_uvcoord;\n"
"void main() {\n"
"  gl_Position = a_position;\n"
"  v_uvcoord = (a_position.xy + 0.5) * 2;\n"
"}\n";


char *FRAGMENT_SHADER_SRC =
"#version 100\n"
"precision mediump float;\n"
"varying vec2 v_uvcoord;\n"
"uniform sampler2D u_texture;\n"
"void main() {\n"
"   gl_FragColor = texture2D(u_texture, v_uvcoord);\n"
"   //test: gl_FragColor = vec4(0,0,1,1);\n"
"}\n";

GLuint program;

void InitShaders()
{
	program = compileShader(vertexShaderCode, fragmentShaderCode);
	if (program == 0) {
		printf("Failed to create OpenGL shader!\n");
		return;
	}

}

GLuint vbo[2];

// The following array holds vec3 data of 
// three vertex positions
static const GLfloat vboVertexData[] = {
	-1.0f, -1.0f, 0.0f,
	1.0f, -1.0f, 0.0f,
	-1.0f,  1.0f, 0.0f,

	1.0f, -1.0f, 0.0f,
	1.0f,  1.0f, 0.0f,
	-1.0f, 1.0f, 0.0f
};

GLint posLoc;

void SetupBuffers()
{
	// Create two Vertex Buffer Objects
		glGenBuffers(2, vbo);
	if (vbo[0] == 0 || vbo[1] == 0) {
		printf("Failed to create GL_ARRAY_BUFFER! Error: %s\n", glGetErrorStr());
		return ;
	}

	// The first buffer holds vertex positions
	// 3 vertices of 3D vectors of float data
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, 6 * 3 * sizeof(float), vboVertexData, GL_STATIC_DRAW);

	
	// Get attribute and uniform pointers from GLSL shader (program)
	glUseProgram(program);
	posLoc = glGetAttribLocation(program, "pos");
	glUseProgram(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void RenderQuad()
{
	

	
	// Bind first vertex buffer
	glEnableVertexAttribArray(posLoc);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glVertexAttribPointer(posLoc, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);



	glUseProgram(program);	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_hTexture[0]);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glBindTexture(GL_TEXTURE_2D, 0);	
	glUseProgram(0);

}
extern "C"
{

	int main(int argv, char** argc)
	{
		unsigned int i;
		struct timeval t1, t2;
		GLuint vbo[2];
		double elapsedTime;


		bcm_host_init();
		if (SetupEGL(TFTGL_LANDSCAPE) != TFTGL_OK) {
			fprintf(stderr, "Failed to initialize TFTGL library! Error: %s\n",
				tftglGetErrorStr());
			return EXIT_FAILURE;
		}

		DISPMANX_DISPLAY_HANDLE_T display = vc_dispmanx_display_open(0);

		DISPMANX_MODEINFO_T info;
		int result = vc_dispmanx_display_get_info(display, &info);


		Setup(800, 480, &info, display);

		// Get viewport size
		GLint viewport[4];
		glGetIntegerv(GL_VIEWPORT, viewport);

		printf("TFT display initialized with EGL! Screen size: %dx%d\n", viewport[2], viewport[3]);

		printf("OpenGL version is (%s)\n", glGetString(GL_VERSION));
		InitShaders();
		SetupBuffers();
		MakeFrameBuffer();

		// Setup ImGui binding
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
		ImGui_ImlES_Init();

		// Setup style
		ImGui::StyleColorsLight();

		ImVec4 clear_color = ImVec4(1.0f / 255.0f, 58.0f / 255.0f, 66.0f / 255.0f, 1.00f);
		glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
		


		Raspberry_Init();
		Raspberry_Reset();
		Raspberry_ToState(GuiState_Menu_VCO1, 0);


		for (int qq = 0; qq < 1000; qq++)
		{
			glViewport(0, 0, 800, 480);
			glClear(GL_COLOR_BUFFER_BIT);

			UseFrameBuffer();
			ImGui_ImlES_NewFrame();

			Raspberry_WindowFrame();

			ImGui::Render();
			ImGui_ImlES_RenderDrawLists(ImGui::GetDrawData());

			UseNormal();

			RenderQuad();

			fullscreen(display, &info);

			//}
		}

		ImGui_ImlES_Shutdown();
		// Cleanup GLSL shader and two buffers
		glDeleteProgram(program);
		glDeleteBuffers(2, vbo);


		vc_dispmanx_display_close(display);
		// Terminates everything (GPIO, LCD, and EGL)


		return EXIT_SUCCESS;
	}
}
