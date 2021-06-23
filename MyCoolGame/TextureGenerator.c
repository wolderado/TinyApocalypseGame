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



void AlphaCut(Image* image, int threshold)
{
	Color* pixels = LoadImageColors(*image);

	if (pixels != NULL)
	{
		for (int y = 0; y < image->height; y++)
		{
			for (int x = 0; x < image->width; x++)
			{
				if (pixels[y * image->width + x].r < threshold)
				{
					pixels[y * image->width + x].r = 255;
					pixels[y * image->width + x].g = 0;
					pixels[y * image->width + x].b = 208;
					pixels[y * image->width + x].a = 0;
				}
			}
		}

		UnloadImageColors(pixels);
	}
}


struct Texture GenerateGrassTexture(int x,int y)
{
	Image im = GenImagePerlinNoise(16, 16, x * 16, y * 16, 1.0F);

	ImageColorContrast(&im, -30);

	Color grassColor = { 76,138,92,255 };
	ImageColorTint(&im, grassColor);

	//Pebbles
	if (GetRandomValue(0, 4) == 0)
	{
		Color pebbleColor = { 100,100,100,255 };
		int pebbleSize = GetRandomValue(1, 4);
		Image newImg = GenImageGradientRadial(16, 16, 0.7F, pebbleColor,BLANK);
		ImageColorContrast(&newImg, -50);
		ImageDraw(&im, newImg, (Rectangle) { .x = 0, .y = 0, .width = newImg.width, .height = newImg.height }, (Rectangle) { .x = GetRandomValue(pebbleSize, (im.width - 1) - pebbleSize), .y = GetRandomValue(pebbleSize, (im.height - 1) - pebbleSize), .width = pebbleSize, .height = pebbleSize }, WHITE);
		UnloadImage(newImg);
	}


	//Random dirt
	if (GetRandomValue(0, 8) == 0)
	{
		Color c = { 82,74,62,255};
		int size = 16;
		Image newImg = GenImageWhiteNoise(size, size, 0.05f);
		ImageColorReplace(&newImg, BLACK, BLANK);
		ImageColorTint(&newImg,c);
		ImageDraw(&im, newImg, (Rectangle) { .x = 0, .y = 0, .width = newImg.width, .height = newImg.height }, (Rectangle) { .x = 0, .y = 0, .width = size, .height = size }, WHITE);
		UnloadImage(newImg);
	}



	Texture result = LoadTextureFromImage(im);
	UnloadImage(im);

	return result;
}




struct Texture GenerateConcreteTexture(int x, int y)
{
	Image im = GenImageSmoothWhiteNoise(16, 16, 0.5f);
	ImageColorContrast(&im, -80);

	//Random dirt
	if (GetRandomValue(0, 12) == 0)
	{
		Color c = { 82,77,73,255 };
		int size = 16;
		Image newImg = GenImageWhiteNoise(size, size, 0.05f);
		ImageColorReplace(&newImg, BLACK, BLANK);
		ImageColorTint(&newImg, c); ImageDraw(&im, newImg, (Rectangle) { .x = 0, .y = 0, .width = newImg.width, .height = newImg.height }, (Rectangle) { .x = 0, .y = 0, .width = size, .height = size }, WHITE);
	}


	//Random checker
	if (GetRandomValue(0, 18) == 0)
	{
		Image newImg = GenImageChecked(16, 16, 1, 1, WHITE, BLACK);
		ImageColorContrast(&newImg, -80);

		ImageDraw(&im, newImg, (Rectangle) { .x = 0, .y = 0, .width = newImg.width, .height = newImg.height }, (Rectangle) { .x = 0, .y = 0, .width = 16, .height = 16 }, WHITE);
	}
	
	//Random thrash
	if (GetRandomValue(0, 18) == 0)
	{
		Image newImg = GenImageColor(16, 16, BLACK);

		int rectCount = GetRandomValue(1, 16);
		for (size_t i = 0; i < rectCount; i++)
		{
			int sizeX = GetRandomValue(1, 6);
			int sizeY = GetRandomValue(1, 6);
			int rndX = GetRandomValue(sizeX, (newImg.width - 1) - sizeX);
			int rndY = GetRandomValue(sizeY, (newImg.height - 1) - sizeY);

			//RND tint
			Color rndC = BLACK; 
			if (GetRandomValue(0, 4) == 0)
			{
				//Colored
				rndC.r = GetRandomValue(100, 120);
				rndC.g = GetRandomValue(100, 120);
				rndC.b = GetRandomValue(100, 120);
				rndC.a = 255;
			}
			else
			{
				//Grayscale
				int val = GetRandomValue(100, 150);
				rndC.r = val;
				rndC.g = val;
				rndC.b = val;
				rndC.a = 255;
			}

			//Shape
			if (GetRandomValue(0, 4) == 0)
			{
				ImageDrawRectangle(&newImg, rndX, rndY, sizeX, sizeY, rndC);
			}
			else
			{
				ImageDrawRectangleLines(&newImg, (Rectangle) { rndX, rndY, sizeX, sizeY }, 1, rndC);
			}
		}

		ImageColorReplace(&newImg, BLACK, BLANK);






		ImageDraw(&im, newImg, (Rectangle) { .x = 0, .y = 0, .width = newImg.width, .height = newImg.height }, (Rectangle) { .x = 0, .y = 0, .width = 16, .height = 16 }, WHITE);
		UnloadImage(newImg);
	}




	Texture result = LoadTextureFromImage(im);
	UnloadImage(im);
	return result;
}





struct Texture GenerateWallTexture(int x, int y)
{
	Image im = GenImageSmoothWhiteNoise(16, 16, 0.5f);
	ImageColorContrast(&im, -80);

	//Base bumps
	int cellSize = GetRandomValue(3, 5);
	Image cells = GenImageCellular(32, 32, cellSize);
	ImageColorInvert(&cells);
	ImageColorContrast(&cells, -80);
	ImageDraw(&im, cells, (Rectangle) { .x = GetRandomValue(0,cells.width/2), .y = GetRandomValue(0, cells.height / 2), .width = cells.width/2, .height = cells.height/2 }, (Rectangle) { .x = 0, .y = 0, .width = 16, .height = 16 }, WHITE);

	int rndNoise = GetRandomValue(0, 25);
	for (int i = 0; i < rndNoise; i++)
	{
		int val = GetRandomValue(120, 130);
		Color c = (Color){val,val,val,255};

		ImageDrawPixel(&im, GetRandomValue(0, 15), GetRandomValue(0, 15), c);
	}



	//Bolts
	Image newImg = GenImageColor(16, 16, BLACK);
	ImageDrawRectangle(&newImg, 1, 1, 1, 1, DARKGRAY);
	ImageDrawRectangle(&newImg, 14, 1, 1, 1, DARKGRAY);
	ImageDrawRectangle(&newImg, 1, 14, 1, 1, DARKGRAY);
	ImageDrawRectangle(&newImg, 14, 14, 1, 1, DARKGRAY);

	ImageColorReplace(&newImg, BLACK, BLANK);
	ImageDraw(&im, newImg, (Rectangle) { .x = 0, .y = 0, .width = newImg.width, .height = newImg.height }, (Rectangle) { .x = 0, .y = 0, .width = 16, .height = 16 }, WHITE);

	UnloadImage(newImg);
	Texture result = LoadTextureFromImage(im);
	UnloadImage(im);

	return result;
}



struct Texture GenerateWoodTexture(int x, int y)
{
	Image im = GenImageSmoothWhiteNoise(16, 16, 0.5f);
	ImageColorContrast(&im, -65);


	int lineCount = GetRandomValue(4, 14);
	int minX = 0;
	for (int i = 0; i < lineCount; i++)
	{
		int maxX = Clamp(minX + 4, 0, 16);
		int x = GetRandomValue(minX, maxX);
		for (int y = 0; y < 16; y++)
		{
			if (GetRandomValue(0, 10) == 0)
			{
				if (GetRandomValue(0, 1) == 0)
					x++;
				else
					x--;
			}

			x = Clamp(x, 0, 15);
			ImageDrawPixel(&im, x, y, (Color) { 105, 105, 105, 255 });

			if(x > minX)
				minX = x;
		}
	}


	ImageDrawLine(&im, 0, 0, 16, 0, DARKGRAY);
	ImageDrawLine(&im, 0, 1, 16, 1, GRAY);

	Image newImg = GenImageColor(16, 16, BLACK);
	ImageDrawRectangle(&newImg, 13, 2, 2, 2, WHITE);
	ImageDrawRectangle(&newImg, 13, 13, 2, 2, WHITE);
	ImageColorReplace(&newImg, BLACK, BLANK);
	ImageDraw(&im, newImg, (Rectangle) { .x = 0, .y = 0, .width = newImg.width, .height = newImg.height }, (Rectangle) { .x = 0, .y = 0, .width = 15, .height = 15 }, WHITE);

	UnloadImage(newImg);
	Texture result = LoadTextureFromImage(im);
	UnloadImage(im);

	return result;
}





struct Texture GenerateRoadTexture(int x, int y)
{
	Image im = GenImageSmoothWhiteNoise(16, 16, 0.5f);
	ImageColorContrast(&im, -50);

	//Random dirt
	if (GetRandomValue(0, 25) == 0)
	{
		Color c = { 82,77,73,255 };
		int size = 16;
		Image newImg = GenImageWhiteNoise(size, size, 0.05f);
		ImageColorReplace(&newImg, BLACK, BLANK);
		ImageColorTint(&newImg, c); ImageDraw(&im, newImg, (Rectangle) { .x = 0, .y = 0, .width = newImg.width, .height = newImg.height }, (Rectangle) { .x = 0, .y = 0, .width = size, .height = size }, WHITE);
	}

	//Random thrash
	if (GetRandomValue(0, 40) == 0)
	{
		Image newImg = GenImageColor(16, 16, BLACK);

		int rectCount = GetRandomValue(1, 16);
		for (size_t i = 0; i < rectCount; i++)
		{
			int sizeX = GetRandomValue(1, 6);
			int sizeY = GetRandomValue(1, 6);
			int rndX = GetRandomValue(sizeX, (newImg.width - 1) - sizeX);
			int rndY = GetRandomValue(sizeY, (newImg.height - 1) - sizeY);

			//RND tint
			Color rndC = BLACK;
			if (GetRandomValue(0, 4) == 0)
			{
				//Colored
				rndC.r = GetRandomValue(100, 120);
				rndC.g = GetRandomValue(100, 120);
				rndC.b = GetRandomValue(100, 120);
				rndC.a = 255;
			}
			else
			{
				//Grayscale
				int val = GetRandomValue(100, 150);
				rndC.r = val;
				rndC.g = val;
				rndC.b = val;
				rndC.a = 255;
			}

			//Shape
			if (GetRandomValue(0, 4) == 0)
			{
				ImageDrawRectangle(&newImg, rndX, rndY, sizeX, sizeY, rndC);
			}
			else
			{
				ImageDrawRectangleLines(&newImg, (Rectangle) { rndX, rndY, sizeX, sizeY }, 1, rndC);
			}
		}

		ImageColorReplace(&newImg, BLACK, BLANK);






		ImageDraw(&im, newImg, (Rectangle) { .x = 0, .y = 0, .width = newImg.width, .height = newImg.height }, (Rectangle) { .x = 0, .y = 0, .width = 16, .height = 16 }, WHITE);
		UnloadImage(newImg);
	}



	Texture result = LoadTextureFromImage(im);
	UnloadImage(im);

	return result;
}



//Color zombieColor = { 20,10,10,255 };
Color zombieColor = { 50,100,50,255 };
Color bloodColor = { 40, 5, 5, 255 };
Color bloodColor2 = { 20, 10, 5, 255 };
struct Texture GenerateZombieTexture()
{
	Image im = GenImageSmoothWhiteNoise(16, 16, 0.5f);
	ImageColorContrast(&im, -75);
	ImageColorTint(&im, zombieColor);
	
	int lineCount = GetRandomValue(0, 6);
	int minX = 0;
	for (int i = 0; i < lineCount; i++)
	{
		int x = GetRandomValue(0, 16);
		int yLen = GetRandomValue(3, 16);
		for (int y = 0; y < yLen; y++)
		{
			if (GetRandomValue(0, 10) == 0)
			{
				if (GetRandomValue(0, 1) == 0)
					x++;
				else
					x--;
			}

			x = Clamp(x, 0, 15);
			ImageDrawPixel(&im, x, y, bloodColor);

			if (x > minX)
				minX = x;
		}
	}

	int rectCount = GetRandomValue(1, 4);
	for (int i = 0; i < rectCount; i++)
	{
		int w = GetRandomValue(0, 8);
		int h = GetRandomValue(1, 4);

		Color c;
		LerpColor(bloodColor, bloodColor2, GetRandomValue(0, 100) * 0.01f, &c);

		if (GetRandomValue(0, 3) == 0)
			ImageDrawCircle(&im, GetRandomValue(0, 16 - w), GetRandomValue(0, 16 - w), w, c);

		if (GetRandomValue(0, 1) == 0)
			ImageDrawRectangle(&im, GetRandomValue(0, 16 - w), GetRandomValue(0, 16 - h), w, h, c);
	}

	int randomBloodDotCount = GetRandomValue(1, 40);
	for (int i = 0; i < randomBloodDotCount; i++)
	{
		Color c;
		LerpColor(bloodColor, bloodColor2, GetRandomValue(0, 100) * 0.01f, &c);
		int x = GetRandomValue(0, 16);
		int y = GetRandomValue(0, 16);

		ImageDrawPixel(&im, x, y, c);

		int extraLineChance = GetRandomValue(0, 1);
		if (extraLineChance == 0)
		{
			int extraLineCount = GetRandomValue(1, 4);
			for (int b = 0; b < extraLineCount; b++)
			{
				int newY = Clamp(y + b,0,16);
				ImageDrawPixel(&im, x, newY, c);
			}
		}
	}


	//ImageColorBrightness(&im, -20);


	Texture result = LoadTextureFromImage(im);
	UnloadImage(im);

	return result;
}

struct Texture GenerateZombieBlankTexture()
{
	Image im = GenImageSmoothWhiteNoise(16, 16, 0.5f);
	ImageColorContrast(&im, -75);
	//ImageColorBrightness(&im, -20);
	ImageColorTint(&im, zombieColor);

	Texture result = LoadTextureFromImage(im);
	UnloadImage(im);

	return result;
}



struct Texture GenerateZombieHeadTexture()
{
	Image im = GenImageSmoothWhiteNoise(16, 16, 0.5f);
	ImageColorContrast(&im, -85);
	//ImageColorBrightness(&im, -20);
	ImageColorTint(&im, zombieColor);

	//Draw Eyes
	Color EyeColor = (Color){ 150,20,20,255 };
	ImageDrawRectangle(&im, 10, 8, 5, 2, EyeColor);
	ImageDrawRectangle(&im, 1, 8, 5, 2, EyeColor);

	//Mouth
	Color mouthColor = (Color){ 15,5,5,255 };

	int rnd = GetRandomValue(0, 4);
	if (rnd == 0)
	{
		//:|
		ImageDrawPixel(&im, 6, 3, mouthColor);
		ImageDrawPixel(&im, 7, 3, mouthColor);
		ImageDrawPixel(&im, 8, 3, mouthColor);
		ImageDrawPixel(&im, 9, 3, mouthColor); 
	}


	if (rnd == 1)
	{
		//:O
		ImageDrawRectangle(&im, 6, 2, 4, 4, mouthColor);
	}

	if (rnd == 2)
	{
		//:|_

		ImageDrawPixel(&im, 4, 3, mouthColor);
		ImageDrawPixel(&im, 5, 3, mouthColor);
		ImageDrawPixel(&im, 6, 3, mouthColor);
		ImageDrawPixel(&im, 7, 3, mouthColor);
		ImageDrawPixel(&im, 8, 3, mouthColor);
		ImageDrawPixel(&im, 9, 3, mouthColor);
		ImageDrawPixel(&im, 4, 2, mouthColor);
		ImageDrawPixel(&im, 5, 2, mouthColor);
		ImageDrawPixel(&im, 4, 1, mouthColor);
	}



	Texture result = LoadTextureFromImage(im);
	UnloadImage(im);

	return result;
}


struct Texture GeneratePistolTexture()
{
	Image im = GenImageSmoothWhiteNoise(16, 16, 0.5f);
	ImageColorContrast(&im, -85);
	//ImageColorBrightness(&im, -20);
	//ImageColorTint(&im, zombieColor);

	UnloadImage(im);
	return LoadTextureFromImage(im);
}


struct Texture GenerateBulletTexture()
{
	Image im = GenImageSmoothWhiteNoise(4, 4, 0.5f);
	ImageColorContrast(&im, -75);
	ImageColorBrightness(&im, -90);
	//ImageColorTint(&im, zombieColor);

	UnloadImage(im);

	return LoadTextureFromImage(im);;
}


struct Texture GenerateAmmoBoxTexture(Color c)
{
	Image im = GenImageSmoothWhiteNoise(16, 16, 0.5f);
	ImageColorContrast(&im, -120);


	ImageDrawRectangle(&im, 3, 4, 3, 8, c);
	ImageDrawRectangle(&im, 7, 4, 3, 8, c);
	ImageDrawRectangle(&im, 11, 4, 3, 8, c);

	//ImageColorTint(&im,c);

	Texture result = LoadTextureFromImage(im);
	UnloadImage(im);

	return result;
}

struct Texture GenerateHealthBoxTexture()
{
	Image im = GenImageSmoothWhiteNoise(16, 16, 0.5f);
	ImageColorContrast(&im, -120);


	ImageDrawRectangle(&im, 6, 2, 3, 12, (Color) {117,186,86,255});
	ImageDrawRectangle(&im, 2, 6, 12, 3, (Color) { 117, 186, 86, 255 });

	//ImageColorTint(&im,c);

	Texture result = LoadTextureFromImage(im);
	UnloadImage(im);

	return result;
}

struct Texture GenerateScrapBoxTexture()
{
	Image im = GenImageSmoothWhiteNoise(16, 16, 0.5f);
	ImageColorContrast(&im, -60);

	ImageDrawText(&im, "S", 5, 2, 2, GRAY);
	ImageDrawText(&im, "S", 5, 3, 2, WHITE);

	Texture result = LoadTextureFromImage(im);
	UnloadImage(im);

	return result;
}


struct Texture GenerateBlankTexture(Color c)
{
	Image im = GenImageColor(16, 16, c);
	Texture result = LoadTextureFromImage(im);
	UnloadImage(im);

	return result;
}


struct Texture GenerateExitTexture()
{
	Image im = GenImageColor(64, 64, (Color) { 50, 70, 50, 255 });

	//ImageDrawTextEx(&im, GetFontDefault(), "E", (Vector2) { 18, 32 }, 8, 2, WHITE);
	ImageDrawText(&im, "E X I T", 18, 25, 6, GRAY);

	ImageFlipVertical(&im);

	Texture result = LoadTextureFromImage(im);
	UnloadImage(im);

	return result;
}