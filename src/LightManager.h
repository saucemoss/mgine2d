#pragma once
#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"
#include <vector>
#include <LDtkLoader/Layer.hpp>

// constants from OpenGL
#define GL_SRC_ALPHA 0x0302
#define GL_MIN 0x8007
#define GL_MAX 0x8008

class LightInfo
{
public:
	Vector2 Position = { 0,0 };
	RenderTexture ShadowMask;
	RenderTexture GlowTexture;
	bool Valid = false;
	bool HasColor = false;
	Color LightColor = WHITE;

	LightInfo();

	LightInfo(const Vector2& pos);

	void Move(const Vector2& position);

	void SetInRadius(float InnerRadius);

	void SetOutRadius(float outerRadius);

	void SetColor(Color color);

	bool BoxInLight(const Rectangle& box);

	void ShadowEdge(const Vector2& sp, const Vector2& ep);

	void UpdateLightMask();

	void Update(std::vector<Rectangle*>& boxes);

	void DrawLightGradient(int centerX, int centerY, float innerRadius, float outterRadius, Color color1, Color color2);

	float OuterRadius = 200;
	float InnerRadius = 50;
	Rectangle Bounds = { -150,-150,300,300 };

	std::vector<std::vector<Vector2>> Shadows;
	bool Dirty = true;
};


class LightManager
{
public:
	LightManager();

	std::vector<LightInfo> m_lights;
	std::vector<Rectangle*> m_light_walls;
	static ldtk::Level* m_level;
	RenderTexture LightMask;
	bool showLines = false;

	void SetupLight(float x, float y, float in_radius, float out_radius, Color c, bool is_color);

	void SetupBoxes();

	void UpdateLights();

	void DrawLightMask();


};

