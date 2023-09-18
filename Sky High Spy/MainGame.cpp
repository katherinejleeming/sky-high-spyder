#define PLAY_IMPLEMENTATION
#define PLAY_USING_GAMEOBJECT_MANAGER
#include "Play.h"

constexpr int DISPLAY_WIDTH{ 1280 };
constexpr int DISPLAY_HEIGHT{ 720 };
constexpr int DISPLAY_SCALE{ 1 };
float AGENT8_SPEED{ 9.0f };
float METEOR_SPEED{ 7.0f };
float ASTEROID_SPEED{ 4.0f };


enum Agent8State
{
	STATE_START = 0,
	STATE_ATTACHED,
	STATE_FLY,
	STATE_PAUSE,
	STATE_WIN,
	STATE_DEAD,
};


struct GameState
{
	int gems{ 3 };
	int score{ 0 };
	float time{ 0 };
	Agent8State agentState = STATE_START;
};

GameState gameState;

enum GameObjectType
{
	TYPE_NULL = -1,
	TYPE_AGENT8,
	TYPE_GEM,
	TYPE_ASTEROID,
	TYPE_METEOR,
	TYPE_DESTROYED,
};

void LoopObject(GameObject& obj, int ObjectWidth, int ObjectHeight);
void SetObjectDirection(GameObject& obj, float speed);
void Agent8FlyControls();
void UpdateAgent8();
void UpdateMeteors();
void UpdateDestroyed();
void Draw();
//void Agent8AttachedControls();
void UpdateGems();
//void UpdateAsteroids();


// The entry point for a PlayBuffer program
void MainGameEntry( PLAY_IGNORE_COMMAND_LINE )
{
	Play::CreateManager( DISPLAY_WIDTH, DISPLAY_HEIGHT, DISPLAY_SCALE );
	Play::LoadBackground("Data\\Backgrounds\\background.png");


	/*std::vector<int> myArray;
	myArray.push_back(10);
	myArray.push_back(6);
	for (int value : myArray)
	{
		int fred = value;

	}*/

	Play::CreateGameObject(TYPE_AGENT8, { DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 2 }, 40, "agent8_fly");
	//Play::CreateGameObject(TYPE_ASTEROID, { 600, 100 }, 40, "asteroid");
	GameObject& obj_agent8 = Play::GetGameObjectByType(TYPE_AGENT8);
	Play::CentreAllSpriteOrigins();
	Play::SetSpriteOrigin("agent8_left", 60, 100);
	Play::SetSpriteOrigin("agent8_right", 60, 100);
	Play::SetSpriteOrigin("meteor", 60, 40);
}

// Called by PlayBuffer every frame (60 times a second!)
bool MainGameUpdate( float elapsedTime )
{
	Draw();

	return Play::KeyDown( VK_ESCAPE );
}

// Gets called once when the player quits the game 
int MainGameExit( void )
{
	Play::DestroyManager();
	return PLAY_OK;
}

void LoopObject(GameObject& obj, int ObjectWidth, int ObjectHeight)
{
	if (obj.pos.x + ObjectWidth < 0)
	{
		obj.pos.x = DISPLAY_WIDTH + ObjectWidth;
	}
	if (obj.pos.x - ObjectWidth > DISPLAY_WIDTH)
	{
		obj.pos.x = 0 - ObjectWidth;
	}
	if (obj.pos.y + ObjectHeight < 0)
	{
		obj.pos.y = DISPLAY_HEIGHT + ObjectHeight;
	}
	if (obj.pos.y - ObjectHeight > DISPLAY_HEIGHT)
	{
		obj.pos.y = 0 - ObjectHeight;
	}
}

void SetObjectDirection(GameObject& obj, float speed)
{
	float x = sin(obj.rotation);
	float y = cos(obj.rotation);

	obj.velocity.x = x * speed;
	obj.velocity.y = -y * speed;
}

void Draw()
{
	Play::ClearDrawingBuffer(Play::cWhite);
	Play::DrawBackground();
	Agent8FlyControls();
	UpdateAgent8();
	UpdateMeteors();
	UpdateDestroyed();
	//Agent8AttachedControls();
	UpdateGems();
	//UpdateAsteroids();
	Play::PresentDrawingBuffer();
}

void Agent8FlyControls()
{
	GameObject& obj_agent8 = Play::GetGameObjectByType(TYPE_AGENT8);

	if (gameState.agentState != STATE_DEAD || STATE_START)
	{
	
		SetObjectDirection(obj_agent8, AGENT8_SPEED);

		if (Play::KeyDown(VK_LEFT))
		{
			obj_agent8.rotation -= 0.1f;
		}

		else if (Play::KeyDown(VK_RIGHT))
		{
			obj_agent8.rotation += 0.1f;
		}
	}

	if (Play::IsLeavingDisplayArea(obj_agent8))
	{
		LoopObject(obj_agent8, 40, 40);
	}

}

void UpdateAgent8()
{
	GameObject& obj_agent8 = Play::GetGameObjectByType(TYPE_AGENT8);

	switch (gameState.agentState)
		{
				case STATE_START:
					Play::DrawFontText("64px", "LEFT AND RIGHT TO ROTATE, UP TO LAUNCH",
						{ DISPLAY_WIDTH / 2, DISPLAY_HEIGHT - 50 }, Play::CENTRE);
					Play::DrawFontText("64px", "FIND AND COLLECT ALL THE GEMS! PRESS SPACE TO START",
						{ DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 2 }, Play::CENTRE);
					
					if (Play::KeyPressed(VK_SPACE) == true)
					{
						//Play::StartAudioLoop("speeddrive");
						gameState.agentState = STATE_FLY;
					}
					break;

				case STATE_ATTACHED:
					//Agent8AttachedControls();
					Play::DrawFontText("105px", "REMAINING GEMS: " + std::to_string(gameState.gems),
						{ DISPLAY_WIDTH / 2, 50 }, Play::CENTRE);

					if ((Play::KeyPressed(VK_UP) == true))
					{
						gameState.agentState = STATE_FLY;
					}
					break;

				case STATE_FLY:
					Agent8FlyControls();
					Play::DrawFontText("105px", "REMAINING GEMS: " + std::to_string(gameState.gems),
						{ DISPLAY_WIDTH / 2, 50 }, Play::CENTRE);
					break;

				/*case STATE_WIN:
					Play::DrawFontText("64px", "YOU WON! PLAY AGAIN? PRESS P TO PLAY AGAIN", { DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 2 }, Play::CENTRE);
					break;*/
	
				case STATE_DEAD:
					
					Play::SetSprite(obj_agent8, "agent8_dead", 0.25f);
					Play::DrawFontText("105px", "GAME OVER :(", { DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 2 }, Play::CENTRE);
					Play::DrawFontText("64px", "PLAY AGAIN? PRESS R TO RESTART",{ DISPLAY_WIDTH / 2, DISPLAY_HEIGHT - 50 }, Play::CENTRE);

					if (!Play::IsVisible(obj_agent8))
					{
						Play::DestroyGameObjectsByType(TYPE_AGENT8);
					}

					if( Play::KeyPressed('R') == true)
					{
						gameState.agentState = STATE_START;
						Play::SetSprite(obj_agent8, "agent8_fly", 0.25f);
						obj_agent8.pos = { DISPLAY_WIDTH/2, DISPLAY_HEIGHT/2 };
						gameState.score = 0;
						gameState.gems = 3;

						for (int meteor_id : Play::CollectGameObjectIDsByType(TYPE_METEOR))
							Play::GetGameObject(meteor_id).type = TYPE_DESTROYED;
					}
					break;
	}

	Play::UpdateGameObject(obj_agent8);
	Play::DrawObjectRotated(obj_agent8);

}


void UpdateDestroyed()
{
	std::vector<int> vDead = Play::CollectGameObjectIDsByType(TYPE_DESTROYED);

	for (int id_dead : vDead)
	{
		GameObject& obj_dead = Play::GetGameObject(id_dead);
		obj_dead.animSpeed = 0.2f;
		Play::UpdateGameObject(obj_dead);

		if (obj_dead.frame % 2)
			Play::DrawObjectRotated(obj_dead, (10 - obj_dead.frame) / 10.0f);

		if (!Play::IsVisible(obj_dead) || obj_dead.frame >= 10)
			Play::DestroyGameObject(id_dead);
	}
}

void UpdateGems()
{
	GameObject& obj_agent8 = Play::GetGameObjectByType(TYPE_AGENT8);
	std::vector<int> vGems = Play::CollectGameObjectIDsByType(TYPE_GEM);

	if (gameState.agentState != STATE_DEAD && gameState.gems > 0)
	{
		if (Play::RandomRoll(500) == 500)
		{
			int id_gem = Play::CreateGameObject(TYPE_GEM, { Play::RandomRoll(DISPLAY_WIDTH) , Play::RandomRoll(DISPLAY_HEIGHT) }, 20, "gem");
			GameObject& obj_gem = Play::GetGameObject(id_gem);
		}

		for (int id_gem : vGems)
		{
			Play::DrawObject(Play::GetGameObject(id_gem));
			GameObject& obj_gem = Play::GetGameObject(id_gem);
			bool hasCollided = false;

			if (Play::IsColliding(obj_gem, obj_agent8))
			{
				Play::PlayAudio("reward");
				Play::SetSprite(obj_gem, "blue_ring", 0.25f);
				hasCollided = true;
				obj_gem.type = TYPE_DESTROYED;
				gameState.gems -= 1;

				if (gameState.gems >= 0)
				{
					//gameState.agentState = STATE_WIN;
				}
			}

			Play::UpdateGameObject(obj_gem);

		}
	}
}


//void UpdateAsteroids()
//{
//	std::vector<int> vAsteroids = Play::CollectGameObjectIDsByType(TYPE_ASTEROID);
//	for (int id_asteroid : vAsteroids)
//	{
//		GameObject& obj_asteroid = Play::GetGameObject(id_asteroid);
//		FloatDirectionObject(obj_asteroid, AGENT8_SPEED);
//
//
//		Play::DrawObjectRotated(obj_asteroid);
//	}
//
//	
//	
//}


void UpdateMeteors()
{
	GameObject& obj_agent8 = Play::GetGameObjectByType(TYPE_AGENT8);
	std::vector<int> vMeteors = Play::CollectGameObjectIDsByType(TYPE_METEOR);

	if (gameState.agentState != STATE_DEAD)
	{
		if (Play::RandomRoll(200) == 200)
		{
			int meteor_id = Play::CreateGameObject(TYPE_METEOR, { 0, Play::RandomRoll(DISPLAY_HEIGHT) }, 45, "meteor");
			GameObject& obj_meteor = Play::GetGameObject(meteor_id);
			obj_meteor.rotation = { PLAY_PI / 3 };
			SetObjectDirection(obj_meteor, METEOR_SPEED);
			Play::SetSprite(obj_meteor, "meteor", 0.25f);
			LoopObject(obj_meteor, 40, 80);
		}


		for (int meteor_id : vMeteors)
		{
			Play::DrawObjectRotated(Play::GetGameObject(meteor_id));
			GameObject& obj_meteor = Play::GetGameObject(meteor_id);
			bool hasCollided = false;

			if (gameState.agentState != STATE_DEAD && Play::IsColliding(obj_meteor, obj_agent8))
			{
				Play::StopAudioLoop("speeddrive");
				Play::PlayAudio("explode");
				hasCollided = true;
				gameState.agentState = STATE_DEAD;
			}


			if (!Play::IsVisible(obj_meteor))
			Play::DestroyGameObject(meteor_id);
			

			Play::UpdateGameObject(obj_meteor);



		}
	}
}