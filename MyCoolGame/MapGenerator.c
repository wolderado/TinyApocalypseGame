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






struct MapTile tiles[101][101];
int mapWidth;
int mapHeight;
int buildingHeight;
Model CeilingModel;
int viewDistance = 40;
Texture miniMapTexture;
Color roadColor = { 20,20,20,255 };
int exitZ;
Texture WhiteTex;
Vector3 ExitPosition;
Texture ExitSignTex;
int CollectibleLimit;
Texture HelipadOuterTex;
Texture HelipadInnerTex;

void LerpColor(Color a, Color b, float t, Color* result)
{
	result->r = a.r + (b.r - a.r) * t;
	result->g = a.g + (b.g - a.g) * t;
	result->b = a.b + (b.b - a.b) * t;
	result->a = 255;
}


void LerpColorWithAlpha(Color a, Color b, float t, Color* result)
{
	result->r = a.r + (b.r - a.r) * t;
	result->g = a.g + (b.g - a.g) * t;
	result->b = a.b + (b.b - a.b) * t;
	result->a = a.a + (b.a - a.a) * t;;
}

int CheckTile(int x, int y)
{
	if (x < mapWidth - 1 && y < mapHeight - 1 && x > 0 && y > 0)
		return tiles[x][y].genType;
	else
		return -1;
}


int NextRoadEndX = -1;
int GetRoadStartPosition()
{
	return NextRoadEndX;
}


int GenerateRoad()
{
	//if (NextRoadEndX == -1)
	NextRoadEndX = GetRandomValue(10, mapWidth - 3);

	int c_x = NextRoadEndX;
	int c_y = 0;
	int limit = mapHeight;
	if (CityProgressed == 5)
		limit = mapHeight / 2;
	while(c_y < limit)
	{
		tiles[c_x][c_y].genType = -2;


		if(c_x > 0)
			tiles[c_x-1][c_y].genType = -2;

		if (c_x < mapWidth-1)
			tiles[c_x + 1][c_y].genType = -2;

		int rnd = GetRandomValue(0, 3);
		if (rnd != 0)
		{
			c_y++;
		}
		else
		{
			rnd = GetRandomValue(0, 1);
			if (rnd == 0)
				c_x++;
			else
				c_x--;

			c_x = Clamp(c_x, 0, mapWidth - 1);
		}

		c_y = Clamp(c_y, 0, mapHeight);
	}


	exitZ = c_x;
	ExitPosition = (Vector3){ exitZ * 2, 1, -mapHeight * 2 };
}

void HollowBuilding(bool randomChance)
{
	//Hollowout
	for (int x = 0; x < mapWidth; x++)
	{
		for (int y = 0; y < mapHeight; y++)
		{
			if (tiles[x][y].genType == 3)
			{
				bool diagonal = CheckTile(x + 1, y + 1) <= 0 || CheckTile(x + 1, y - 1) <= 0 || CheckTile(x - 1, y + 1) <= 0 || CheckTile(x - 1, y - 1) <= 0;
				bool chance = randomChance == true && GetRandomValue(0, 25) == 0;
				if (CheckTile(x + 1, y) == 4 || CheckTile(x - 1, y) == 4 || CheckTile(x, y + 1) == 4 || CheckTile(x, y - 1) == 4)
					chance = false;

				if (diagonal || CheckTile(x + 1, y) <= 0 || CheckTile(x - 1, y) <= 0 || CheckTile(x, y + 1) <= 0 || CheckTile(x, y - 1) <= 0 || chance)
					tiles[x][y].genType = 2;
			}
		}
	}
}


void CreateRandomWalls()
{
	for (int x = 0; x < mapWidth - 1; x++)
	{
		for (int y = 0; y < mapHeight - 1; y++)
		{
			if (tiles[x][y].genType == 3)
			{
				bool chance = GetRandomValue(0, 25) == 0;
				if (CheckTile(x + 1, y) == 4 || CheckTile(x - 1, y) == 4 || CheckTile(x, y + 1) == 4 || CheckTile(x, y - 1) == 4)
					chance = false;

				if (chance)
					tiles[x][y].genType = 2;
			}
		}
	}
}


void CreateBuildingDoor()
{
	for (int x = 0; x < mapWidth - 1; x++)
	{
		for (int y = 0; y < mapHeight - 1; y++)
		{
			if (tiles[x][y].genType == 2)
			{
				int ChanceMax = 20;
				bool PathCheck = false;
				int rightTileType = CheckTile(x + 1, y);
				int leftTileType = CheckTile(x - 1, y);

				if ((rightTileType <= 0 && leftTileType == 3) || (leftTileType <= 0 || rightTileType == 3))
				{
					PathCheck = true;

					if (CheckTile(x + 1, y) == -2 || CheckTile(x - 1, y) == -2)
						ChanceMax = 1;
				}

				int upTileType = CheckTile(x, y + 1);
				int downTileType = CheckTile(x, y - 1);

				if ((upTileType <= 0 && downTileType == 3) || (upTileType <= 0 || downTileType == 3))
				{
					PathCheck = true;

					if (CheckTile(x , y + 1) == -2 || CheckTile(x , y - 1) == -2)
						ChanceMax = 1;
				}

				//Wall check
				bool WallCheck = false;
				if (CheckTile(x + 1, y) == 2 && CheckTile(x - 1, y) == 2)
				{
					WallCheck = true;
				}


				if (CheckTile(x , y + 1) == 2 && CheckTile(x , y - 1) == 2)
				{
					WallCheck = true;
				}


				if (PathCheck == true && WallCheck == true)
				{
					if (GetRandomValue(0, ChanceMax) == 0)
					{
						tiles[x][y].genType = 4;
					}
				}
			}
		}
	}
}


void GenerateBuilding()
{
	bool StartPositionFound = false;
	int rndX;
	int rndY;
	/*while (StartPositionFound == false)
	{
		rndX = GetRandomValue(0, mapWidth-1);
		rndY = GetRandomValue(0, mapHeight-1);
		if (tiles[rndX][rndY].genType == EmptyType)
		{
			StartPositionFound = true;
		}
	}*/

	rndX = GetRandomValue(0, mapWidth - 1);
	rndY = GetRandomValue(0, mapHeight - 1);

	int maxBuildingWidth = GetRandomValue(4, (mapWidth-1) / 3);
	int maxBuildingHeight = GetRandomValue(4, (mapHeight-1) / 3);

	if (rndX + maxBuildingWidth > mapWidth)
		maxBuildingWidth = mapWidth - rndX;

	if (rndY + maxBuildingHeight > mapHeight)
		maxBuildingHeight = mapHeight - rndY;

	for (int x = rndX; x < (rndX + maxBuildingWidth); x++)
	{
		for (int y = rndY; y < rndY + maxBuildingHeight; y++)
		{
			if (tiles[x][y].genType == 0 || tiles[x][y].genType == 3)
			{
				tiles[x][y].genType = 3;
			}
			else
			{
				break;
			}
		}
	}



	//tiles[rndX][rndY].genType = BuildingType;
}

void DeleteMap()
{
	for (int x = 0; x < mapWidth - 1; x++)
	{
		for (int y = 0; y < mapHeight - 1; y++)
		{
			tiles[x][y].genType = 0;

			if(sizeof(tiles[x][y].texture) > 0)
				UnloadTexture(tiles[x][y].texture);


			if (sizeof(tiles[x][y].floortexture) > 0)
			{
				UnloadTexture(tiles[x][y].floortexture);
			}

			tiles[x][y].col = WHITE;
			tiles[x][y].entityCount = 0;
			tiles[x][y].texture = (Texture2D){0};
		}
	}
}


Vector2 GetEmptyBuildingTile()
{
	int checktileX = GetRandomValue(10, 90);
	int checktileY = GetRandomValue(10, 90);

	while (tiles[checktileX][checktileY].genType != 3)
	{
		checktileX = GetRandomValue(10, 90);
		checktileY = GetRandomValue(10, 90);
	}

	return (Vector2) { checktileX, checktileY };
}


void GenerateHelipad()
{
	for (int x = NextRoadEndX - 1; x < NextRoadEndX+2; x++)
	{
		if (x > 0 && x < (mapWidth - 1))
		{
			for (int y = (mapHeight/2)-1; y < (mapHeight / 2) + 2; y++)
			{
				if (y > 0 && y < (mapHeight - 1))
				{
					tiles[x][y].genType = -3;
				}
			}
		}
	}

	tiles[NextRoadEndX][mapHeight/2].genType = -4;
}

Texture DefaultAboveWallTex;

int GenerateMap()
{
	mapWidth = 100;
	mapHeight = 100;

	DeleteMap();

	WhiteTex = GenerateBlankTexture(WHITE);
	ExitSignTex = GenerateExitTexture();
	HelipadOuterTex = GenerateBlankTexture((Color) { 242, 192, 53, 255 });
	HelipadInnerTex = GenerateBlankTexture((Color) { 196, 196, 196, 255 });

	GenerateRoad();

	if (CityProgressed < 5)
	{
		int buildingCount = 15;
		for (int i = 0; i < buildingCount; i++)
		{
			GenerateBuilding();
		}



		HollowBuilding(true);

		CreateBuildingDoor();


		CreateRandomWalls();

		DefaultAboveWallTex = GenerateWallTexture(0, 0);
	}
	else
	{
		GenerateHelipad();
	}



	for (int x = 0; x < 100; x++)
	{
		for (int y = 0; y < 100; y++)
		{
			tiles[x][y].x = x;
			tiles[x][y].y = y;

			Color c = (Color){ .a=255};
			c.r = GetRandomValue(240, 254);
			c.g = c.r;
			c.b = c.r;

			if (tiles[x][y].genType == 0)
			{
				tiles[x][y].floortexture = GenerateGrassTexture(x, y);
			}
			else if(tiles[x][y].genType == -2)
			{
				tiles[x][y].floortexture = GenerateRoadTexture(x, y);
				c = roadColor;
			}
			else if (tiles[x][y].genType == -3)
			{
				tiles[x][y].floortexture = HelipadOuterTex;
			}
			else if (tiles[x][y].genType == -4)
			{
				tiles[x][y].floortexture = HelipadInnerTex;
			}
			else
			{
				tiles[x][y].floortexture = GenerateConcreteTexture(x, y);
			}


			if (tiles[x][y].genType == 2)
			{
				tiles[x][y].texture = GenerateWallTexture(x, y);
			}

			if (tiles[x][y].genType == 4)
			{
				tiles[x][y].texture = GenerateWoodTexture(x, y);
			}


			tiles[x][y].col = c;
		}
	}


	Image im = GenImageColor(mapWidth, mapHeight, LIGHTGRAY);
	for (int x = 0; x < mapWidth-1; x++)
	{
		for (int y = 0; y < mapHeight-1; y++)
		{
			if (tiles[x][y].genType == 2)
				ImageDrawPixel(&im, x, y, BLACK);

			if (tiles[x][y].genType == -2)
				ImageDrawPixel(&im, x, y, BLUE);

			if (tiles[x][y].genType == 3)
				ImageDrawPixel(&im, x, y, DARKGRAY);

			if (tiles[x][y].genType == 4)
				ImageDrawPixel(&im, x, y, BROWN);
		}
	}

	miniMapTexture = LoadTextureFromImage(im);

	//Generate collectibles
	if (CityProgressed < 5)
	{
		CollectibleLimit = 15 + (CityProgressed * 8);

		for (int i = 0; i < CollectibleLimit; i++)
		{
			int type = 4;
			int rndNumber = GetRandomValue(0, 100);
			if (rndNumber < 25)
				type = 3;
			else if (rndNumber < 70)
				type = GetRandomValue(0, 3);

			struct Vector2 emptyTilePos = GetEmptyBuildingTile();
			SpawnCollectable((Vector2) { emptyTilePos.x * 2, emptyTilePos.y * -2 }, type);
		}
	}
	else
	{
		for (int i = 0; i < 80; i++)
		{
			int type = 3;
			int rndNumber = GetRandomValue(0, 100);
			if (rndNumber < 70)
				type = GetRandomValue(0, 3);

			struct Vector2 emptyTilePos = GetEmptyTile();
			SpawnCollectable((Vector2) { emptyTilePos.x * 2, emptyTilePos.y * -2 }, type);
		}
	}


	//Model ceiling = LoadModelFromMesh(GenMeshPlane(2, 2, 5, 5));

	return 1;
}



Color CalculateFogColor(Color c,float dist)
{
	float ratio = Clamp((dist / (((float)viewDistance)*0.9f)),0,1);
	Color newC;
	//For some reason smooth fog only happens at twice the color values
	Color skyColor = { .r = 0,.g = 30,.b = 108,.a = 255};
	LerpColor(c, skyColor , ratio,&newC);
	return newC;
}




Color CalculateFogColorWithAlpha(Color c, float dist)
{
	float ratio = Clamp((dist / (((float)viewDistance) * 0.9f)), 0, 1);
	Color newC;
	//For some reason smooth fog only happens at twice the color values
	Color skyColor = { .r = 0,.g = 30,.b = 108,.a = 0 };
	LerpColor(c, skyColor, ratio, &newC);
	/*newC.r = LerpColor(c. * ratio * SKYBLUE.r;
	newC.g = c.g * ratio * SKYBLUE.g;
	newC.b = c.b * ratio * SKYBLUE.b;
	newC.a = c.a;
	*/
	//Color newColor = { .r = c.r * ratio,.g = c.g * ratio, .b = c.b * ratio};
	return newC;
}




void CreateAboveWall(Texture targetTex, Vector3 tilePos, int x, int y, float dist)
{
	//return;
	//Build AboveWall

	Vector3 buildingCube = tilePos;

	for (int h = 1; h < 20; h++)
	{
		buildingCube.y = (((float)h) + 0.5f) * 2.0f;
		DrawCubeTexture(targetTex, buildingCube, 2.0f, 2.0f, 2.0f, CalculateFogColor(tiles[x][y].col, dist));
	}

}

void DrawDoor(int x, int y,Vector3 tilePos,float dist,Vector3 playerPos)
{
	struct MapTile playerTile = GetClosestGrid(playerPos.x, playerPos.z);
	bool IsOpen = (playerTile.x == x && playerTile.y == y) || tiles[x][y].entityCount > 0;


	float w = 2.0f;
	float l = 2.0f;
	float xOffset = 0;
	float zOffset = 0;
	if (CheckTile(x + 1, y) != 2 && CheckTile(x - 1, y) != 2)
	{
		if (!IsOpen)
			w = 0.5f;
		else
			w = 0.01f;

		zOffset = 1.9F;
	}


	if (CheckTile(x, y + 1) != 2 && CheckTile(x, y - 1) != 2)
	{
		if (!IsOpen)
			l = 0.5f;
		else
			l = 0.01f;


		xOffset = -1.9F;
	}



	if (IsOpen == false)
	{
		DrawCubeTexture(tiles[x][y].texture, tilePos, w, 2.0f, l, CalculateFogColor(DARKBROWN, dist));
	}
	else
	{
		Vector3 openPos = Vector3Add(tilePos, (Vector3) { xOffset, 0.0f, zOffset});
		DrawCubeTexture(tiles[x][y].texture, openPos, 1.9f, 1.9f, 1.9f, CalculateFogColor(DARKBROWN, dist));
	}



	CreateAboveWall(DefaultAboveWallTex, tilePos, x, y, dist);
}


void DrawMap(Vector3 playerPos)
{
	Vector3 tilePos;
	for (int x = 0; x < mapWidth; x++)
	{
		for (int y = 0; y < mapHeight; y++)
		{
			tilePos.x = x * 2;
			tilePos.z = -y * 2;
			tilePos.y = 1;
			

			Vector3 pos = { .x = playerPos.x, .y = playerPos.y, .z = playerPos.z};
			float dist = Vector3Distance(playerPos, tilePos);

			if (dist < viewDistance)
			{
				//Wall
				if (tiles[x][y].genType == 2)
				{
					DrawCubeTexture(tiles[x][y].texture, tilePos, 2.0f, 2.0f, 2.0f, CalculateFogColor(tiles[x][y].col, dist));
					CreateAboveWall(tiles[x][y].texture, tilePos,x,y,dist);
				}

				//Door
				if (tiles[x][y].genType == 4)
				{
					DrawDoor(x,y,tilePos,dist, playerPos);
				}

				//Floor
				tilePos.y = 0;
				if (tiles[x][y].genType !=  2 )
				{
					DrawPlaneTexture(tiles[x][y].floortexture, tilePos, 2.0f, 2.0f, CalculateFogColor(tiles[x][y].col, dist));
				}


				//Ceiling
				tilePos.y = 2.0f;
				if (tiles[x][y].genType == 3)
				{
					DrawFlippedPlane(tilePos, (Vector2) { 2.0f, 2.0f }, CalculateFogColor(DARKGRAY, dist));
				}
			}
		}
	}

	//Exit sign
	if (CityProgressed < 5)
	{
		float dist = Vector3Distance(playerPos, ExitPosition);
		DrawCubeTexture(ExitSignTex, ExitPosition, 2.0f, 1.0f, 0.2f, CalculateFogColorWithAlpha(WHITE, dist));
		DrawCubeTexture(WhiteTex, Vector3Add(ExitPosition, (Vector3) { 0, 0, -0.025f }), 2.2f, 1.2f, 0.2f, CalculateFogColorWithAlpha(DARKGRAY, dist));
		DrawCubeTexture(WhiteTex, Vector3Add(ExitPosition, (Vector3) { -0.4f, -0.3f, -0.1f }), 0.2f, 2.0f, 0.2f, CalculateFogColorWithAlpha((Color) { 50, 50, 50, 255 }, dist));
		DrawCubeTexture(WhiteTex, Vector3Add(ExitPosition, (Vector3) { 0.4f, -0.3f, -0.1f }), 0.2f, 2.0f, 0.2f, CalculateFogColorWithAlpha((Color) { 50, 50, 50, 255 }, dist));



		//Calculate map exit
		if (Vector3Distance(GetPlayerRef()->position, ExitPosition) < 2 && InEndScreen == false)
		{
			OpenEndScreen();
		}
	}
	else
	{
		if (HelicopterStarted == false)
		{
			//ExitPosition = (Vector3){ exitZ * 2, 1, -mapHeight * 2 };
			struct MapTile EscapeTile = tiles[NextRoadEndX][mapHeight / 2];
			float dist = Vector3Distance(playerPos, (Vector3) { EscapeTile.x * 2, 0, EscapeTile.y * -2 });
			if (dist < 4)
			{
				HelicopterStarted = true;
			}
		}
	}
}




struct MapTile GetClosestGrid(float x, float y)
{
	int roundedX = abs(round(x / 2.0f));
	int roundedY = abs(round(y / 2.0f));

	roundedX = Clamp(roundedX, 0, mapWidth - 1);
	roundedY = Clamp(roundedY, 0, mapHeight - 1);

	return tiles[roundedX][roundedY];
}



struct MapTile* GetClosestGridRef(float x, float y)
{
	int roundedX = abs(round(x / 2.0f));
	int roundedY = abs(round(y / 2.0f));

	roundedX = Clamp(roundedX, 0, mapWidth - 1);
	roundedY = Clamp(roundedY, 0, mapHeight - 1);

	return &tiles[roundedX][roundedY];
}


void DisposeMap()
{
	free(tiles);
	NextRoadEndX = -1;
}


void DrawMinimap(int posX,int posY)
{ 
	DrawTexture(miniMapTexture, posX, posY, WHITE);
}

Vector2 GetEmptyTile()
{
	int checktileX = GetRandomValue(10, 90);
	int checktileY = GetRandomValue(20, 90);

	while (tiles[checktileX][checktileY].genType != 0 && tiles[checktileX][checktileY].genType != 3)
	{
		checktileX = GetRandomValue(10, 90);
		checktileY = GetRandomValue(20, 90);
	}

	return (Vector2) { checktileX, checktileY };
}



Vector2 GetWaveSpawnTile()
{
	int checktileX = GetRandomValue(1, 99);
	int checktileY = GetRandomValue(1, 20);

	return (Vector2) { checktileX, checktileY };
}

void ResetEntityData()
{
	for (int x = 0; x < mapWidth; x++)
	{
		for (int y = 0; y < mapHeight; y++)
		{
			tiles[x][y].entityCount = 0;
			tiles[x][y].middlePoint = (Vector3){ 0, 1.0f, 0};
		}
	}
}

Vector3 GetNormal(Vector3 dir)
{
	Vector3 invertedDir = Vector3Negate(dir);
	float threshold = 0.1f;

	Vector3 normal;
	normal = (Vector3){ 1,0,0 };
	if (Vector3DotProduct(invertedDir, normal) > threshold)
		return normal;

	normal = (Vector3){ -1,0,0 };
	if (Vector3DotProduct(invertedDir, normal) > threshold)
		return normal;

	normal = (Vector3){ 0,0,1 };
	if (Vector3DotProduct(invertedDir, normal) > threshold)
		return normal;

	normal = (Vector3){ 0,0,-1 };
	if (Vector3DotProduct(invertedDir, normal) > threshold)
		return normal;
}

