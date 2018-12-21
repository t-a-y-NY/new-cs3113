#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>

#include <math.h>

#include "ShaderProgram.h"
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp" 

#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

SDL_Window* displayWindow;

class Paddle
{
public:
	Paddle(float position_x, float position_y, float width, float height) : 
		position_x(position_x), position_y(position_y), width(width), height(height) {}

	float position_x, position_y;
	float width, height;
	float velocity_x, velocity_y;
};

class Player : public Paddle
{
public:
	Player(float position_x, float position_y, float width, float height) : Paddle(position_x, position_y, width, height) {}
};

class Enemy : public Paddle 
{
public:
	Enemy(float position_x, float position_y, float width, float height) : 
		Paddle(position_x, position_y, width, height), velocity_x(0.0f), velocity_y(6.0f) {}

	float velocity_x;
	float velocity_y;

	int textureID;
};

class Ball
{
public:
	Ball() : position_x(0.0f), position_y(0.0f), width(0.5f), height(0.5f), velocity_x(9.0f), velocity_y(12.0f) {}

	float position_x, position_y;
	float width, height;
	float velocity_x;
	float velocity_y;

	int textureID;
};


void collisionHandler(const Paddle& paddle, Ball& ball)
{
	if (ball.position_y > paddle.position_y) // ball above paddle
	{
		if ((ball.position_y - paddle.position_y) - ball.height / 2 - paddle.height / 2 < 0) // if y-collision
		{
			// there's a y-collision. Now let's check for an x-collision

			if (paddle.position_x > ball.position_x) // ball left of paddle
			{
				if ((paddle.position_x - ball.position_x) - paddle.width / 2 - ball.width / 2 < 0) // if x-collision
				{
					ball.position_x -= fabs((paddle.position_x - ball.position_x) - paddle.width / 2 - ball.width / 2) + 0.5f;
					ball.velocity_x *= -1;

					ball.position_y += (ball.position_y - paddle.position_y) - ball.height / 2 - paddle.height / 2 + 0.5f;
					ball.velocity_y *= -1;
				}
			}
			else if (paddle.position_x < ball.position_x) // ball right of paddle
			{
				if ((paddle.position_x - ball.position_x) - paddle.width / 2 - ball.width / 2 > 0) // if x-collision
				{
					ball.position_x += fabs((paddle.position_x - ball.position_x) - paddle.width / 2 - ball.width / 2) + 0.5f;
					ball.velocity_x *= -1;

					ball.position_y += (ball.position_y - paddle.position_y) - ball.height / 2 - paddle.height / 2 + 0.5f;
					ball.velocity_y *= -1;
				}
			}
		}
	}
	else if (ball.position_y < paddle.position_y) // ball below paddle
	{
		if ((ball.position_y - paddle.position_y) - ball.height / 2 - paddle.height / 2 > 0) // if y-collision
		{
			if (paddle.position_x > ball.position_x) // ball left of paddle
			{
				if ((paddle.position_x - ball.position_x) - paddle.width / 2 - ball.width / 2 < 0) // if x-collision
				{
					ball.position_x -= fabs((paddle.position_x - ball.position_x) - paddle.width / 2 - ball.width / 2) + 0.5f;
					ball.velocity_x *= -1;

					ball.position_y -= (ball.position_y - paddle.position_y) - ball.height / 2 - paddle.height / 2 + 0.5f;
					ball.velocity_y *= -1;
				}
			}
			else if (paddle.position_x < ball.position_x) // ball right of paddle
			{
				if ((paddle.position_x - ball.position_x) - paddle.width / 2 - ball.width / 2 > 0) // if x-collision
				{
					ball.position_x += fabs((paddle.position_x - ball.position_x) - paddle.width / 2 - ball.width / 2) + 0.5f;
					ball.velocity_x *= -1;

					ball.position_y -= (ball.position_y - paddle.position_y) - ball.height / 2 - paddle.height / 2 + 0.5f;
					ball.velocity_y *= -1;
				}
			}
		}
	}
}

int main(int argc, char *argv[])
{
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Pong Unleashed", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 360, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);

#ifdef _WINDOWS
    glewInit();
#endif

	// start setup

	glViewport(0, 0, 640, 360);

	ShaderProgram program;
	program.Load(RESOURCE_FOLDER"vertex.glsl", RESOURCE_FOLDER"fragment.glsl"); 
	glm::mat4 projectionMatrix = glm::mat4(1.0f);
	// glm::mat4 modelMatrix = glm::mat4(1.0f);
	glm::mat4 viewMatrix = glm::mat4(1.0f);
	projectionMatrix = glm::ortho(-1.777f*10, 1.777f*10, -1.0f*10, 1.0f*10, -1.0f, 1.0f);
	glUseProgram(program.programID);

	// end setup

    SDL_Event event;
    bool done = false;

	float lastFrameTicks = 0.0f;
	Player player(12.0f, 2.0f, 0.5f, 5.0f); // position_x(12.0f), position_y(2.0f), width(0.5f), height(5.0f)
	Enemy enemy(-12.0f, 2.0f, 0.5f, 5.0f); // position_x(-12.0f), position_y(2.0f), width(0.5f), height(5.0f), velocity_x(0.0f), velocity_y(6.0f)
	Ball ball;

	while (!done) 
	{
		// time-keeping start

		float ticks = (float)SDL_GetTicks() / 1000.0f;
		float elapsed = ticks - lastFrameTicks;
		lastFrameTicks = ticks;

		// time-keeping end

		// projection matrix and view matrix and whatever glClear() does
		glClear(GL_COLOR_BUFFER_BIT);

		program.SetProjectionMatrix(projectionMatrix);
		program.SetViewMatrix(viewMatrix);
		// end 


		// on every frame (in main loop)
		glm::mat4 player_modelMatrix = glm::mat4(1.0f); // set to identity
		player_modelMatrix = glm::translate(player_modelMatrix, glm::vec3(player.position_x, player.position_y, 0.0f)); //translate matrix by position of entity

		// draw object
		// rinse and repeat on next frame

		// draw

		program.SetModelMatrix(player_modelMatrix);

		float player_vertices[] = {
			-player.width / 2.0f, player.height / 2.0f,
			player.width / 2.0f, player.height / 2.0f,
			player.width / 2.0f, -player.height / 2.0f,
			-player.width / 2.0f, -player.height / 2.0f
		};

		glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, player_vertices);
		glEnableVertexAttribArray(program.positionAttribute);
		glDrawArrays(GL_POLYGON, 0, 4);
		glDisableVertexAttribArray(program.positionAttribute);





		glm::mat4 enemy_modelMatrix = glm::mat4(1.0f);
		enemy.position_x += enemy.velocity_x * elapsed;
		enemy.position_y += enemy.velocity_y * elapsed;
		if (enemy.position_y + (enemy.height / 2.0f) >= 1.0f * 10 || enemy.position_y - (enemy.height / 2.0f) <= -1.0f * 10)
			enemy.velocity_y *= -1;
		enemy_modelMatrix = glm::translate(enemy_modelMatrix, glm::vec3(enemy.position_x, enemy.position_y, 0.0f));

		program.SetModelMatrix(enemy_modelMatrix);
		
		float enemy_vertices[] = {
			-enemy.width / 2.0f, enemy.height / 2.0f,
			enemy.width / 2.0f, enemy.height / 2.0f,
			enemy.width / 2.0f, -enemy.height / 2.0f,
			-enemy.width / 2.0f, -enemy.height / 2.0f
		};

		glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, enemy_vertices);
		glEnableVertexAttribArray(program.positionAttribute);
		glDrawArrays(GL_POLYGON, 0, 4);
		glDisableVertexAttribArray(program.positionAttribute);




		// ball
		glm::mat4 ball_modelMatrix = glm::mat4(1.0f); // set to identity
		ball_modelMatrix = glm::translate(ball_modelMatrix, glm::vec3(ball.position_x, ball.position_y, 0.0f)); //translate matrix by position of entity

		// draw object
		// rinse and repeat on next frame

		// draw
		
		ball.position_x += ball.velocity_x * elapsed;
		ball.position_y += ball.velocity_y * elapsed;
		if (ball.position_y + (ball.height / 2.0f) >= 1.0f * 10 || ball.position_y - (ball.height / 2.0f) <= -1.0f * 10)
			ball.velocity_y *= -1;
		if (ball.position_x + (ball.height / 2.0f) >= 1.777f * 10 || ball.position_x - (ball.height / 2.0f) <= -1.777f * 10)
		{
			ball.position_x = 0.0f;
			ball.position_y = 0.0f;
		}
		collisionHandler(player, ball);
		collisionHandler(enemy, ball);

		program.SetModelMatrix(ball_modelMatrix);

		float ball_vertices[] = {
			-ball.width / 2.0f, ball.height / 2.0f,
			ball.width / 2.0f, ball.height / 2.0f,
			ball.width / 2.0f, -ball.height / 2.0f,
			-ball.width / 2.0f, -ball.height / 2.0f
		};

		glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, ball_vertices);
		glEnableVertexAttribArray(program.positionAttribute);
		glDrawArrays(GL_POLYGON, 0, 4);
		glDisableVertexAttribArray(program.positionAttribute);
		// end ball


		SDL_GL_SwapWindow(displayWindow);
		// end draw

		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE)
			{
				done = true;
			}
			else if (event.type == SDL_KEYDOWN)
			{
				if (event.key.keysym.scancode == SDL_SCANCODE_UP)
					player.position_y += 0.5f;
				else if (event.key.keysym.scancode == SDL_SCANCODE_DOWN)
					player.position_y -= 0.5f;
			}
		}
        glClear(GL_COLOR_BUFFER_BIT);
    }
    
    SDL_Quit();
    return 0;
}
