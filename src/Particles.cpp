#include "Particles.h"
#include "GameScreen.h"

std::vector<ParticleEmitter*> ParticlesManager::emitters;
RenderTexture2D ParticlesManager::particlesRenderT;
Texture2D ParticlesManager::ParticlesTexture;

ParticlesManager::ParticlesManager()
{
	particlesRenderT = LoadRenderTexture(GameScreen::LevelMgr->currentLdtkLevel->size.x, GameScreen::LevelMgr->currentLdtkLevel->size.y);
	//particlesRenderT = LoadRenderTexture(1920, 1080);
}
