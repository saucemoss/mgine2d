#include "Shaders.h"
#include "Settings.h"

Shaders::Shaders()
{

	shdrOutline = LoadShader(0, TextFormat("res/shaders/glsl%i/outline.fs", GLSL_VERSION));
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

}


void Shaders::Apply(std::string shader_name, Texture2D sprite, Rectangle source, Rectangle dest, Vector2 origin, float rotation)
{

	if (shader_name == "outline")
	{
		//SetShaderValue(shdrOutline, outlineSizeLoc, &outlineSize, SHADER_UNIFORM_FLOAT);
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
}

void Shaders::ResetShaders()
{
		BeginTextureMode(RenderOutlineShaderTexture);
		ClearBackground(BLANK);
		EndTextureMode();
}

void Shaders::Draw()
{

	BeginShaderMode(shdrOutline);
	DrawTexturePro(OutlineShaderTexture,
		{ 0, 0, (float)OutlineShaderTexture.width, -(float)OutlineShaderTexture.height },
		{ 0, 0, settings::screenWidth, settings::screenHeight },
		{ 0, 0 }, 0, WHITE);
	EndShaderMode();

	
	//BeginTextureMode(RenderOutlineShaderTexture);
	//BeginBlendMode(BLEND_SUBTRACT_COLORS);
	//DrawTexturePro(OutlineShaderTexture,
	//	{ 0, 0, (float)OutlineShaderTexture.width, -(float)OutlineShaderTexture.height },
	//	{ 0, 0, settings::screenWidth, settings::screenHeight },
	//	{ 0, 0 }, 0, WHITE);
	//EndBlendMode();
	//EndTextureMode();
	

}
