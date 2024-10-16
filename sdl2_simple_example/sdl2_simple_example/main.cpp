#include <GL/glew.h>
#include <chrono>
#include <thread>
#include <exception>
#include <glm/glm.hpp>
#include <SDL2/SDL_events.h>
#include "imgui_impl_sdl2.h"
#include "MyWindow.h"
#include "IL/il.h"
#include "Windows.h"
using namespace std;

using hrclock = chrono::high_resolution_clock;
using u8vec4 = glm::u8vec4;
using ivec2 = glm::ivec2;
using vec3 = glm::dvec3;

static const ivec2 WINDOW_SIZE(1024, 1024);
static const unsigned int FPS = 60;
static const auto FRAME_DT = 1.0s / FPS;

static void init_openGL() {
	glewInit();
	if (!GLEW_VERSION_3_0) throw exception("OpenGL 3.0 API is not available.");
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.5, 0.5, 0.5, 1.0);
}

static void init_deviL() {
	ilInit();
}

wchar_t* convertToWideString(const char* str) {
	int length = MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0);
	wchar_t* wideStr = new wchar_t[length];
	MultiByteToWideChar(CP_UTF8, 0, str, -1, wideStr, length);
	return wideStr;
}



GLfloat vertices[] = {
	//delante
	0.0f,  0.0f,  0.0f,   1.0f,  0.0f,  0.0f,   1.0f, -1.0f,  0.0f,   0.0f, -1.0f,  0.0f,  
	//derecha
	0.0f,  0.0f,  0.0f,   0.0f, -1.0f,  0.0f,   0.0f, -1.0f, -1.0f,   0.0f,  0.0f, -1.0f,  
	//arriba
	0.0f,  0.0f,  0.0f,   0.0f,  0.0f, -1.0f,   1.0f,  0.0f, -1.0f,   1.0f,  0.0f,  0.0f, 
	//abajo
	1.0f, -1.0f,  0.0f,   1.0f, -1.0f, -1.0f,   0.0f, -1.0f, -1.0f,   0.0f, -1.0f,  0.0f,  
	//atras
	1.0f,  0.0f, -1.0f,   0.0f,  0.0f, -1.0f,   0.0f, -1.0f, -1.0f,   1.0f, -1.0f, -1.0f,  
	//izquierda
	1.0f,  0.0f,  0.0f,   1.0f,  0.0f, -1.0f,   1.0f, -1.0f, -1.0f,   1.0f, -1.0f,  0.0f  
};

GLuint indices[] = {
	// delante
	0, 1, 2,  2, 3, 0,
	// derecha
	4, 5, 6,  6, 7, 4,
	// arriba
	8, 9, 10,  10, 11, 8,
	// abajo
	12, 13, 14,  14, 15, 12,
	// atrás
	16, 17, 18,  18, 19, 16,
	// izquierda
	20, 21, 22,  22, 23, 20
};

GLfloat colors[] = {
	//delante rojo
	1.0f, 0.0f, 0.0f, 1.0f,  
	1.0f, 0.0f, 0.0f, 1.0f, 
	1.0f, 0.0f, 0.0f, 1.0f,  
	1.0f, 0.0f, 0.0f, 1.0f,  

	//derecha verde
	0.0f, 1.0f, 0.0f, 1.0f,  
	0.0f, 1.0f, 0.0f, 1.0f,  
	0.0f, 1.0f, 0.0f, 1.0f, 
	0.0f, 1.0f, 0.0f, 1.0f,  

	//arrriba azul
	0.0f, 0.0f, 1.0f, 1.0f,  
	0.0f, 0.0f, 1.0f, 1.0f,  
	0.0f, 0.0f, 1.0f, 1.0f, 
	0.0f, 0.0f, 1.0f, 1.0f,  

	//abajo amarillo
	1.0f, 1.0f, 0.0f, 1.0f,  
	1.0f, 1.0f, 0.0f, 1.0f, 
	1.0f, 1.0f, 0.0f, 1.0f, 
	1.0f, 1.0f, 0.0f, 1.0f,  

	//atras cian
	0.0f, 1.0f, 1.0f, 1.0f,  
	0.0f, 1.0f, 1.0f, 1.0f,  
	0.0f, 1.0f, 1.0f, 1.0f,  
	0.0f, 1.0f, 1.0f, 1.0f,  

	//izquierda magenta
	1.0f, 0.0f, 1.0f, 1.0f,  
	1.0f, 0.0f, 1.0f, 1.0f,  
	1.0f, 0.0f, 1.0f, 1.0f,  
	1.0f, 0.0f, 1.0f, 1.0f   
};

GLfloat texCoords[] = {
	// delante
	0.0f, 0.0f,   1.0f, 0.0f,   1.0f, 1.0f,   0.0f, 1.0f,

	// derecha
	0.0f, 0.0f,   1.0f, 0.0f,   1.0f, 1.0f,   0.0f, 1.0f,

	// arriba
	0.0f, 0.0f,   1.0f, 0.0f,   1.0f, 1.0f,   0.0f, 1.0f,

	// abajo
	0.0f, 0.0f,   1.0f, 0.0f,   1.0f, 1.0f,   0.0f, 1.0f,

	// atrás
	0.0f, 0.0f,   1.0f, 0.0f,   1.0f, 1.0f,   0.0f, 1.0f,

	// izquierda
	0.0f, 0.0f,   1.0f, 0.0f,   1.0f, 1.0f,   0.0f, 1.0f
};

GLuint ImageTexture() {

	//Genera ID de la foto
	ILuint imageID;
	ilGenImages(1, &imageID);
	ilBindImage(imageID);

	ilLoadImage((const wchar_t*)"MinecraftFace.png");

	// Convert the image to RGBA format
	ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);

	// Generate texture
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	// Set texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // Use linear filtering for better quality
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Upload texture data
	GLubyte* imageData = ilGetData();
	if (imageData) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ilGetInteger(IL_IMAGE_WIDTH),
			ilGetInteger(IL_IMAGE_HEIGHT), 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);
	}

	// Cleanup
	ilDeleteImages(1, &imageID); // Delete image from DevIL

	return textureID; // Return the texture ID
}

void renderTexture(){

	
	GLubyte checkerImage[64][64][4];
	for (int i = 0; i < 64; i++) {
		for (int j = 0; j < 64; j++) {
			int c = ((((i & 0x8) == 0) ^ (((j & 0x8)) == 0))) * 255;
			checkerImage[i][j][0] = (GLubyte)c;
			checkerImage[i][j][1] = (GLubyte)c;
			checkerImage[i][j][2] = (GLubyte)c;
			checkerImage[i][j][3] = (GLubyte)255;
		}
	}
	
	GLuint textureID;
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 64, 64, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, checkerImage);

	glEnable(GL_TEXTURE_2D);

}

static void display_func() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, 1.0f, 0.1f, 100.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(3.0f, 3.0f, 3.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

	static float angle = 0.0f;
	angle += 0.1f;

	glRotatef(angle, 0.0f, 1.0f, 0.0f);

	//renderTexture();

	ImageTexture();


	/*
		glBegin(GL_TRIANGLES);  // draw a cube with 12 triangles
		// front face =================
		glVertex3f(0.0f, 0.0f, 0.0f);    // v0-v1-v2
		glVertex3f(1.0f, 0.0f, 0.0f);
		glVertex3f(1.0f, -1.0f, 0.0f);
		glColor4f(1.0f, 0.0f, 0.0f, 0.0f);//red

		glVertex3f(1.0f, -1.0f, 0.0f);    // v2-v3-v0
		glVertex3f(0.0f, -1.0f, 0.0f);
		glVertex3f(0.0f, 0.0f, 0.0f);



		// right face =================
		glVertex3f(0.0f, 0.0f, 0.0f);    // v0-v3-v4
		glVertex3f(0.0f, -1.0f, 0.0f);
		glVertex3f(0.0f, -1.0f, -1.0f);

		glVertex3f(0.0f, -1.0f, -1.0f);    // v4-v5-v0
		glVertex3f(0.0f, 0.0f, -1.0f);
		glVertex3f(0.0f, 0.0f, 0.0f);

		// top face ===================
		glVertex3f(0.0f, 0.0f, 0.0f);    // v0-v5-v6
		glVertex3f(0.0f, 0.0f, -1.0f);
		glVertex3f(1.0f, 0.0f, -1.0f);

		glVertex3f(1.0f, 0.0f, -1.0f);    // v6-v1-v0F
		glVertex3f(1.0f, 0.0f, 0.0f);
		glVertex3f(0.0f, 0.0f, 0.0f);

		//...                 // draw other 3 faces
		// bottom face ===================
		glVertex3f(1.0f, -1.0f, 0.0f);    // v3-v2-v7
		glVertex3f(1.0f, -1.0f, -1.0f);
		glVertex3f(0.0f, -1.0f, -1.0f);

		glVertex3f(0.0f, -1.0f, -1.0f);    // v7-v4-v3
		glVertex3f(0.0f, -1.0f, 0.0f);
		glVertex3f(1.0f, -1.0f, 0.0f);

		// back face =====================
		glVertex3f(1.0f, 0.0f, -1.0f);    // v6-v5-v4
		glVertex3f(0.0f, 0.0f, -1.0f);
		glVertex3f(0.0f, -1.0f, -1.0f);

		glVertex3f(0.0f, -1.0f, -1.0f);    // v4-v7-v6
		glVertex3f(1.0f, -1.0f, -1.0f);
		glVertex3f(1.0f, 0.0f, -1.0f);

		// left face ===================
		glVertex3f(1.0f, 0.0f, 0.0f);    // v1-v6-v7
		glVertex3f(1.0f, 0.0f, -1.0f);
		glVertex3f(1.0f, -1.0f, -1.0f);

		glVertex3f(1.0f, -1.0f, -1.0f);    // v7-v2-v1
		glVertex3f(1.0f, -1.0f, 0.0f);
		glVertex3f(1.0f, 0.0f, 0.0f);

			glEnd();
		*/
	 //Esto es cubo de forma directa

	//Activa las cosas
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	//punteros a vertices y colores
	glVertexPointer(3, GL_FLOAT, 0, vertices);

	//colorin colorado, aqui estan los colores pintados
	glColorPointer(4, GL_FLOAT, 0, colors);

	//textura
	glTexCoordPointer(2, GL_FLOAT, 0, texCoords);

	// Dibuja el cubo usando el array de índices
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, indices);

	//Desactiva las cosas
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

static bool processEvents() {
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_QUIT:
			return false;
			break;

		default:
			ImGui_ImplSDL2_ProcessEvent(&event);
			break;
		}
	}
	return true;
}

int main(int argc, char** argv) {
	MyWindow window("SDL2 Simple Example", WINDOW_SIZE.x, WINDOW_SIZE.y);

	init_openGL();
	init_deviL();

	while (processEvents()) {
		const auto t0 = hrclock::now();
		display_func(); 
		window.swapBuffers();
		const auto t1 = hrclock::now();
		const auto dt = t1 - t0;
		if(dt<FRAME_DT) this_thread::sleep_for(FRAME_DT - dt);
	}

	return 0;
}