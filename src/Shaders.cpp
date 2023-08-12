#include "Shaders.h"
#include "Settings.h"

Shaders::Shaders()
{

	shdrOutline = LoadShader(0, TextFormat("res/shaders/glsl%i/outline2.fs", GLSL_VERSION));
	// Get shader locations
	int outlineSizeLoc = GetShaderLocation(shdrOutline, "outlineSize");
	int outlineColorLoc = GetShaderLocation(shdrOutline, "outlineColor");
	int textureSizeLoc = GetShaderLocation(shdrOutline, "textureSize");
	// Set shader values (they can be changed later)
	SetShaderValue(shdrOutline, outlineSizeLoc, &outlineSize, SHADER_UNIFORM_FLOAT);
	SetShaderValue(shdrOutline, outlineColorLoc, outlineColor, SHADER_UNIFORM_VEC4);
	SetShaderValue(shdrOutline, textureSizeLoc, textureSize, SHADER_UNIFORM_VEC2);

	RenderOutlineShaderTexture = LoadRenderTexture(settings::screenWidth, settings::screenHeight);
	OutlineShaderTexture = RenderOutlineShaderTexture.texture;


	perlinShader = LoadShader(0, TextFormat("res/shaders/glsl%i/perlin.fs", GLSL_VERSION));
	scaleLoc = GetShaderLocation(perlinShader, "scale");
	offsetLoc = GetShaderLocation(perlinShader, "offset");
	timeLoc = GetShaderLocation(perlinShader, "u_time");
	SetShaderValue(perlinShader, timeLoc, &time, SHADER_UNIFORM_FLOAT);
	SetShaderValue(perlinShader, scaleLoc, &scale, SHADER_UNIFORM_FLOAT);
	SetShaderValue(perlinShader, offsetLoc, offset, SHADER_UNIFORM_VEC2);
	
}


void Shaders::ApplyOutline(Texture2D sprite, Rectangle source, Rectangle dest, Vector2 origin, float rotation)
{

	BeginTextureMode(RenderOutlineShaderTexture);
	DrawTexturePro(sprite,
		source,
		dest,
		origin,
		rotation,
		WHITE);
	EndTextureMode();
	OutlineShaderTexture = RenderOutlineShaderTexture.texture;
}

void Shaders::ApplyPerlin()
{
	offset[0] += 0.0002f;
	SetShaderValue(perlinShader, offsetLoc, offset, SHADER_UNIFORM_VEC2);
	BeginTextureMode(RenderPerlinTexture);

	BeginShaderMode(perlinShader);
	DrawTexturePro(RenderPerlinTexture.texture,
		{ 0, 0, (float)PerlinTexture.width, -(float)PerlinTexture.height },
		{ 0, 0, (float)PerlinTexture.width, (float)PerlinTexture.height },
		{ 0, 0 }, 0.0f, WHITE);
	EndShaderMode();
	EndTextureMode();
	PerlinTexture = RenderPerlinTexture.texture;
}

void Shaders::ResetShaders()
{
	BeginTextureMode(RenderOutlineShaderTexture);
	ClearBackground(BLANK);
	EndTextureMode();
}

void Shaders::DrawOutlines()
{
	BeginShaderMode(shdrOutline);
	DrawTexturePro(OutlineShaderTexture,
		{ 0, 0, (float)OutlineShaderTexture.width, -(float)OutlineShaderTexture.height },
		{ 0, 0, settings::screenWidth, settings::screenHeight },
		{ 0, 0 }, 0, WHITE);
	EndShaderMode();
}

void Shaders::DrawPerlin()
{
	DrawTexturePro(PerlinTexture,
		{ 0, 0, (float)PerlinTexture.width, -(float)PerlinTexture.height },
		{ 0, 0, (float)PerlinTexture.width, (float)PerlinTexture.height },
		{ 0, 0 }, 0.0f, Fade(WHITE, 0.4f));
}
