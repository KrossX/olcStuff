/* Copyright (c) 2026 KrossX <krossx@live.com>
 * License: http://www.opensource.org/licenses/mit-license.html  MIT License
 */

#pragma warning(push, 1)
#define OLC_USE_STB_IMAGE
#define OLC_PGE3_APPLICATION
#include "olcPixelGameEngine3.h"

#define OLC_PGEX3_MINIAUDIO
#include "olcPGEX3_Miniaudio.h"
#pragma warning(pop)

#define WND_WIDTH  796
#define WND_HEIGHT 448

//#define WND_WIDTH  398
//#define WND_HEIGHT 224

namespace bin2h
{
#if defined(__EMSCRIPTEN__)
#include "data/models_em.h"
#else
#include "data/models.h"
#endif

#include "data/background_ps.h"
#include "data/main_ps.h"
#include "data/main_vs.h"
#include "data/mus_gothic_dark_loop_64k.h"
#include "data/mus_havoc_loop_64k.h"
#include "data/noise.h"
#include "data/palette.h"
#include "data/snd_boom.h"
#include "data/snd_change.h"
#include "data/snd_hit.h"
#include "data/snd_hit2.h"
#include "data/snd_hit3.h"
#include "data/snd_powerup.h"
#include "data/snd_shoot.h"
#include "data/snd_shoot2.h"
}

namespace mesh
{
	struct mesh
	{
		std::vector<olc::vf4d> pos;
		//std::vector<olc::vf4d> norm;
		std::vector<olc::vf2d> uv;
		std::vector<olc::Pixel> col;
		//olc::Structure layout = olc::Structure::List;
	};

	mesh ship;
	mesh bullet_ball;
	mesh bullet_stick;
	mesh enemy_saucer;
	mesh enemy_sentry;
	mesh enemy_raptor;
	mesh enemy_suzanne;

	void load(mesh &out, float *data, int count)
	{
		for(int i = 0; i < count;) {
			out.pos.push_back({data[i++],data[i++],data[i++]}); out.uv.push_back({data[i++],data[i++]}); out.col.push_back(olc::Colour::WHITE);
			out.pos.push_back({data[i++],data[i++],data[i++]}); out.uv.push_back({data[i++],data[i++]}); out.col.push_back(olc::Colour::WHITE);
			out.pos.push_back({data[i++],data[i++],data[i++]}); out.uv.push_back({data[i++],data[i++]}); out.col.push_back(olc::Colour::WHITE);
		}
	}
}

class ksx_pewpew : public olc::PixelGameEngine
{
#define MAX_ENEMIES 64
#define MAX_PARTICLES 128
#define MAX_WEAPONS 3

#include "audio.cpp"
#include "enemies.cpp"
#include "input.cpp"
#include "particles.cpp"

	bool isFullscreen = false;

	olc::gpu::Shader_GLSL33 bg_render, player_shader, enemy_shader, particle_shader;
	olc::Image bg_image;
	olc::vf2d bg_off, bg_region = olc::vf2d(WND_WIDTH, WND_HEIGHT) / 64.0f;

	olc::Image palette;

	// From PGE3 3DCube example
	// Matrices for 3D projection, view and world transforms
	olc::mf4d matProj;
	olc::mf4d matView;
	olc::mf4d matWorld;

	// Position in 3D space of "the camera"
	olc::vf4d vViewTranslate = { 0.0f, 5.0f, -40.0f };

	struct {
		double posx = 0;
		float posy  = 0;
		float roll = 0;
		float speed = 1.0f;
		bool hold = false;
		float hp = 10.0f;
	} player;

	double total_time = 0;
	double world_posx = 0;
	double enemy_next = 5.0;

	int powerup_count = 0;
	
	double change_next;
	float change_message_timer = 0;
	std::string change_message;
	
	bool boss_active = false;
	bool boss_killed = false;
	float enemy_weapon_rate = 1.0f;
	
	unsigned int enemy_spawn_tick = 0;
	
	olc::vf2d uv_offset_world = {0,0};
	olc::vf2d uv_offset_enemy = {0,0};
	olc::vf2d uv_offset_player = {0,0};
	
	bool compile_shader(olc::gpu::Shader_GLSL33 &prog, std::string vs_main, std::string ps_main)
	{
		prog.SetVertexShaderSource(olc::gpu::Shader::VS_DefaultHeader() + vs_main);
		prog.SetPixelShaderSource(olc::gpu::Shader::PS_DefaultHeader() + ps_main);

		std::string shader_error = prog.Compile();

		if(shader_error != "OK") {
			std::cout << "VERTEX SHADER" << std::endl;
			std::cout << "================================================================================" << std::endl;
			std::cout << olc::gpu::Shader::VS_DefaultHeader() + vs_main << std::endl;
			std::cout << "PIXEL SHADER" << std::endl;
			std::cout << "================================================================================" << std::endl;
			std::cout << olc::gpu::Shader::PS_DefaultHeader() + ps_main << std::endl;
			std::cout << "Error compiling shader: " << shader_error << std::endl;
			return false;
		}

		return true;
	}


	void mesh_draw(mesh::mesh &m, bool textured, olc::Pixel col = olc::Colour::WHITE)
	{
		if(textured)
			draw.Mesh(olc::Structure::List, m.pos, m.col, m.uv, palette, col);
		else
			draw.Mesh(olc::Structure::List, m.pos, m.col, col);
	}
	
	enum {
		SCENE_START = 0,
		SCENE_MAIN,
		SCENE_MAIN_BOSS,
		SCENE_GAMEOVER
	};
	
	int current_scene = SCENE_START;
	float scene_timer = 0;

public:
	bool OnUserCreate() override
	{
		if(!audio_init())
			return false;

		std::string bg_ps((char*)bin2h::background_ps_glsl, sizeof(bin2h::background_ps_glsl));
		std::string main_ps((char*)bin2h::main_ps_glsl, sizeof(bin2h::main_ps_glsl));
		std::string main_vs((char*)bin2h::main_vs_glsl, sizeof(bin2h::main_vs_glsl));

		if(!compile_shader(bg_render, olc::gpu::Shader::VS_DefaultMain(), bg_ps))
			return false;

		if(!compile_shader(player_shader, main_vs, main_ps))
			return false;
		
		if(!compile_shader(enemy_shader, main_vs, main_ps))
			return false;
		
		if(!compile_shader(particle_shader, main_vs, main_ps))
			return false;

		bg_render.CreateUniform("bg_offset");
		bg_render.CreateUniform("bg_region");
		
		bg_render.CreateUniform("uv_offset");
		player_shader.CreateUniform("uv_offset");
		player_shader.CreateUniform("bg_white");
		enemy_shader.CreateUniform("uv_offset");
		enemy_shader.CreateUniform("bg_white");
		particle_shader.CreateUniform("uv_offset");
		particle_shader.CreateUniform("bg_white");

		CreateImageFromMemory(bg_image, bin2h::noise_png, sizeof(bin2h::noise_png), {true});
		CreateImageFromMemory(palette, bin2h::palette_png, sizeof(bin2h::palette_png));

		load(mesh::ship, bin2h::obj::ship, bin2h::obj::ship_cnt);
		load(mesh::bullet_ball, bin2h::obj::bullet_ball, bin2h::obj::bullet_ball_cnt);
		load(mesh::bullet_stick, bin2h::obj::bullet_stick, bin2h::obj::bullet_stick_cnt);
		load(mesh::enemy_saucer, bin2h::obj::saucer, bin2h::obj::saucer_cnt);
		load(mesh::enemy_sentry, bin2h::obj::sentry, bin2h::obj::sentry_cnt);
		load(mesh::enemy_raptor, bin2h::obj::raptor, bin2h::obj::raptor_cnt);
		load(mesh::enemy_suzanne, bin2h::obj::suzanne, bin2h::obj::suzanne_cnt);

		matProj.perspective(25.0f * 3.14159f / 180.0f, float(WND_WIDTH) / float(WND_HEIGHT), 30.0f, 400.0f);
		draw.SetProjectionMatrix(matProj);

		draw.SetCullMode(olc::CullMode::ClockWise);

		srand((unsigned int)time(NULL));
		change_next = 6.0 + (float)rand() / (float)RAND_MAX * 6.0;

		weapon_init();
		particle_init();
		enemy_init();

		return true;
	}

	void check_player_dead()
	{
		for(particle &p : enemy_bullet) {
			if(!p.active) continue;

			olc::vf2d dist = olc::vf2d((float)player.posx - p.pos.x, player.posy - p.pos.y);


			if(p.type == BULLET_ENEMY_POWERUP) {
				if(dist.mag2() < 9.0) {
					p.active = false;
					audio_playpan(SND_POWERUP, (float)player.posx, player.posy);
					powerup_count++;
				}
			} else {
				if(dist.mag2() < 1.0) {
					p.active = false;

					player.hp -= p.damage;
					if(player.hp <= 0) {
						particle_add({(float)player.posx, player.posy}, {0,0}, {0,0}, PARTICLE_EXPLOSION);
						audio_playpan(SND_BOOM, (float)player.posx, player.posy);
					} else {

						if(p.type == BULLET_ENEMY_POWERUP) {
							audio_playpan(SND_POWERUP, (float)player.posx, player.posy);
						} else {
							audio_playpan(SND_HIT2, (float)player.posx, player.posy);
						}
					}
				}
			}
		}

		for(enemy &e : enemy_list) {
			if(!e.active) continue;

			olc::vf2d dist = olc::vf2d((float)player.posx - e.pos.x, player.posy - e.pos.y);

			if(dist.mag2() < 1.0) {
				e.active = false;

				particle_add(e.pos, e.spd * 0.5f, {0,0}, PARTICLE_EXPLOSION);
				audio_playpan(SND_BOOM, e.pos.x, e.pos.y);

				player.hp -= e.hp;
				if(player.hp <= 0) {
					particle_add({(float)player.posx, player.posy}, {0,0}, {0,0}, PARTICLE_EXPLOSION);
				} else {
					audio_playpan(SND_HIT2, (float)player.posx, player.posy);
				}
			}
		}
		
		if(player.hp <= 0) {
			current_scene = SCENE_GAMEOVER;
		}
	}

	void send_enemies(void)
	{
		if(total_time < enemy_next)
			return;

		enemy_next += 4.0f;
		
		if(boss_active) {
			switch(enemy_spawn_tick%3) {
				case 0: enemy_factory(ENEMY_SPAWN_RAPTOR_LEFT, ENEMY_LEVEL_HIGH); break;
				case 1: enemy_factory(ENEMY_SPAWN_RAPTOR_RIGHT, ENEMY_LEVEL_HIGH); break;
				// 2...
			}
		} else {
			switch(enemy_spawn_tick%37) {
				case 0: enemy_factory(ENEMY_SPAWN_SAUCER_LEFT, ENEMY_LEVEL_LOW); break;
				case 1: enemy_factory(ENEMY_SPAWN_SAUCER_RIGHT, ENEMY_LEVEL_LOW); break;
				case 2: enemy_factory(ENEMY_SPAWN_SAUCER_LEFT, ENEMY_LEVEL_LOW); break;
				case 3: enemy_factory(ENEMY_SPAWN_SAUCER_RIGHT, ENEMY_LEVEL_LOW); break;
				case 4: enemy_factory(ENEMY_SPAWN_RAPTOR_LEFT, ENEMY_LEVEL_LOW); break;
				case 5: enemy_factory(ENEMY_SPAWN_RAPTOR_RIGHT, ENEMY_LEVEL_LOW); break;

				case  6: enemy_factory(ENEMY_SPAWN_SAUCER_LEFT, ENEMY_LEVEL_MID); break;
				case  7: enemy_factory(ENEMY_SPAWN_SAUCER_RIGHT, ENEMY_LEVEL_MID); break;
				case  8: enemy_factory(ENEMY_SPAWN_SAUCER_LEFT, ENEMY_LEVEL_MID); break;
				case  9: enemy_factory(ENEMY_SPAWN_SAUCER_RIGHT, ENEMY_LEVEL_MID); break;
				case 10: enemy_factory(ENEMY_SPAWN_RAPTOR_LEFT, ENEMY_LEVEL_MID); break;
				case 11: enemy_factory(ENEMY_SPAWN_RAPTOR_RIGHT, ENEMY_LEVEL_MID); break;

				case 12: enemy_factory(ENEMY_SPAWN_SENTRY, ENEMY_LEVEL_MID); break;
				// 13 to 18  ....

				case 19: enemy_factory(ENEMY_SPAWN_SAUCER_LEFT, ENEMY_LEVEL_HIGH); break;
				case 20: enemy_factory(ENEMY_SPAWN_SAUCER_RIGHT, ENEMY_LEVEL_HIGH); break;
				case 21: enemy_factory(ENEMY_SPAWN_SAUCER_LEFT, ENEMY_LEVEL_HIGH); break;
				case 22: enemy_factory(ENEMY_SPAWN_SAUCER_RIGHT, ENEMY_LEVEL_HIGH); break;
				case 23: enemy_factory(ENEMY_SPAWN_RAPTOR_LEFT, ENEMY_LEVEL_HIGH); break;
				case 24: enemy_factory(ENEMY_SPAWN_RAPTOR_RIGHT, ENEMY_LEVEL_HIGH); break;

				case 25: enemy_factory(ENEMY_SPAWN_SENTRY, ENEMY_LEVEL_MID); break;
				// 26...
				case 27: enemy_factory(ENEMY_SPAWN_RAPTOR_LEFT, ENEMY_LEVEL_HIGH); break;
				case 28: enemy_factory(ENEMY_SPAWN_RAPTOR_RIGHT, ENEMY_LEVEL_HIGH); break;
				// 29..30..
				case 31: enemy_factory(ENEMY_SPAWN_RAPTOR_LEFT, ENEMY_LEVEL_HIGH); break;
				case 32: enemy_factory(ENEMY_SPAWN_RAPTOR_RIGHT, ENEMY_LEVEL_HIGH); break;
				case 33: ma_sound_set_fade_in_milliseconds(sound[MUS_HAVOC].GetMASound(), 0.25f, 0.0f, 5000); break;
				case 34: ma_sound_set_fade_in_milliseconds(sound[MUS_GOTHIC].GetMASound(), 0, 0.25f, 1000); break;
				case 35:
					sound[MUS_GOTHIC].Seek(0.0f);
					sound[MUS_GOTHIC].Play(true);
					enemy_factory(ENEMY_SPAWN_SUZANNE, ENEMY_LEVEL_HIGH);
					boss_active = true;
					break;
			}
		}

		enemy_spawn_tick++;
	}


	void draw_ui(void)
	{
		//#define WND_WIDTH  796
		//#define WND_HEIGHT 448

		draw.FilledRect({0,416}, {796,32}, olc::Colour::BLACK, olc::Pixel(255,255,255,200));
		draw.FilledRect({0,399}, {796,16}, olc::Colour::BLACK, olc::Pixel(255,255,255,150));


		float factor_t = std::clamp((float)(change_next - total_time), 0.0f, 24.0f);
		float factor_s = std::sinf((float)total_time * 30.0f / (factor_t+1)) * 0.5f + 0.5f;

		olc::Pixel col = olc::PixelLerp(olc::Colour::TANGERINE, olc::Colour::WHITE, factor_s);

		draw.String({ 4, 404 }, "Change: ", olc::Colour::TANGERINE);
		draw.FilledRect({68, 404}, {factor_t * 30.1666f,8}, col, olc::Colour::WHITE);
		// draw.FilledRect({68, 404}, {724,8}, col, olc::Colour::WHITE);
		
		if(change_message_timer > 0) {
			float posx = (WND_WIDTH - change_message.length() * 8) / 2.0f;
			factor_s = std::sinf((float)total_time * 30.0f) * 0.5f + 0.5f;
			col = olc::PixelLerp(olc::Colour::TANGERINE, olc::Colour::WHITE, factor_s);
			draw.String({ posx, 390 }, change_message, col);
		}
		
		if(powerup_count > 0) {
			factor_s = std::sinf((float)total_time * 24.0f) * 0.5f + 0.5f;
			col = olc::PixelLerp(olc::Colour::TANGERINE, olc::Colour::RED, factor_s);
			draw.String({ 4, 356 },"UP     UP      UP", col, {2,2});
			draw.String({ 4, 380 },"\\/     \\/      \\/", col, {2,2});
		}


		//message = std::format("Z:{:2d} X:{:2d} C:{:2d}             HP:{:3d}", 1, 1, 1, (int)player.hp);
		draw.String({ 4, 421 },"Z:   X:   C:               HP:", olc::Colour::TANGERINE, {3,3});

		auto message = std::format("  {:2d}   {:2d}   {:2d}                {:3d}",
			player_weapon[PLAYER_WEAPON_Z].level,
			player_weapon[PLAYER_WEAPON_X].level,
			player_weapon[PLAYER_WEAPON_C].level, (int)player.hp);

		draw.String({ 4, 421 }, message, olc::Colour::WHITE, {3,3});
	}

	void do_change() {
		float rnum1 = (float)rand() / (float)RAND_MAX ;
		float rnum2 = (float)rand() / (float)RAND_MAX ;
		float rnum3 = (float)rand() / (float)RAND_MAX ;
		
		if(rnum1 < 0.1) { // WORLD CHANGE
			change_message = "World change!";
			uv_offset_world.x += 1.0f/16.0f;
		} else if (rnum1 < 0.8) { // PLAYER CHANGE
			if(rnum2 < 0.1) { // VISUAL
				change_message = "Player change!";
				uv_offset_player.x += 1.0f/16.0f;
			} else if(rnum2 < 0.7) { // WEAPON
				if(rnum3 < 0.8) { // POSITIVE
					change_message = "Player weapons are faster!";
					player_weapon[PLAYER_WEAPON_Z].rate *= 0.9f;
					player_weapon[PLAYER_WEAPON_X].rate *= 0.9f;
					player_weapon[PLAYER_WEAPON_C].rate *= 0.9f;
				} else { // NEGATIVE
					change_message = "Player weapons are slower!";
					player_weapon[PLAYER_WEAPON_Z].rate *= 1.1f;
					player_weapon[PLAYER_WEAPON_X].rate *= 1.1f;
					player_weapon[PLAYER_WEAPON_C].rate *= 1.1f;
				}
			} else { // LIFE
				if(rnum3 < 0.8) { // POSITIVE
					change_message = "Player life double!";
					player.hp *= 2.0f;
				} else { // NEGATIVE
					change_message = "Player life halved!";
					player.hp *= 0.5f;
				}
			}
		} else { // ENEMY CHANGE
			if(rnum2 < 0.7) { // VISUAL
				uv_offset_enemy.x += 1.0f/16.0f;
			} else { // WEAPON
				if(rnum3 < 0.2) { // POSITIVE
					enemy_weapon_rate *= 0.9f;
				} else { // NEGATIVE
					enemy_weapon_rate *= 1.1f;
				}
			}
		}
	}
	
	void scene_main(float fElapsedTime)
	{
		scene_timer += fElapsedTime;
		
		check_input(fElapsedTime);
		Weapon_step(fElapsedTime);

		particle_step(player_bullet, fElapsedTime);
		check_enemy_dead();

		particle_step(enemy_bullet, fElapsedTime);
		check_player_dead();

		enemy_step(fElapsedTime);

		particle_step(particle_misc, fElapsedTime);

		send_enemies();

		// CHANGE!

		if(total_time > change_next) {
			change_next += 24.0 + (float)rand() / (float)RAND_MAX * 12.0;
			change_message_timer = 6.0f;
			sound[SND_CHANGE].Play();
			do_change();
		} else {
			change_message_timer -= fElapsedTime;
		}

		// DRAWING BACKGROUND

		draw.EnableDepth(false);
		draw.SetCullMode(olc::CullMode::None);
		draw.Clear(olc::Colour::BLACK);

		bg_off.y -= fElapsedTime * 0.25f;
		if(bg_off.y < -1) bg_off.y += 1;

		bg_off.x = (float)world_posx * -0.02f;

		draw.SetShader(bg_render);
		draw.SetShaderTexture(1, palette);
		draw.SetShaderUniform("bg_offset", bg_off);
		draw.SetShaderUniform("bg_region", bg_region);
		draw.SetShaderUniform("uv_offset", uv_offset_world);
		draw.ImageRect(bg_image, {0,0}, {WND_WIDTH,WND_HEIGHT});

		// DRAWING PLAYFIELD
		olc::Pixel bgcol = palette.Sample(olc::vf2d(49/64.0f, 46/64.0f) + uv_offset_world);

		draw.EnableDepth(true);
		draw.SetCullMode(olc::CullMode::ClockWise);
		draw.SetShader(player_shader);
		draw.SetShaderUniform("uv_offset", uv_offset_player);
		draw.SetShaderUniform("bg_white", bgcol);

		vViewTranslate.x = -(float)world_posx;

		olc::mf4d matViewRotateX, matViewTranslate;
		matViewRotateX.rotateX(75.0f * 3.14159f / 180.0f);
		matViewTranslate.translate(vViewTranslate);
		matView = matViewTranslate * matViewRotateX;
		draw.SetViewMatrix(matView);

		olc::mf4d matTrans, matRotY;
		matTrans.translate((float)player.posx, player.posy, 0.0f);
		matRotY.rotateY(player.roll * 0.5);

		matWorld = matTrans * matRotY;
		draw.SetModelMatrix(matWorld);
		mesh_draw(mesh::ship, true);

		if(player.hold) mesh_draw(mesh::bullet_ball, false, olc::Colour::RED);

		draw.SetShader(enemy_shader);
		draw.SetShaderUniform("uv_offset", uv_offset_enemy);
		draw.SetShaderUniform("bg_white", bgcol);
		enemy_count = 0;
		enemy_draw();
		
		draw.SetShader(particle_shader);
		draw.SetShaderUniform("uv_offset", olc::vf2d(0,0));
		draw.SetShaderUniform("bg_white", bgcol);
		particle_count = 0;
		particle_draw(particle_misc);
		particle_draw(enemy_bullet);
		particle_draw(player_bullet);

		// DRAWING UI

		draw.ResetShader();
		draw_ui();
		
		if(current_scene == SCENE_GAMEOVER) {
			scene_timer = 0;
		}
	}
	
	void scene_start(float fElapsedTime)
	{
		scene_timer += fElapsedTime;
		//scene_timer = 4.0f;
		
		// DRAWING BACKGROUND

		draw.EnableDepth(false);
		draw.SetCullMode(olc::CullMode::None);
		draw.Clear(olc::Colour::BLACK);

		bg_off.y -= fElapsedTime * 0.25f;
		if(bg_off.y < -1) bg_off.y += 1;

		bg_off.x = (float)world_posx * -0.02f;

		draw.SetShader(bg_render);
		draw.SetShaderTexture(1, palette);
		draw.SetShaderUniform("bg_offset", bg_off);
		draw.SetShaderUniform("bg_region", bg_region);
		draw.SetShaderUniform("uv_offset", uv_offset_world);
		draw.ImageRect(bg_image, {0,0}, {WND_WIDTH,WND_HEIGHT});

		// DRAWING PLAYFIELD
		olc::Pixel bgcol = palette.Sample(olc::vf2d(49/64.0f, 46/64.0f) + uv_offset_world);
		
		draw.EnableDepth(true);
		draw.SetCullMode(olc::CullMode::ClockWise);
		draw.SetShader(player_shader);
		draw.SetShaderUniform("uv_offset", uv_offset_player);
		draw.SetShaderUniform("bg_white", bgcol);

		vViewTranslate.x = -(float)world_posx;

		olc::mf4d matViewRotateX, matViewTranslate;
		matViewRotateX.rotateX(75.0f * 3.14159f / 180.0f);
		matViewTranslate.translate(vViewTranslate);
		matView = matViewTranslate * matViewRotateX;
		draw.SetViewMatrix(matView);

		float factor = std::clamp((5.0f - scene_timer) / 2.0f, 0.0f, 1.0f);
		float posy = 10.0f * factor * factor;

		olc::mf4d matTrans;
		matTrans.translate(0.0f, posy, 0.0f);
		matWorld = matTrans;
		draw.SetModelMatrix(matWorld);
		mesh_draw(mesh::ship, true);
		
		draw.String({ 5, 5 },"A game by KrossX for the OLC CodeJam 2026: Change\n\n"
			"\t3d models made using Blender\n"
			"\tsfx generated with bfxr.net\n"
			"\tmusic by PeriTune (peritune.com)\n",
			olc::Colour::BLACK, {2,2});
		
		
		draw.String({ 4, 4 },"A game by KrossX for the OLC CodeJam 2026: Change\n\n"
			"\t3d models made using Blender\n"
			"\tsfx generated with bfxr.net\n"
			"\tmusic by PeriTune (peritune.com)\n",
			olc::Colour::TANGERINE, {2,2});

		factor = std::clamp(1.0f - scene_timer, 0.0f, 1.0f);
		draw.FilledRect({0,0}, {WND_WIDTH,WND_HEIGHT}, olc::Colour::WHITE,
							olc::Pixel(255,255,255,(unsigned char)(255 * factor * factor)));
							
							
		if(scene_timer > 5.0f) {
			scene_timer = 0;
			current_scene = SCENE_MAIN;

			ma_sound_set_fade_in_milliseconds(sound[MUS_HAVOC].GetMASound(), 0.0f, 0.25f, 1000);
			sound[MUS_HAVOC].Seek(0.0f);
			sound[MUS_HAVOC].Play(true);
		}
	}

	void scene_gameover(float fElapsedTime)
	{
		if(scene_timer == 0) {
			
			if(ma_sound_get_volume(sound[MUS_HAVOC].GetMASound()) > 0.0f)
				ma_sound_set_fade_in_milliseconds(sound[MUS_HAVOC].GetMASound(), -1.0f, 0.0f, 3000);
			if(ma_sound_get_volume(sound[MUS_GOTHIC].GetMASound()) > 0.0f)
				ma_sound_set_fade_in_milliseconds(sound[MUS_GOTHIC].GetMASound(), -1.0f, 0.0f, 3000);
		}
		
		scene_timer += fElapsedTime;
		
		// STUFF

		particle_step(player_bullet, fElapsedTime);
		check_enemy_dead();
		particle_step(enemy_bullet, fElapsedTime);
		enemy_step(fElapsedTime);
		particle_step(particle_misc, fElapsedTime);

		// DRAWING BACKGROUND

		draw.EnableDepth(false);
		draw.SetCullMode(olc::CullMode::None);
		draw.Clear(olc::Colour::BLACK);

		bg_off.y -= fElapsedTime * 0.25f;
		if(bg_off.y < -1) bg_off.y += 1;

		bg_off.x = (float)world_posx * -0.02f;

		draw.SetShader(bg_render);
		draw.SetShaderTexture(1, palette);
		draw.SetShaderUniform("bg_offset", bg_off);
		draw.SetShaderUniform("bg_region", bg_region);
		draw.SetShaderUniform("uv_offset", uv_offset_world);
		draw.ImageRect(bg_image, {0,0}, {WND_WIDTH,WND_HEIGHT});

		// DRAWING PLAYFIELD
		olc::Pixel bgcol = palette.Sample(olc::vf2d(49/64.0f, 46/64.0f) + uv_offset_world);

		draw.EnableDepth(true);
		draw.SetCullMode(olc::CullMode::ClockWise);
		draw.SetShader(player_shader);
		draw.SetShaderUniform("uv_offset", uv_offset_player);
		draw.SetShaderUniform("bg_white", bgcol);

		vViewTranslate.x = -(float)world_posx;

		olc::mf4d matViewRotateX, matViewTranslate;
		matViewRotateX.rotateX(75.0f * 3.14159f / 180.0f);
		matViewTranslate.translate(vViewTranslate);
		matView = matViewTranslate * matViewRotateX;
		draw.SetViewMatrix(matView);
		
		if(player.hp > 0) {
			olc::mf4d matTrans, matRotY;
			matTrans.translate((float)player.posx, player.posy, 0.0f);
			matRotY.rotateY(player.roll * 0.5);

			matWorld = matTrans * matRotY;
			draw.SetModelMatrix(matWorld);
			mesh_draw(mesh::ship, true);
		}

		draw.SetShader(enemy_shader);
		draw.SetShaderUniform("uv_offset", uv_offset_enemy);
		draw.SetShaderUniform("bg_white", bgcol);
		enemy_count = 0;
		enemy_draw();
		
		draw.SetShader(particle_shader);
		draw.SetShaderUniform("uv_offset", olc::vf2d(0,0));
		draw.SetShaderUniform("bg_white", bgcol);
		particle_count = 0;
		particle_draw(particle_misc);
		particle_draw(enemy_bullet);
		particle_draw(player_bullet);
		
		/// DRAW UI
		
		float factor = std::clamp(scene_timer/3.0f, 0.0f, 1.0f);
		draw.FilledRect({0,0}, {WND_WIDTH,WND_HEIGHT}, olc::Colour::BLACK,
							olc::Pixel(255,255,255,(unsigned char)(255 * factor * factor * 0.8f)));
							
		
		factor = 1.0f - (float)abs(std::sin(TotalTimeElapsed() * 2.0));
		olc::Pixel col = olc::PixelLerp(olc::Colour::DARK_RED, olc::Colour::RED, factor);
		
		std::string msg = boss_killed ? "YOU WIN" : "YOU LOSE";
							
		float msgz = msg.length() * 40.0f;
		float posx = (WND_WIDTH - msgz) / 2.0f;
		float posy = (WND_HEIGHT - 40) / 2.0f;

		draw.String({ posx, posy }, msg, col, {5,5});
	}
	
	bool OnUserUpdate(float fElapsedTime) override
	{
		static float fade = 0;
		
		if(!IsFocused()) {
			draw.Clear(olc::Colour::VERY_DARK_GREY);
			fade = 0.25f;

			float factor = 1.0f - (float)abs(std::sin(TotalTimeElapsed() * 2.0));

			olc::Pixel col = olc::PixelLerp(olc::Colour::GREY, olc::Colour::WHITE, factor);

			std::string msg = total_time > 0? "CONTINUE" : "START";

			float msgz = msg.length() * 40.0f;
			float posx = (WND_WIDTH - msgz) / 2.0f;
			float posy = (WND_HEIGHT - 40) / 2.0f;

			draw.String({ posx, posy }, msg, col, {5,5});

			col = olc::PixelLerp(olc::Colour::DARK_RED, olc::Colour::RED, factor);

			factor = factor * factor * 40.0f;

			draw.String({ posx - 80.0f + factor, posy - 18.0f}, "{", col, {5,10});
			draw.String({ posx + msgz + 40.0f - factor, posy - 18.0f}, "}", col, {5,10});
			return true;
		}

		switch(current_scene) {
			case SCENE_START:     scene_start(fElapsedTime); break;
			case SCENE_MAIN:      scene_main(fElapsedTime); break;
			case SCENE_MAIN_BOSS: scene_main(fElapsedTime); break;
			case SCENE_GAMEOVER:  scene_gameover(fElapsedTime); break;
		}
		
		draw.ResetShader();
		
		if(fade > 0) {
			float factor = std::clamp(fade * 4.0f, 0.0f, 1.0f);
			draw.FilledRect({0,0}, {WND_WIDTH,WND_HEIGHT}, olc::Colour::VERY_DARK_GREY, 
								olc::Pixel(255,255,255,(unsigned char)(255 * factor * factor)));

			fade -= fElapsedTime;
		}
		
		total_time += fElapsedTime;

		return true;
	}
};

int main(void)
{
	ksx_pewpew pge;
	olc::PGEConfig cfg;

	cfg.vScreenSize = {WND_WIDTH, WND_HEIGHT};
	cfg.vPixelSize = {1,1};
	cfg.sAppName = "olcJam2026: CHANGE";
	cfg.bVSync = true;

	if (pge.Construct(cfg))
		pge.Start();

	return 0;
}