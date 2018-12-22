#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>

#include <vector>
#include <math.h>
#include <stdlib.h>

#include "ShaderProgram.h"
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp" 

#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

SDL_Window* displayWindow;

const float gravity = -20.0f;
const float friction_x = 5.0f;
const float friction_y = 0.0f;
const float maxXSpeed = 20.0f;

int entityCounter = 0; // to uniquely identify every entity in the game

float lerp(float v0, float v1, float t)
{
	return (1.0 - t)*v0 + t * v1;
}

class Entity
{
public:
	Entity(float pos_x, float pos_y, float vel_x, float vel_y, float accel_x, float width, float height, bool isStatic, bool isEnemy = false)
		: pos_x(pos_x), pos_y(pos_y), vel_x(vel_x), vel_y(vel_y),
		accel_x(accel_x), accel_y(gravity), width(width), height(height), isStatic(isStatic), uniqueIdentifier(entityCounter + 1)
	{
		entityCounter += 1;
	}

	float pos_x, pos_y;
	float vel_x, vel_y;
	float accel_x, accel_y;

	float width, height;

	bool isStatic; // true if static, false if dynamic
	int uniqueIdentifier;
	bool isEnemy;

	void Draw(ShaderProgram& program, float elapsed);
	void collisionHandler();
};

class Ground : public Entity
{
public:
	/* doesn't take isStatic, vel_x, vel_y, or accel values as parameters
	*/
	Ground(float pos_x, float pos_y, float width, float height)
		: Entity(pos_x, pos_y, 0.0f, 0.0f, 0.0f, width, height, true) {}

};

class Player : public Entity
{
public:
	/* doesn't take isStatic value as parameter
	*/
	Player(float pos_x, float pos_y, float vel_x, float vel_y, float accel_x, float width, float height)
		: Entity(pos_x, pos_y, vel_x, vel_y, accel_x, width, height, false) {}

};

class Enemy : public Entity
{
public:
	Enemy(float pos_x, float pos_y, float vel_x, float vel_y, float accel_x, float width, float height)
		: Entity(pos_x, pos_y, vel_x, vel_y, accel_x, width, height, false, true) {}

	void move();

	bool checkIfDead();
};

void Enemy::move()
{
	if (rand() % 2 + 1 == 1)
		vel_x -= 20.0f;
	else if (rand() % 2 + 1 == 2)
		vel_x += 20.0f;
}

bool Enemy::checkIfDead()
{
	bool dead = false;
	if (pos_y < -20.0f)
		dead = true;

	return dead;
}

std::vector<Entity> entities;

void Entity::collisionHandler()
{
	for (int i = 0; i <= entities.size() - 1; i++)
	{
		bool xCollision = false;
		bool yCollision = false;
		bool right = false;
		bool left = false;
		bool up = false;
		bool down = false;

		float xDiff = 0;
		float yDiff = 0;

		if (entities[i].uniqueIdentifier != uniqueIdentifier)
		{
			// right/left check
			if (pos_x > entities[i].pos_x)
				right = true;
			else if (pos_x < entities[i].pos_x)
				left = true;

			// x collision check
			if (right)
				xDiff = pos_x - entities[i].pos_x - (width / 2) - (entities[i].width / 2);
			else if (left)
				xDiff = entities[i].pos_x - pos_x - (width / 2) - (entities[i].width / 2);

			if (xDiff <= 0)
				xCollision = true;

			// up/down check
			if (pos_y > entities[i].pos_y)
				up = true;
			else if (pos_y < entities[i].pos_y)
				down = true;

			// y collision check
			if (up)
				yDiff = pos_y - entities[i].pos_y - (height / 2) - (entities[i].height / 2);
			else if (down)
				yDiff = entities[i].pos_y - pos_y - (height / 2) - (entities[i].height / 2);

			if (yDiff <= 0)
				yCollision = true;

			// collision handling

			if (xCollision && yCollision)
			{
				/*
				if (right)
					pos_x += 1.0f;
				else if (left)
					pos_x -= 1.0f;
				*/

				if (entities[i].isEnemy == true)
				{
					pos_x = 0.0f; 
					pos_y = 0.0f; 
					vel_x = 0.0f; 
					vel_y = 0.0f; 
					accel_x = 0.0f;
				}

				if (up)
				{
					pos_y = entities[i].pos_y + entities[i].height / 2 + height / 2;
					vel_y = 0;
				}
				else if (down)
					pos_y -= yDiff + 1.0f;
			}
		}
	}



	/*
	bool collisionXLeft = false;
	bool collisionXRight = false;
	bool collisionYAbove = false;
	bool collisionYBelow = false;
	for (int i = 0; i <= entities.size() - 2; i++) // skip last one
	{
		for (int j = i + 1; j <= entities.size() - 1; j++)
		{
			if (entities[j].pos_x > entities[i].pos_x) // left
			{
				if (fabs(entities[j].pos_x - entities[i].pos_x) - fabs(entities[j].width / 2) - fabs(entities[i].width / 2) < 0) // if collision
					collisionXLeft = true;
			}
			else if (entities[j].pos_x < entities[i].pos_x) // right
			{
				if (fabs(entities[i].pos_x - entities[j].pos_x) - fabs(entities[j].width / 2) - fabs(entities[i].width / 2) < 0) // if collision
					collisionXRight = true;
			}

			if (entities[j].pos_y < entities[i].pos_y) // above
			{
				if (fabs(entities[j].pos_y - entities[i].pos_y) - fabs(entities[j].height / 2) - fabs(entities[i].height / 2) < 0)
					collisionYAbove = true;
			}
			else if (entities[j].pos_y > entities[i].pos_y) // below
			{
				if (fabs(entities[j].pos_y - entities[i].pos_y) - fabs(entities[j].height / 2) - fabs(entities[i].height / 2) < 0)
					collisionYBelow = true;
			}

			if ((collisionXLeft == true || collisionXRight == true) && (collisionYAbove == true || collisionYBelow == true))
			{
				if (collisionXLeft == true)
					entities[i].pos_x -= fabs(entities[j].pos_x - entities[i].pos_x) - fabs(entities[j].width / 2) - fabs(entities[i].width / 2);
				else if (collisionXRight == true)
					entities[i].pos_x += fabs(entities[i].pos_x - entities[j].pos_x) - fabs(entities[j].width / 2) - fabs(entities[i].width / 2);

				if (collisionYAbove == true)
					entities[i].pos_y += fabs(entities[j].pos_y - entities[i].pos_y) - fabs(entities[j].height / 2) - fabs(entities[i].height / 2);
				else if (collisionYBelow == true)
					entities[i].pos_y -= fabs(entities[j].pos_y - entities[i].pos_y) - fabs(entities[j].height / 2) - fabs(entities[i].height / 2);
			}
		}
	}
	*/

}

void Entity::Draw(ShaderProgram& program, float elapsed)
{
	glm::mat4 modelMatrix = glm::mat4(1.0f); // set to identity
	
	if (isStatic == false)
	{
		/*
		if (vel_y != 0)
			accel_y = gravity;
		vel_y += accel_y * elapsed;
		vel_y += gravity * elapsed;
		pos_y += vel_y * elapsed;

		vel_x += accel_x * elapsed;
		pos_x += vel_x * elapsed;
		*/

		if (vel_y != 0)
			vel_y += gravity * elapsed;

		if (vel_y <= 0)
			vel_x = lerp(vel_x, 0.0f, elapsed * friction_x);
		vel_y = lerp(vel_y, 0.0f, elapsed * friction_y);

		vel_x += accel_x * elapsed;
		vel_y += accel_y * elapsed;

		if (vel_x > maxXSpeed)
			vel_x = maxXSpeed;
		else if (vel_x < -maxXSpeed)
			vel_x = -maxXSpeed;

		pos_y += vel_y * elapsed;
		pos_x += vel_x * elapsed;

		collisionHandler();
	}

	modelMatrix = glm::translate(modelMatrix, glm::vec3(pos_x, pos_y, 0.0f));
	program.SetModelMatrix(modelMatrix);

	

	float vertices[] = {
		-width / 2.0f, height / 2.0f,
		width / 2.0f, height / 2.0f,
		width / 2.0f, -height / 2.0f,
		-width / 2.0f, -height / 2.0f
	};

	glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
	glEnableVertexAttribArray(program.positionAttribute);
	glDrawArrays(GL_POLYGON, 0, 4);
	glDisableVertexAttribArray(program.positionAttribute);
}


/*
void spriteStuff()
{
	int index = 20;
	int spriteCountX = 8;
	int spriteCountY = 4;
	float u = (float)(((int)index) % spriteCountX) / (float)spriteCountX;
	float v = (float)(((int)index) / spriteCountX) / (float)spriteCountY;
	float spriteWidth = 1.0 / (float)spriteCountX;
	float spriteHeight = 1.0 / (float)spriteCountY;
	GLfloat spriteUVs[] = { u, v,
						  u, v + spriteHeight,
						  u + spriteWidth, v + spriteHeight,
						  u + spriteWidth, v
	};
}

void DrawSpriteSheetSprite(ShaderProgram& program, int index, int spriteCountX, int spriteCountY)
{
	float u = (float)(((int)index) % spriteCountX) / (float)spriteCountX;
	float v = (float)(((int)index) / spriteCountX) / (float)spriteCountY;
	float spriteWidth = 1.0 / (float)spriteCountX;
	float spriteHeight = 1.0 / (float)spriteCountY;

	float texCoords[] = {
		u, v + spriteHeight,
		u + spriteWidth, v,
		u, v,
		u + spriteWidth, v,
		u, v + spriteHeight,
		u + spriteWidth, v + spriteHeight
	};

	float vertices[] = {
		-0.5f, -0.5f,
		0.5f, 0.5f,
		-0.5f, 0.5f,
		0.5f, 0.5f,
		-0.5f, -0.5f,
		0.5f, -0.5f
	};

	// draw this data
}

*/

const Uint8 *keys = SDL_GetKeyboardState(NULL);

int main(int argc, char *argv[])
{
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("Platformer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 360, SDL_WINDOW_OPENGL);
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
	
	projectionMatrix = glm::ortho(-1.777f * 20, 1.777f * 20, -1.0f * 20, 1.0f * 20, -1.0f, 1.0f);
	glUseProgram(program.programID);

	// end setup

	float lastFrameTicks = 0.0f;

	// instantiate entities and add to vector

	Player player(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 2.0f, 2.0f);
	entities.push_back(player);

	Ground ground(0.0f, -20.0f, 30.0f, 12.0f);
	entities.push_back(ground);

	Enemy enemy(10.0f, 0.0f, 0.0f, 0.0f, 0.0f, 2.0f, 2.0f);
	entities.push_back(enemy);

	SDL_Event event;
	bool done = false;
	while (!done)
	{
		

		// projection matrix and view matrix and whatever glClear() does
		glClear(GL_COLOR_BUFFER_BIT);

		program.SetProjectionMatrix(projectionMatrix);
		

		// time-keeping start

		float ticks = (float)SDL_GetTicks() / 2000.0f;
		float elapsed = ticks - lastFrameTicks;
		lastFrameTicks = ticks;

		player.Draw(program, elapsed);
		ground.Draw(program, elapsed);
		enemy.Draw(program, elapsed);

		enemy.move();
		if (enemy.checkIfDead())
		{
			for (int i = 0; i <= entities.size() - 1; i++)
			{
				if (entities[i].uniqueIdentifier == enemy.uniqueIdentifier)
					entities.erase(entities.begin() + i);
			}
		}
			

		glm::mat4 viewMatrix = glm::mat4(1.0f);
		viewMatrix = glm::translate(viewMatrix, glm::vec3(-player.pos_x, -player.pos_y, 0.0f));
		program.SetViewMatrix(viewMatrix);



		SDL_GL_SwapWindow(displayWindow);
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE)
				done = true;
			else if (event.type == SDL_KEYDOWN)
			{
				/*
				if (event.key.keysym.scancode == SDL_SCANCODE_LEFT)
					player.vel_x -= 5.0f;
				else if (event.key.keysym.scancode == SDL_SCANCODE_RIGHT)
					player.vel_x += 5.0f;
				else if (event.key.keysym.scancode == SDL_SCANCODE_UP)
					player.vel_y = 14.0f;
				*/

				if (keys[SDL_SCANCODE_LEFT])
					player.vel_x -= 20.0f;
				if (keys[SDL_SCANCODE_RIGHT])
					player.vel_x += 20.0f;
				if (keys[SDL_SCANCODE_UP])
					player.vel_y = 20.0f;
			}
		}
		glClear(GL_COLOR_BUFFER_BIT);
	}

	SDL_Quit();
	return 0;
}
