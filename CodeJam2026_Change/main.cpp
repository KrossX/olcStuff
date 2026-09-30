/* Copyright (c) 2026 KrossX <krossx@live.com>
 * License: http://www.opensource.org/licenses/mit-license.html  MIT License
 */

#pragma warning(push, 1)
#define OLC_USE_STB_IMAGE
#define OLC_PGE3_APPLICATION
#include "olcPixelGameEngine3.h"
#pragma warning(pop)


#define WND_WIDTH  796
#define WND_HEIGHT 448

//#define WND_WIDTH  398
//#define WND_HEIGHT 224

namespace bin2h
{
#include "data/background_ps.h"
#include "data/main_ps.h"
#include "data/main_vs.h"
#include "data/models_obj.h"
#include "data/noise.h"
#include "data/palette.h"
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
	} player;
	
	double world_posx = 0;
	
	void check_input(float dt)
	{
		float roll = 0;
		
		if (keyboard.GetKey(olc::Key::F11).bPressed) {
			isFullscreen = !isFullscreen;
			ShowFullScreen(isFullscreen);
		}
		
		if (keyboard.GetKey(olc::Key::UP).bHeld) {
			player.posy -= 20.0f * dt;
			if(player.posy < -20.0f) player.posy = -20.0f;
		}
		
		if (keyboard.GetKey(olc::Key::DOWN).bHeld) {
			player.posy += 20.0f * dt;
			if(player.posy > 0) player.posy = 0;
		}

		if (keyboard.GetKey(olc::Key::LEFT).bHeld) {
			player.posx -= 20.0 * dt;
			roll -= dt;
			
			if((player.posx - world_posx) < -11.0) {
				world_posx -= 20.0 * dt;
			}
		}
		
		if (keyboard.GetKey(olc::Key::RIGHT).bHeld) { 
			player.posx += 20.0 * dt;
			roll += dt;
			
			if((player.posx - world_posx) > 11.0f) {
				world_posx += 20.0 * dt;
			}
		}
		
		if(std::abs(roll) < dt) {
			player.roll += -player.roll * dt * 8.0f;
		} else {
			player.roll += roll * 8.0f;
			player.roll = player.roll < -1 ? -1 : player.roll > 1 ? 1 : player.roll;
		}
		
		if(keyboard.GetKey(olc::Key::Z).bPressed) {
			particle_add({(float)player.posx,player.posy}, {0.0f,-80.0f}, 1);
		}
	}
	
	struct particle {
		olc::vf2d pos, spd;
		bool active;
		int type;
		float lifetime;
	};
	
#define MAX_BULLETS 64	
	
	particle enemy_bullet[MAX_BULLETS];
	particle player_bullet[MAX_BULLETS];
	
	void particle_add(olc::vf2d pos, olc::vf2d speed, int type = 0)
	{
		particle p = {pos, speed, true, type, 0};
		particle *set = type == 1 ? player_bullet : enemy_bullet;
				
		for(int i = 0; i < MAX_BULLETS; i++) {
			if(set[i].active) continue;
			set[i] = p;
			break;
		}
	}
	
	void particle_step(particle *set, float dt)
	{
		for(int i = 0; i < MAX_BULLETS; i++) {
			if(!set[i].active) continue;
			
			set[i].pos += set[i].spd * dt;
			set[i].lifetime += dt;
			
			if(set[i].lifetime > 5.0f || set[i].pos.y > 0 || set[i].pos.y < -200.0f)
				set[i].active = false;
			
		}
	}
	
	int particle_count;
	
	void particle_draw(particle *set)
	{
		olc::mf4d world;
	
		for(int i = 0; i < MAX_BULLETS; i++) {
			if(!set[i].active) continue;
			
			particle_count++;
			
			world.identity();
			world.translate(set[i].pos.x, set[i].pos.y, 0.0f);
			draw.SetModelMatrix(world);

			//draw.Mesh(mesh::bullet_ball.layout, mesh::bullet_ball.pos, mesh::bullet_ball.col, mesh::bullet_ball.uv, palette);
			
			if(set[i].type == 0) {
				//draw.Mesh(mesh::bullet_ball.layout, mesh::bullet_ball.pos, mesh::bullet_ball.col, olc::Colour::MAGENTA);
				mesh_draw(mesh::bullet_ball, false, olc::Colour::MAGENTA);
			} else {
				//draw.Mesh(mesh::bullet_stick.layout, mesh::bullet_stick.pos, mesh::bullet_stick.col, olc::Colour::CYAN);
				mesh_draw(mesh::bullet_stick, false, olc::Colour::RED);
			}
		}
	}
	
	bool compile_shader(olc::gpu::Shader_GLSL33 &prog, std::string vs_main, std::string ps_main)
	{
		prog.SetVertexShaderSource(olc::gpu::Shader::VS_DefaultHeader() + vs_main);
		prog.SetPixelShaderSource(olc::gpu::Shader::PS_DefaultHeader() + ps_main);
		
		std::string shader_error = prog.Compile();
		
		if(shader_error != "OK") {
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
	
#define MAX_ENEMIES 30
	olc::vf2d enemy_pos[MAX_ENEMIES];
	float enemy_timer[MAX_ENEMIES];
	
	void enemy_reset(int idx)
	{
		int newposx = (int)world_posx + (rand() % 50 - 25) * 4;
		
		enemy_pos[idx] = {(float)newposx, -300.0f - (rand() % 10) * 4.0f};
		enemy_timer[idx] = -1.0f - (rand() % 800) / 800.0f;
	}
	
	void enemy_step(float dt)
	{
		for(int i = 0; i < MAX_ENEMIES; i++) {
			enemy_pos[i].y += dt * 40.0f;
			enemy_timer[i] += dt;
			
			if(enemy_timer[i] > 1) {
				enemy_timer[i] -= 1.0f;
				
				if(enemy_pos[i].y < -20.0f) {
					olc::vf2d ppos((float)player.posx, player.posy);
					olc::vf2d diff = ppos - enemy_pos[i];
					particle_add(enemy_pos[i], diff.norm() * 100.0f);
				}
			}

			if(enemy_pos[i].y > 4.0f) {
				enemy_reset(i);
			}
		}
	}
	
	void enemy_draw(void)
	{
		for(int i = 0; i < MAX_ENEMIES; i++) {
			olc::mf4d world, scale, rotate;

			rotate.rotateZ((float)TotalTimeElapsed() * 2.0f);
			scale.scale(3.0f, 3.0f, 3.0f);
			world.translate(enemy_pos[i].x, enemy_pos[i].y, 0.0f);
			
			draw.SetModelMatrix(world * scale * rotate);
			
			mesh_draw(mesh::enemy_saucer, true);
		}
	}
	
	void check_enemy_dead()
	{
		for(int i = 0; i < MAX_ENEMIES; i++) {
			for(int j = 0; j < MAX_BULLETS; j++) {
				if(!player_bullet[j].active) continue;
			
				olc::vf2d dist = enemy_pos[i] - player_bullet[j].pos;
				if(dist.mag2() < 9.0) {
					enemy_reset(i);
					player_bullet[j].active = false;
				}
			}
		}
	}
	

public:
	bool OnUserCreate() override
	{
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
		
		for(int i = 0; i < MAX_BULLETS; i++) {
			player_bullet[i].active = false;
			enemy_bullet[i].active = false;
		}
		
		for(int i = 0; i < MAX_ENEMIES; i++) {
			enemy_reset(i);
		}

		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		
		check_input(fElapsedTime);
		particle_step(player_bullet, fElapsedTime);
		
		check_enemy_dead();
		
		particle_step(enemy_bullet, fElapsedTime);
		enemy_step(fElapsedTime);
		
		draw.Clear(olc::Colour::BLACK);
		
		bg_off.y -= fElapsedTime * 0.25f;
		if(bg_off.y < -1) bg_off.y += 1;
		
		bg_off.x = (float)world_posx * -0.02f;
		
		draw.SetShader(bg_render);
		draw.SetShaderTexture(1, palette);
		draw.SetShaderUniform("bg_offset", bg_off);
		draw.SetShaderUniform("bg_region", bg_region);
		draw.ImageRect(bg_image, {0,0}, {WND_WIDTH,WND_HEIGHT});
		
		draw.SetShader(main_render);
		float tfactor = (float)std::sin(TotalTimeElapsed() * 4.0);
		tfactor = tfactor * tfactor * tfactor;
		
		vViewTranslate.x = -(float)world_posx;

		olc::mf4d matViewRotateX, matViewTranslate;
		matViewRotateX.rotateX(75.0f * 3.14159f / 180.0f);
		matViewTranslate.translate(vViewTranslate);
		matView = matViewTranslate * matViewRotateX;
		draw.SetViewMatrix(matView);
		
		olc::mf4d matTrans, matRotX, matRotY;
		matTrans.translate((float)player.posx, player.posy, 0.0f);
		
		matRotY.rotateY(player.roll * 0.5);
		//matRotX.rotateX(45 * 3.14159f / 180.0f);
		
		matWorld = matTrans * matRotX * matRotY;
		draw.SetModelMatrix(matWorld);
		
		mesh_draw(mesh::ship, true);
		//draw.Mesh(mesh::ship.layout, mesh::ship.pos, mesh::ship.col, mesh::ship.uv, palette);
		//draw.Mesh(mesh::bullet_ball.layout, mesh::bullet_ball.pos, mesh::bullet_ball.col, mesh::bullet_ball.uv, palette);
		
		enemy_draw();
		
		particle_count = 0;
		particle_draw(enemy_bullet);
		particle_draw(player_bullet);
		
		draw.ResetShader();
		
		auto debug_msg = std::format("X:{: 4.2f}  Y:{: 4.2f}, BALLS:{: d}", (float)player.posx, -player.posy, particle_count);
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