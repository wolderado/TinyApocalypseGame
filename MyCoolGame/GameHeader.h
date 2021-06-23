#pragma once

struct Zombie
{
	
	bool spawned;
	bool alerted;
	Vector3 position;
	float angle;
	float MoveSpeed;
	float Health;
	float distToPlayer;
	int textureType;
	double hitCooldown;
	double MoveAnimTimer;
	int strength;
	int type;
};




typedef struct MapTile
{
	bool empty;
	Color col;
	Texture2D texture;
	Texture2D floortexture;
	int genType;
	int x;
	int h;
	int y;
	int entityCount;
	Vector3 middlePoint;
	struct Zombie* zombieInThisTileRef;
};


enum WeaponType
{
	Pistol,
	MachineGun,
	Rifle,
	Shotgun
};


typedef struct Weapon
{
	enum WeaponType type;
	Texture texture;
	float cooldownDuration;
	int bullet;
	bool FastShooter;
	int Noise;
	int Damage;
	

	float lastShotTime;
};


typedef struct MainPlayer
{
	Vector3 position;
	Vector3 direction;
	float RotateSpeedMin;
	float RotateSpeedMax;
	float MouseRotateSpeed;
	float MoveSpeed;
	float angle;

	int Health;
	int MaxHealth;
	float ExtraDamage;
	int AlertDistance;
	int Scrap;


	float WeaponSwitchTimer;
	struct Weapon AllWeapons[4];
	struct Weapon *equippedWeapon;
};


bool GameLoading;
int CityProgressed;
bool InEndScreen;
bool GameStarted;
bool HelicopterStarted;
bool GameFinished;
float HelicopterTimer;


void DrawHealthBar();

struct Texture GenerateGrassTexture(int x, int y);

struct MapTile GetClosestGrid(float x, float y);

struct MapTile* GetClosestGridRef(float x, float y);

struct Texture GenerateConcreteTexture(int x, int y);

struct Texture GenerateRoadTexture(int x, int y);

struct Texture GenerateWallTexture(int x, int y);

struct Texture GenerateWoodTexture(int x, int y);

struct Texture GenerateZombieTexture();

struct Texture GenerateZombieHeadTexture();

struct Texture GenerateZombieBlankTexture();

struct Texture GeneratePistolTexture();

struct Texture GenerateBulletTexture();

struct Texture GenerateAmmoBoxTexture(Color c);

Color CalculateFogColor(Color c, float dist);

struct MainPlayer GetPlayer();

struct MainPlayer* GetPlayerRef();

struct Camera3D* GetCameraRef();

Vector3 GetCameraPosition();

float GetPlayerAngle();

float GetCamSwingY();

float GetCamSwingX();

void DrawWeapon();

void Shoot();

Vector2 GetEmptyTile();

void CreateZombie(Vector2 pos);

void ResetEntityData();

void LerpColor(Color a, Color b, float t, Color* result);

void LerpColorWithAlpha(Color a, Color b, float t, Color* result);

void PushCamera(Vector3 force);

bool HurtZombie(struct Zombie* z, int dmg);

void CreateGunParticle(Vector3 pos, Vector3 dir, int count);

void CreateGunSmokeParticle(Vector3 pos, Vector3 dir, int count);

void CreateBloodParticle(Vector3 pos, Vector3 dir, int count);

void CreateZombieDieParticle(Vector3 pos, Vector3 dir, Vector3 zombiePos);

void CreateGunShellParticle(Vector3 pos, Vector3 dir, int count);

void CreateBulletWallParticle(Vector3 pos, Vector3 dir, int count);

void ProcessAndDrawParticles();

void InitializeParticleSystem();

Vector3 GetNormal(Vector3 dir);

void HurtPlayer(int damage);

void SpawnCollectable(Vector2 position, int type);

void ProcessCollectables();

struct Texture GenerateHealthBoxTexture();

struct Texture GenerateScrapBoxTexture();

struct Texture GenerateBlankTexture(Color c);

void PrintToScreen(char targetText[]);

Color CalculateFogColor(Color c, float dist);

struct Texture GenerateExitTexture();

void OpenEndScreen();

void PrepareMap();

Color CalculateFogColorWithAlpha(Color c, float dist);

void DestroyZombies();

Vector2 GetWaveSpawnTile();