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


struct Particle
{
	bool Alive;
	Vector3 position;
	Vector3 initialForce;
	Vector3 size;
	Vector3 angles;
	float gravity;
	float accGravity;
	float time;
	float lifetime;
	float forceDecaySpeed;
	Texture tex;
	Color startColor;
	Color endColor;
	Color col;
};

int particleCreateIndex;
struct Particle AllParticles[5000];

Texture whiteTex;

void InitializeParticleSystem()
{
	//WhiteTex
	Image im = GenImageColor(4, 4, WHITE);
	whiteTex = LoadTextureFromImage(im);
}


void CreateNewParticle(struct Particle particleProperties)
{
	particleProperties.accGravity = 0;
	particleProperties.time = 0;
	particleProperties.Alive = true;
	particleProperties.col = particleProperties.startColor;
	AllParticles[particleCreateIndex] = particleProperties;
	particleCreateIndex = (particleCreateIndex + 1)%5000;
}


void CreateGunParticle(Vector3 pos, Vector3 dir, int count)
{
	Vector3 pVector = Vector3Perpendicular(dir);
	Vector3 rndPosOffset;
	Vector3 rndDirOffset;
	Vector3 rndForce;
	Vector3 targetDir;
	for (int i = 0; i < count; i++)
	{
		targetDir = dir;

		struct Particle p;
		p.gravity = 0;

		rndPosOffset = (Vector3){ GetRandomValue(-100,100) * 0.0001f, GetRandomValue(-100, 100) * 0.0001f, GetRandomValue(-100, 100) * 0.0001f };
		pos = Vector3Add(pos, rndPosOffset);


		p.position = pos;
		p.tex = whiteTex;
		p.startColor = (Color){200,200,50,100};
		p.endColor = (Color){ 200,200,50,0 };
		p.size = (Vector3){0.02f,0.02f,0.02f };
		p.size = Vector3Scale(p.size, GetRandomValue(50, 110) * 0.01f);

		rndDirOffset = Vector3Scale(pVector, GetRandomValue(-100,100) * 0.0005f);
		targetDir = Vector3Add(targetDir, rndDirOffset);


		rndForce = Vector3Scale(targetDir, GetRandomValue(100, 200) * 0.001f);
		p.initialForce = rndForce;
		p.forceDecaySpeed = 4.0;

		p.lifetime = (GetRandomValue(60, 140) * 0.01f) * 0.2f;

		CreateNewParticle(p);
	}
}



void CreateBulletWallParticle(Vector3 pos, Vector3 dir, int count)
{
	Vector3 pVector = Vector3Perpendicular(dir);
	Vector3 rndPosOffset;
	Vector3 rndDirOffset;
	Vector3 rndForce;
	Vector3 targetDir;
	for (int i = 0; i < count; i++)
	{
		targetDir = dir;

		struct Particle p;
		//p.gravity = GetRandomValue(70, 110) * 0.0000001f;
		p.gravity = 0;

		rndPosOffset = (Vector3){ GetRandomValue(-100,100) * 0.00002f, GetRandomValue(-100, 100) * 0.00002f, GetRandomValue(-100, 100) * 0.00002f };
		pos = Vector3Add(pos, rndPosOffset);


		p.position = pos;
		p.tex = whiteTex;
		p.startColor = (Color){ 150,200,50,100 };
		p.endColor = (Color){ 200,200,50,0 };
		p.size = (Vector3){ 0.1f,0.1f,0.1f };
		p.size = Vector3Scale(p.size, GetRandomValue(50, 110) * 0.01f);

		rndDirOffset = Vector3Scale(pVector, GetRandomValue(30, 100) * 0.0005f);
		targetDir = Vector3Add(targetDir, rndDirOffset);


		rndForce = Vector3Scale(targetDir, GetRandomValue(0, 100) * 0.002f);
		rndDirOffset = Vector3Add(rndDirOffset, Vector3Scale((Vector3) { 0,-1.0f, 0 }, GetRandomValue(50, 100) * 0.01f));
		p.initialForce = rndForce;
		p.forceDecaySpeed = 1.0f;

		p.lifetime = (GetRandomValue(60, 140) * 0.01f) * 0.2f;

		CreateNewParticle(p);
	}
}




void CreateGunShellParticle(Vector3 pos, Vector3 dir, int count)
{
	Vector3 pVector = Vector3Perpendicular(dir);
	Vector3 rndPosOffset;
	Vector3 rndDirOffset;
	Vector3 rndForce;
	Vector3 targetDir;
	for (int i = 0; i < count; i++)
	{
		targetDir = pVector;

		struct Particle p;
		p.gravity = GetRandomValue(70,110) * 0.00001f;

		/*rndPosOffset = (Vector3){ GetRandomValue(-100,100) * 0.0001f, GetRandomValue(-100, 100) * 0.0001f, GetRandomValue(-100, 100) * 0.0001f };
		pos = Vector3Add(pos, rndPosOffset);*/


		p.position = pos;
		p.tex = whiteTex;
		p.startColor = (Color){ 100,100,50,230 };
		p.endColor = (Color){ 100,100,50,230 };
		p.size = (Vector3){ 0.01f,0.01f,0.02f };

		float angle = (float)(atan2(dir.x, -dir.z)) * RAD2DEG;
		p.angles.y = angle;

		//p.size = Vector3Scale(p.size, GetRandomValue(90, 110) * 0.01f);

		rndDirOffset = Vector3Scale(pVector, GetRandomValue(-100, 100) * 0.00005f);
		rndDirOffset = Vector3Add(rndDirOffset, Vector3Scale((Vector3) {0,1.0f,0}, GetRandomValue(10, 100) * 0.0005f));
		targetDir = Vector3Add(targetDir, rndDirOffset);


		rndForce = Vector3Scale(targetDir, GetRandomValue(90, 110) * 0.001f);
		p.initialForce = rndForce;
		p.forceDecaySpeed = 8.0;

		p.lifetime = (GetRandomValue(100, 110) * 0.01f) * 20.0f;

		CreateNewParticle(p);
	}
}

void CreateGunSmokeParticle(Vector3 pos, Vector3 dir, int count)
{
	Vector3 pVector = Vector3Perpendicular(dir);
	Vector3 rndPosOffset;
	Vector3 rndDirOffset;
	Vector3 rndForce;
	Vector3 targetDir;
	for (int i = 0; i < count; i++)
	{
		targetDir = dir;

		struct Particle p;
		p.gravity = -GetRandomValue(0, 100) * 0.000005f;

		rndPosOffset = (Vector3){ GetRandomValue(-100,100) * 0.0001f, GetRandomValue(-100, 100) * 0.0001f, GetRandomValue(-100, 100) * 0.0001f };
		pos = Vector3Add(pos, rndPosOffset);


		p.position = pos;
		p.tex = whiteTex;
		p.startColor = (Color){ 110,110,110,100 };
		p.endColor = (Color){ 0,0,0,0 };
		p.size = (Vector3){ 0.02f,0.02f,0.02f };
		p.size = Vector3Scale(p.size, GetRandomValue(50, 110) * 0.01f);

		rndDirOffset = Vector3Scale(pVector, GetRandomValue(-100, 100) * 0.00005f);
		targetDir = Vector3Add(targetDir, rndDirOffset);


		rndForce = Vector3Scale(targetDir, GetRandomValue(10, 100) * 0.001f);
		p.initialForce = rndForce;
		p.forceDecaySpeed = 1.0;

		p.lifetime = (GetRandomValue(60, 140) * 0.01f) * 0.8f;

		CreateNewParticle(p);
	}
}


void CreateBloodParticle(Vector3 pos, Vector3 dir, int count)
{
	Vector3 pVector = Vector3Perpendicular(dir);
	Vector3 rndPosOffset;
	Vector3 rndDirOffset;
	Vector3 rndForce;
	Vector3 targetDir;
	for (int i = 0; i < count; i++)
	{
		targetDir = dir;

		struct Particle p;
		p.lifetime = 3;
		p.gravity = GetRandomValue(25, 100) * 0.00005f;

		rndPosOffset = (Vector3){ GetRandomValue(-100,100) * 0.0005f, GetRandomValue(-100, 100) * 0.0002f, GetRandomValue(-100, 100) * 0.0005f };
		pos = Vector3Add(pos, rndPosOffset);


		p.position = pos;
		p.tex = whiteTex;
		Color bloodColor;
		LerpColor((Color) { 200, 50, 20, 200 }, (Color) { 100, 25, 10, 200 }, GetRandomValue(0, 100) * 0.01f, &bloodColor);
		bloodColor.a = GetRandomValue(100, 200);

		p.angles.y = GetRandomValue(0, 360);
		p.startColor = bloodColor;
		p.endColor = (Color){ 0,0,0,0 };
		p.size = (Vector3){ 0.08f,0.08f,0.08f };
		p.size = Vector3Scale(p.size, GetRandomValue(50, 110) * 0.01f);

		rndDirOffset = Vector3Scale(pVector, GetRandomValue(-100, 100) * 0.003f);
		targetDir = Vector3Add(targetDir, rndDirOffset);


		rndForce = Vector3Scale(targetDir, GetRandomValue(0, 100) * 0.001f);
		rndForce = Vector3Add(rndForce, Vector3Scale((Vector3) {0,1,0},GetRandomValue(0, 100) * 0.0002f));
		p.initialForce = rndForce;
		p.forceDecaySpeed = 3.0f;

		p.lifetime = (GetRandomValue(60, 140) * 0.01f) * 2.0f;

		CreateNewParticle(p);
	}
}


void CreatePlayerHurtBloodParticle(Vector3 pos, Vector3 dir, int count)
{
	Vector3 pVector = Vector3Perpendicular(dir);
	Vector3 rndPosOffset;
	Vector3 rndDirOffset;
	Vector3 rndForce;
	Vector3 targetDir;
	for (int i = 0; i < count; i++)
	{
		targetDir = dir;

		struct Particle p;
		p.lifetime = 3;
		p.gravity = GetRandomValue(25, 100) * 0.00005f;

		rndPosOffset = (Vector3){ GetRandomValue(-100,100) * 0.002f, GetRandomValue(-100, 100) * 0.001f, GetRandomValue(-100, 100) * 0.002f };
		pos = Vector3Add(pos, rndPosOffset);


		p.position = pos;
		p.tex = whiteTex;
		Color bloodColor;
		LerpColor((Color) { 200, 50, 20, 200 }, (Color) { 100, 25, 10, 200 }, GetRandomValue(0, 100) * 0.01f, & bloodColor);
		bloodColor.a = GetRandomValue(100, 200);

		p.angles.y = GetRandomValue(0, 360);
		p.startColor = bloodColor;
		p.endColor = (Color){ 0,0,0,0 };
		p.size = (Vector3){ 0.15f,0.15f,0.15f };
		p.size = Vector3Scale(p.size, GetRandomValue(80, 110) * 0.01f);

		rndDirOffset = Vector3Scale(pVector, GetRandomValue(-100, 100) * 0.003f);
		targetDir = Vector3Add(targetDir, rndDirOffset);


		rndForce = Vector3Scale(targetDir, GetRandomValue(0, 100) * 0.001f);
		rndForce = Vector3Add(rndForce, Vector3Scale((Vector3) { 0, 1, 0 }, GetRandomValue(0, 100) * 0.0002f));
		p.initialForce = rndForce;
		p.forceDecaySpeed = 3.0f;

		p.lifetime = (GetRandomValue(60, 140) * 0.01f) * 3.0f;

		CreateNewParticle(p);
	}
}


void CreateZombieDieParticle(Vector3 pos, Vector3 dir,Vector3 zombiePos)
{
	Vector3 pVector = Vector3Perpendicular(dir);
	Vector3 rndDirOffset;
	Vector3 rndForce;
	Vector3 targetDir;
	//Big particles
	for (int i = 0; i < 20; i++)
	{
		targetDir = dir;

		struct Particle p;
		p.lifetime = 3;
		p.gravity = GetRandomValue(0, 100) * 0.0001f;

		Vector3 rndPosOffset = (Vector3){ GetRandomValue(-100,100) * 0.001f, GetRandomValue(-100, 100) * 0.001f, GetRandomValue(-100, 100) * 0.001f };

		p.position = Vector3Add(zombiePos, rndPosOffset);;
		p.tex = whiteTex;
		Color bloodColor;
		LerpColor((Color) { 150, 50, 20, 200 }, (Color) { 150, 25, 10, 200 }, GetRandomValue(0, 100) * 0.01f, &bloodColor);
		bloodColor.a = GetRandomValue(50, 100);

		p.angles.y = GetRandomValue(0, 360);
		p.startColor = bloodColor;
		p.endColor = (Color){ 0,0,0,0 };
		p.size = (Vector3){ 0.3f,0.3f,0.3f };
		p.size = Vector3Scale(p.size, GetRandomValue(50, 150) * 0.01f);

		rndDirOffset = Vector3Scale(pVector, GetRandomValue(0, 100) * 0.003);
		targetDir = Vector3Add(targetDir, rndDirOffset);


		rndForce = Vector3Scale(targetDir, GetRandomValue(0, 100) * 0.001f);
		rndForce = Vector3Add(rndForce, Vector3Scale((Vector3) { 0, 1, 0 }, GetRandomValue(0, 100) * 0.0002f));
		p.initialForce = rndForce;
		p.forceDecaySpeed = 2.00f;

		p.lifetime = (GetRandomValue(60, 140) * 0.01f) * 3.0f;

		CreateNewParticle(p);
	}


	//Small particles
	for (int i = 0; i < 40; i++)
	{
		targetDir = dir;

		struct Particle p;
		p.lifetime = 3;
		p.gravity = GetRandomValue(0, 100) * 0.00001f;

		Vector3 rndPosOffset = (Vector3){ GetRandomValue(-100,100) * 0.0008f, GetRandomValue(-100, 100) * 0.0005f, GetRandomValue(-100, 100) * 0.0008f };
		//pos = Vector3Add(pos, rndPosOffset);
		//pos = Vector3Add(pos, (Vector3) { 0, -0.2f, 0 });


		p.position = Vector3Add(pos, rndPosOffset);;
		p.tex = whiteTex;
		Color bloodColor;
		LerpColor((Color) { 150, 50, 20, 200 }, (Color) { 150, 25, 10, 200 }, GetRandomValue(0, 100) * 0.01f, &bloodColor);
		bloodColor.a = GetRandomValue(50, 100);

		p.angles.y = GetRandomValue(0, 360);

		p.startColor = (Color){ 150, 25, 10, 200 };
		p.endColor = (Color){ 0,0,0,0 };
		p.size = (Vector3){ 0.15f,0.15f,0.15f };
		p.size = Vector3Scale(p.size, GetRandomValue(60, 110) * 0.01f);

		rndDirOffset = Vector3Scale(pVector, GetRandomValue(0, 100) * 0.0003f);
		targetDir = Vector3Add(targetDir, rndDirOffset);


		rndForce = Vector3Scale(targetDir, GetRandomValue(0, 100) * 0.001f);
		rndForce = Vector3Add(rndForce, Vector3Scale((Vector3) { 0, 1, 0 }, GetRandomValue(0, 100) * 0.00005f));
		p.initialForce = rndForce;
		p.forceDecaySpeed = 2.00f;

		p.lifetime = (GetRandomValue(60, 140) * 0.01f) * 2.0f;

		CreateNewParticle(p);
	}


	//Big green particles
	for (int i = 0; i < 3; i++)
	{
		targetDir = dir;

		struct Particle p;

		p.lifetime = (GetRandomValue(60, 140) * 0.01f) * 1.0f;
		p.gravity = GetRandomValue(80, 120) * 0.0001f;

		//rndPosOffset = (Vector3){ GetRandomValue(-100,100) * 0.001f, GetRandomValue(-100, 100) * 0.001f, GetRandomValue(-100, 100) * 0.001f };
		//pos = Vector3Add(pos, rndPosOffset);

		p.position = zombiePos;
		p.tex = whiteTex;
		Color bloodColor;
		LerpColor((Color) { 20, 25, 20, 255 }, (Color) { 30, 50, 30, 255 }, GetRandomValue(0, 100) * 0.01f, &bloodColor);
		//bloodColor.a = GetRandomValue(255, 255);

		p.angles.y = GetRandomValue(0, 360);
		p.startColor = bloodColor;
		//p.endColor = p.startColor;
		p.endColor = (Color){ 20,25,20,255 };
		p.size = (Vector3){ GetRandomValue(70, 120) * 0.005f,GetRandomValue(70, 120) * 0.008f,GetRandomValue(70, 120) * 0.005f };
		//p.size = Vector3Scale(p.size, GetRandomValue(50, 150) * 0.01f);

		rndDirOffset = Vector3Scale(pVector, GetRandomValue(0, 100) * 0.01f);
		targetDir = Vector3Add(targetDir, rndDirOffset);


		rndForce = Vector3Scale(targetDir, GetRandomValue(0, 100) * 0.002f);
		rndForce = Vector3Add(rndForce, Vector3Scale((Vector3) { 0, 1, 0 }, GetRandomValue(80, 120) * 0.001f));
		p.initialForce = rndForce;
		p.forceDecaySpeed = 4.0f;


		CreateNewParticle(p);
	}
}


void DrawParticle(int i)
{
	struct Particle p = AllParticles[i];
	DrawCubeRotatedAngle(p.tex, p.position, p.size.x, p.size.y, p.size.z, p.angles.x, p.angles.y, p.angles.z, p.col);
}


void ProcessAndDrawParticles()
{
	float lifetimeRatio;
	for (int i = 0; i < 5000; i++)
	{
		if (AllParticles[i].Alive)
		{
			AllParticles[i].time += GetFrameTime();

			if (AllParticles[i].position.y <= 0.05f)
				AllParticles[i].position.y = 0.05f;

			Vector3 particlePos = AllParticles[i].position;


			if (AllParticles[i].position.y > 0.06f)
			{
				particlePos.y -= AllParticles[i].accGravity;
				AllParticles[i].accGravity += AllParticles[i].gravity;
			}
			else
			{
				AllParticles[i].initialForce.y = 0;
				AllParticles[i].gravity = 0;
			}
			


			particlePos = Vector3Add(particlePos,AllParticles[i].initialForce);

			if (AllParticles[i].lifetime > 0)
			{
				lifetimeRatio = Clamp(AllParticles[i].time / AllParticles[i].lifetime,0.0f,1.0f);

				if(lifetimeRatio >= 1)
				{
					AllParticles[i].Alive = false;
					AllParticles[i] = (const struct Particle){ 0 };
				}
			}
			else
				lifetimeRatio = 0;

			AllParticles[i].initialForce = Vector3Lerp(AllParticles[i].initialForce, (Vector3) { 0, 0, 0 }, GetFrameTime() * AllParticles[i].forceDecaySpeed);
			AllParticles[i].position = particlePos;

			LerpColorWithAlpha(AllParticles[i].startColor, AllParticles[i].endColor, lifetimeRatio, &AllParticles[i].col);


			DrawParticle(i);
		}
	}
}


