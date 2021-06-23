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



struct Collectable
{
	bool Active;
	struct Vector3 position;
	int type;
};



struct Collectable AllCollectables[100];
int CollectableSpawnIndex;

Texture CollectableTextures[3];
Texture WhiteTex;

void InitializeCollectables()
{
	WhiteTex = GenerateBlankTexture((Color) { 150, 150, 150, 255 });
	CollectableTextures[0] = GenerateAmmoBoxTexture((Color) {173,173,173,255});
	CollectableTextures[1] = GenerateHealthBoxTexture();
	CollectableTextures[2] = GenerateScrapBoxTexture();

	//TODO make textures
}

void SpawnCollectable(Vector2 position,int type)
{
	AllCollectables[CollectableSpawnIndex].Active = true;
	AllCollectables[CollectableSpawnIndex].position = (Vector3){ position.x, 0.3f ,position.y };
	AllCollectables[CollectableSpawnIndex].type = type;

	CollectableSpawnIndex = (CollectableSpawnIndex + 1) % 100;
}


void ApplyCollectable(int i)
{

	if (AllCollectables[i].type == 0)
	{
		int bulletCount = GetRandomValue(30, 100);
		GetPlayerRef()->AllWeapons[1].bullet += bulletCount;
		PrintToScreen(TextFormat("Picked Up %02i Machine Gun Ammo", bulletCount));
	}

	if (AllCollectables[i].type == 1)
	{
		int bulletCount = GetRandomValue(3, 9);
		GetPlayerRef()->AllWeapons[2].bullet += bulletCount;
		PrintToScreen(TextFormat("Picked Up %01i Rifle Ammo", bulletCount));
	}

	if (AllCollectables[i].type == 2)
	{
		int bulletCount = GetRandomValue(8, 20);
		GetPlayerRef()->AllWeapons[3].bullet += bulletCount;
		PrintToScreen(TextFormat("Picked Up %02i Shotgun Ammo", bulletCount));
	}


	if (AllCollectables[i].type == 3)
	{
		int health = GetRandomValue(30, 40);
		GetPlayerRef()->Health += health;

		if (GetPlayerRef()->Health > GetPlayerRef()->MaxHealth)
			GetPlayerRef()->Health = GetPlayerRef()->MaxHealth;

		PrintToScreen(TextFormat("Gained %02i Health", health));
	}


	if (AllCollectables[i].type == 4)
	{
		int scrapCount = GetRandomValue(5, 25);
		GetPlayerRef()->Scrap += scrapCount;
		PrintToScreen(TextFormat("Picked Up %01i Scrap", scrapCount));
	}

}

float collectablesCommonAngle;
void DrawCollectable(int i,float dist)
{
	Vector3 collectablePosition;
	collectablePosition = AllCollectables[i].position;
	collectablePosition = Vector3Add(collectablePosition, (Vector3) {0,0.1 * sin(GetTime()),0});


	if (AllCollectables[i].type < 3)
	{
		Color textureTint = WHITE;
		Color frameTint = GRAY;

		switch (AllCollectables[i].type)
		{
			case 0:
				textureTint = (Color){ 209, 204, 109, 255 };
				frameTint = (Color){ 133, 130, 89, 255 };
				break;

			case 1:
				textureTint = (Color){ 179, 135, 105, 255 };
				frameTint = (Color){ 89, 67, 53, 255 };
				break;

			case 2:
				textureTint = (Color){ 209, 56, 56, 255 };
				frameTint = (Color){ 92, 39, 39, 255 };
				break;

			default:
				break;
		}

		DrawCubeRotatedAngle(CollectableTextures[0], collectablePosition, 0.5f, 0.2f, 0.5f, 0, collectablesCommonAngle, 0, CalculateFogColorWithAlpha(textureTint, dist));
		DrawCubeRotatedAngle(WhiteTex, Vector3Add(collectablePosition, (Vector3) { 0, -0.02f, 0 }), 0.6f, 0.19f, 0.6f, 0, collectablesCommonAngle, 0, CalculateFogColorWithAlpha(frameTint, dist));
	}

	if (AllCollectables[i].type == 3)
	{
		DrawCubeRotatedAngle(CollectableTextures[1], collectablePosition, 0.5f, 0.2f, 0.5f, 0, collectablesCommonAngle, 0, CalculateFogColorWithAlpha((Color) { 135, 168, 119, 255 },dist));
		DrawCubeRotatedAngle(WhiteTex, Vector3Add(collectablePosition, (Vector3) { 0, -0.02f, 0 }), 0.6f, 0.19f, 0.6f, 0, collectablesCommonAngle, 0, CalculateFogColorWithAlpha((Color) {59,102,39,255},dist));
	}
	

	if (AllCollectables[i].type == 4)
	{
		DrawCubeRotatedAngle(CollectableTextures[2], collectablePosition, 0.5f, 0.2f, 0.5f, 0, collectablesCommonAngle, 0, CalculateFogColorWithAlpha((Color) { 173, 173, 173, 255 },dist));
		DrawCubeRotatedAngle(WhiteTex, Vector3Add(collectablePosition, (Vector3) { 0, -0.02f, 0 }), 0.6f, 0.19f, 0.6f, 0, collectablesCommonAngle, 0, CalculateFogColorWithAlpha((Color) { 80, 80, 80, 255 },dist));
	}
	
	//DrawCubeRotatedOneSidedTexture(CollectableTextures[0], collectablePosition, 0.5f, 0.2f, 0.5f, 0, (Vector3) { 0, 1, 0 }, WHITE);
}

void ProcessCollectables()
{
	Vector3 playerPos = GetPlayerRef()->position;
	
	collectablesCommonAngle += GetFrameTime() * 60;
	float dist;
	for (int i = 0; i < 100; i++)
	{
		if (AllCollectables[i].Active)
		{
			dist = Vector3Distance(playerPos, AllCollectables[i].position);
			if (dist < 2)
			{
				if (AllCollectables[i].type != 3)
				{
					AllCollectables[i].Active = false;
					ApplyCollectable(i);
				}
				else if (GetPlayerRef()->Health < GetPlayerRef()->MaxHealth)
				{
					AllCollectables[i].Active = false;
					ApplyCollectable(i);
				}
			}

			DrawCollectable(i, dist);
		}
	}
}