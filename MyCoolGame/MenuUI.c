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

Color selectedColor;
Color unselectedColor;
Color notEnoughColor;
Color notEnoughSelectedColor;
int upgradeSelectedIndex;
float lastPressedTime;
int lastAppliedIndex;
float SelectedAnimTimer;
bool GameLoadStarted = false;
int LoadGameCooldown = 0;

void DrawLoadingScreen()
{
	DrawRectangle(0, 0, 900, 500, BLACK);
	DrawText("LOADING...", 600, 400, 30, WHITE);
}

void ApplyChoice()
{
	struct MainPlayer *player = GetPlayerRef();
	bool Success = false;

	if (upgradeSelectedIndex == 0 && player->Scrap > 24)
	{
		player->Scrap -= 25;
		player->MaxHealth += 25;
		Success = true;
	}


	if (upgradeSelectedIndex == 1 && player->Scrap > 29)
	{
		player->Scrap -= 30;
		player->ExtraDamage += 0.25f;
		Success = true;
	}

	if (upgradeSelectedIndex == 2 && player->Scrap > 19)
	{
		player->Scrap -= 20;
		player->MoveSpeed += 0.025f;
		Success = true;
	}


	if (upgradeSelectedIndex == 3 && player->Scrap > 14)
	{
		if (player->AlertDistance > 0)
		{
			player->Scrap -= 15;
			player->AlertDistance -= 1;
			Success = true;
		}
	}


	if (upgradeSelectedIndex == 4)
	{
		GameLoading = true;
		LoadGameCooldown = 0;
	}


	if (Success)
	{
		lastAppliedIndex = upgradeSelectedIndex;
		SelectedAnimTimer = 1.0f;
	}
}


float alpha = 0;
float alpha2 = 0;

void DrawPlayerDeathScreen()
{
	if (alpha < 254)
		alpha += GetFrameTime() * 200;
	else
	{
		alpha = 255;

		if (alpha2 < 254)
			alpha2 += GetFrameTime() * 50;
		else
			alpha2 = 255;
	}

	DrawRectangle(0, 0, 900, 500, (Color) {50,0,0,alpha});
	DrawText("You're dead!", 300, 200, 30, (Color) { 220, 100, 100, alpha });

	DrawText("Press ESC to exit", 250, 300, 30, (Color) { 220, 100, 100, alpha2 });
}


void DrawStartGameScreen()
{
	DrawRectangle(0, 0, 900, 500, BLACK);
	//DrawRectangle(399, 0, 3, 500, GRAY);
	DrawText("Tiny Apocalypse", 260, 20, 30, WHITE);
	DrawText("by Wolderado", 320, 60, 20, WHITE);

	DrawText("Follow the road to move to next city", 210, 120, 20, WHITE);
	DrawText("Travel to 5 cities to reach helicopter and escape", 150, 150, 20, WHITE);

	DrawText("WASD to Move", 320, 220, 20, WHITE);
	DrawText("Click to Shoot", 325, 240, 20, WHITE);

	DrawText("Press Enter To Start", 220, 380, 30, WHITE);

	if ((IsKeyPressed(KEY_ENTER)))
	{
		GameStarted = true;
		InEndScreen = false;
	}
}


void DrawCraftingScreen()
{
	DrawRectangle(0, 0, 900, 500, BLACK);
	DrawText("Moving to next city", 250, 20, 30, WHITE);
	DrawText(TextFormat("%01i City Left Until Escape", (5 - CityProgressed)), 300, 80, 20, WHITE);
	DrawText("Upgrade Stats", 330, 150, 20, WHITE);


	if ((IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) && upgradeSelectedIndex < 4)
	{
		upgradeSelectedIndex++;
		lastPressedTime = GetTime();
	}

	if ((IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) && upgradeSelectedIndex > 0)
	{
		upgradeSelectedIndex--;
		lastPressedTime = GetTime();
	}

	if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER))
	{
		ApplyChoice();
	}



	if (SelectedAnimTimer > 0)
		SelectedAnimTimer -= GetFrameTime() * 2;

	//Draw Texts according to selectedIndex
	int targetX = 260;
	int playerScrap = GetPlayer().Scrap;
	Color textColor = unselectedColor;

	targetX = lastAppliedIndex == 0 ? 260 + (sin(SelectedAnimTimer) * 10) : 260;
	textColor = upgradeSelectedIndex == 0 ? (playerScrap > 24 ? selectedColor : notEnoughSelectedColor) : (playerScrap > 24 ? unselectedColor : notEnoughColor);
	DrawText("Upgrade Max Health", targetX, 220, upgradeSelectedIndex == 0 ? 16 : 15, textColor);
	DrawText("(25 Scraps)", 460, 220, upgradeSelectedIndex == 0 ? 16 : 15, textColor);


	targetX = lastAppliedIndex == 1 ? 260 + (sin(SelectedAnimTimer) * 10) : 260;
	textColor = upgradeSelectedIndex == 1 ? (playerScrap > 29 ? selectedColor : notEnoughSelectedColor) : (playerScrap > 29 ? unselectedColor : notEnoughColor);
	DrawText("Upgrade Damage", targetX, 250, upgradeSelectedIndex == 1 ? 16 : 15, textColor);
	DrawText("(30 Scraps)", 460, 250, upgradeSelectedIndex == 1 ? 16 : 15, textColor);

	targetX = lastAppliedIndex == 2 ? 260 + (sin(SelectedAnimTimer) * 10) : 260;
	textColor = upgradeSelectedIndex == 2 ? (playerScrap > 19 ? selectedColor : notEnoughSelectedColor) : (playerScrap > 19 ? unselectedColor : notEnoughColor);
	DrawText("Upgrade Move Speed", targetX, 280, upgradeSelectedIndex == 2 ? 16 : 15, textColor);
	DrawText("(20 Scraps)", 460, 280, upgradeSelectedIndex == 2 ? 16 : 15, textColor);

	targetX = lastAppliedIndex == 3 ? 260 + (sin(SelectedAnimTimer) * 10) : 260;
	textColor = upgradeSelectedIndex == 3 ? (playerScrap > 14 ? selectedColor : notEnoughSelectedColor) : (playerScrap > 14 ? unselectedColor : notEnoughColor);
	DrawText("Upgrade Visibility", targetX, 310, upgradeSelectedIndex == 3 ? 16 : 15, textColor);
	DrawText("(15 Scraps)", 460, 310, upgradeSelectedIndex == 3 ? 16 : 15, textColor);

	DrawText("Continue", 365, 360, upgradeSelectedIndex == 4 ? 20 : 16, upgradeSelectedIndex == 4 ? selectedColor : unselectedColor);
}




void DrawHelicopterTimer()
{
	if (GetPlayerRef()->Health > 0)
	{
		if (HelicopterTimer > 0)
		{
			HelicopterTimer -= GetFrameTime();

			int minute = HelicopterTimer / 60;
			int seconds = ((int)HelicopterTimer % 60);

			//DrawRectangle(399, 0, 3, 500, GRAY);
			DrawText(TextFormat("%01i:", minute), 370, 80, 30, (Color) { 255, 150, 143, 255 });
			DrawText(TextFormat("%02i", seconds), 395, 80, 30, (Color) { 255, 150, 143, 255 });
		}
		else
		{
			InEndScreen = true;
			GameFinished = true;
		}
	}
}


float waitTimer;
void DrawGameFinishedScreen()
{
	if (alpha < 254)
		alpha += GetFrameTime() * 100;
	else
	{
		alpha = 255;

		if (alpha2 < 254)
			alpha2 += GetFrameTime() * 50;
		else
			alpha2 = 255;
	}

	DrawRectangle(0, 0, 900, 500, (Color) { 0, 20, 0, alpha });
	DrawText("You escaped!", 300, 100, 30, (Color) { 100, 220, 100, alpha });

	DrawText("Thanks for playing!", 250, 150, 30, (Color) { 100, 220, 100, alpha2 });


	DrawText("Made by Wolderado", 260, 230, 30, (Color) { 100, 220, 100, alpha2 });

	DrawText("Press ESC to exit", 270, 380, 30, (Color) { 220, 100, 100, alpha2 });
}

void ProcessAndDrawUI()
{
	if (HelicopterStarted)
	{
		DrawHelicopterTimer();
	}

	if (InEndScreen && GameLoading == false)
	{
		if (GetPlayerRef()->Health <= 0)
		{
			DrawPlayerDeathScreen();
		}
		else
		{
			if (GameStarted)
			{
				if (GameFinished == false)
					DrawCraftingScreen();
				else
					DrawGameFinishedScreen();
			}
			else
			{
				DrawStartGameScreen();
			}
		}
	}


	if (GameLoading)
	{
		DrawLoadingScreen();

		LoadGameCooldown++;
		if (LoadGameCooldown > 59)
		{
			if (GameLoadStarted == false)
			{
				GameLoadStarted = true;

				PrepareMap();
				GameLoading = false;
				InEndScreen = false;
				GameLoadStarted = false;
			}
		}
	}
}



void OpenEndScreen()
{
	if (InEndScreen == false)
	{
		upgradeSelectedIndex = 0;
		CityProgressed++;
		InEndScreen = true;

		selectedColor  = (Color){ 207,198,120,255 };
		unselectedColor = (Color){ 120,120,120,255 };
		notEnoughColor = (Color){120,70,70,255};
		notEnoughSelectedColor = (Color){ 220,120,120,255 };
	}
}