#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>

#include "ShaderProgram.h"
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <ctime>
#include <vector>

#ifdef _WINDOWS
#define RESOURCE_FOLDER "" // perhaps this directory?????????????????????????????????????????????????????????????
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

#define STB_IMAGE_IMPLEMENTATION

SDL_Window* displayWindow;




class Player
{
public:
	Player() 
	{
		x = 0.0f;
		y = 0.0f;

		height = 2.0f;
	}

	// methods. Just realized I don't really need that many, if everything is public...

	void shoot()
	{

	}

	void Draw(ShaderProgram& p) {}


	// attributes
	float x, y;
	/// float rotation; 
	float width;
	float height;
	float velocity;
	/// float x_dir, y_dir;
	int textureID;
};






class Bullet // or Heart, if we don't want to be violent. Also, I really don't remember much about inheritance
{
public:
	Bullet()
	{
		width = 0.2f;
		height = 0.5f;

		velocity = 0.25f; // 0.25 units per frame
	}

	void Draw(ShaderProgram& p)
	{
		float vertices[] = { -0.1f, 0.25f,
							  0.1f, 0.25f,
							  0.1f, -0.25f,
							 -0.1f, -0.25f };

		glVertexAttribPointer(p.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
		glEnableVertexAttribArray(p.positionAttribute);
		glDrawArrays(GL_POLYGON, 0, 4);
		glDisableVertexAttribArray(p.positionAttribute);
	}

	// attributes
	float x, y;
	float velocity;

	/// float rotation; 
	float width;
	float height;
	/// float x_dir, y_dir;

	int textureID;
};





int main(int argc, char* argv[])
{
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Space Invaders", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 360, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);

#ifdef _WINDOWS
    glewInit();
#endif

	// beginning of setup

	glViewport(0, 0, 640, 360);
	ShaderProgram program;
	program.Load(RESOURCE_FOLDER"vertex.glsl", RESOURCE_FOLDER"fragment.glsl");
	glm::mat4 projectionMatrix = glm::mat4(1.0f);
	glm::mat4 viewMatrix = glm::mat4(1.0f);
	projectionMatrix = glm::ortho(-1.777f*5, 1.777f*5, -1.0f*5, 1.0f*5, -1.0f, 1.0f);
	glUseProgram(program.programID);

	/// time setup
	float lastFrameTicks = 0.0f;
	/// end time setup

	// end of setup







	int counter = 0;

	Player player;

	
	Bullet bullet;


    SDL_Event event;
    bool done = false;
    while (!done) 
	{

		// on every frame (in main loop)
		glm::mat4 modelMatrix = glm::mat4(1.0f); // set to identity
		modelMatrix = glm::translate(modelMatrix, glm::vec3(player.x, player.y, 0.0f)); //translate matrix by position of entity
		// draw object
		// rinse and repeat on next frame



		// start

		glClear(GL_COLOR_BUFFER_BIT);
		program.SetModelMatrix(modelMatrix);
		program.SetProjectionMatrix(projectionMatrix);
		program.SetViewMatrix(viewMatrix);
		
		float vertices[] = { -0.5f, 0.5f, 
			                  0.5f, 0.5f, 
			                  0.5f, -0.5f, 
							 -0.5f, -0.5f };

		glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
		glEnableVertexAttribArray(program.positionAttribute);
		glDrawArrays(GL_POLYGON, 0, 4);
		glDisableVertexAttribArray(program.positionAttribute);
		SDL_GL_SwapWindow(displayWindow);

		// end







		







		// time-keeping start

		float ticks = (float)SDL_GetTicks() / 1000.0f;
		float elapsed = ticks - lastFrameTicks;
		lastFrameTicks = ticks;

		// time-keeping end




		/* translation

		if (counter == 0)
			modelMatrix = glm::translate(modelMatrix, glm::vec3(2.0f, 0.0f, 1.0f));
		counter++;

		*/
		






        while (SDL_PollEvent(&event)) 
		{
            if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) 
			{
                done = true;
            }
			else if (event.type == SDL_KEYDOWN) 
			{
				if (event.key.keysym.scancode == SDL_SCANCODE_LEFT) 
					player.x -= 0.25f;
				else if (event.key.keysym.scancode == SDL_SCANCODE_RIGHT)
					player.x += 0.25f;
				else if (event.key.keysym.scancode == SDL_SCANCODE_SPACE)
				{

					player.shoot();
				}
			}
        }
        glClear(GL_COLOR_BUFFER_BIT);
    }
    
    SDL_Quit();
    return 0;
}
