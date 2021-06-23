


#ifndef raymath
#define raymath
#include <raymath.h>
#endif




#ifndef raylib
#define raylib
#include "raylib.h"
#endif

#ifndef gameh
#define gameh
#include "GameHeader.h"
#endif


struct Zombie zombies[10000];
int zombieCount;
bool AIMovePerm = true;
Texture BasicZombieTexture[10];
Texture BasicZombieHeadTexture[10];
Texture BasicZombieArmTexture;

void InitializeZombies()
{
	zombieCount = 10000;
	//zombies = malloc(zombieCount * sizeof(struct Zombie));

	for (int i = 0; i < 10; i++)
	{
		BasicZombieTexture[i] = GenerateZombieTexture();
	}
	

	for (int i = 0; i < 10; i++)
	{
		BasicZombieHeadTexture[i] = GenerateZombieHeadTexture();
	}
	

	BasicZombieArmTexture = GenerateZombieBlankTexture();


	for (int i = 0; i < zombieCount; i++)
	{
		zombies[i].spawned = false;
	}
}

int zombieCreateIndex = 0;
void CreateZombie(Vector2 pos)
{
	if (zombieCreateIndex < zombieCount)
	{
		zombies[zombieCreateIndex].spawned = true;
		zombies[zombieCreateIndex].Health = 1;
		zombies[zombieCreateIndex].spawned = true;
		zombies[zombieCreateIndex].position.x = pos.x * 2;
		zombies[zombieCreateIndex].position.y = 1.0f;
		zombies[zombieCreateIndex].position.z = pos.y * -2;
		zombies[zombieCreateIndex].textureType = GetRandomValue(0, 9);
		zombies[zombieCreateIndex].angle = GetRandomValue(0, 360);
		zombies[zombieCreateIndex].MoveSpeed = GetRandomValue(100, 110) * 0.00075f;
		zombies[zombieCreateIndex].MoveAnimTimer = GetRandomValue(0, 10);
		zombies[zombieCreateIndex].strength = 5;
		zombies[zombieCreateIndex].type = 0;

		if (HelicopterStarted)
			zombies[zombieCreateIndex].alerted = true;


		//Special Zombies
		float rnd = GetRandomValue(0,100);

		if (rnd < 10)
		{
			rnd = GetRandomValue(0, 1);
			if (rnd == 0)
			{
				zombies[zombieCreateIndex].type = 1;
				zombies[zombieCreateIndex].Health = 8;
				zombies[zombieCreateIndex].MoveSpeed *= 0.75;
				zombies[zombieCreateIndex].strength = 25;
			}

			if (rnd == 1)
			{
				zombies[zombieCreateIndex].type = 2;
				zombies[zombieCreateIndex].Health = 1;
				zombies[zombieCreateIndex].MoveSpeed *= 2;
				zombies[zombieCreateIndex].strength = 10;
			}
		}

		zombieCreateIndex++;
	}
}


bool CheckCollision(int i,Vector3 newMoveDir,float exaggerateAmount)
{
	//Collision
	Vector3 ExagMoveVector = Vector3Scale(newMoveDir, exaggerateAmount);
	Vector3 exagNextPosition = Vector3Add(ExagMoveVector, zombies[i].position);
	struct MapTile closestFutureTile = GetClosestGrid(exagNextPosition.x, -exagNextPosition.z);
	struct MapTile playerTile = GetClosestGrid(GetPlayer().position.x, -GetPlayer().position.z);

	bool FutureTileValid = true;

	if (closestFutureTile.genType == 2)
		FutureTileValid = false;


	if (exagNextPosition.z > 0 || exagNextPosition.z < -199 || exagNextPosition.x < 0 || exagNextPosition.x > 199)
		FutureTileValid = false;

	return FutureTileValid;

}


void PushZombies()
{
	for (int i = 0; i < zombieCreateIndex; i++)
	{
		struct MapTile closestTile = GetClosestGrid(zombies[i].position.x, -zombies[i].position.z);


		if (closestTile.entityCount > 1)
		{
			Vector3 averagePoint = Vector3Scale(closestTile.middlePoint, (1.0f / (float)(closestTile.entityCount)));
			Vector3 dir = Vector3Subtract(zombies[i].position, averagePoint);
			dir = Vector3Scale(dir, 0.04f);
			dir.y = 0;

			if (CheckCollision(i, dir, 1.2f) && AIMovePerm)
			{
				zombies[i].position = Vector3Add(zombies[i].position, dir);
			}
		}
	}
}


void CheckAlerted(int i)
{
	int alertDist = GetPlayerRef()->AlertDistance;

	float timeSinceLastShoot = (GetTime() - GetPlayerRef()->equippedWeapon->lastShotTime);

	if (timeSinceLastShoot < 0.1f)
	{
		alertDist += GetPlayerRef()->equippedWeapon->Noise;
	}

	bool InDistance = zombies[i].distToPlayer < alertDist;

	if (GetClosestGrid(zombies[i].position.x, zombies[i].position.z).genType == 3 && GetClosestGrid(GetPlayerRef()->position.x, GetPlayerRef()->position.z).genType != 3)
		InDistance = false;


	if (InDistance)
	{
		zombies[i].alerted = true;
	}
}


void ExecuteZombieAI(int i)
{
	if (!GameStarted || InEndScreen)
		return;

	if (zombies[i].spawned)
	{
		zombies[i].distToPlayer = Vector3Distance(zombies[i].position, GetPlayer().position);


		if (zombies[i].alerted)
		{
			struct MapTile* closestTile = GetClosestGridRef(zombies[i].position.x, -zombies[i].position.z);

			//Rotate to player
			Vector3 dir = Vector3Normalize(Vector3Subtract(GetPlayer().position, zombies[i].position));
			dir.y = 0;
			zombies[i].angle = (float)(atan2(-dir.z, dir.x)) * RAD2DEG;

			Vector3 nextPosition;
			Vector3 MoveVector = { 0,0,0 };

			if (zombies[i].distToPlayer < 1)
			{
				//Hit player
				double time = GetTime();

				if (time > zombies[i].hitCooldown)
				{
					Vector3 pushDir = dir;
					pushDir = Vector3Scale(pushDir, 0.5f + (GetRandomValue(-5, 5) * 0.01f));

					float upForce = 0.3f;
					if (closestTile->genType == 3 || closestTile->genType == 4)
						upForce = 0.03f;

					pushDir = Vector3Add(pushDir, (Vector3) { 0, upForce, 0 });

					if (AIMovePerm)
					{
						HurtPlayer(zombies[i].strength);
						ApplyForceToPlayer(pushDir);
					}

					zombies[i].hitCooldown = time + 3;
				}
			}
			else
			{

				//Set Movement
				if (AIMovePerm)
				{
					MoveVector = Vector3Scale(dir, zombies[i].MoveSpeed);
					double deltaTime = GetFrameTime();
					zombies[i].MoveAnimTimer += deltaTime * 4.0 * (zombies[i].MoveSpeed * 10);
				}
			}
			

			//Push save
			closestTile->entityCount++;
			closestTile->middlePoint = Vector3Add(closestTile->middlePoint,zombies[i].position);
			closestTile->middlePoint.y = 1.0f;




			//Collision
			bool CollisionResult = CheckCollision(i, MoveVector, 1.0f);
			if (CollisionResult == true)
			{
				//Move zombie
				nextPosition = Vector3Add(MoveVector, zombies[i].position);
				zombies[i].position = nextPosition;
			}

			closestTile->zombieInThisTileRef = &zombies[i];

		}
		else
		{
			CheckAlerted(i);
		}
	}
	else
		zombies[i].distToPlayer = 9999;
	
}




float ZombieWaveTimer;
void ExecuteAI()
{
	ResetEntityData();

	for (int i = 0; i < zombieCreateIndex; i++)
	{
		ExecuteZombieAI(i);
	}

	if (HelicopterStarted && !InEndScreen)
	{
		ZombieWaveTimer -= GetFrameTime();
		if (ZombieWaveTimer <= 0)
		{
			ZombieWaveTimer = 30;
			Vector3 spawnPos;
			for (int i = 0; i < 1000; i++)
			{
				CreateZombie(GetWaveSpawnTile());
			}
		}
	}

	PushZombies();
}


void LerpColor2(Color a, Color b, float t, Color* result)
{
	result->r = a.r + (b.r - a.r) * t;
	result->g = a.g + (b.g - a.g) * t;
	result->b = a.b + (b.b - a.b) * t;
	result->a = a.a + (b.a - a.a) * t;
}

void LerpAlpha(Color a, float targetAlpha, float t, Color* result)
{
	result->r = a.r;
	result->g = a.g;
	result->b = a.b;
	result->a = a.a + (targetAlpha - a.a) * t;
}

bool HurtZombie(struct Zombie* z, int dmg)
{
	z->Health -= dmg;
	if (z->Health <= 0)
	{
		z->position = (Vector3) { 0,0,0 };
		z->spawned = false;
	}

	return (z->Health <= 0);
}

struct AnimationData
{
	Vector3 angles;
	Vector3 legoffset;
	Vector3 armsangles;
	Vector3 armsoffset;
};

struct AnimationData CalculateAnimation(int i, float lookAtAngle)
{
	struct AnimationData result;

	float ratio = fabs(sin(zombies[i].MoveAnimTimer));

	//Leg Angle
	Vector3 angles;
	angles.x = Lerp(-30,30,ratio);
	angles.y = -lookAtAngle;
	angles.z = 0;
	result.angles = angles;

	//Leg Position
	Vector3 position;
	float offsetForward = Lerp(-0.15f, 0.15f, 1.0f - ratio);
	double angle = lookAtAngle;
	Vector3 forwardDir = (Vector3){ sin((angle + 90) * DEG2RAD), 0, cos((angle + 90) * DEG2RAD) };
	Vector3 pos = Vector3Scale(Vector3Normalize(forwardDir), offsetForward);

	position.x = pos.x;
	position.y = 0.0f;
	position.z = pos.z;
	result.legoffset = position;


	//Arm Position
	offsetForward = Lerp(0.0f, -0.1f, 1.0f - ratio);
	pos = Vector3Scale((Vector3){0,1.0f,0}, offsetForward);
	Vector3 WalkPosition;
	WalkPosition.x = 0;
	WalkPosition.y = pos.y;
	WalkPosition.z = 0;

	float time = GetTime();
	if (time < zombies[i].hitCooldown)
	{
		float ratio = 1.0f - Clamp(((zombies[i].hitCooldown - time) - 2.0f) / 1.0f,0,1);

		//Arm Rotation
		angles.x = Lerp(-80, 0, ratio);
		angles.y = -lookAtAngle;
		angles.z = 0;
		result.armsangles = angles;

		//Arm Position Offset
		position;
		offsetForward = Lerp(0.0f, -0.2f, 1.0f - ratio);
		angle = lookAtAngle;
		forwardDir = (Vector3){ sin((angle + 90) * DEG2RAD), 0, cos((angle + 90) * DEG2RAD) };
		pos = Vector3Scale(Vector3Normalize(forwardDir), offsetForward);
		float offsetUp = Lerp(0.0f, 0.3f, 1.0f - ratio);

		position.x = pos.x;
		position.y = offsetUp;
		position.z = pos.z;
		WalkPosition = Vector3Add(WalkPosition, position);
	}
	else
	{
		angles.x = 0;
		angles.y = -lookAtAngle;
		angles.z = 0;
		result.armsangles = angles;
	}


	result.armsoffset = WalkPosition;


	return result;
}


void DrawZombieBody(int i)
{
	double angle = zombies[i].angle;
	Vector3 rightDir = (Vector3){ sin((angle+90) * DEG2RAD), 0, cos((angle + 90) * DEG2RAD) };
	Vector3 forwardDir = (Vector3){ sin((angle) * DEG2RAD), 0, cos((angle) * DEG2RAD) };
	Vector3 zombiePosition = zombies[i].position;
	Color zColor1 = (Color){ 200,200,200,255 };
	Color zColor2 = (Color){ 100,100,100,255 };


	Color zombieColor;
	LerpColor(zColor1, zColor2, (i%100) * 0.01f, &zombieColor);

	//zombieColor = WHITE;

	//Color foggedColor = CalculateFogColor(zombieColor, distToPlayer);
	//foggedColor.a = 1.0f - Clamp(distToPlayer / 40.0f,0,1);
	Color foggedColor = WHITE;

	if (zombies[i].distToPlayer > 20)
		LerpColor2(zombieColor, BLANK,Clamp((zombies[i].distToPlayer-20.0f)/20.0f,0,1),&foggedColor);
	
	foggedColor.r = zombieColor.r;
	foggedColor.g = zombieColor.g;
	foggedColor.b = zombieColor.b;

	if (zombies[i].type == 1)
	{
		foggedColor = (Color){ 9,138,4,foggedColor.a };
	}

	if (zombies[i].type == 2)
	{
		foggedColor = (Color){ 219,42,42,foggedColor.a };
	}
	
	//Calculate animation
	struct AnimationData legData = CalculateAnimation(i, angle);

	//Body
	Vector3 bodyPos = zombiePosition;
	if (zombies[i].alerted)
	{
		Vector3 shake = Vector3Scale(forwardDir, GetRandomValue(-100, 100) * 0.0001f);
		bodyPos = Vector3Add(bodyPos, shake);
	}


	if (zombies[i].type == 1)
		DrawCubeRotated(BasicZombieTexture[zombies[i].textureType], Vector3Add(bodyPos, (Vector3) { 0, -0.0f, 0 }), 0.6f, 0.9f, 1.0f, angle, (Vector3) { 0, 1.0f, 0 }, foggedColor);
	else
		DrawCubeRotated(BasicZombieTexture[zombies[i].textureType], Vector3Add(bodyPos, (Vector3) { 0, -0.2f, 0 }), 0.4f, 0.7f, 0.8f, angle, (Vector3) { 0, 1.0f, 0 }, foggedColor);
	

	Vector3 rightLeg = Vector3Scale(Vector3Normalize(forwardDir), 0.2f);
	rightLeg = Vector3Add(rightLeg, (Vector3) { 0, -0.7f, 0.0f });
	rightLeg = Vector3Add(zombies[i].position, rightLeg);

	rightLeg = Vector3Add(rightLeg, legData.legoffset);
	DrawCubeRotatedAngle(BasicZombieTexture[zombies[(i + 2) % 10].textureType], rightLeg, 0.3f, 0.55f, 0.3f, legData.angles.x, legData.angles.y, legData.angles.z, foggedColor);

	Vector3 leftLeg = Vector3Scale(Vector3Normalize(forwardDir), -0.2f);
	leftLeg = Vector3Add(leftLeg, (Vector3) { 0, -0.7f, 0.0f });
	leftLeg = Vector3Add(zombies[i].position, leftLeg);
	legData = CalculateAnimation(i, angle+180);
	leftLeg = Vector3Add(leftLeg, legData.legoffset);
	DrawCubeRotatedAngle(BasicZombieTexture[zombies[(i+1) % 10].textureType], leftLeg, 0.3f, 0.55f, 0.3f, legData.angles.x, legData.angles.y, legData.angles.z, foggedColor);
	
	
	legData = CalculateAnimation(i, angle);

	//Head

	Vector3 headPos = Vector3Scale(Vector3Normalize(rightDir), 0.1f);
	headPos = Vector3Add(zombies[i].position, headPos);
	if (zombies[i].alerted)
	{
		Vector3 shake = Vector3Scale(forwardDir, GetRandomValue(-100, 100) * 0.00015f);
		headPos = Vector3Add(headPos, shake);
	}

	if (zombies[i].type == 1)
		DrawCubeRotatedOneSidedTexture(BasicZombieHeadTexture[i % 10], Vector3Add(headPos, (Vector3) { 0, 0.6f, 0 }), 0.4f, 0.4f, 0.4f, angle, (Vector3) { 0, 1.0f, 0 }, foggedColor);
	else if(zombies[i].type != 2)
		DrawCubeRotatedOneSidedTexture(BasicZombieHeadTexture[i%10], Vector3Add(headPos, (Vector3) { 0, 0.3f, 0 }) , 0.4f, 0.4f, 0.4f, angle, (Vector3) { 0, 1.0f, 0 }, foggedColor);

	

	//Right arm
	Vector3 rightHandPos = Vector3Scale(Vector3Normalize(forwardDir), 0.5f);
	Vector3 rightHandOffset = Vector3Scale(Vector3Normalize(rightDir), 0.2f);
	if (zombies[i].type == 1)
	{
		rightHandOffset = Vector3Scale(Vector3Normalize(forwardDir), 0.15f);
		rightHandOffset = Vector3Add(rightHandOffset, (Vector3) { 0, 0.15f, 0 });
	}

	rightHandPos = Vector3Add(rightHandPos, rightHandOffset);
	rightHandPos = Vector3Add(rightHandPos, (Vector3) {0,0.05f,0});
	rightHandPos = Vector3Add(zombies[i].position, rightHandPos);
	rightHandPos = Vector3Add(rightHandPos, legData.armsoffset);




	DrawCubeRotatedAngle(BasicZombieArmTexture, rightHandPos, 0.7f, 0.3f, 0.3f, legData.armsangles.x, legData.armsangles.y, legData.armsangles.z, foggedColor);


	//Left arm
	Vector3 leftHandPos = Vector3Scale(Vector3Normalize(forwardDir), -0.5f);
	Vector3 leftHandOffset = Vector3Scale(Vector3Normalize(rightDir), 0.2f);
	if (zombies[i].type == 1)
	{
		leftHandOffset = Vector3Scale(Vector3Normalize(forwardDir), -0.15f);
		leftHandOffset = Vector3Add(leftHandOffset, (Vector3) { 0, 0.15f, 0 });
	}


	leftHandPos = Vector3Add(leftHandPos, leftHandOffset);
	leftHandPos = Vector3Add(leftHandPos, (Vector3) { 0, 0.05f, 0 });
	leftHandPos = Vector3Add(zombies[i].position, leftHandPos);
	leftHandPos = Vector3Add(leftHandPos, legData.armsoffset);
	DrawCubeRotatedAngle(BasicZombieArmTexture, leftHandPos, 0.7f, 0.3f, 0.3f,legData.armsangles.x, legData.armsangles.y, legData.armsangles.z, foggedColor);
}

void DrawZombies()
{
	for (int i = 0; i < zombieCreateIndex; i++)
	{
		if(zombies[i].spawned)
			DrawZombieBody(i);
	}
}


int GetZombieCount()
{
	return zombieCreateIndex;
}



void DestroyZombies()
{
	for (int i = 0; i < zombieCount; i++)
	{
		zombies[i].spawned = false;
	}
}