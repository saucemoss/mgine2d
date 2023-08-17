#pragma once
#include <raylib.h>
#include <string>
#include "Settings.h"

#if defined(PLATFORM_DESKTOP)
#define GLSL_VERSION            330
#else   // PLATFORM_RPI, PLATFORM_ANDROID, PLATFORM_WEB
#define GLSL_VERSION            330
#endif

class Shaders 
{
public:

	Shaders();

    void ApplyOutline(Texture2D sprite, Rectangle source, Rectangle dest, Vector2 origin, float rotation);
	void ApplyPerlin();
	void ResetShaders();
    void DrawOutlines();
	void DrawPerlin();
	void Pixelize();


	RenderTexture2D RenderOutlineShaderTexture;
	Texture2D OutlineShaderTexture;

    Shader shdrOutline;
	float outlineSize = 1.0f;
	float outlineColor[4] = { 0.0f, 0.90f, 0.15f, 1.0f };      // Normalized GREEN color 
	float textureSize[2] = { settings::screenWidth,settings::screenWidth };
    int outlineSizeLoc;
    int outlineColorLoc;
    int textureSizeLoc;

	RenderTexture2D RenderPerlinTexture;
	Texture2D PerlinTexture;
	Shader perlinShader;
	float scale = 1.0f;
	float offset[2] = { 0,0 };
	float time;
	int scaleLoc;
	int offsetLoc;
	int timeLoc;

	Shader pixelizer;

};