#pragma once
#include <raylib.h>
#include <string>
#include "Settings.h"

#if defined(PLATFORM_DESKTOP)
#define GLSL_VERSION            330
#else   // PLATFORM_RPI, PLATFORM_ANDROID, PLATFORM_WEB
#define GLSL_VERSION            100
#endif

class Shaders 
{
public:

	Shaders();

    void Apply(std::string shader_name, Texture2D sprite, Rectangle source, Rectangle dest, Vector2 origin, float rotation);
	void ResetShaders();
    void Draw();

	RenderTexture2D RenderOutlineShaderTexture;
	Texture2D OutlineShaderTexture;

    Shader shdrOutline;
	float outlineSize = 1.0f;
	float outlineColor[4] = { 0.0f, 0.90f, 0.15f, 1.0f };      // Normalized GREEN color 
	float textureSize[2] = { settings::screenWidth,settings::screenWidth };

    int outlineSizeLoc;
    int outlineColorLoc;
    int textureSizeLoc;


};