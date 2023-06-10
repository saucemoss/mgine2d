#pragma once
#include "Animations.h"
#include "Entity.h"
#include "Settings.h"
class AnimatedDecor : public Animated, public Entity
{
public:
	AnimatedDecor(Rectangle rect, int tile);
	~AnimatedDecor();
	// Inherited via Animated
	virtual void InitAnimations() override;

	// Inherited via Entity
	virtual void Draw() override;
	virtual void Update(float dt) override;
private:
	std::string m_texture_name;
	bool m_is_right = true;
	int m_tile;
	Rectangle m_rectangle;

	//shader test
	Shader shdrOutline;

	float outlineSize = 3.0f;
	float outlineColor[4] = { 1.0f, 0.0f, 0.0f, 1.0f };      // Normalized RED color 
	float textureSize[2] = { settings::screenWidth,settings::screenWidth };

	int outlineSizeLoc;
	int outlineColorLoc;
	int textureSizeLoc;

	RenderTexture2D RenderShaderTexture;
	Texture2D ShaderTexture;

	//Shader bloom;

};