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

namespace bin2h
{
#include "data/background_ps.h"
#include "data/bullet_ball_obj.h"
#include "data/main_vs.h"
#include "data/noise.h"
#include "data/palette.h"
#include "data/ship_obj.h"
}

namespace mesh 
{
	struct mesh
	{
		std::vector<olc::vf4d> pos;
		std::vector<olc::vf4d> norm;
		std::vector<olc::vf2d> uv;
		std::vector<olc::Pixel> col;
		olc::Structure layout = olc::Structure::List;
	};
	
	mesh ship;
	mesh bullet_ball;
	
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
	olc::vf4d vViewTranslate = { 0.0f, 5.0f, -20.0f };
	
	struct {
		olc::vf2d pos;
		float roll = 0;
	} player;
	
	float world_sideways;
	
	void check_input(float dt)
	{
		float roll = 0;
		world_sideways = 0;
		
		if (keyboard.GetKey(olc::Key::UP).bHeld) {
			player.pos.y -= 20.0f * dt;
			if(player.pos.y < -20.0f) player.pos.y = -20.0f;
		}
		
		if (keyboard.GetKey(olc::Key::DOWN).bHeld) {
			player.pos.y += 20.0f * dt;
			if(player.pos.y > 0) player.pos.y = 0;
		}

		if (keyboard.GetKey(olc::Key::LEFT).bHeld) {
			player.pos.x -= 20.0f * dt;
			roll -= dt;
			
			if(player.pos.x < -11.0f) {
				player.pos.x = -11.0f;
				world_sideways = 1.0f;
			}
		}
		
		if (keyboard.GetKey(olc::Key::RIGHT).bHeld) { 
			player.pos.x += 20.0f * dt;
			roll += dt;
			
			if(player.pos.x > 11.0f) {
				player.pos.x = 11.0f;
				world_sideways = -1.0f;
			}
		}
		
		if(std::abs(roll) < dt) {
			player.roll += -player.roll * dt * 8.0f;
		} else {
			player.roll += roll * 8.0f;
			player.roll = player.roll < -1 ? -1 : player.roll > 1 ? 1 : player.roll;
		}
		
		if(keyboard.GetKey(olc::Key::Z).bPressed) {
			particle_add(player.pos, {0,-4000.0f*dt});
		}
	}
	
	int particle_count;
	bool  particle_active[256];
	olc::vf2d particle_pos[256];
	olc::vf2d particle_spd[256];
	
	void particle_add(olc::vf2d pos, olc::vf2d speed)
	{
		for(int i = 0; i < 256; i++) {
			if(particle_active[i]) continue;
			particle_active[i] = true;
			particle_pos[i] = pos;// + olc::vf2d(0,-1.0f);
			particle_spd[i] = speed;
			break;
		}
	}
	
	void particle_step(float dt)
	{
		for(int i = 0; i < 256; i++) {
			if(!particle_active[i]) continue;
			particle_pos[i] += particle_spd[i] * dt;
			
			if(particle_pos[i].y < -80.0f)
				particle_active[i] = false;
			
		}
	}
	
	void particle_draw(void)
	{
		olc::mf4d world;

		particle_count = 0;
		
		for(int i = 0; i < 256; i++) {
			if(!particle_active[i]) continue;
			
			particle_count++;
			
			world.identity();
			world.translate(particle_pos[i].x, particle_pos[i].y, 0.0f);
			draw.SetModelMatrix(world);

			//draw.Mesh(mesh::bullet_ball.layout, mesh::bullet_ball.pos, mesh::bullet_ball.col, mesh::bullet_ball.uv, palette);
			draw.Mesh(mesh::bullet_ball.layout, mesh::bullet_ball.pos, mesh::bullet_ball.col, olc::Colour::MAGENTA);
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
	

public:
	bool OnUserCreate() override
	{
		std::string bg_ps((char*)bin2h::background_ps_glsl, sizeof(bin2h::background_ps_glsl));
		std::string main_vs((char*)bin2h::main_vs_glsl, sizeof(bin2h::main_vs_glsl));
		
		if(!compile_shader(bg_render, olc::gpu::Shader::VS_DefaultMain(), bg_ps))
			return false;
		
		if(!compile_shader(main_render, main_vs, olc::gpu::Shader::PS_DefaultMain()))
			return false;

		bg_render.CreateUniform("bg_offset");
		bg_render.CreateUniform("bg_region");

		CreateImageFromMemory(bg_image, bin2h::noise_png, sizeof(bin2h::noise_png), {true});
		CreateImageFromMemory(palette, bin2h::palette_png, sizeof(bin2h::palette_png));
		
		load(mesh::ship, bin2h::ship_obj, bin2h::ship_obj_total);
		load(mesh::bullet_ball, bin2h::bullet_ball_obj, bin2h::bullet_ball_obj_total);

		matProj.perspective(50.0f * 3.14159f / 180.0f, float(WND_WIDTH) / float(WND_HEIGHT), 0.1f, 100.0f);
		draw.SetProjectionMatrix(matProj);
		
		draw.SetCullMode(olc::CullMode::ClockWise);
		
		for(int i = 0; i < 256; i++) {
			particle_active[i] = false;
		}

		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		
		check_input(fElapsedTime);
		particle_step(fElapsedTime);
		
		
		draw.Clear(olc::Colour::BLACK);
		
		bg_off.y -= fElapsedTime * 0.25f;
		if(bg_off.y < -1) bg_off.y += 1;
		
		bg_off.x += world_sideways * fElapsedTime * 0.5f;
		
		draw.SetShader(bg_render);
		draw.SetShaderTexture(1, palette);
		draw.SetShaderUniform("bg_offset", bg_off);
		draw.SetShaderUniform("bg_region", bg_region);
		draw.ImageRect(bg_image, {0,0}, {WND_WIDTH,WND_HEIGHT});
		
		draw.SetShader(main_render);
		float tfactor = (float)std::sin(TotalTimeElapsed() * 4.0);
		tfactor = tfactor * tfactor * tfactor;
		
		olc::mf4d matViewRotateX, matViewTranslate;
		matViewRotateX.rotateX(60.0f * 3.14159f / 180.0f);
		matViewTranslate.translate(vViewTranslate);
		matView = matViewTranslate * matViewRotateX;
		draw.SetViewMatrix(matView);
		
		olc::mf4d matTrans, matRotX, matRotY;
		matTrans.translate(player.pos.x, player.pos.y, 0.0f);
		
		matRotY.rotateY(player.roll * 0.5);
		//matRotX.rotateX(45 * 3.14159f / 180.0f);
		
		matWorld = matTrans * matRotX * matRotY;
		draw.SetModelMatrix(matWorld);
		
		draw.Mesh(mesh::ship.layout, mesh::ship.pos, mesh::ship.col, mesh::ship.uv, palette);
		//draw.Mesh(mesh::bullet_ball.layout, mesh::bullet_ball.pos, mesh::bullet_ball.col, mesh::bullet_ball.uv, palette);
		
		particle_draw();
		
		draw.ResetShader();

		auto debug_msg = std::format("X:{: 4.2f}  Y:{: 4.2f}, BALLS:{: d}", player.pos.x, -player.pos.y, particle_count);
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

	if (pge.Construct(cfg))
		pge.Start();

	return 0;
}