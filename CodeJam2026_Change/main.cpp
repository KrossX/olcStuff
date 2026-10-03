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
#include "data/noise.h"
#include "data/palette.h"
#include "data/snd_boom.h"
#include "data/snd_hit.h"
#include "data/snd_shoot.h"
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
#define MAX_ENEMIES 32
#define MAX_PARTICLES 64
#define MAX_WEAPONS 3

#include "audio.cpp"
#include "enemies.cpp"
#include "input.cpp"
#include "particles.cpp"

	bool isFullscreen = false;

	olc::gpu::Shader_GLSL33 bg_render, main_render;
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
	} player;

	double world_posx = 0;


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

		if(!compile_shader(main_render, main_vs, main_ps))
			return false;

		bg_render.CreateUniform("bg_offset");
		bg_render.CreateUniform("bg_region");

		CreateImageFromMemory(bg_image, bin2h::noise_png, sizeof(bin2h::noise_png), {true});
		CreateImageFromMemory(palette, bin2h::palette_png, sizeof(bin2h::palette_png));

		load(mesh::ship, bin2h::obj::ship, bin2h::obj::ship_cnt);
		load(mesh::bullet_ball, bin2h::obj::bullet_ball, bin2h::obj::bullet_ball_cnt);
		load(mesh::bullet_stick, bin2h::obj::bullet_stick, bin2h::obj::bullet_stick_cnt);
		load(mesh::enemy_saucer, bin2h::obj::saucer, bin2h::obj::saucer_cnt);

		matProj.perspective(25.0f * 3.14159f / 180.0f, float(WND_WIDTH) / float(WND_HEIGHT), 30.0f, 400.0f);
		draw.SetProjectionMatrix(matProj);

		draw.SetCullMode(olc::CullMode::ClockWise);


		weapon_init();
		particle_init();
		enemy_init();

		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		static bool firstboot = true;

		if(!IsFocused()) {
			draw.Clear(olc::Colour::VERY_DARK_GREY);
			
			float factor = 1.0f - (float)abs(std::sin(TotalTimeElapsed() * 2.0));

			olc::Pixel col = olc::PixelLerp(olc::Colour::GREY, olc::Colour::WHITE, factor);
			
			std::string msg = firstboot? "START" : "CONTINUE";
			
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
		
		firstboot = false;
		
		
		check_input(fElapsedTime);
		Weapon_step(fElapsedTime);
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
		draw.ImageRect(bg_image, {0,0}, {WND_WIDTH,WND_HEIGHT});

		// DRAWING PLAYFIELD

		draw.EnableDepth(true);
		draw.SetCullMode(olc::CullMode::ClockWise);
		draw.SetShader(main_render);

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

		enemy_count = 0;
		enemy_draw();

		particle_count = 0;
		particle_draw(particle_misc);
		particle_draw(enemy_bullet);
		particle_draw(player_bullet);

		// DRAWING UI

		draw.ResetShader();

		auto debug_msg = std::format("POS:{:4.2f},{:4.2f} BALLS:{:d}, ENEMIES:{:d}", (float)player.posx, -player.posy, particle_count, enemy_count);
		draw.String({ 5, 5 }, debug_msg, olc::Colour::BLACK);
		draw.String({ 4, 4 }, debug_msg, olc::Colour::TANGERINE);

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
	cfg.bVSync = false;

	if (pge.Construct(cfg))
		pge.Start();

	return 0;
}