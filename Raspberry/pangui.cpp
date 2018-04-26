#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h> 
#include <bcm_host.h>
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

// Add our helper for creating OpenGL shaders and buffers
#include "gl_helper.h"

// The following array holds vec3 data of 
// three vertex positions
static const GLfloat vboVertexData[] = {
   -1.0f, -1.0f, 0.0f,
	1.0f, -1.0f, 0.0f,
	0.0f,  1.0f, 0.0f,
};

// The following array holds vec3 dara of 
// three vertex colors 
static const GLfloat vboColorData[] = {
    1.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 1.0f,
};

// The following are GLSL shaders for rendering
// a triangle on the screen
// Tutorial: https://www.khronos.org/assets/uploads/books/openglr_es_20_programming_guide_sample.pdf
#define STRINGIFY(x) #x
static const char* vertexShaderCode = STRINGIFY(
	attribute vec3 pos;
	attribute vec3 col;
	varying vec3 v_col;
	uniform float rot;
	void main() {
		v_col = col;
		float r = rot * 0.0174533; // degrees to radians
		float x = pos.x * cos(r) - pos.y * sin(r);
		float y = pos.x * sin(r) + pos.y * cos(r);
		gl_Position = vec4(x, y, pos.z, 1.0);
	}
);

static const char* fragmentShaderCode = STRINGIFY(
	varying vec3 v_col;
	void main() {
		gl_FragColor = vec4(v_col, 1.0);
	}
);

uint32_t vc_image_ptr;
DISPMANX_RESOURCE_HANDLE_T resource ;
VC_RECT_T dst_rect;
VC_RECT_T src_rect;
VC_DISPMANX_ALPHA_T alpha =
    {
        DISPMANX_FLAGS_ALPHA_FIXED_ALL_PIXELS, 
        255, /*alpha 0->255*/
        0
    };
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

unsigned char thepix [800*480*4];




void
fullscreen(
    DISPMANX_DISPLAY_HANDLE_T display,
    DISPMANX_MODEINFO_T *info)
{
    int width = 800;
    int height = 480 ;

    VC_IMAGE_TYPE_T type = VC_IMAGE_RGBA32;
    int pitch = ALIGN_UP(width * 4, 32);
    
	glReadPixels(0,0,800,480, GL_RGBA, GL_UNSIGNED_BYTE, thepix);
	
  
  //  printf("displaying snapshot %dx%d\n", width, height);
    showSnapshot(display, info, type, width, height, pitch, thepix);
    
}
#include <tftgl.h>
extern unsigned int tftglInitEgl(unsigned int flags);
extern unsigned int tftglInitDisplay(unsigned int flags);
unsigned int tadaaaa(unsigned int flags){
	printf("a") ;
	unsigned int res;
		
	printf("b") ;
	//res = tftglInitDisplay(flags);
//	if(res != TFTGL_OK)return res;
	//	printf("c") ;

	res = tftglInitEgl(flags);
	if(res != TFTGL_OK)return res;
		printf("d") ;

	return TFTGL_OK;
}


//==============================================================================
int main(int argv, char** argc){
	unsigned int i;
	struct timeval t1, t2;
	GLuint vbo[2];
	GLuint program;
	double elapsedTime;
	
	
	bcm_host_init();
	if(tadaaaa(TFTGL_LANDSCAPE) != TFTGL_OK){
		fprintf(stderr, "Failed to initialize TFTGL library! Error: %s\n",
			tftglGetErrorStr());
		return EXIT_FAILURE;
	}

	DISPMANX_DISPLAY_HANDLE_T display = vc_dispmanx_display_open(0);
    
    DISPMANX_MODEINFO_T info;
    int result = vc_dispmanx_display_get_info(display, &info);
    

	Setup(800,480,&info,display);

	// Get viewport size
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	
	printf("TFT display initialized with EGL! Screen size: %dx%d\n",
		viewport[2], viewport[3]);
	
	printf("OpenGL version is (%s)\n", glGetString(GL_VERSION));
	
	// See gl_helper.h for more infor or read any OpenGL tutorial for compiling
	// GLSL shaders (there are quite many on Google).
	program = compileShader(vertexShaderCode, fragmentShaderCode);
	if(program == 0){
		printf("Failed to create OpenGL shader!\n");
		return EXIT_FAILURE;
	}
	
	// Create two Vertex Buffer Objects
	glGenBuffers(2, vbo);
	if(vbo[0] == 0 || vbo[1] == 0){
		printf("Failed to create GL_ARRAY_BUFFER! Error: %s\n", glGetErrorStr());
		return EXIT_FAILURE;
	}
	
	// The first buffer holds vertex positions
	// 3 vertices of 3D vectors of float data
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, 3 * 3 * sizeof(float), vboVertexData, GL_STATIC_DRAW);
	
	// The second buffer holds color data for each vertex
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, 3 * 3 * sizeof(float), vboColorData, GL_STATIC_DRAW);
	
	// Get attribute and uniform pointers from GLSL shader (program)
	glUseProgram(program);
	GLint posLoc = glGetAttribLocation(program, "pos");
	GLint colorLoc = glGetAttribLocation(program, "col");
	GLint rotLoc = glGetUniformLocation(program, "rot");
	for (int qq=0;qq<10000;qq++)
	{
	
		//for(i = 0; i <= 36; i++){
			// Clean screen with black color
			glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
			glClear(GL_COLOR_BUFFER_BIT);
		
			// Bind first vertex buffer
			glEnableVertexAttribArray(posLoc);
			glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
			glVertexAttribPointer(posLoc, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
			
			// Bind second vertex buffer
			glEnableVertexAttribArray(colorLoc);
			glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
			glVertexAttribPointer(colorLoc, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
			
			// Set rotation
			glUniform1f(rotLoc, qq*0.2);
			
			// Draw triangles, total of 3 vertices
			glDrawArrays(GL_TRIANGLES, 0, 3);
			
			fullscreen(display, &info);

		//}
	}
	
	
	// Cleanup GLSL shader and two buffers
	glDeleteProgram(program);
	glDeleteBuffers(2, vbo);
	
	
	 vc_dispmanx_display_close(display);
	// Terminates everything (GPIO, LCD, and EGL)
	
	
	return EXIT_SUCCESS;
}