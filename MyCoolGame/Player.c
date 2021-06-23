

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





struct MainPlayer myPlayer;

Camera3D *cam;
Texture2D PlayerMinimapTex;
bool Initialized = false;
Texture2D BlackBckTex;
int LogIndex = 0;
float healthValue1;
float healthValue2;



void InitializePlayer(Camera3D *gameCam)
{
	cam = gameCam;
	myPlayer.position = cam->position;

	myPlayer.RotateSpeedMin = 0.01f;
	myPlayer.RotateSpeedMax = 0.06f;
	myPlayer.MoveSpeed = 0.2F;
	myPlayer.MouseRotateSpeed = 0.005f;
	myPlayer.WeaponSwitchTimer = -1;
	myPlayer.MaxHealth = 100;
	myPlayer.Health = myPlayer.MaxHealth;
	myPlayer.AlertDistance = 20;
	healthValue1 = myPlayer.Health;
	healthValue2 = myPlayer.Health;
	myPlayer.ExtraDamage = 1;

	Color c = (Color){ .r = 100,.g = 255, .b = 100,.a = 200 };

	Image im = GenImageGradientRadial(8, 8, 0.35, c, BLANK);
	ImageDrawCircle(&im, im.width / 2, im.height / 2, (im.width / 3) - 1, c);
	PlayerMinimapTex = LoadTextureFromImage(im);

	c = (Color){ .r = 20,.g = 20, .b = 20,.a = 50 };
	im = GenImageColor(200, 140, c);
	BlackBckTex = LoadTextureFromImage(im);



	//Setup Weapons
	//0: Pistol
	//1: Machinegun
	//2: Rifle
	//3: Shotgun
	myPlayer.AllWeapons[0] = (struct Weapon){ .type = Pistol, .cooldownDuration = 0.4f, .Noise = 0, .Damage = 2, .FastShooter = true, .bullet = 9999 };
	myPlayer.AllWeapons[1] = (struct Weapon){ .type = MachineGun, .cooldownDuration = 0.1f, .Noise = 1, .Damage = 1, .FastShooter = true};
	myPlayer.AllWeapons[2] = (struct Weapon){ .type = Rifle, .cooldownDuration = 1.5f, .Noise = 5,.Damage = 5};
	myPlayer.AllWeapons[3] = (struct Weapon){ .type = Shotgun, .cooldownDuration = 0.8f, .Noise = 8,.Damage = 2};
	myPlayer.equippedWeapon = &myPlayer.AllWeapons[0];




	Initialized = true;
	HideCursor();
	DisableCursor();
}




float RotateSpeed;
float LastMouseX;
double angle = PI;
Vector3 velocity;
Vector3 drift;
Vector3 externalvelocity;
Vector3 externaltargetvelocity;
Vector3 CameraVelocity;
float targetexternalvelocityForce;
Vector3 camAnimPosOffset = { 0,0,0 };
float WalkAnimXSwing;
float WalkAnimYSwing;
float gravityPower = 1;
bool ApplyExternalForce = false;
bool SlowExternalForce;
int nextWeapon = 0;
bool weaponSwitched = false;

void ApplyForceToPlayer(Vector3 force)
{
	gravityPower = 1;
	externalvelocity = force;
	externaltargetvelocity = (Vector3){ 0,0,0 };
	ApplyExternalForce = true;
	SlowExternalForce = false;
}

void SwitchWeapon(int newWeaponID)
{
	
	myPlayer.equippedWeapon = &myPlayer.AllWeapons[newWeaponID];
}

void SwitchingWeaponAnimation()
{
	if (myPlayer.WeaponSwitchTimer > -1)
	{
		myPlayer.WeaponSwitchTimer -= GetFrameTime() * 16;
		if (weaponSwitched == false && myPlayer.WeaponSwitchTimer <= 0)
		{
			weaponSwitched = true;
			SwitchWeapon(nextWeapon);
		}
	}

	myPlayer.WeaponSwitchTimer = Clamp(myPlayer.WeaponSwitchTimer, -1, 1);
}

void StartSwitchingWeapon(int newWeaponID)
{
	nextWeapon = newWeaponID;
	myPlayer.WeaponSwitchTimer = 1;
	weaponSwitched = false;
}

int ProcessInput()
{
	if (InEndScreen || !GameStarted)
		return;

	float rotateInput = 0;
	float mouseXDelta = GetMouseX() - LastMouseX;
	LastMouseX = GetMouseX();

	if (IsKeyDown(KEY_RIGHT))
	{
		rotateInput += RotateSpeed;
	}

	if (IsKeyDown(KEY_LEFT))
	{
		rotateInput -= RotateSpeed;
	}

	/*myPlayer.MoveSpeed = 0.2f;
	if (IsKeyDown(KEY_LEFT_SHIFT))
	{
		myPlayer.MoveSpeed = 2;
	}*/


	rotateInput += mouseXDelta * myPlayer.MouseRotateSpeed;


	if (rotateInput == 0)
	{
		RotateSpeed = myPlayer.RotateSpeedMin;
	}
	else
	{
		if (RotateSpeed < myPlayer.RotateSpeedMax)
			RotateSpeed += 0.001f;
	}

	angle += rotateInput;

	//Update rotation
	Vector3 dirVector = { cos(angle),0,sin(angle) };

	myPlayer.direction = dirVector;


	cam->target = Vector3Add(myPlayer.position,Vector3Scale(dirVector, 100));
	myPlayer.angle = angle;


	float inputForward = 0;
	if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W))
	{
		inputForward += myPlayer.MoveSpeed;
	}

	if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S))
	{
		inputForward -= myPlayer.MoveSpeed;
	}

	float inputRight = 0;
	if (IsKeyDown(KEY_D))
	{
		inputRight += myPlayer.MoveSpeed;
	}

	if (IsKeyDown(KEY_A))
	{
		inputRight -= myPlayer.MoveSpeed;
	}

	if (IsKeyPressed(KEY_SPACE) && myPlayer.position.y < 1.05f)
	{
		ApplyForceToPlayer((Vector3) { 0, 0.3f, 0 });
	}

	//Normalize input
	bool IsInputReceiving = abs(inputRight) > myPlayer.MoveSpeed * 0.01f || abs(inputForward) > myPlayer.MoveSpeed * 0.01f;
	float len = sqrtf(powf(fabs(inputRight), 2) + powf(fabs(inputForward),2)) / myPlayer.MoveSpeed;

	if (len > 0)
	{
		inputRight = inputRight / len;
		inputForward = inputForward / len;
	}


	velocity = Vector3Scale(dirVector, inputForward);
	

	//Side movement
	Vector3 rightVector = { cos(angle+(PI/2)),0,sin(angle + (PI / 2))};
	Vector3 scaledRightVector = Vector3Scale(rightVector, inputRight);
	velocity = Vector3Add(velocity, scaledRightVector);



	if (inputForward != 0 || inputRight != 0)
	{
		drift = velocity;
	}
	else
	{
		drift = Vector3Scale(drift, 0.8f);
		velocity = Vector3Add(velocity, drift);
	}


	//External force
	if (ApplyExternalForce)
	{
		if (SlowExternalForce == false)
		{
			externaltargetvelocity = Vector3Add(Vector3Scale(externalvelocity, 0.2f), externaltargetvelocity);

			if (Vector3Length(externaltargetvelocity) > Vector3Length(externalvelocity) - 0.01f)
			{
				SlowExternalForce = true;
			}
		}
		else
		{
			if (myPlayer.position.y > 1.0f)
			{
				externaltargetvelocity = Vector3Scale(externaltargetvelocity, 0.95f);

				if (externaltargetvelocity.x < 0.01f && externaltargetvelocity.y < 0.01f && externaltargetvelocity.z < 0.01f)
					ApplyExternalForce = false;
			}
			else
			{
				externaltargetvelocity = (Vector3){ 0,0,0 };
				ApplyExternalForce = false;
			}
		}
		
		velocity = Vector3Add(velocity, externaltargetvelocity);

	}


	//Apply gravity
	if (SlowExternalForce == true || ApplyExternalForce == false)
	{
		if (myPlayer.position.y > 1.0f)
		{
			gravityPower += 0.02f;
			velocity = Vector3Add(velocity, (Vector3) { 0, -0.1f * gravityPower, 0 });
		}
		else
			gravityPower = 1;
	}


	//collision
	Vector3 playerNextLocation = Vector3Add(myPlayer.position, velocity);
	Vector3 exagVel = Vector3Scale(velocity, 4.0F);
	Vector3 exagPlayerNextLocation = Vector3Add(myPlayer.position, exagVel);

	struct MapTile nextGrid = GetClosestGrid(exagPlayerNextLocation.x, -exagPlayerNextLocation.z);

	bool MovePerm = true;
	if (nextGrid.genType == 2)
		MovePerm = false;

	if (nextGrid.genType == 3 && playerNextLocation.y > 1.8f)
	{
		velocity.y = 0;
		externalvelocity.y = 0;
		externaltargetvelocity.y = 0;
		MovePerm = false;
	}
	else
	{
		myPlayer.position.y = playerNextLocation.y;
	}


	if (playerNextLocation.z > 0 || playerNextLocation.z < -199 || playerNextLocation.x < 0 || playerNextLocation.x > 199)
		MovePerm = false;
	

	if (MovePerm)
	{
		myPlayer.position = playerNextLocation;
		//myPlayer.position.y = playerNextLocation.y;

		//Camera animation

		if (inputForward != 0 || inputRight != 0)
		{
			WalkAnimXSwing += 0.1f;
			WalkAnimYSwing += 0.2f;


			Vector3 headSway = { 0,0,0 };
			headSway = Vector3Scale(rightVector, sin(WalkAnimXSwing) * 0.1f);

			camAnimPosOffset.x = headSway.x;
			camAnimPosOffset.y = sin(WalkAnimYSwing) * 0.1f;
			camAnimPosOffset.z = headSway.z;
		}
		else
		{
			WalkAnimXSwing = 0.0f;
			WalkAnimYSwing = 0.0f;
			camAnimPosOffset.y = Lerp(camAnimPosOffset.y, 0.0f, 0.05f);
			camAnimPosOffset.x = Lerp(camAnimPosOffset.x, 0.0f, 0.05f);
		}
	}
	else
	{
		//drift = Vector3Scale(velocity, 0.0f);
		velocity.x = 0;
		velocity.z = 0;
	}


	Vector3 finalCamPos;
	finalCamPos = Vector3Add(myPlayer.position, camAnimPosOffset);
	CameraVelocity = Vector3Lerp(CameraVelocity, (Vector3) { 0, 0, 0 }, GetFrameTime() * 8);
	finalCamPos = Vector3Add(finalCamPos, CameraVelocity);
	finalCamPos = Vector3Add(finalCamPos, (Vector3) { 0, 0.2f, 0 });



	cam->position = finalCamPos;


	//Shooting
	if ((IsMouseButtonPressed(0) || IsKeyPressed(KEY_RIGHT_CONTROL)) || myPlayer.equippedWeapon->FastShooter && (IsMouseButtonDown(0) || IsKeyDown(KEY_LEFT_CONTROL)))
	{
		Shoot();
	}

	//DrawWeapon(false);


	//Change weapon
	if (myPlayer.WeaponSwitchTimer <= 0)
	{
		if (IsKeyDown(KEY_ONE))
			StartSwitchingWeapon(0);

		if (IsKeyDown(KEY_TWO) && myPlayer.AllWeapons[1].bullet > 0)
			StartSwitchingWeapon(1);

		if (IsKeyDown(KEY_THREE) && myPlayer.AllWeapons[2].bullet > 0)
			StartSwitchingWeapon(2);

		if (IsKeyDown(KEY_FOUR) && myPlayer.AllWeapons[3].bullet > 0)
			StartSwitchingWeapon(3);
	}

	SwitchingWeaponAnimation();



	return angle;
}




void PushCamera(Vector3 force)
{
	CameraVelocity = force;
}



void DrawMinimapPlayer(int posX, int posY)
{
	int scaledX = round(myPlayer.position.x / 2) * 2;
	int scaledY = round(myPlayer.position.z / 2) * 2;

	//DrawTexture(PlayerMinimapTex, posX + (scaledX / 2), posY + (scaledY / 2), WHITE);

	struct MapTile nextGrid = GetClosestGrid(myPlayer.position.x, -myPlayer.position.z);
	DrawTexture(PlayerMinimapTex, posX + (nextGrid.x), posY + (nextGrid.y), WHITE);


}


void DrawStatsUI()
{
	if (GameStarted == false || GameFinished)
		return;

	//Draw stats
	Rectangle r;
	r.x = 680;
	r.y = 130;
	r.width = 100;
	r.height = 120;
	DrawRectangleRounded(r, 0.2f, 1, (Color) { 0, 0, 0, 50 });
	float offset = 3;
	r.x += offset;
	r.y += offset;
	r.width -= offset + 1;
	r.height -= offset + 1;
	DrawRectangleRoundedLines(r, 0.22f, 1, 3, (Color) { 0, 0, 0, 65 });

	DrawText("Stats", r.x + 35, r.y + 10, 7, GRAY);
	DrawText(TextFormat("Max Health:%03i", myPlayer.MaxHealth), r.x + 10, r.y + 30, 7, GRAY);
	DrawText(TextFormat("Damage:%03i", (int)(myPlayer.ExtraDamage * 100.0f)), r.x + 10, r.y + 50, 7, GRAY);
	DrawText(TextFormat("Walk Speed:%02i", (int)((myPlayer.MoveSpeed / 0.2f) * 10)), r.x + 10, r.y + 70, 7, GRAY);
	DrawText(TextFormat("Visibility:%02i", (myPlayer.AlertDistance)), r.x + 10, r.y + 90, 7, GRAY);

	//Scrap Count Panel
	r.x = 680;
	r.y = 260;
	r.width = 100;
	r.height = 20;
	DrawRectangleRounded(r, 0.4f, 1, (Color) { 0, 0, 0, 50 });
	offset = 3;
	r.x += offset;
	r.y += offset;
	r.width -= offset + 1;
	r.height -= offset + 1;
	DrawRectangleRoundedLines(r, 0.44f, 1, 3, (Color) { 0, 0, 0, 65 });
	DrawText(TextFormat("Scraps:%03i", myPlayer.Scrap), r.x + 10, r.y + 5, 7, GRAY);

	//Tutorial text
	if (InEndScreen == false)
	{
		if(CityProgressed < 5)
			DrawText("Follow The Road To Exit", 320, 20, 15, GRAY);
		else
		{
			if (HelicopterStarted == false)
			{
				DrawText("Follow The Road And Find The Helipad", 250, 20, 15, GRAY);
			}
			else
			{
				DrawText("Survive Until Helicopter Arrives", 280, 20, 15, GRAY);
			}
		}
	}
}



void HurtPlayer(int damage)
{
	if (InEndScreen)
		return;

	CreatePlayerHurtBloodParticle(Vector3Add(myPlayer.position, (Vector3) { 0, 1.0f, 0 }), myPlayer.direction, 30);

	myPlayer.Health = myPlayer.Health - damage;
	if (myPlayer.Health <= 0)
	{
		InEndScreen = true;
	}
}




void DrawHealthBar()
{
	DrawRectangle(350, 435, 100, 5, (Color) { 38, 51, 40, 80 });

	healthValue2 = Lerp(healthValue2, healthValue1, GetFrameTime());
	DrawRectangle(350, 435, healthValue2, 5, (Color) { 255, 50, 50,80 });

	Color HealthHighColor = (Color){ 54,145,72,255 };
	Color HealthLowColor = (Color){ 176,14,14,255 };
	Color finalHealthColor;
	LerpColor(HealthLowColor, HealthHighColor, healthValue1 / 100, &finalHealthColor);


	healthValue1 = Lerp(healthValue1, myPlayer.Health, GetFrameTime() * 4);
	DrawRectangle(350, 435, healthValue1, 5, finalHealthColor);




	//DrawText(TextFormat("%03i", myPlayer.Health), 340, 430, 15, WHITE);

}



struct MainPlayer GetPlayer()
{
	return myPlayer;
}

struct MainPlayer* GetPlayerRef()
{
	return &myPlayer;
}


Vector3 GetCameraPosition()
{
	return cam->position;
}


float GetPlayerAngle()
{
	return myPlayer.angle;
}


float GetCamSwingY()
{
	return WalkAnimYSwing;
}

float GetCamSwingX()
{
	return WalkAnimXSwing;
}