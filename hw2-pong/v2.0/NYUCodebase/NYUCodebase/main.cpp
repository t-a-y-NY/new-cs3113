#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>

#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

#include "ShaderProgram.h"
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"

SDL_Window* displayWindow;

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"







class Paddles
{
public:
	float x;
	float y;

	float width;
	float height;
};


void setup()
{
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("My Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 360, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);

#ifdef _WINDOWS
	glewInit();
#endif

	glViewport(0, 0, 640, 360);
	ShaderProgram program;
	program.Load(RESOURCE_FOLDER"vertex.glsl", RESOURCE_FOLDER"fragment.glsl");

	glm::mat4 projectionMatrix = glm::mat4(1.0f);
	glm::mat4 modelMatrix = glm::mat4(1.0f);
	glm::mat4 viewMatrix = glm::mat4(1.0f);
	projectionMatrix = glm::ortho(-1.777f, 1.777f, -1.0f, 1.0f, -1.0f, 1.0f);
}


int main(int argc, char *argv[])
{
	setup();
	
	glUseProgram(program.programID);

    SDL_Event event;
    bool done = false;
    while (!done) {
		glClear(GL_COLOR_BUFFER_BIT);

		program.SetModelMatrix(modelMatrix);
		program.SetProjectionMatrix(projectionMatrix);
		program.SetViewMatrix(viewMatrix);
		
		float vertices[] = { 0.5f, -0.5f, 0.5f, 0.5f, -0.5f, -0.5f };
		
		glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
		glEnableVertexAttribArray(program.positionAttribute);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glDisableVertexAttribArray(program.positionAttribute);

		SDL_GL_SwapWindow(displayWindow);

		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
				done = true;
			}
			else if (event.type == SDL_MOUSEMOTION) {
				float unitX = (((float)event.motion.x / 640.0f) * 3.554f) - 1.777f;
				float unitY = (((float)(360 - event.motion.y) / 360.0f) * 2.0f) - 1.0f;
			}
		}

        glClear(GL_COLOR_BUFFER_BIT);
    }
    
    SDL_Quit();
    return 0;
}
