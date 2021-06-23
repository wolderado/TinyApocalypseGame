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


struct Bullet
{
	bool Active;
	Vector3 position;
	Vector3 dir;
	float speed;
	float timeOutTime;
	int damage;
	bool PunchThrough;
};

struct Bullet bullets[1000];

Texture BulletTexture;
Vector3 weaponSwayOffset;
Texture MuzzleTex;
void InitializeWeapon()
{
	GetPlayerRef()->equippedWeapon->texture = GeneratePistolTexture();
	BulletTexture = GenerateBulletTexture();
	Image newImg = GenImageGradientRadial(40, 40, 0.1f, (Color) { 200, 200, 100, 100 }, BLANK);
	ImageResize(&newImg, 200, 200);
	MuzzleTex = LoadTextureFromImage(newImg);

	DrawWeapon(true);

}

float lastAttackTime;
int bulletCreateIndex;
bool CreateBullet = false;
void Shoot()
{
	struct MainPlayer* player = GetPlayerRef();
	if (player->equippedWeapon->type == Pistol)
		player->equippedWeapon->bullet = 100;

	if (GetTime() > lastAttackTime && player->equippedWeapon->bullet > 0)
	{
		player->equippedWeapon->bullet -= 1;
		lastAttackTime = GetTime() + player->equippedWeapon->cooldownDuration;
		bulletCreateIndex = (bulletCreateIndex + 1) % 1000;

		Vector3 NormalizedDir = Vector3Normalize(GetPlayer().direction);
		bullets[bulletCreateIndex].dir = NormalizedDir;
		CreateBullet = true;

		player->equippedWeapon->lastShotTime = GetTime();
	}
}


Vector3 oldCamPos = { 0,0,0 };
Vector3 weaponSmoothForward;
Vector3 weaponSmoothRight;
float weaponSmoothAngle;
float muzzleActiveTime = 0;
Texture MuzzleTex;
Vector3 MuzzlePos;

void DrawWeapon(bool firstDraw)
{
	struct MainPlayer player = GetPlayer();
	Vector3 playerPos = player.position;
	Vector3 camPos = GetCameraPosition(); // Vector3Add(playerPos, (Vector3) { 0, 0.2f, 0 });
	Texture gunTex = player.equippedWeapon->texture;


	Vector3 NormalizedDir = Vector3Normalize(player.direction);

	double angle = (double)(atan2(-NormalizedDir.z, NormalizedDir.x)) * (double)RAD2DEG + 90;

	Vector3 forwardDir = NormalizedDir;
	Vector3 pVector = Vector3Perpendicular(forwardDir);

	weaponSmoothForward = Vector3Lerp(weaponSmoothForward, forwardDir, GetFrameTime() * 8);

	Vector3 switchingGunOffset = Vector3Lerp((Vector3) { 0.0, -0.3f, 0.0f },(Vector3) { 0.0, 0.0f, 0.0f }, fabsf(player.WeaponSwitchTimer));;

	//Putting every weapong here is a bad coding practice but..
	//I'm not sure how to do it better without OOP
	//Lots of repeated code :(

	if (GetPlayer().equippedWeapon->type == Pistol)
	{
		Vector3 pistolPos;

		Vector3 offset = oldCamPos;
		offset = Vector3Add(offset, Vector3Scale(Vector3Normalize(forwardDir), 0.35f));
		offset = Vector3Add(offset, Vector3Scale(Vector3Normalize(pVector), 0.13f));
		offset = Vector3Subtract(offset, oldCamPos);


		float walkAnim = GetCamSwingY();
		offset.y += sin(walkAnim) * 0.02f;
		
		offset.y += switchingGunOffset.y;

		weaponSwayOffset = Vector3Lerp(weaponSwayOffset, offset, GetFrameTime() * 12);

		if (firstDraw)
		{
			weaponSwayOffset = Vector3Add(offset, (Vector3) { 0, 0.2f, 0 });
		}



		Vector3 mPos = Vector3Add(weaponSwayOffset, camPos);
		mPos = Vector3Add(mPos, (Vector3) { 0, -0.1, 0 });
		mPos = Vector3Add(mPos, Vector3Scale(Vector3Normalize(forwardDir), 0.1f));
		//offset = Vector3Add(mPos, Vector3Scale(Vector3Normalize(pVector), 0.1f));
		MuzzlePos = mPos;

		//Walk anim
		pistolPos = Vector3Add(weaponSwayOffset, oldCamPos);
		pistolPos = Vector3Add(pistolPos, (Vector3) { 0, -0.1f, 0 });


		//Shoot offset
		float cooldown = player.equippedWeapon->cooldownDuration;
		if (cooldown == 0)
			cooldown = 1;

		float atkOffset = Clamp((lastAttackTime - GetTime()) / cooldown, 0.0f, 1.0f);
		Vector3 weaponKickVector = Vector3Scale(Vector3Normalize(forwardDir), atkOffset * -0.1f);
		Vector3 weaponUpKickVector = Vector3Scale((Vector3) { 0, 1, 0 }, atkOffset * 0.01f);
		pistolPos = Vector3Add(pistolPos, weaponKickVector);
		pistolPos = Vector3Add(pistolPos, weaponUpKickVector);

		float gunAngleY = -angle;

		//Main gun cube
		DrawCubeRotatedAngle(gunTex, pistolPos, 0.03f, 0.03f, 0.2f, 0, gunAngleY, 0, (Color) { 30, 30, 40, 255 });

		//Back recticle
		Vector3 recticlePos = Vector3Add(pistolPos, Vector3Scale(Vector3Normalize(forwardDir), -0.09f));
		DrawCubeRotatedAngle(gunTex, Vector3Add(recticlePos, (Vector3) { 0, 0.015f, 0 }), 0.02f, 0.01f, 0.0025f, 0, gunAngleY, 0, (Color) { 50, 50, 55, 255 });

		//Forward recticle
		recticlePos = Vector3Add(pistolPos, Vector3Scale(Vector3Normalize(forwardDir), 0.09f));
		DrawCubeRotatedAngle(gunTex, Vector3Add(recticlePos, (Vector3) { 0, 0.015f, 0 }), 0.01f, 0.01f, 0.0025f, 0, gunAngleY, 0, (Color) { 50, 50, 55, 255 });

		//Handle
		Vector3 handlePos = Vector3Add(pistolPos, Vector3Scale(Vector3Normalize(forwardDir), -0.081f));
		DrawCubeRotatedAngle(gunTex, Vector3Add(handlePos, (Vector3) { 0, -0.035f, 0 }), 0.032f, 0.07f, 0.04f, 0, gunAngleY, 0, (Color) { 40, 40, 60, 255 });

		//Bottom half
		handlePos = Vector3Add(pistolPos, Vector3Scale(Vector3Normalize(forwardDir), 0.0f));
		DrawCubeRotatedAngle(gunTex, Vector3Add(handlePos, (Vector3) { 0, -0.01f, 0 }), 0.031f, 0.015f, 0.2f, 0, gunAngleY, 0, (Color) { 40, 40, 60, 255 });


		//Create pistol bullet
		if (CreateBullet)
		{
			Vector3 bulletExitPoint = Vector3Add(pistolPos, Vector3Scale(Vector3Normalize(pVector), 0.07f));
			bulletExitPoint = Vector3Add(bulletExitPoint, Vector3Scale(Vector3Normalize(forwardDir), -0.00f));
			bulletExitPoint = Vector3Add(bulletExitPoint, (Vector3) { 0, -0.05f, 0 });

			bullets[bulletCreateIndex].position = bulletExitPoint;
			bullets[bulletCreateIndex].dir = forwardDir;
			bullets[bulletCreateIndex].speed = 0.4f;
			bullets[bulletCreateIndex].Active = true;
			bullets[bulletCreateIndex].timeOutTime = GetTime() + 10;
			bullets[bulletCreateIndex].damage = round((float)player.equippedWeapon->Damage * player.ExtraDamage);

			CreateBullet = false;
			muzzleActiveTime = 0.03f;


			PushCamera(Vector3Scale(forwardDir,-0.3f));


			CreateGunParticle(bulletExitPoint, forwardDir,5);
			CreateGunSmokeParticle(bulletExitPoint, forwardDir,4);

			Vector3 shellPos = Vector3Add(pistolPos, Vector3Scale(Vector3Normalize(forwardDir), 0.2f));
			shellPos = Vector3Add(pistolPos, Vector3Scale(Vector3Normalize(pVector), -0.07f));
			CreateGunShellParticle(shellPos, forwardDir, 1);
		}
	}


	if (GetPlayer().equippedWeapon->type == MachineGun)
	{
		Vector3 pistolPos;

		Vector3 offset = oldCamPos;
		offset = Vector3Add(offset, Vector3Scale(Vector3Normalize(forwardDir), 0.4f));
		offset = Vector3Add(offset, Vector3Scale(Vector3Normalize(pVector), 0.13f));
		offset = Vector3Subtract(offset, oldCamPos);


		float walkAnim = GetCamSwingY();
		offset.y += sin(walkAnim) * 0.02f;

		offset.y += switchingGunOffset.y;


		weaponSwayOffset = Vector3Lerp(weaponSwayOffset, offset, GetFrameTime() * 12);

		if (firstDraw)
		{
			weaponSwayOffset = Vector3Add(offset, (Vector3) { 0, 0.2f, 0 });
		}


		//Muzzle point
		Vector3 mPos = Vector3Add(weaponSwayOffset, camPos);
		mPos = Vector3Add(mPos, (Vector3) { 0, -0.1, 0 });
		mPos = Vector3Add(mPos, Vector3Scale(Vector3Normalize(forwardDir), 0.2f));
		//offset = Vector3Add(mPos, Vector3Scale(Vector3Normalize(pVector), 0.1f));
		MuzzlePos = mPos;

		//Walk anim
		pistolPos = Vector3Add(weaponSwayOffset, oldCamPos);
		pistolPos = Vector3Add(pistolPos, (Vector3) { 0, -0.1f, 0 });


		//Shoot offset
		float cooldown = player.equippedWeapon->cooldownDuration;
		if (cooldown == 0)
			cooldown = 1;

		float atkOffset = Clamp((lastAttackTime - GetTime()) / cooldown, 0.0f, 1.0f);
		Vector3 weaponKickVector = Vector3Scale(Vector3Normalize(forwardDir), atkOffset * -0.1f);
		Vector3 weaponUpKickVector = Vector3Scale((Vector3) { 0, 1, 0 }, atkOffset * 0.01f);
		pistolPos = Vector3Add(pistolPos, weaponKickVector);
		pistolPos = Vector3Add(pistolPos, weaponUpKickVector);

		float gunAngleY = -angle;

		//Main gun cube
		Vector3 boxPos = Vector3Add(pistolPos, Vector3Scale(Vector3Normalize(forwardDir), 0.05f));
		DrawCubeRotatedAngle(gunTex, boxPos, 0.04f, 0.03f, 0.35f, 0, gunAngleY, 0, (Color) { 40, 50, 40, 255 });

		//Back recticle
		Vector3 recticlePos = Vector3Add(pistolPos, Vector3Scale(Vector3Normalize(forwardDir), -0.09f));
		DrawCubeRotatedAngle(gunTex, Vector3Add(recticlePos, (Vector3) { 0, 0.015f, 0 }), 0.03f, 0.015f, 0.0025f, 0, gunAngleY, 0, (Color) { 60, 60, 60, 255 });

		//Forward recticle
		recticlePos = Vector3Add(pistolPos, Vector3Scale(Vector3Normalize(forwardDir), 0.15f));
		DrawCubeRotatedAngle(gunTex, Vector3Add(recticlePos, (Vector3) { 0, 0.015f, 0 }), 0.01f, 0.01f, 0.0025f, 0, gunAngleY, 0, (Color) { 60, 65, 60, 255 });

		//Backward Handle
		Vector3 handlePos = Vector3Add(pistolPos, Vector3Scale(Vector3Normalize(forwardDir), -0.06f));
		DrawCubeRotatedAngle(gunTex, Vector3Add(handlePos, (Vector3) { 0, -0.035f, 0 }), 0.032f, 0.07f, 0.04f, 0, gunAngleY, 0, (Color) { 40, 40, 40, 255 });

		//Forward Handle2
		handlePos = Vector3Add(pistolPos, Vector3Scale(Vector3Normalize(forwardDir), 0.1f));
		DrawCubeRotatedAngle(gunTex, Vector3Add(handlePos, (Vector3) { 0, -0.05f, 0 }), 0.032f, 0.07f, 0.04f, 0, gunAngleY, 0, (Color) { 40, 50, 40, 255 });


		//Back Shoulder piece
		handlePos = Vector3Add(pistolPos, Vector3Scale(Vector3Normalize(forwardDir), -0.125f));
		DrawCubeRotatedAngle(gunTex, Vector3Add(handlePos, (Vector3) { 0, 0.0f, 0 }), 0.0405f, 0.035f, 0.01f, 0, gunAngleY, 0, (Color) { 40, 40, 40, 255 });



		//Bottom half
		handlePos = Vector3Add(pistolPos, Vector3Scale(Vector3Normalize(forwardDir), -0.05f));
		DrawCubeRotatedAngle(gunTex, Vector3Add(handlePos, (Vector3) { 0, -0.01f, 0 }), 0.043f, 0.02f, 0.14f, 0, gunAngleY, 0, (Color) { 40, 40, 40, 255 });


		//Create pistol bullet
		if (CreateBullet)
		{
			Vector3 bulletExitPoint = Vector3Add(pistolPos, Vector3Scale(Vector3Normalize(pVector), 0.07f));
			bulletExitPoint = Vector3Add(bulletExitPoint, Vector3Scale(Vector3Normalize(forwardDir), -0.00f));
			bulletExitPoint = Vector3Add(bulletExitPoint, (Vector3) { 0, -0.05f, 0 });

			bullets[bulletCreateIndex].position = bulletExitPoint;
			bullets[bulletCreateIndex].dir = forwardDir;
			bullets[bulletCreateIndex].speed = 0.4f;
			bullets[bulletCreateIndex].Active = true;
			bullets[bulletCreateIndex].timeOutTime = GetTime() + 10;
			bullets[bulletCreateIndex].damage = round((float)player.equippedWeapon->Damage * player.ExtraDamage);


			CreateBullet = false;
			muzzleActiveTime = 0.03f;



			PushCamera(Vector3Scale(forwardDir, -0.3f));


			CreateGunParticle(bulletExitPoint, forwardDir, 8);
			CreateGunSmokeParticle(bulletExitPoint, forwardDir, 5);



			Vector3 shellPos = Vector3Add(pistolPos, Vector3Scale(Vector3Normalize(forwardDir), 0.2f));
			shellPos= Vector3Add(pistolPos, Vector3Scale(Vector3Normalize(pVector), -0.07f));
			CreateGunShellParticle(shellPos, forwardDir, 1);
		}
	}


	//:(
	if (GetPlayer().equippedWeapon->type == Rifle)
	{
		Vector3 pistolPos;

		Vector3 offset = oldCamPos;
		offset = Vector3Add(offset, Vector3Scale(Vector3Normalize(forwardDir), 0.4f));
		offset = Vector3Add(offset, Vector3Scale(Vector3Normalize(pVector), 0.13f));
		offset = Vector3Subtract(offset, oldCamPos);


		float walkAnim = GetCamSwingY();
		offset.y += sin(walkAnim) * 0.02f;

		offset.y += switchingGunOffset.y;


		weaponSwayOffset = Vector3Lerp(weaponSwayOffset, offset, GetFrameTime() * 12);

		if (firstDraw)
		{
			weaponSwayOffset = Vector3Add(offset, (Vector3) { 0, 0.2f, 0 });
		}


		//Muzzle point
		Vector3 mPos = Vector3Add(weaponSwayOffset, camPos);
		mPos = Vector3Add(mPos, (Vector3) { 0, -0.1, 0 });
		mPos = Vector3Add(mPos, Vector3Scale(Vector3Normalize(forwardDir), 0.2f));
		//offset = Vector3Add(mPos, Vector3Scale(Vector3Normalize(pVector), 0.1f));
		MuzzlePos = mPos;

		//Walk anim
		pistolPos = Vector3Add(weaponSwayOffset, oldCamPos);
		pistolPos = Vector3Add(pistolPos, (Vector3) { 0, -0.1f, 0 });


		//Shoot offset
		float cooldown = player.equippedWeapon->cooldownDuration;
		if (cooldown == 0)
			cooldown = 1;

		float atkOffset = Clamp((lastAttackTime - GetTime()) / cooldown, 0.0f, 1.0f);
		Vector3 weaponKickVector = Vector3Scale(Vector3Normalize(forwardDir), atkOffset * -0.2f);
		Vector3 weaponUpKickVector = Vector3Scale((Vector3) { 0, 1, 0 }, atkOffset * 0.03f);
		pistolPos = Vector3Add(pistolPos, weaponKickVector);
		pistolPos = Vector3Add(pistolPos, weaponUpKickVector);

		float gunAngleY = -angle;

		//Main gun cube
		Vector3 boxPos = Vector3Add(pistolPos, Vector3Scale(Vector3Normalize(forwardDir), -0.05f));
		DrawCubeRotatedAngle(gunTex, boxPos, 0.02f, 0.02f, 0.35f, 0, gunAngleY, 0, (Color) { 48, 37, 26, 255 });

		//Forward recticle
		Vector3 recticlePos = Vector3Add(pistolPos, Vector3Scale(Vector3Normalize(forwardDir), 0.0f));
		DrawCubeRotatedAngle(gunTex, Vector3Add(recticlePos, (Vector3) { 0, 0.015f, 0 }), 0.01f, 0.005f, 0.01f, 0, gunAngleY, 0, (Color) { 50, 45, 50, 255 });


		//Bottom half
		Vector3 handlePos = Vector3Add(pistolPos, Vector3Scale(Vector3Normalize(forwardDir), -0.05f));
		DrawCubeRotatedAngle(gunTex, Vector3Add(handlePos, (Vector3) { 0, -0.015f, 0 }), 0.02f, 0.015f, 0.2f, 0, gunAngleY, 0, (Color) { 40, 40, 40, 255 });


		//Create pistol bullet
		if (CreateBullet)
		{
			Vector3 bulletExitPoint = Vector3Add(pistolPos, Vector3Scale(Vector3Normalize(pVector), 0.07f));
			bulletExitPoint = Vector3Add(bulletExitPoint, Vector3Scale(Vector3Normalize(forwardDir), -0.00f));
			bulletExitPoint = Vector3Add(bulletExitPoint, (Vector3) { 0, -0.05f, 0 });

			bullets[bulletCreateIndex].position = bulletExitPoint;
			bullets[bulletCreateIndex].dir = forwardDir;
			bullets[bulletCreateIndex].speed = 0.4f;
			bullets[bulletCreateIndex].Active = true;
			bullets[bulletCreateIndex].timeOutTime = GetTime() + 10;
			bullets[bulletCreateIndex].damage = round((float)player.equippedWeapon->Damage * player.ExtraDamage);
			bullets[bulletCreateIndex].PunchThrough = true;

			CreateBullet = false;
			muzzleActiveTime = 0.03f;



			PushCamera(Vector3Scale(forwardDir, -0.5f));


			CreateGunParticle(bulletExitPoint, forwardDir, 15);
			CreateGunSmokeParticle(bulletExitPoint, forwardDir, 8);



			Vector3 shellPos = Vector3Add(pistolPos, Vector3Scale(Vector3Normalize(forwardDir), 0.2f));
			shellPos = Vector3Add(pistolPos, Vector3Scale(Vector3Normalize(pVector), -0.07f));
			CreateGunShellParticle(shellPos, forwardDir, 1);
		}
	}

	if (GetPlayer().equippedWeapon->type == Shotgun)
	{
		Vector3 pistolPos;

		Vector3 offset = oldCamPos;
		offset = Vector3Add(offset, Vector3Scale(Vector3Normalize(forwardDir), 0.4f));
		offset = Vector3Add(offset, Vector3Scale(Vector3Normalize(pVector), 0.13f));
		offset = Vector3Subtract(offset, oldCamPos);


		float walkAnim = GetCamSwingY();
		offset.y += sin(walkAnim) * 0.02f;

		offset.y += switchingGunOffset.y;


		weaponSwayOffset = Vector3Lerp(weaponSwayOffset, offset, GetFrameTime() * 12);

		if (firstDraw)
		{
			weaponSwayOffset = Vector3Add(offset, (Vector3) { 0, 0.2f, 0 });
		}


		//Muzzle point
		Vector3 mPos = Vector3Add(weaponSwayOffset, camPos);
		mPos = Vector3Add(mPos, (Vector3) { 0, -0.1, 0 });
		mPos = Vector3Add(mPos, Vector3Scale(Vector3Normalize(forwardDir), 0.2f));
		//offset = Vector3Add(mPos, Vector3Scale(Vector3Normalize(pVector), 0.1f));
		MuzzlePos = mPos;

		//Walk anim
		pistolPos = Vector3Add(weaponSwayOffset, oldCamPos);
		pistolPos = Vector3Add(pistolPos, (Vector3) { 0, -0.1f, 0 });


		//Shoot offset
		float cooldown = player.equippedWeapon->cooldownDuration;
		if (cooldown == 0)
			cooldown = 1;

		float atkOffset = Clamp((lastAttackTime - GetTime()) / cooldown, 0.0f, 1.0f);
		Vector3 weaponKickVector = Vector3Scale(Vector3Normalize(forwardDir), atkOffset * -0.2f);
		Vector3 weaponUpKickVector = Vector3Scale((Vector3) { 0, 1, 0 }, atkOffset * 0.03f);
		pistolPos = Vector3Add(pistolPos, weaponKickVector);
		pistolPos = Vector3Add(pistolPos, weaponUpKickVector);

		float gunAngleY = -angle;

		//Main gun cube
		Vector3 boxPos = Vector3Add(pistolPos, Vector3Scale(Vector3Normalize(forwardDir), -0.2f));
		boxPos = Vector3Add(pistolPos, Vector3Scale(Vector3Normalize(pVector), -0.011f));
		DrawCubeRotatedAngle(gunTex, boxPos, 0.02f, 0.02f, 0.45f, 0, gunAngleY, 0, (Color) { 38, 27, 26, 255 });

		Vector3 boxPos2 = Vector3Add(pistolPos, Vector3Scale(Vector3Normalize(forwardDir), -0.2f));
		boxPos2 = Vector3Add(pistolPos, Vector3Scale(Vector3Normalize(pVector), 0.011f));
		DrawCubeRotatedAngle(gunTex, boxPos2, 0.02f, 0.02f, 0.45f, 0, gunAngleY, 0, (Color) { 38, 27, 26, 255 });

		//Forward recticle
		/*Vector3 recticlePos = Vector3Add(pistolPos, Vector3Scale(Vector3Normalize(forwardDir), 0.0f));
		DrawCubeRotatedAngle(gunTex, Vector3Add(recticlePos, (Vector3) { 0, 0.015f, 0 }), 0.01f, 0.005f, 0.01f, 0, gunAngleY, 0, (Color) { 50, 45, 50, 255 });
		*/

		//Bottom half
		Vector3 handlePos = Vector3Add(pistolPos, Vector3Scale(Vector3Normalize(forwardDir), -0.10f));
		DrawCubeRotatedAngle(gunTex, Vector3Add(handlePos, (Vector3) { 0, -0.015f, 0 }), 0.05f, 0.015f, 0.2f, 0, gunAngleY, 0, (Color) { 30, 30, 30, 255 });


		//Create bullet
		if (CreateBullet)
		{
			Vector3 bulletExitPoint = Vector3Add(pistolPos, Vector3Scale(Vector3Normalize(pVector), 0.07f));
			bulletExitPoint = Vector3Add(bulletExitPoint, Vector3Scale(Vector3Normalize(forwardDir), -0.00f));
			bulletExitPoint = Vector3Add(bulletExitPoint, (Vector3) { 0, -0.05f, 0 });

			for (int i = -6; i < 6; i++)
			{



				Vector3 splitVec = Vector3Scale(Vector3Normalize(pVector), i * 0.015F);
				splitVec.y = GetRandomValue(-100, 100) * 0.00015f;



				bullets[bulletCreateIndex].position = bulletExitPoint;
				bullets[bulletCreateIndex].dir = Vector3Add(forwardDir, splitVec);;
				bullets[bulletCreateIndex].speed = 0.4f - (GetRandomValue(0,100) * 0.002f);
				bullets[bulletCreateIndex].Active = true;
				bullets[bulletCreateIndex].timeOutTime = GetTime() + 10;
				bullets[bulletCreateIndex].damage = round((float)player.equippedWeapon->Damage * player.ExtraDamage);

				bulletCreateIndex = (bulletCreateIndex + 1) % 1000;

				CreateBullet = false;
				muzzleActiveTime = 0.03f;


			}


			PushCamera(Vector3Scale(forwardDir, -0.8f));


			CreateGunParticle(bulletExitPoint, forwardDir, 20);
			CreateGunSmokeParticle(bulletExitPoint, forwardDir, 30);



			Vector3 shellPos = Vector3Add(pistolPos, Vector3Scale(Vector3Normalize(forwardDir), 0.2f));
			shellPos = Vector3Add(pistolPos, Vector3Scale(Vector3Normalize(pVector), -0.07f + (GetRandomValue(-100, 100) * 0.0001f)));
			CreateGunShellParticle(shellPos, forwardDir, 1);
		}
	}


	oldCamPos = camPos;
}


void DrawMuzzle()
{
	//Draw muzzle
	if (muzzleActiveTime > 0)
	{
		muzzleActiveTime -= GetFrameTime();
		Vector2 muzzlePos = GetWorldToScreen(MuzzlePos, *GetCameraRef());


		DrawTexture(MuzzleTex, muzzlePos.x - (MuzzleTex.width/2), muzzlePos.y - (MuzzleTex.height / 2), WHITE);
	}
}

void ProcessBullets()
{
	for (int i = 0; i < 1000; i++)
	{
		if (bullets[i].Active)
		{
			Vector3 forward = bullets[i].dir;

			float angle = (float)(atan2(bullets[i].dir.x, -bullets[i].dir.z))* RAD2DEG;

			forward = Vector3Scale(forward, bullets[i].speed);

			bullets[i].position = Vector3Add(bullets[i].position,forward);

			//DrawCubeRotatedAngle(BulletTexture, bullets[i].position, 0.4f, 0.4f, 0.4f, 0, angle, 0, WHITE);
			DrawCubeRotatedAngle(BulletTexture,bullets[i].position, 0.05f, 0.05f, 0.2f,0, angle,0, WHITE);

			if (GetTime() > bullets[i].timeOutTime)
			{
				bullets[i].Active = false;
			}

			//Destroy if out of bounds

			if (bullets[i].position.z > 0 || bullets[i].position.z < -199 || bullets[i].position.x < 0 || bullets[i].position.x > 199)
				bullets[i].Active = false;

			//Collision
			bool BulletHit = false;
			struct MapTile closestTile = GetClosestGrid(bullets[i].position.x, bullets[i].position.z);
			if (closestTile.entityCount > 0 && bullets[i].position.y < 1.1f)
			{
				Vector3 invertedDir = Vector3Negate(bullets[i].dir);
				//CreateBloodParticle(bullets[i].position, invertedDir, GetRandomValue(10,30));

				if(bullets[i].PunchThrough == false)
					BulletHit = true;

				bool zombieDied = HurtZombie(closestTile.zombieInThisTileRef, bullets[i].damage);
				if (zombieDied)
				{
					//Can't get zombie position for some reason (Returns empty vector3)
					CreateZombieDieParticle(bullets[i].position, invertedDir, bullets[i].position);
				}
				else
				{
					CreateBloodParticle(bullets[i].position, invertedDir, 20);
				}
			}

			if (closestTile.genType == 2 || closestTile.genType == 4)
			{
				BulletHit = true;

				CreateBulletWallParticle(bullets[i].position, Vector3Negate(bullets[i].dir), 3);
			}



			if (BulletHit)
			{
				bullets[i].PunchThrough = false;
				bullets[i].Active = false;
			}
		}
	}


}


void DrawWeaponUI()
{
	//Draw UI

	struct MainPlayer p = GetPlayer();

	if (p.equippedWeapon->type == Pistol)
		DrawText("Pistol", 380, 360, 15, WHITE);


	if (p.equippedWeapon->type == MachineGun)
		DrawText("Machine Gun", 354, 360, 15, WHITE);

	if (p.equippedWeapon->type == Rifle)
		DrawText("Rifle", 377, 360, 15, WHITE);

	if (p.equippedWeapon->type == Shotgun)
		DrawText("Shotgun", 365, 360, 15, WHITE);
	

	//Draw ammo if not pistol
	if (p.equippedWeapon->type != Pistol)
	{
		DrawText(TextFormat("Ammo:%02i", p.equippedWeapon->bullet), 362, 390, 15, WHITE);
	}
	else
	{
		DrawText("Ammo:Infinite", 355, 390, 15, WHITE);
	}


	//Draw Gun keys

	int KeyStartX = 365;

	Color c = WHITE;
	for (int i = 0; i < 4; i++)
	{
		c = WHITE;
		if (p.AllWeapons[i].bullet <= 0)
			c = DARKGRAY;

		char snum[5];
		itoa((i + 1), snum, 10);
		DrawText(snum, KeyStartX + ((i) * 20), 420, 10, c);
	}
}