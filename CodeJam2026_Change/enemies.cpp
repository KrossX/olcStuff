enum {
	ENEMY_SAUCER = 0,
	ENEMY_SENTRY,
	ENEMY_RAPTOR,
	ENEMY_SUZANNE
};

enum {
	ENEMY_ENTER = 0,
	ENEMY_NORMAL,
	ENEMY_EXIT
};

enum {
	ENEMY_LEVEL_LOW = 1,
	ENEMY_LEVEL_MID,
	ENEMY_LEVEL_HIGH,
	ENEMY_LEVEL_BOSS
};


struct enemy {
	olc::vf2d pos, spd;
	bool active;
	int type;
	int level;
	int state;
	int pattern;
	float timer;
	float red;
	float hp;
};

enemy enemy_list[MAX_ENEMIES];
int enemy_count;

void enemy_init(void)
{
	for(int i = 0; i < MAX_ENEMIES; i++) {
		enemy_list[i].active = false;
	}
}

void enemy_add(olc::vf2d pos, olc::vf2d speed, int type, int level, float timer, float hitpoints)
{
	for(enemy &e : enemy_list) {
		if(e.active) continue;
		e = {pos, speed * level * 0.3f, true, type, level, ENEMY_ENTER, 0, timer, 0, hitpoints * level};
		return;
	}

	//int newposx = (int)world_posx + (rand() % 50 - 25) * 4;
	//enemy_pos[idx] = {(float)newposx, -300.0f - (rand() % 10) * 4.0f};
	//enemy_timer[idx] = -1.0f - (rand() % 800) / 800.0f;
}


// shooting 0.38

void enemy_step(float dt)
{
	for(enemy &e : enemy_list) {
		if(!e.active) continue;

		e.pos += e.spd * dt;
		e.timer += dt;
		e.red -= dt * 3.0f;

		float aposx = std::abs(e.pos.x - (float)world_posx);
		float edge = (30.0f - e.pos.y) * 0.38f;

		switch(e.state) {
		case ENEMY_ENTER:
			if(aposx < edge)
				e.state = ENEMY_NORMAL;
			else if(e.timer > 5.0f)
				e.active = false;
			break;

		case ENEMY_NORMAL: {
			float rate = 2.0f - e.level * 0.3f;
			rate *= enemy_weapon_rate;
			
			if(e.timer > rate) {
				e.timer -= rate;
				
				switch(e.type) {
				case ENEMY_SAUCER: 
					if(aposx < edge && e.pos.y < -20.0f) {
						olc::vf2d ppos((float)player.posx, player.posy);
						olc::vf2d diff = ppos - e.pos;
						particle_add(e.pos, diff.norm() * 50.0f, {0,0}, BULLET_ENEMY_PINK);
					}
					break;
					
				case ENEMY_SENTRY:
					if(aposx < edge && e.pos.y < -20.0f) {
						olc::vf2d ppos((float)player.posx, player.posy);
						olc::vf2d diff = ppos - e.pos;
						particle_add(e.pos, diff.norm() * 60.0f, {0,0}, BULLET_ENEMY_BLUE);
					}
					break;

				case ENEMY_RAPTOR:
					if(aposx < edge && e.pos.y < -20.0f) {
						olc::vf2d ppos((float)player.posx, player.posy);
						olc::vf2d diff = olc::vf2d(ppos - e.pos).norm();
						particle_add(e.pos, diff * 80.0f, {0,0}, BULLET_ENEMY_PINK);
						particle_add(e.pos - diff, diff * 80.0f, {0,0}, BULLET_ENEMY_PINK);
					}
					break;

				case ENEMY_SUZANNE:
					e.timer += rate * 0.5f;
				
					olc::vf2d ppos((float)player.posx, player.posy);

					olc::vf2d npos = olc::vf2d(e.pos.x - 10.0f, e.pos.y);
					olc::vf2d diff = ppos - npos;
					particle_add(npos, diff.norm() * 100.0f, {0,0}, BULLET_ENEMY_BLUE);
					
					npos.x = e.pos.x + 10.0f;
					diff = ppos - npos;
					particle_add(npos, diff.norm() * 100.0f, {0,0}, BULLET_ENEMY_PINK);
					audio_playpan(SND_SHOOT2, e.pos.x, e.pos.y);
					break;
				}
			}
			
			
			switch(e.type) {
				case ENEMY_SAUCER: 
					e.spd.y += dt * 10.0f;
					break;
				
				case ENEMY_RAPTOR:
					e.spd.x -= (e.pos.x - (float)player.posx) * dt;
					e.spd.y -= (e.pos.y - player.posy) * dt;
					break;

				case ENEMY_SUZANNE:
					if(e.pos.y > -200.0f) e.pos.y = -200.0f;
					break;
			}
			
			if(aposx > edge || e.pos.y > 0.0f) {
				e.state = ENEMY_EXIT;
				e.timer = 0;
			}
			} break;

		case ENEMY_EXIT:
			if(aposx > (edge + 5.0f) || e.pos.y > 4.0f || e.timer > 3.0f)
				e.active = false;
			break;
		}
	}
}

void enemy_draw(void)
{
	for(enemy &e : enemy_list) {
		if(!e.active) continue;

		enemy_count++;
		
		olc::Pixel hitcol = e.red > 0 ? PixelLerp(olc::Colour::WHITE, olc::Colour::RED, e.red * e.red) : olc::Colour::WHITE;
		olc::mf4d world, scale, rotZ, rotY;
		
		float angle, sz = 1.0f + e.level;
		scale.scale(sz, sz, sz);
		
		switch(e.type) {
		case ENEMY_SAUCER:
			rotZ.rotateZ((float)total_time * 2.0f);
			world.translate(e.pos.x, e.pos.y, 0.0f);
			draw.SetModelMatrix(world * scale * rotZ);
			mesh_draw(mesh::enemy_saucer, true, hitcol);
			break;

		case ENEMY_SENTRY:
			rotZ.rotateZ((float)total_time * -0.5f);
			world.translate(e.pos.x, e.pos.y, 0.0f);
			draw.SetModelMatrix(world * scale * rotZ);
			mesh_draw(mesh::enemy_sentry, true, hitcol);
			break;

		case ENEMY_RAPTOR:
			angle = std::atan2f(-e.spd.x, e.spd.y);
			rotY.rotateY((float)total_time * 4.0f);
			rotZ.rotateZ(angle);
			world.translate(e.pos.x, e.pos.y, 0.0f);
			draw.SetModelMatrix(world * scale * rotZ * rotY);
			mesh_draw(mesh::enemy_raptor, true, hitcol);
			break;

		case ENEMY_SUZANNE:
			//rotZ.rotateZ((float)total_time * 2.0f);
			sz = 30.0f;
			scale.scale(sz, sz, sz);
			world.translate(e.pos.x, e.pos.y, -6.0f);
			draw.SetModelMatrix(world * scale * rotZ);
			mesh_draw(mesh::enemy_suzanne, true, hitcol);
			break;
		
		default:
			world.translate(e.pos.x, e.pos.y, 0.0f);
			draw.SetModelMatrix(world * scale * rotZ);
			mesh_draw(mesh::bullet_ball, false, olc::Colour::MAGENTA);
			break;
		}
	}
}

void check_enemy_dead()
{
	for(enemy &e : enemy_list) {
		if(!e.active) continue;

		for(particle &p : player_bullet) {
			if(!p.active) continue;

			olc::vf2d dist = e.pos - p.pos;
			if(e.type == ENEMY_SUZANNE) dist *= 0.25;
			
			if(dist.mag2() < 9.0) {
				p.active = false;

				e.hp -= p.damage;
				if(e.hp <= 0) {
					e.active = false;
					particle_add(e.pos, e.spd * 0.5f, {0,0}, PARTICLE_EXPLOSION);
					audio_playpan(SND_BOOM, e.pos.x, e.pos.y);
					
					if(e.type == ENEMY_SUZANNE) {
						boss_active = false;
						boss_killed = true;
						current_scene = SCENE_GAMEOVER;
					}
					
					if(((rand() % 800) / 800.0f) < 0.03) {
						particle_add(e.pos, {0.0f, 10.0f}, {0,0}, BULLET_ENEMY_POWERUP, 0, 3.0f);
					}
				} else {
					e.red = 1.0f;

					for(int i = 0; i < 3; i++) {
						float angle = (float)rand() / (float)RAND_MAX * 3.14159f * 2.0f;
						float speed = (float)rand() / (float)RAND_MAX * 10.0f + 20.0f;
						float spdx = e.spd.x * 0.5f + std::sinf(angle) * speed;
						float spdy = e.spd.y * 0.5f + speed;
						float spdz = std::cosf(angle) * speed;

						particle_add(e.pos, {spdx, spdy}, {0,spdz}, PARTICLE_SPARK);
					}

					audio_playpan(SND_HIT, e.pos.x, e.pos.y);
				}
			}
		}
	}
}

enum {
	ENEMY_SPAWN_SAUCER_LEFT = 0,
	ENEMY_SPAWN_SAUCER_RIGHT,
	ENEMY_SPAWN_RAPTOR_LEFT,
	ENEMY_SPAWN_RAPTOR_RIGHT,
	ENEMY_SPAWN_SENTRY,
	ENEMY_SPAWN_SUZANNE,
	ENEMY_SPAWN_TOTAL
};


void enemy_factory(int spawntype, int level)
{
	//olc::vf2d pos = { (float)world_posx + (30.0f +10.0f) * 0.4f, -10.0f }; //(30.0f - e.pos.y) * 0.38f
	olc::vf2d pos = { (float)world_posx + 30.0f, -40.0f }; //(30.0f - e.pos.y) * 0.38f
	olc::vf2d offset = olc::vf2d(4.0f, 0.0f).norm();
	
	switch(spawntype) {
		// left to right
		case ENEMY_SPAWN_SAUCER_LEFT:	{
			olc::vf2d pos = { (float)world_posx - 30.0f, -40.0f };
			olc::vf2d offset = olc::vf2d(-8.0f, 1.0f).norm();
			for(int i = 0; i < 10; i++) {
				enemy_add(pos + offset*(i+1)*10.0f, -offset * 80.0f, ENEMY_SAUCER, level, 0, 1);
			}
		} break;
		
		// right to left
		case ENEMY_SPAWN_SAUCER_RIGHT: {
			olc::vf2d pos = { (float)world_posx + 30.0f, -40.0f };
			olc::vf2d offset = olc::vf2d(8.0f, 1.0f).norm();
			for(int i = 0; i < 10; i++) {
				enemy_add(pos + offset*(i+1)*10.0f, -offset * 80.0f, ENEMY_SAUCER, level, 0, 1);
			}
		} break;
		
		// left to right
		case ENEMY_SPAWN_RAPTOR_LEFT:	{
			olc::vf2d pos = { (float)world_posx - 100.0f, -200.0f };
			olc::vf2d offset = olc::vf2d(-4.0f, 4.0f).norm();
			for(int i = 0; i < 10; i++) {
				float timer = -1.0f - (rand() % 800) / 800.0f;
				enemy_add(pos + offset*(i+1)*7.0f, -offset * 40.0f, ENEMY_RAPTOR, level, timer, 2);
			}
		} break;
		
		// right to left
		case ENEMY_SPAWN_RAPTOR_RIGHT: {
			olc::vf2d pos = { (float)world_posx + 100.0f, -200.0f };
			olc::vf2d offset = olc::vf2d(4.0f, 4.0f).norm();
			for(int i = 0; i < 10; i++) {
				float timer = -1.0f - (rand() % 800) / 800.0f;
				enemy_add(pos + offset*(i+1)*7.0f, -offset * 40.0f, ENEMY_RAPTOR, level, timer, 2);
			}
		} break;
		
		case ENEMY_SPAWN_SENTRY: {
			for(int i = 0; i < 7; i++) {
				for(int j = 0; j < 5; j++) {
					int newposx = (int)world_posx + (i-3) * 30;
					olc::vf2d pos = {(float)newposx, -300.0f - j * 40.0f};
					//float timer = -1.0f - (rand() % 800) / 800.0f;
					enemy_add(pos, {0, 20.0f}, ENEMY_SENTRY, level, -3.0f, 5);
				}
			}
		} break;
		
		case ENEMY_SPAWN_SUZANNE: {
			float timer = -1.0f - (rand() % 800) / 800.0f;
			enemy_add({(float)world_posx, -300.0f }, {0,10.0f}, ENEMY_SUZANNE, ENEMY_LEVEL_BOSS, 0, 100);
		}
	}
}