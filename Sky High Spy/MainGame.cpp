#define PLAY_IMPLEMENTATION
#define PLAY_USING_GAMEOBJECT_MANAGER
#include "Play.h"

constexpr int DISPLAY_WIDTH{ 1280 };
constexpr int DISPLAY_HEIGHT{ 720 };
constexpr int DISPLAY_SCALE{ 1 };
float AGENT8_SPEED{ 9.0f };
float METEOR_SPEED{ 7.0f };
float ASTEROID_SPEED{ 4.0f };
int ASTEROID_RADIUS{ 70 };

enum Agent8State
{
	STATE_APPEAR = 0,
	STATE_ATTACHED,
	STATE_FLY,
	STATE_DEAD,
};

enum PlayState
{
	STATE_START,
	STATE_PLAY,
	STATE_PAUSE,
	STATE_WIN,
	STATE_GAMEOVER,
};

struct GameState
{
	int gems{ 3 };
	int score{ 0 };
	float time{ 0 };
	Agent8State agentState = STATE_APPEAR;
};

GameState gameState;


enum GameObjectType
{
	TYPE_NULL = -1,
	TYPE_AGENT8,
	TYPE_GEM,
	TYPE_ASTEROID,
	TYPE_SPECIAL,
	TYPE_METEOR,
	TYPE_DESTROYED,
	TYPE_PARTICLE,
	TYPE_PIECES,
};

void LoopObject(GameObject& obj, int ObjectWidth, int ObjectHeight);
void SetObjectDirection(GameObject& obj, float speed);
void Agent8FlyControls();
void UpdateAgent8();
void UpdateMeteors();
void UpdateDestroyed();
void Draw();
void Agent8AttachedControls();
void UpdateGems();
void UpdateAsteroids();
void CreateParticles();
void UpdateParticles();
void UpdatePieces();

float Randomize(int range, float multiplier = 1.f)
{
	return (float)(rand() % range) * multiplier;
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

// The entry point for a PlayBuffer program
void MainGameEntry( PLAY_IGNORE_COMMAND_LINE )
{
	Play::CreateManager( DISPLAY_WIDTH, DISPLAY_HEIGHT, DISPLAY_SCALE );
	Play::LoadBackground("Data\\Backgrounds\\background.png");
	Play::CreateGameObject(TYPE_AGENT8, { DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 2 }, 40, "agent8_fly");
	
	Play::CentreAllSpriteOrigins();
	Play::SetSpriteOrigin("agent8_left", 64, 110);
	Play::SetSpriteOrigin("agent8_right", 64, 110);
	Play::SetSpriteOrigin("meteor", 60, 40);
	Play::SetSpriteOrigin("asteroid", 75, {ASTEROID_RADIUS});

	/*int id_asteroid = Play::CreateGameObject(TYPE_ASTEROID, { DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 3 }, 40, "asteroid");
	GameObject& obj_asteroid1 = Play::GetGameObject(id_asteroid);
	obj_asteroid1.rotation = Randomize(628, 0.01);
	Play::SetSprite(obj_asteroid1, "asteroid", 0.25f);*/

}

bool MainGameUpdate( float elapsedTime )
{
	gameState.time += elapsedTime;
	Draw();
	return Play::KeyDown( VK_ESCAPE );
}

int MainGameExit( void )
{
	Play::DestroyManager();
	return PLAY_OK;
}



void Draw()
{
	Play::ClearDrawingBuffer(Play::cWhite);
	Play::DrawBackground();

	UpdateAsteroids();
	UpdateMeteors();
	UpdateDestroyed();
	UpdateAgent8();
	UpdateParticles();
	UpdatePieces();
	Play::PresentDrawingBuffer();
}

void Agent8FlyControls()
{
	GameObject& obj_agent8 = Play::GetGameObjectByType(TYPE_AGENT8);

	SetObjectDirection(obj_agent8, AGENT8_SPEED);

	if (gameState.agentState != STATE_DEAD)
	{
			if (Play::KeyDown(VK_LEFT))
			{
				obj_agent8.rotation -= 0.1f;

			}

			else if (Play::KeyDown(VK_RIGHT))
			{
				obj_agent8.rotation += 0.1f;

			}

			else
			{
				obj_agent8.animSpeed = 0;
			}
	}


	if (Play::IsLeavingDisplayArea(obj_agent8))
	{
		LoopObject(obj_agent8, 40, 40);
	}

}

void Agent8AttachedControls()
{
	GameObject& obj_agent8 = Play::GetGameObjectByType(TYPE_AGENT8);

	
		if (Play::KeyDown(VK_LEFT))
		{
			obj_agent8.rotation -= 0.1f;
			
			Play::SetSprite(obj_agent8, "agent8_left", 0.25f);
		}

		else if (Play::KeyDown(VK_RIGHT))
		{
			obj_agent8.rotation += 0.1f;
			
			Play::SetSprite(obj_agent8, "agent8_right", 0.25f);
		}

		if (Play::KeyPressed(VK_UP))
		{
			obj_agent8.pos.x += 110 + ASTEROID_RADIUS * sin(obj_agent8.rotation);  
			obj_agent8.pos.y += 110 + ASTEROID_RADIUS * -cos(obj_agent8.rotation);
			Play::PlayAudio("explode");
			GameObject& obj_specialA = Play::GetGameObjectByType(TYPE_SPECIAL);
			Play::DestroyGameObjectsByType(TYPE_SPECIAL);

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
				case STATE_APPEAR:
					Play::DrawFontText("64px", "LEFT AND RIGHT TO ROTATE, UP TO LAUNCH",
						{ DISPLAY_WIDTH / 2, DISPLAY_HEIGHT - 50 }, Play::CENTRE);
					Play::DrawFontText("64px", "FIND AND COLLECT ALL THE GEMS! PRESS SPACE TO START",
						{ DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 3 }, Play::CENTRE);
					
					if (Play::KeyPressed(VK_SPACE) == true)
					{
						//Play::StartAudioLoop("speeddrive");
						gameState.agentState = STATE_FLY;
					}
					break;

				case STATE_ATTACHED:
					
					Play::SetSprite(obj_agent8, "agent8_right", 0.25f);
					Agent8AttachedControls();
					Play::DrawFontText("105px", "REMAINING GEMS: " + std::to_string(gameState.gems),
						{ DISPLAY_WIDTH / 2, 50 }, Play::CENTRE);
					Play::UpdateGameObject(obj_agent8);

					if (Play::KeyPressed(VK_UP) == true)
					{
						gameState.agentState = STATE_FLY;
						obj_agent8.pos -= obj_agent8.velocity * 40;
					}

					break;

				case STATE_FLY:
					Play::SetSprite(obj_agent8, "agent8_fly", 0.25f);
					Agent8FlyControls();
					CreateParticles();
					Play::UpdateGameObject(obj_agent8);
					UpdateGems();
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
					Play::UpdateGameObject(obj_agent8);

						for (int meteor_id : Play::CollectGameObjectIDsByType(TYPE_METEOR))
							Play::GetGameObject(meteor_id).type = TYPE_DESTROYED;

						for (int id_asteroid : Play::CollectGameObjectIDsByType(TYPE_ASTEROID))
							Play::GetGameObject(id_asteroid).type = TYPE_DESTROYED;

					if( Play::KeyPressed('R') == true)
					{
						gameState.agentState = STATE_APPEAR;
						Play::SetSprite(obj_agent8, "agent8_fly", 0.25f);
						obj_agent8.pos = { DISPLAY_WIDTH/2, DISPLAY_HEIGHT/2 };
						gameState.score = 0;
						gameState.gems = 3;
						gameState.time = 0;

					}

					break;
	}

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
	GameObject& obj_specialA = Play::GetGameObjectByType(TYPE_SPECIAL);
	std::vector<int> vGems = Play::CollectGameObjectIDsByType(TYPE_GEM);

	for (int id_gem : vGems)
	{
		Play::DrawObject(Play::GetGameObject(id_gem));
		GameObject& obj_gem = Play::GetGameObject(id_gem);
		bool hasCollided = false;

		if (Play::IsColliding(obj_gem, obj_agent8) && gameState.gems > 0)
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

	}

	for (int id_gem : vGems)
	{
		Play::UpdateGameObject(Play::GetGameObject(id_gem));
	}
}

void UpdateAsteroids()
{
	if (gameState.time > 5)
	{
		int id_asteroid = Play::CreateGameObject(TYPE_ASTEROID, { Play::RandomRollRange(0, 1280), Play::RandomRollRange(0, 720) }, 40, "asteroid");
		GameObject& obj_asteroid = Play::GetGameObject(id_asteroid);
		obj_asteroid.rotation = Randomize(628, 0.01);
		SetObjectDirection(obj_asteroid, ASTEROID_SPEED);
		Play::SetSprite(obj_asteroid, "asteroid", 0.25f);

		gameState.time = 0;
	}


	GameObject& obj_agent8 = Play::GetGameObjectByType(TYPE_AGENT8);
	std::vector<int> vAsteroids = Play::CollectGameObjectIDsByType(TYPE_ASTEROID);

	for (int id : vAsteroids)
	{
		GameObject& obj_asteroid = Play::GetGameObject(id);
		bool hasCollided = false;

		if (gameState.agentState != STATE_ATTACHED && Play::IsColliding(obj_asteroid, obj_agent8))
		{
			hasCollided = true;
			obj_agent8.velocity = obj_asteroid.velocity;
			obj_asteroid.type = TYPE_SPECIAL;
			gameState.agentState = STATE_ATTACHED;
			obj_agent8.pos = obj_asteroid.pos;
			obj_agent8.rotation += Play::DegToRad(180);
		}

		if (!Play::IsVisible(obj_asteroid))
			Play::DestroyGameObject(id);
	}

	GameObject& obj_specialA = Play::GetGameObjectByType(TYPE_SPECIAL);

	if (obj_specialA.GetId() != (Play::noObject.GetId()))
	{
		vAsteroids.push_back(obj_specialA.GetId());
	}

	std::vector<int> vPieces = Play::CollectGameObjectIDsByType(TYPE_PIECES);
	for (int id_pieces : vPieces)
	{
		GameObject& obj_pieces = Play::GetGameObject(id_pieces);
	}

	std::vector<int> vGems = Play::CollectGameObjectIDsByType(TYPE_GEM);
	for (int id_gem : vGems)
	{
		GameObject& obj_gem = Play::GetGameObject(id_gem);
	}


	if ((gameState.agentState == STATE_ATTACHED) && (Play::KeyPressed(VK_UP)))
	{

		int id_gem = Play::CreateGameObject(TYPE_GEM, { obj_specialA.pos }, 20, "gem");
		GameObject& obj_gem = Play::GetGameObject(id_gem);
		obj_gem.pos = obj_specialA.oldPos;

		for (int n = 0; n < 3; n++)
		{
			int id_pieces = Play::CreateGameObject(TYPE_PIECES, { obj_specialA.pos }, 10, "asteroid_pieces_3");
			GameObject& obj_pieces(Play::GetGameObject(id_pieces));
			obj_pieces.frame = n;
			obj_pieces.rotation = -n * Play::DegToRad(120);
		}

	}
		

	for (int id : vAsteroids)
	{

		LoopObject((Play::GetGameObject(id)), 40, 40);
		Play::DrawObjectRotated(Play::GetGameObject(id));
		Play::UpdateGameObject(Play::GetGameObject(id));

	}
}

void UpdatePieces()
{
	std::vector<int> vPieces { Play::CollectGameObjectIDsByType(TYPE_PIECES) };
	for (int id : vPieces)
	{
		GameObject& obj_pieces (Play::GetGameObject(id));
		obj_pieces.pos.x = obj_pieces.pos.x + sin(obj_pieces.rotation) * 10; //controls movement of pieces
		obj_pieces.pos.y = obj_pieces.pos.y - cos(obj_pieces.rotation) * 10;

		if (!Play::IsVisible(obj_pieces))
			Play::DestroyGameObject(id);
	}

	for (int id : vPieces)
	{
		GameObject& obj_pieces(Play::GetGameObject(id));
		Play::DrawObject(obj_pieces);
	}
}


void UpdateMeteors()
{
	GameObject& obj_agent8 = Play::GetGameObjectByType(TYPE_AGENT8);
	std::vector<int> vMeteors = Play::CollectGameObjectIDsByType(TYPE_METEOR);

	if (gameState.agentState != STATE_APPEAR && gameState.agentState != STATE_DEAD)
	{
		if (Play::RandomRoll(300) == 300)
		{
			int meteor_id = Play::CreateGameObject(TYPE_METEOR, { Play::RandomRollRange(0,1280), Play::RandomRollRange(0,720) }, 45, "meteor"); //look at this 
			GameObject& obj_meteor = Play::GetGameObject(meteor_id);
			obj_meteor.rotation = Randomize(628, 0.01);
			SetObjectDirection(obj_meteor, METEOR_SPEED);
			Play::SetSprite(obj_meteor, "meteor", 0.25f);
			LoopObject(obj_meteor, 40, 80);
		}


		for (int meteor_id : vMeteors)
		{
			GameObject& obj_meteor = Play::GetGameObject(meteor_id);
			bool hasCollided = false;

			if (gameState.agentState != STATE_DEAD && Play::IsColliding(obj_meteor, obj_agent8))
			{
				Play::StopAudioLoop("speeddrive");
				Play::PlayAudio("combust");
				hasCollided = true;
				gameState.agentState = STATE_DEAD;
			}

			Play::DrawObjectRotated(Play::GetGameObject(meteor_id));

			if (!Play::IsVisible(obj_meteor))
			Play::DestroyGameObject(meteor_id);

			Play::UpdateGameObject(obj_meteor);



		}
	}
}

void CreateParticles()
{
	/*GameObject& objAgent8 = Play::GetGameObjectByType(TYPE_AGENT8);

	Play::CreateGameObject(TYPE_PARTICLE, { objAgent8.pos.x + Play::RandomRollRange(-20, 20), objAgent8.pos.y + Play::RandomRollRange(-15, 15) }, 50, "particle");
	Play::CreateGameObject(TYPE_PARTICLE, { objAgent8.pos.x + Play::RandomRollRange(-20, 20), objAgent8.pos.y + Play::RandomRollRange(-15, 15) }, 50, "particle");*/
}

void UpdateParticles()
{
	//std::vector<int> vParticles = Play::CollectGameObjectIDsByType(TYPE_PARTICLE);
	//GameObject& obj_specialA = Play::GetGameObjectByType(TYPE_SPECIAL);

	//if (gameState.agentState == STATE_ATTACHED && (Play::KeyPressed(VK_UP)))
	//{
	//	for (int n = 0; n < 3; n++)
	//	{
	//		int id = Play::CreateGameObject(TYPE_PIECES, { obj_specialA.pos }, 10, "asteroid_pieces_3");
	//		GameObject& obj_pieces(Play::GetGameObject(id));
	//		obj_pieces.frame = n;
	//		obj_pieces.rotation = -n * Play::DegToRad(120);
	//	}
	//}

	//for (int id_particle : vParticles)
	//{
	//	GameObject& obj_particle = Play::GetGameObject(id_particle);
	//	obj_particle.frame++;

	//	if (obj_particle.frame >= 20 )
	//		obj_particle.type = TYPE_DESTROYED;

	//	Play::DrawObjectRotated(obj_particle);
	//	Play::UpdateGameObject(obj_particle);

	//}
}