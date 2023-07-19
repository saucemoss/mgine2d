#include "LightManager.h"
#include "LevelManager.h"

LightManager::LightManager()
{
	LightMask = LoadRenderTexture(LevelManager::currentLdtkLevel->size.x, LevelManager::currentLdtkLevel->size.y);
}

void LightManager::SetupBoxes()
{
	
	for (auto& s : LevelManager::solid_tiles)
	{
		m_light_walls.push_back(&s->m_rectangle);
	}
	for (auto& e : LevelManager::level_entities_safe)
	{
		Collidable* c = dynamic_cast<Collidable*>(e.get());
		if (c != nullptr)
		{
			if (c->m_colliderTag == M_BLOCK)
			{
				m_light_walls.push_back(&c->m_rectangle);
			}
		}
	}
	
}

void LightManager::UpdateLights()
{
	if (IsGamepadButtonPressed(0, GAMEPAD_BUTTON_MIDDLE_RIGHT))
		showLines = !showLines;

	bool dirtyLights = false;
	for (auto& light : m_lights)
	{
		if (light.Dirty)
			dirtyLights = true;

		light.Update(m_light_walls);
	}

	// update the light mask
	if (dirtyLights)
	{
		// build up the light mask
		BeginTextureMode(LightMask);
		ClearBackground(BLACK);

		// force the blend mode to only set the alpha of the destination
		rlSetBlendFactors(GL_SRC_ALPHA, GL_SRC_ALPHA, GL_MIN);
		rlSetBlendMode(BLEND_CUSTOM);

		for (auto& light : m_lights)
		{
			//	if (light.Valid)
			DrawTextureRec(light.ShadowMask.texture, Rectangle{ 0, 0, (float)light.ShadowMask.texture.width, -(float)light.ShadowMask.texture.height }, Vector2Zero(), WHITE);
		}

		rlDrawRenderBatchActive();
		// go back to normal
		rlSetBlendMode(BLEND_ALPHA);
		EndTextureMode();
	}
}

void LightManager::DrawLightMask()
{
	rlSetBlendMode(BLEND_ADDITIVE);
	for (auto& light : m_lights)
	{
		if (light.HasColor)
		{
			DrawTextureRec(light.GlowTexture.texture, Rectangle{ 0, 0, (float)light.GlowTexture.texture.width, -(float)light.GlowTexture.texture.height }, Vector2Zero(), WHITE);
		}
			
	}
	rlDrawRenderBatchActive();
	rlSetBlendMode(BLEND_ALPHA);

	DrawTextureRec(LightMask.texture, Rectangle{ 0, 0, (float)LightMask.texture.width, -(float)LightMask.texture.height }, Vector2Zero(), ColorAlpha(WHITE, LevelManager::m_darkness_strength));

	// draw light sources
	//for (auto& light : m_lights)
	//	DrawCircle(int(light.Position.x), int(light.Position.y), 10, light.LightColor);

	if (showLines)
	{
		for (std::vector<Vector2> shadow : m_lights[0].Shadows)
		{
			DrawTriangleFan(&shadow[0], 4, DARKPURPLE);
		}

		for (auto& box : m_light_walls)
		{
			if (m_lights[0].BoxInLight(*box))
				DrawRectangleRec(*box, PURPLE);
		}

		for (auto& box : m_light_walls)
		{
			DrawRectangleLines(box->x, box->y, box->width, box->height, DARKBLUE);
		}

	}

	DrawText(TextFormat("Lights %d", (int)m_lights.size()), 1050, 20, 20, GREEN);
}

void LightManager::SetupLight(float x, float y, float in_radius, float out_radius, Color c, bool is_color)
{
	m_lights.emplace_back(Vector2{ x,y });
	if (is_color)
	{
		m_lights.back().SetColor(c);
	}
	m_lights.back().SetInRadius(in_radius);
	m_lights.back().SetOutRadius(out_radius);
}

LightInfo::LightInfo()
{
	ShadowMask = LoadRenderTexture(LevelManager::currentLdtkLevel->size.x, LevelManager::currentLdtkLevel->size.y);
	GlowTexture = LoadRenderTexture(LevelManager::currentLdtkLevel->size.x, LevelManager::currentLdtkLevel->size.y);
	UpdateLightMask();
}

LightInfo::LightInfo(const Vector2& pos)
{
	ShadowMask = LoadRenderTexture(LevelManager::currentLdtkLevel->size.x, LevelManager::currentLdtkLevel->size.y);
	GlowTexture = LoadRenderTexture(LevelManager::currentLdtkLevel->size.x, LevelManager::currentLdtkLevel->size.y);
	UpdateLightMask();
	Position = pos;
}

void LightInfo::Move(const Vector2& position)
{
	Position = position;
	Dirty = true;
}

void LightInfo::SetInRadius(float inRadius)
{
	InnerRadius = inRadius;
	Dirty = true;
}

void LightInfo::SetOutRadius(float outRadius)
{
	OuterRadius = outRadius;
	Dirty = true;
}

void LightInfo::SetColor(Color color)
{
	HasColor = true;
	LightColor = color;
}

bool LightInfo::BoxInLight(const Rectangle& box)
{
	return CheckCollisionRecs(Bounds, box);
}

void LightInfo::ShadowEdge(const Vector2& sp, const Vector2& ep)
{
	float extension = OuterRadius * 2;

	Vector2 spVector = Vector2Normalize(Vector2Subtract(sp, Position));
	Vector2 spProjection = Vector2Add(sp, Vector2Scale(spVector, extension));

	Vector2 epVector = Vector2Normalize(Vector2Subtract(ep, Position));
	Vector2 epProjection = Vector2Add(ep, Vector2Scale(epVector, extension));

	std::vector<Vector2> polygon;
	polygon.push_back(sp);
	polygon.push_back(ep);
	polygon.push_back(epProjection);
	polygon.push_back(spProjection);

	Shadows.push_back(polygon);
}

void LightInfo::UpdateLightMask()
{
	BeginTextureMode(ShadowMask);

	ClearBackground(WHITE);

	// force the blend mode to only set the alpha of the destination
	rlSetBlendFactors(GL_SRC_ALPHA, GL_SRC_ALPHA, GL_MIN);
	rlSetBlendMode(BLEND_CUSTOM);

	if (Valid)
		DrawLightGradient(Position.x, Position.y, InnerRadius, OuterRadius, ColorAlpha(WHITE, 0), WHITE);
	rlDrawRenderBatchActive();
	rlSetBlendMode(BLEND_ALPHA);

	rlSetBlendFactors(GL_SRC_ALPHA, GL_SRC_ALPHA, GL_MAX);
	rlSetBlendMode(BLEND_CUSTOM);

	for (std::vector<Vector2> shadow : Shadows)
	{
		DrawTriangleFan(&shadow[0], 4, WHITE);
	}

	rlDrawRenderBatchActive();
	// go back to normal
	rlSetBlendMode(BLEND_ALPHA);

	EndTextureMode();

	BeginTextureMode(GlowTexture);
	ClearBackground(BLANK);
	if (Valid)
		DrawLightGradient(Position.x, Position.y, InnerRadius, OuterRadius, ColorAlpha(LightColor, 0.75f), ColorAlpha(LightColor, 0));
	rlDrawRenderBatchActive();

	rlSetBlendFactors(GL_SRC_ALPHA, GL_SRC_ALPHA, GL_MIN);
	rlSetBlendMode(BLEND_CUSTOM);

	for (std::vector<Vector2> shadow : Shadows)
	{
		DrawTriangleFan(&shadow[0], 4, BLANK);
	}

	rlDrawRenderBatchActive();
	// go back to normal
	rlSetBlendMode(BLEND_ALPHA);

	EndTextureMode();
}

void LightInfo::Update(std::vector<Rectangle*>& boxes)
{

	if (!Dirty)
		return;

	Dirty = false;
	Bounds.x = Position.x - OuterRadius;
	Bounds.y = Position.y - OuterRadius;
	Bounds.width = OuterRadius * 2;
	Bounds.height = OuterRadius * 2;

	Shadows.clear();

	for (auto& box : boxes)
	{
		// are we in a box
		if (CheckCollisionPointRec(Position, *box))
			return;

		if (!CheckCollisionRecs(*box, Bounds))
			continue;

		// compute shadow volumes for the faces we are opposite to
		// top
		Vector2 sp = { box->x, box->y };
		Vector2 ep = { box->x + box->width, box->y };

		if (Position.y > ep.y)
			ShadowEdge(sp, ep);

		// right
		sp = ep;
		ep.y += box->height;
		if (Position.x < ep.x)
			ShadowEdge(sp, ep);

		// bottom
		sp = ep;
		ep.x -= box->width;
		if (Position.y < ep.y)
			ShadowEdge(sp, ep);

		// left
		sp = ep;
		ep.y -= box->height;
		if (Position.x > ep.x)
			ShadowEdge(sp, ep);

		// add the actual box as a shadow to get the corner of it.
		// If the map is going to draw the box, then don't do this
		std::vector<Vector2> polygon;
		polygon.emplace_back(Vector2{ box->x, box->y });
		polygon.emplace_back(Vector2{ box->x, box->y + box->height });
		polygon.emplace_back(Vector2{ box->x + box->width, box->y + box->height });
		polygon.emplace_back(Vector2{ box->x + box->width, box->y });
		Shadows.push_back(polygon);
	}

	Valid = true;
	UpdateLightMask();
}

void LightInfo::DrawLightGradient(int centerX, int centerY, float innerRadius, float outterRadius, Color color1, Color color2)
{
	rlCheckRenderBatchLimit(3 * 3 * 36);

	if (innerRadius == 0.0f)
	{
		DrawCircleGradient(centerX, centerY, outterRadius, color1, color2);
		return;
	}

	rlBegin(RL_TRIANGLES);
	for (int i = 0; i < 360; i += 10)
	{
		// inner triangle at color1
		rlColor4ub(color1.r, color1.g, color1.b, color1.a);
		rlVertex2f((float)centerX, (float)centerY);
		rlVertex2f((float)centerX + sinf(DEG2RAD * i) * innerRadius, (float)centerY + cosf(DEG2RAD * i) * innerRadius);
		rlVertex2f((float)centerX + sinf(DEG2RAD * (i + 10)) * innerRadius, (float)centerY + cosf(DEG2RAD * (i + 10)) * innerRadius);

		if (outterRadius > innerRadius)
		{
			rlVertex2f((float)centerX + sinf(DEG2RAD * (i + 10)) * innerRadius, (float)centerY + cosf(DEG2RAD * (i + 10)) * innerRadius);
			rlVertex2f((float)centerX + sinf(DEG2RAD * i) * innerRadius, (float)centerY + cosf(DEG2RAD * i) * innerRadius);
			rlColor4ub(color2.r, color2.g, color2.b, color2.a);
			rlVertex2f((float)centerX + sinf(DEG2RAD * i) * outterRadius, (float)centerY + cosf(DEG2RAD * i) * outterRadius);

			rlColor4ub(color1.r, color1.g, color1.b, color1.a);
			rlVertex2f((float)centerX + sinf(DEG2RAD * (i + 10)) * innerRadius, (float)centerY + cosf(DEG2RAD * (i + 10)) * innerRadius);
			rlColor4ub(color2.r, color2.g, color2.b, color2.a);
			rlVertex2f((float)centerX + sinf(DEG2RAD * i) * outterRadius, (float)centerY + cosf(DEG2RAD * i) * outterRadius);
			rlVertex2f((float)centerX + sinf(DEG2RAD * (i + 10)) * outterRadius, (float)centerY + cosf(DEG2RAD * (i + 10)) * outterRadius);
		}

	}
	rlEnd();
}
