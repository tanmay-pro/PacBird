#include <GLFW/glfw3.h>
#include "game.h"
#include "resource_manager.h"
#include "sprite_renderer.h"
#include "game_object.h"
#include "text_renderer.h"
#include "ball_object_collisions.h"
#include <cstdlib>
#include <unistd.h>


// Game-related State data
SpriteRenderer  *Renderer;
//GameObject      *Player;
TextRenderer  *Text;
BallObject     *Ball;
Collision CheckCollision(BallObject &one, GameObject &two); // AABB - Circle collision

Direction VectorDirection(glm::vec2 target)
{
	glm::vec2 compass[] = {
			glm::vec2(0.0f, 1.0f),	// up
			glm::vec2(1.0f, 0.0f),	// right
			glm::vec2(0.0f, -1.0f),	// down
			glm::vec2(-1.0f, 0.0f)	// left
	};
	float max = 0.0f;
	unsigned int best_match = -1;
	for (unsigned int i = 0; i < 4; i++)
	{
		float dot_product = glm::dot(glm::normalize(target), compass[i]);
		if (dot_product > max)
		{
			max = dot_product;
			best_match = i;
		}
	}
	return (Direction)best_match;
}


Game::Game(unsigned int width, unsigned int height): State(GAME_MENU), Keys(), Width(width), Height(height), change(false), start_time(time(0))
{

}

Game::~Game()
{
	delete Renderer;
//	delete Player;
	delete Ball;
}


void increment(int curr[])
{	srand(time(0));
	int r = (rand() % (2)) + 1;
	if(r == 1)
	{
		if(curr[0] != 9)
		{
			curr[0]++;
		}
	}
//	else if(r == 2)
//	{
//		if(curr[1] != 0)
//		{
//			curr[1]--;
//		}
//	}
	else if(r == 2)
	{
		if(curr[1] != 16)
		{
			curr[1]++;
		}
	}
}

void check_valid(vector<vector<int>> &mapping) {
	int end[2] = {9, 16};
	int curr[2] = {1, 1};
	while(curr[0] != end[0] || curr[1] != end[1])
	{
		increment(curr);
		mapping[curr[0]][curr[1]] = 0;
	}
}


void make_map(int level) {
	srand(time(0));
	int num_monsters[] = {6, 9, 12};
	vector<vector<int>> mapping(11, vector<int>(18,-1));
	for(int i = 0; i < 11; i ++)
	{
		for (int j=0; j < 18; j++)
		{
			if(i ==0 || j == 0 || i == 10 || j == 17)
			{
				mapping[i][j] = 1;
			}
		}
	}
	mapping[0][0] = 0;
	mapping[0][1] = 0;
	mapping[1][0] = 0;
	mapping[1][1] = 0;
	//mapping[9][16] = 0;
	check_valid(mapping);
	mapping[9][16] = 3;
	int num_coins = 10;
	int walls[] = {15, 20, 25};
	while(walls[level])
	{
		int x = rand() % 9;
		int y = rand() % 16;
		if(mapping[x + 1][y + 1] == -1)
		{
			mapping[x + 1][y + 1]= 1;
			walls[level]--;
		}
	}
	while(num_coins)
	{
		int x = rand() % 9;
		int y = rand() % 16;
		if(mapping[x + 1][y + 1] == -1)
		{
			mapping[x + 1][y + 1] = 2;
			num_coins--;
		}
	}
	while(num_monsters[level])
	{
		int x = rand() % 9;
		int y = rand() % 16;
		if(mapping[x + 1][y + 1] == -1)
		{
			mapping[x + 1][y + 1]= 4;
			num_monsters[level]--;
		}
	}
	for(int i=0; i < 11; i++)
	{
		for(int j=0; j < 18; j++)
		{
			if(mapping[i][j] == -1)
			{
				mapping[i][j] = 0;
			}
		}
	}
	
	FILE *fptr;
	
	if (level == 0)
		fptr = fopen("/home/tanmaypro/Desktop/Sem4/cg/assi-2/src/levels/one.lvl", "w");
	else if (level == 1)
		fptr = fopen("/home/tanmaypro/Desktop/Sem4/cg/assi-2/src/levels/two.lvl", "w");
	else if (level == 2)
		fptr = fopen("/home/tanmaypro/Desktop/Sem4/cg/assi-2/src/levels/three.lvl", "w");
	
	for (auto i = 0; i < 11; i++) {
		for (auto j = 0; j < 18; j++) {
			std::fprintf(fptr, "%d ", mapping[i][j]);
		}
		std::fprintf(fptr, "\n");
	}
	fclose(fptr);
}

void Game::Init()
{
	srand(time(0));
	// load shaders
	ResourceManager::LoadShader("/home/tanmaypro/Desktop/Sem4/cg/assi-2/src/shaders/sprite.vs", "/home/tanmaypro/Desktop/Sem4/cg/assi-2/src/shaders/sprite.frag", nullptr, "sprite");
	// configure shaders
	glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(this->Width),
	                                  static_cast<float>(this->Height), 0.0f, -1.0f, 1.0f);
	ResourceManager::GetShader("sprite").Use().SetInteger("image", 0);
	ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);
	// set render-specific controls
	Shader myShader;
	myShader = ResourceManager::GetShader("sprite");
	Renderer = new SpriteRenderer(myShader);
	
	ResourceManager::LoadTexture("/home/tanmaypro/Desktop/Sem4/cg/assi-2/src/textures/background.jpg", false, "background");
	ResourceManager::LoadTexture("/home/tanmaypro/Desktop/Sem4/cg/assi-2/src/textures/egg.png", true, "block");
	ResourceManager::LoadTexture("/home/tanmaypro/Desktop/Sem4/cg/assi-2/src/textures/block_solid.png", false, "block_solid");
	ResourceManager::LoadTexture("/home/tanmaypro/Desktop/Sem4/cg/assi-2/src/textures/character.png", true, "char");
	ResourceManager::LoadTexture("/home/tanmaypro/Desktop/Sem4/cg/assi-2/src/textures/door.png", true, "door");
	ResourceManager::LoadTexture("/home/tanmaypro/Desktop/Sem4/cg/assi-2/src/textures/monster.png", true, "monster");
	
	// load levels
	make_map(0);
	make_map(1);
	make_map(2);
	GameLevel one; one.Load("/home/tanmaypro/Desktop/Sem4/cg/assi-2/src/levels/one.lvl", this->Width, this->Height);
	GameLevel two; two.Load("/home/tanmaypro/Desktop/Sem4/cg/assi-2/src/levels/two.lvl", this->Width, this->Height);
	GameLevel three; three.Load("/home/tanmaypro/Desktop/Sem4/cg/assi-2/src/levels/three.lvl", this->Width, this->Height);
	this->Levels.push_back(one);
	this->Levels.push_back(two);
	this->Levels.push_back(three);
	this->Level = 0;
	this->light = true;

//	glm::vec2 playerPos = glm::vec2(this->Width / 2.0f - PLAYER_SIZE.x / 2.0f, this->Height - PLAYER_SIZE.y);
//	Player = new GameObject(playerPos, PLAYER_SIZE, ResourceManager::GetTexture("char"));
	
	glm::vec2 ballPos = glm::vec2(0.0f, 0.0f);
	Ball = new BallObject(ballPos, BALL_RADIUS, INITIAL_BALL_VELOCITY,
	                      ResourceManager::GetTexture("char"));
	
	Text = new TextRenderer(this->Width, this->Height);
	Text->Load("/home/tanmaypro/Desktop/Sem4/cg/assi-2/src/fonts/OCRAEXT.TTF", 24);
}


bool Check_Collision(GameObject& one, GameObject& two) // AABB - AABB collision
{
	// collision x-axis?
	std::tuple<int, int, int, int> ans(0, 0, 0, 0);
	bool collisionX = one.Position.x + one.Size.x >= two.Position.x &&
	                  two.Position.x + two.Size.x >= one.Position.x;
	bool collisionY = one.Position.y + one.Size.y >= two.Position.y &&
	                  two.Position.y + two.Size.y >= one.Position.y;
	
	return collisionX && collisionY;
}


bool Game::enemyCollision(GameObject& enemy)
{
	for (GameObject& box : this->Levels[this->Level].Bricks)
	{
		if(!box.IsMonster && box.IsSolid && !box.IsDoor)
		{
			if (!box.Destroyed)
			{
				if (Check_Collision(enemy, box))
				{
					return true;
				}
			}
		}
	}
	return false;
}

void Game::Update(float dt)
{
	Ball->Move(dt, this->Width);
	this->DoCollisions();
	
	if (this->State == GAME_ACTIVE && this->change)
	{
		this->change = false;
		this->NextLevel();
	}
	float velocity = PLAYER_VELOCITY * dt /3;
	if(this->State == GAME_ACTIVE)
	{
		for(GameObject & enemy:this->Levels[this->Level].Bricks)
		{
			if(enemy.IsMonster)
			{
				if(!enemy.Right)
				{
					if (enemy.Position.y >= 0.0f)
					{
						enemy.Position.y -= velocity;
						if (enemyCollision(enemy))
						{
							enemy.Position.y += velocity;
							enemy.Right = true;
						}
					}
					else
					{
						enemy.Right = true;
					}
				}
				if(enemy.Right)
				{
					if(enemy.Position.y <= (float)this->Width - enemy.Size.y)
					{
						enemy.Position.y += velocity;
						if (enemyCollision(enemy))
						{
							enemy.Position.y -= velocity;
							enemy.Right = false;
						}
					}
					else
					{
						enemy.Right = false;
					}
				}
			}
		}
	}
	if(this->State == GAME_ACTIVE)
	{
		glm::vec3 lightPos = glm::vec3(Ball->Position.x, Ball->Position.y, 0.0f);
		ResourceManager::GetShader("sprite").Use().SetVector3f("lightPos", lightPos);
		ResourceManager::GetShader("sprite").Use().SetInteger("checkLight", this->light);
	}
	
}

void Game::ProcessInput(float dt)
{
	if (this->State == GAME_MENU)
	{
		if (this->Keys[GLFW_KEY_ENTER])
			this->State = GAME_ACTIVE;
	}
	else if (this->State == GAME_ACTIVE)
	{
		float velocity = PLAYER_VELOCITY * dt;
		if (this->Keys[GLFW_KEY_A])
		{
			if (Ball->Position.x >= 0.0f)
				Ball->Position.x -= velocity;
		}
		if (this->Keys[GLFW_KEY_D])
		{
			if (Ball->Position.x <= this->Width - Ball->Size.x)
				Ball->Position.x += velocity;
		}
		if (this->Keys[GLFW_KEY_S])
		{
			if (Ball->Position.y <= this->Height - Ball->Size.y)
				Ball->Position.y += velocity;
		}
		if (this->Keys[GLFW_KEY_W])
		{
			if (Ball->Position.y >= 0.0f)
				Ball->Position.y -= velocity;
		}
	}
}

void Game::Render()
{
	Texture2D myTexture;
	Renderer->DrawSprite(myTexture, glm::vec2(200, 200), glm::vec2(300, 400), 45.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	if(this->State == GAME_ACTIVE || this->State == GAME_MENU)
	{
		Texture2D myTexture2;
		myTexture2 = ResourceManager::GetTexture("background");
		Renderer->DrawSprite(myTexture2, glm::vec2(0.0f, 0.0f), glm::vec2(this->Width, this->Height), 0.0f);
		this->Levels[this->Level].Draw(*Renderer);
		Ball->Draw(*Renderer);
	}
	if(this->State == GAME_MENU)
	{
		Text->RenderText("Press ENTER to start", Width/2 - 100.0f, Height / 2, 1.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	}
	if(this->State == GAME_ACTIVE)
	{
		std::stringstream ss;
		ss << Ball->score;
		time_t curr = time(0) - this->start_time;
		std::stringstream ss_time;
		ss_time << curr;
		std::stringstream ss_level;
		ss_level << this->Level + 1;
		
		Text->RenderText("Points:" + ss.str(), 5.0f, 5.0f, 1.0f);
		Text->RenderText("Time Elapsed:" + ss_time.str(), 250.0f, 5.0f, 1.0f);
		Text->RenderText("Level:" + ss_level.str(), 700.0f, 5.0f, 1.0f);
	}
	if(this->State == GAME_LOST)
	{
		std::stringstream ss;
		ss << Ball->score;
		std::stringstream ss_level;
		ss_level << this->Level + 1;
		
		Text->RenderText("FINAL SCORE:" + ss.str(), 5.0f, 5.0f, 1.0f);
		Text->RenderText("LEVEL:" + ss_level.str(), 700.0f, 5.0f, 1.0f);
		Text->RenderText("GAME OVER", Width/2 - 100.0f, Height / 2, 1.0f, glm::vec3(1.0f, 0.0f, 0.0f) );
	}
	if(this->State == GAME_WIN)
	{
		std::stringstream ss;
		ss << Ball->score;
		
		Text->RenderText("FINAL SCORE:" + ss.str(), Width/2 - 100.0f, Height / 2 - 100.0f, 1.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		Text->RenderText("FINISHED", Width/2 - 100.0f, Height / 2, 1.0f, glm::vec3(0.0f, 1.0f, 0.0f) );
		
	}
}


Collision CheckCollision(BallObject &one, GameObject &two) // AABB - Circle collision
{
	// get center point circle first
	glm::vec2 center(one.Position + one.Radius);
	// calculate AABB info (center, half-extents)
	glm::vec2 aabb_half_extents(two.Size.x / 2.0f, two.Size.y / 2.0f);
	glm::vec2 aabb_center(
			two.Position.x + aabb_half_extents.x,
			two.Position.y + aabb_half_extents.y
	);
	// get difference vector between both centers
	glm::vec2 difference = center - aabb_center;
	glm::vec2 clamped = glm::clamp(difference, -aabb_half_extents, aabb_half_extents);
	// add clamped value to AABB_center and we get the value of box closest to circle
	glm::vec2 closest = aabb_center + clamped;
	// retrieve vector between center circle and closest point AABB and check if length <= radius
	difference = closest - center;
//	return glm::length(difference) < one.Radius;
	if (glm::length(difference) <= one.Radius)
		return std::make_tuple(true, VectorDirection(difference), difference);
	else
		return std::make_tuple(false, UP, glm::vec2(0.0f, 0.0f));
}

void Game::NextLevel()
{
	if (this->Level == 0)
		this->Levels[0].Load("/home/tanmaypro/Desktop/Sem4/cg/assi-2/src/levels/one.lvl", this->Width, this->Height);
	else if (this->Level == 1)
		this->Levels[1].Load("/home/tanmaypro/Desktop/Sem4/cg/assi-2/src/levels/two.lvl", this->Width, this->Height);
	else if (this->Level == 2)
		this->Levels[2].Load("/home/tanmaypro/Desktop/Sem4/cg/assi-2/src/levels/three.lvl", this->Width, this->Height);
	
	Ball->Reset(glm::vec2(0.0f, 0.0f), INITIAL_BALL_VELOCITY);
}


void Game::DoCollisions()
{
	for (GameObject &box : this->Levels[this->Level].Bricks)
	{
		if (!box.Destroyed)
		{
			Collision collision = CheckCollision(*Ball, box);
			if (std::get<0>(collision)) // if collision is true
			{
				// destroy block if not solid
				if (!box.IsSolid && !this->light && !box.IsDoor)
				{
					box.Destroyed = true;
					Ball->score = Ball->score + 2;
				}
				if (!box.IsSolid && this->light && !box.IsDoor)
				{
					box.Destroyed = true;
					Ball->score++;
				}
				if(box.IsDoor)
				{
					if(this->Level == 2)
					{
						this->State = GAME_WIN;
					}
					else
					{
						box.Destroyed = true;
						this->Level ++;
						this ->change = true;
					}
				}
				if(box.IsMonster)
				{
					this->State = GAME_LOST;
				}
				// collision resolution
				Direction dir = std::get<1>(collision);
				glm::vec2 diff_vector = std::get<2>(collision);
				if (dir == LEFT || dir == RIGHT) // horizontal collision
				{
					Ball->Velocity.x = -Ball->Velocity.x; // reverse horizontal velocity
					// relocate
					float penetration = Ball->Radius - std::abs(diff_vector.x);
					if (dir == LEFT)
						Ball->Position.x += penetration; // move ball to right
					else
						Ball->Position.x -= penetration; // move ball to left;
				}
				else // vertical collision
				{
					Ball->Velocity.y = -Ball->Velocity.y; // reverse vertical velocity
					// relocate
					float penetration = Ball->Radius - std::abs(diff_vector.y);
					if (dir == UP)
						Ball->Position.y -= penetration; // move ball back up
					else
						Ball->Position.y += penetration; // move ball back down
				}
			}
		}
	}
}