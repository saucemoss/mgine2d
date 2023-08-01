#include "AnimatedDecor.h"


AnimatedDecor::AnimatedDecor(Rectangle rect, int tile)
	:
	m_tile(tile),
	m_rectangle(rect)
{
	InitAnimations();
	EnitityManager::Add(this);
	

	//Shader test
	bloom = LoadShader(0, TextFormat("res/shaders/glsl%i/bloom.fs", 330));
	shdrOutline = LoadShader(0, TextFormat("res/shaders/glsl%i/outline.fs", 330));
	
	// Get shader locations
	outlineSizeLoc = GetShaderLocation(shdrOutline, "outlineSize");
	outlineColorLoc = GetShaderLocation(shdrOutline, "outlineColor");
	textureSizeLoc = GetShaderLocation(shdrOutline, "textureSize");

	// Set shader values (they can be changed later)
	SetShaderValue(shdrOutline, outlineSizeLoc, &outlineSize, SHADER_UNIFORM_FLOAT);
	SetShaderValue(shdrOutline, outlineColorLoc, outlineColor, SHADER_UNIFORM_VEC4);
	SetShaderValue(shdrOutline, textureSizeLoc, textureSize, SHADER_UNIFORM_VEC2);
	//RenderShaderTexture = LoadRenderTexture(settings::screenWidth, settings::screenHeight);
}

AnimatedDecor::~AnimatedDecor()
{
	EnitityManager::Remove(this);
}

void AnimatedDecor::InitAnimations()
{
	sprite = TextureLoader::GetTexture("DECOR_ANIM");
	animations->InitializeDecorAnimations();
	SetAnimation("DECOR_" + std::to_string(m_tile));
	
}

void AnimatedDecor::Draw(int l)
{
	auto spritePosX = m_rectangle.x;
	auto spritePosY = m_rectangle.y;

	Rectangle cframe = m_is_right ? CurrentFrame() : Rectangle{ CurrentFrame().x,
																	CurrentFrame().y,
																	CurrentFrame().width * -1,
																	CurrentFrame().height };


	DrawTexturePro(*sprite,
		cframe,
		Rectangle{ spritePosX,spritePosY,settings::tileSize,settings::tileSize },
		{ 0,0 },
		0.0f,
		WHITE);



	//BeginShaderMode(bloom);
	//DrawTexturePro(ShaderTexture,
	//	{ 0, 0, (float)ShaderTexture.width, -(float)ShaderTexture.height },
	//	{ 0, 0, settings::screenWidth, settings::screenHeight },
	//	{ 0, 0 }, 0, WHITE);
	//EndShaderMode();


}

void AnimatedDecor::Update(float dt)
{
	SwitchFrames(dt);


	//	auto spritePosX = m_rectangle.x;
	//	auto spritePosY = m_rectangle.y;

	//	Rectangle cframe = m_is_right ? CurrentFrame() : Rectangle{ CurrentFrame().x,
	//																	CurrentFrame().y,
	//																	CurrentFrame().width * -1,
	//																	CurrentFrame().height };
	//	BeginTextureMode(RenderShaderTexture);
	//	DrawTexturePro(*sprite,
	//		cframe,
	//		Rectangle{ spritePosX,spritePosY,settings::tileSize,settings::tileSize },
	//		{ 0,0 },
	//		0.0f,
	//		WHITE);
	//	EndTextureMode();
	//
	//ShaderTexture = RenderShaderTexture.texture;



}
