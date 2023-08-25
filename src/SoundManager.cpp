#include "SoundManager.h"
#include <vector>

std::unordered_map<std::string, Sound> SoundManager::sounds;

void SoundManager::LoadSounds()
{
	sounds.emplace("swish1", LoadSound("res/sound/swish/swish-1.wav"));
	sounds.emplace("swish2", LoadSound("res/sound/swish/swish-2.wav"));
	sounds.emplace("swish3", LoadSound("res/sound/swish/swish-3.wav"));
	sounds.emplace("swish4", LoadSound("res/sound/swish/swish-4.wav"));
	sounds.emplace("swish5", LoadSound("res/sound/swish/swish-5.wav"));
	sounds.emplace("swish6", LoadSound("res/sound/swish/swish-6.wav"));
	sounds.emplace("swish7", LoadSound("res/sound/swish/swish-7.wav"));
	sounds.emplace("swish8", LoadSound("res/sound/swish/swish-8.wav"));
	sounds.emplace("swish9", LoadSound("res/sound/swish/swish-9.wav"));

	sounds.emplace("hit1", LoadSound("res/sound/hit/hit-01.wav"));
	sounds.emplace("hit2", LoadSound("res/sound/hit/hit-02.wav"));
	sounds.emplace("hit3", LoadSound("res/sound/hit/hit-03.wav"));
	sounds.emplace("hit4", LoadSound("res/sound/hit/hit-04.wav"));
	sounds.emplace("hit5", LoadSound("res/sound/hit/hit-05.wav"));
	sounds.emplace("hit6", LoadSound("res/sound/hit/hit-06.wav"));
	sounds.emplace("hit7", LoadSound("res/sound/hit/hit-07.wav"));
	sounds.emplace("hit8", LoadSound("res/sound/hit/hit-08.wav"));

	sounds.emplace("monster-1", LoadSound("res/sound/monster/monster-1.wav"));
	sounds.emplace("monster-2", LoadSound("res/sound/monster/monster-2.wav"));
	sounds.emplace("monster-3", LoadSound("res/sound/monster/monster-3.wav"));
	sounds.emplace("monster-4", LoadSound("res/sound/monster/monster-4.wav"));
	sounds.emplace("monster-5", LoadSound("res/sound/monster/monster-5.wav"));
	sounds.emplace("monster-6", LoadSound("res/sound/monster/monster-6.wav"));
	sounds.emplace("monster-7", LoadSound("res/sound/monster/monster-7.wav"));
	sounds.emplace("monster-8", LoadSound("res/sound/monster/monster-8.wav"));
	sounds.emplace("monster-9", LoadSound("res/sound/monster/monster-9.wav"));
	sounds.emplace("monster-10", LoadSound("res/sound/monster/monster-10.wav"));
	sounds.emplace("slime2short", LoadSound("res/sound/monster/slime_s-02.wav"));
	sounds.emplace("slime1", LoadSound("res/sound/monster/slime-01.wav"));
	sounds.emplace("slime2", LoadSound("res/sound/monster/slime-02.wav"));
	sounds.emplace("wing_flap", LoadSound("res/sound/monster/wings_flap.wav"));

	sounds.emplace("splat1", LoadSound("res/sound/monster/splat/splat-01.wav"));
	sounds.emplace("splat2", LoadSound("res/sound/monster/splat/splat-02.wav"));
	sounds.emplace("splat3", LoadSound("res/sound/monster/splat/splat-03.wav"));
	sounds.emplace("splat4", LoadSound("res/sound/monster/splat/splat-04.wav"));
	sounds.emplace("splat5", LoadSound("res/sound/monster/splat/splat-05.wav"));
	sounds.emplace("splat6", LoadSound("res/sound/monster/splat/splat-06.wav"));
	sounds.emplace("splat7", LoadSound("res/sound/monster/splat/splat-07.wav"));
	sounds.emplace("splat8", LoadSound("res/sound/monster/splat/splat-08.wav"));


	sounds.emplace("hurt1", LoadSound("res/sound/monster/hurt/hurt1.wav"));
	sounds.emplace("hurt2", LoadSound("res/sound/monster/hurt/hurt2.wav"));
	sounds.emplace("hurt3", LoadSound("res/sound/monster/hurt/hurt3.wav"));
	sounds.emplace("hurt4", LoadSound("res/sound/monster/hurt/hurt4.wav"));
	sounds.emplace("hurt5", LoadSound("res/sound/monster/hurt/hurt5.wav"));
	sounds.emplace("hurt6", LoadSound("res/sound/monster/hurt/hurt6.wav"));
	sounds.emplace("hurt7", LoadSound("res/sound/monster/hurt/hurt7.wav"));
	sounds.emplace("hurt8", LoadSound("res/sound/monster/hurt/hurt8.wav"));
	sounds.emplace("hurt9", LoadSound("res/sound/monster/hurt/hurt9.wav"));
	sounds.emplace("hurt10", LoadSound("res/sound/monster/hurt/hurt10.wav"));
	sounds.emplace("hurt11", LoadSound("res/sound/monster/hurt/hurt11.wav"));
	sounds.emplace("hurt12", LoadSound("res/sound/monster/hurt/hurt12.wav"));
	sounds.emplace("hurt13", LoadSound("res/sound/monster/hurt/hurt13.wav"));
	sounds.emplace("hurt14", LoadSound("res/sound/monster/hurt/hurt14.wav"));
	sounds.emplace("hurt15", LoadSound("res/sound/monster/hurt/hurt15.wav"));
	sounds.emplace("hurt16", LoadSound("res/sound/monster/hurt/hurt16.wav"));

	sounds.emplace("robo1", LoadSound("res/sound/robotic/robot1.wav"));
	sounds.emplace("robo2", LoadSound("res/sound/robotic/robot2.wav"));
	sounds.emplace("robo3", LoadSound("res/sound/robotic/robot3.wav"));
	sounds.emplace("robo4", LoadSound("res/sound/robotic/robot4.wav"));
	sounds.emplace("robo5", LoadSound("res/sound/robotic/robotstand.wav"));
	sounds.emplace("robo6", LoadSound("res/sound/robotic/robotstep1.wav"));
	sounds.emplace("robo7", LoadSound("res/sound/robotic/robotstep3.wav"));
	sounds.emplace("robo8", LoadSound("res/sound/robotic/robotstep4.wav"));
	sounds.emplace("robo9", LoadSound("res/sound/robotic/robotstep4.wav"));
	sounds.emplace("robo10", LoadSound("res/sound/robotic/robotstomp.wav"));

	sounds.emplace("agro1", LoadSound("res/sound/collected/agro1.wav"));
	sounds.emplace("agro2", LoadSound("res/sound/collected/agro2.wav"));
	sounds.emplace("axe_pickup", LoadSound("res/sound/collected/axe_pickup.wav"));
	sounds.emplace("axe_throw", LoadSound("res/sound/collected/axe_throw.wav"));
	sounds.emplace("axe_solid_hit", LoadSound("res/sound/collected/axe_solid_hit.wav"));
	sounds.emplace("background_wind", LoadSound("res/sound/collected/background_wind.wav"));
	sounds.emplace("call_sw", LoadSound("res/sound/collected/call_sw.wav"));
	sounds.emplace("Engine", LoadSound("res/sound/collected/Engine.wav"));
	sounds.emplace("jump", LoadSound("res/sound/collected/jump2.wav"));
	sounds.emplace("land", LoadSound("res/sound/collected/land.wav"));
	sounds.emplace("grunt", LoadSound("res/sound/collected/grunt.wav"));
	sounds.emplace("dead", LoadSound("res/sound/collected/dead.wav"));
	sounds.emplace("elevator", LoadSound("res/sound/collected/elevator.wav"));
	sounds.emplace("elevator_stop", LoadSound("res/sound/collected/elevator_stop.wav"));
	sounds.emplace("door_close", LoadSound("res/sound/collected/door_close.wav"));
	SetSoundVolume(sounds["door_close"], 0.7f);
	sounds.emplace("light_ambient", LoadSound("res/sound/collected/lights_noise-01.wav"));
	sounds.emplace("flicker", LoadSound("res/sound/collected/lights_noise-02.wav"));
	sounds.emplace("glass_shatter", LoadSound("res/sound/collected/glass_shatter.wav"));
	sounds.emplace("metal_hit", LoadSound("res/sound/collected/metal_hit.wav"));
	sounds.emplace("metal_fall", LoadSound("res/sound/collected/metal_fall.wav"));
	sounds.emplace("ambient_hum", LoadSound("res/sound/collected/ambient_hum.wav"));
	sounds.emplace("denied", LoadSound("res/sound/collected/denied.wav"));
	sounds.emplace("pc_work", LoadSound("res/sound/collected/pc_work.wav"));
	sounds.emplace("beep-01", LoadSound("res/sound/collected/beep-01.wav"));
	sounds.emplace("beep-02", LoadSound("res/sound/collected/beep-02.wav"));
	sounds.emplace("beep-03", LoadSound("res/sound/collected/beep-03.wav"));
	sounds.emplace("beep-04", LoadSound("res/sound/collected/beep-04.wav"));
	sounds.emplace("beep-05", LoadSound("res/sound/collected/beep-05.wav"));
	sounds.emplace("creepy_whistle", LoadSound("res/sound/collected/creepy_whistle.wav"));
	sounds.emplace("electroswitch-01", LoadSound("res/sound/collected/electroswitch-01.wav"));
	sounds.emplace("electroswitch-02", LoadSound("res/sound/collected/electroswitch-02.wav"));
	sounds.emplace("generator-01", LoadSound("res/sound/collected/generator-01.wav"));
	sounds.emplace("generator-02", LoadSound("res/sound/collected/generator-02.wav"));
	sounds.emplace("secret", LoadSound("res/sound/collected/hopefull.wav"));
	sounds.emplace("secret2", LoadSound("res/sound/collected/hopefull2.wav"));
	sounds.emplace("frog_run", LoadSound("res/sound/collected/frog_run.wav"));
	sounds.emplace("whirl1", LoadSound("res/sound/collected/whirl-01.wav"));
	sounds.emplace("whirl2", LoadSound("res/sound/collected/whirl-02.wav"));
	sounds.emplace("hookshot", LoadSound("res/sound/collected/hookshot.wav"));
	sounds.emplace("steam_thud", LoadSound("res/sound/collected/steam_thud.wav"));
	sounds.emplace("steam", LoadSound("res/sound/collected/steam.wav"));
	sounds.emplace("start_roar", LoadSound("res/sound/collected/start_roar.wav"));

	//glass
	sounds.emplace("glass_shatter", LoadSound("res/sound/collected/glass_shatter.wav"));
	sounds.emplace("glass_thud", LoadSound("res/sound/collected/glass_thud.wav"));
	sounds.emplace("glass_thud2", LoadSound("res/sound/collected/glass_thud2.wav"));
	sounds.emplace("glass_explosion", LoadSound("res/sound/collected/glass_explosion.wav"));


	sounds.emplace("step1", LoadSound("res/sound/steps/stepstone01.wav"));
	sounds.emplace("step2", LoadSound("res/sound/steps/stepstone02.wav"));
	sounds.emplace("step3", LoadSound("res/sound/steps/stepstone03.wav"));
	sounds.emplace("step4", LoadSound("res/sound/steps/stepstone04.wav"));
	sounds.emplace("step5", LoadSound("res/sound/steps/stepstone05.wav"));

	sounds.emplace("woodhit1", LoadSound("res/sound/collected/wood_hit1.wav"));
	SetSoundVolume(sounds["woodhit1"], 0.7f);
	sounds.emplace("woodhit2", LoadSound("res/sound/collected/wood_hit2.wav"));
	SetSoundVolume(sounds["woodhit2"], 0.7f);
	sounds.emplace("woodhit3", LoadSound("res/sound/collected/wood_hit3.wav"));
	SetSoundVolume(sounds["woodhit3"], 0.7f);
	sounds.emplace("crate_break", LoadSound("res/sound/collected/crate_break.wav"));





}

void SoundManager::PlayRandSounds(std::string* names, int count)
{
	int rand = GetRandomValue(0, count - 1);
	std::vector<Sound> s;
	for (int i = 0; i < count; i++)
	{
		s.push_back(sounds[names[i]]);
	}
	PlaySound(s[rand]);
}
