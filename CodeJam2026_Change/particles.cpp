enum {
	BULLET_PLAYER_W1 = 0,
	BULLET_PLAYER_W2,
	BULLET_PLAYER_W3,

	BULLET_ENEMY_PINK,
	BULLET_ENEMY_BLUE,

	PARTICLE_SPARK,
	PARTICLE_EXPLOSION,
	PARTICLE_SMOKE
};

struct particle {
	olc::vf2d pos, spd, psz;
	bool active;
	int type;
	float lifetime;
	float damage;
};

particle enemy_bullet[MAX_PARTICLES];
particle player_bullet[MAX_PARTICLES];
particle particle_misc[MAX_PARTICLES];

int particle_count;

void particle_init(void)
{
	for(int i = 0; i < MAX_PARTICLES; i++) {
		enemy_bullet[i].active = false;
		player_bullet[i].active = false;
		particle_misc[i].active = false;
	}
}

void particle_add(olc::vf2d pos, olc::vf2d speed, olc::vf2d spz, int type, float damage = 1.0f)
{
	particle pnew = {pos, speed, spz, true, type, 0, damage};
	particle *pstack = nullptr, *oldest;

	switch(type) {
	case BULLET_PLAYER_W1:
	case BULLET_PLAYER_W2:
	case BULLET_PLAYER_W3:
		pstack = player_bullet;
		break;

	case BULLET_ENEMY_PINK:
	case BULLET_ENEMY_BLUE:
		pstack = enemy_bullet;
		break;

	case PARTICLE_SPARK:
	case PARTICLE_EXPLOSION:
	case PARTICLE_SMOKE:
		pstack = particle_misc;
		break;

	default:
		return;
	}

	for(int i = 0; i < MAX_PARTICLES; i++) {
		if(pstack[i].active) continue;
		pstack[i] = pnew;
		return;
	}

	oldest = pstack;

	for(int i = 0; i < MAX_PARTICLES; i++) {
		if(pstack[i].lifetime > oldest->lifetime)
			oldest = &pstack[i];
	}

	*oldest = pnew;
}

void particle_step(particle *pstack, float dt)
{
	for(int i = 0; i < MAX_PARTICLES; i++) {
		if(!pstack[i].active) continue;

		pstack[i].pos += pstack[i].spd * dt;
		pstack[i].psz.x += pstack[i].psz.y * dt;

		pstack[i].lifetime += dt;

		if(pstack[i].lifetime > 10.0f || pstack[i].pos.y > 0 || pstack[i].pos.y < -200.0f)
			pstack[i].active = false;
	}
}

void particle_draw_silhouette(olc::vf2d pos, float sz, mesh::mesh &m, olc::Pixel col)
{
	olc::mf4d world, scale;

	world.identity();
	world.translate(pos.x, pos.y, 0.0f);

	scale.scale(sz+0.2f, sz+0.2f, sz+0.2f);
	draw.SetModelMatrix(world * scale);
	draw.SetCullMode(olc::CullMode::CounterClockWise);
	mesh_draw(m, false, olc::Colour::BLACK);

	scale.scale(sz, sz, sz);
	draw.SetModelMatrix(world * scale);
	draw.SetCullMode(olc::CullMode::ClockWise);
	mesh_draw(m, false, col);
}

void particle_draw_smoke(particle &smoke)
{
	olc::mf4d world, scale;

	world.identity();
	world.translate(smoke.pos.x, smoke.pos.y, 0.0f);

	float sz = smoke.lifetime;
	scale.scale(sz, sz, sz);
	draw.SetModelMatrix(world * scale);
	mesh_draw(mesh::bullet_ball, false, olc::Colour::BLACK);
}

void particle_draw_explosion(particle &boom)
{
	if(boom.lifetime >= 1.0f) {
		boom.active = false;
		return;
	}

	olc::mf4d world, scale;

	world.identity();
	world.translate(boom.pos.x, boom.pos.y, 0.0f);

	//float sz = 5.0f + boom.lifetime * 20.0f;
	float factor = boom.lifetime - 1.0f;
	factor = factor * factor;

	float sz = 5.0f + (1.0f-factor)*30.0f;
	scale.scale(sz, sz, sz);
	draw.SetModelMatrix(world * scale);

	draw.EnableDepth(false);
	int alpha = (int)(factor * 255.0f);
	mesh_draw(mesh::bullet_ball, false, olc::Pixel(255,255,0,alpha));
	draw.EnableDepth(true);
}

void particle_draw_sparks(particle &spark)
{
	if(spark.lifetime >= 1.0f) {
		spark.active = false;
		return;
	}

	olc::mf4d world, scale;

	world.identity();
	world.translate(spark.pos.x, spark.pos.y, spark.psz.x);

	float sz = 0.5f - spark.lifetime * 0.4f;
	scale.scale(sz, sz, sz);
	draw.SetModelMatrix(world * scale);

	draw.EnableDepth(false);
	olc::Pixel col = PixelLerp(olc::Colour::WHITE, olc::Colour::TANGERINE,  spark.lifetime);
	mesh_draw(mesh::bullet_ball, false, col);
	draw.EnableDepth(true);
}

void particle_draw(particle *pstack)
{
	for(int i = 0; i < MAX_PARTICLES; i++) {
		if(!pstack[i].active) continue;

		particle_count++;

		switch(pstack[i].type) {
		case BULLET_PLAYER_W1:
			particle_draw_silhouette(pstack[i].pos, 1.0f, mesh::bullet_ball, olc::Colour::TANGERINE);
			break;

		case BULLET_PLAYER_W2:
			particle_draw_silhouette(pstack[i].pos, 1.0f, mesh::bullet_ball, olc::Pixel(140,200,40));
			break;

		case BULLET_PLAYER_W3:
			particle_draw_silhouette(pstack[i].pos, 1.0f, mesh::bullet_ball, olc::Pixel(40,200,100));
			break;

		case BULLET_ENEMY_PINK:
			particle_draw_silhouette(pstack[i].pos, 2.0f, mesh::bullet_ball, olc::Pixel(255,41,115));
			break;

		case BULLET_ENEMY_BLUE:
			particle_draw_silhouette(pstack[i].pos, 2.0f, mesh::bullet_ball, olc::Pixel(49,189,255));
			break;

		case PARTICLE_SPARK:
			particle_draw_sparks(pstack[i]);
			break;

		case PARTICLE_EXPLOSION:
			particle_draw_explosion(pstack[i]);
			break;

		case PARTICLE_SMOKE:
			particle_draw_smoke(pstack[i]);
			break;

		default:
			break;
		}
	}
}