#define MAX_PARTICLES 32

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
	olc::vf2d pos, spd;
	bool active;
	int type;
	float lifetime;
	int damage;
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

void particle_add(olc::vf2d pos, olc::vf2d speed, int type = 0, int damage = 1)
{
	particle pnew = {pos, speed, true, type, 0, damage};
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
		pstack[i].lifetime += dt;

		if(pstack[i].lifetime > 5.0f || pstack[i].pos.y > 0 || pstack[i].pos.y < -200.0f)
			pstack[i].active = false;
	}
}

void particle_draw(particle *pstack)
{
	olc::mf4d world, scale;
	
	scale.scale(2.0f, 2.0f, 2.0f);
	

	for(int i = 0; i < MAX_PARTICLES; i++) {
		if(!pstack[i].active) continue;

		particle_count++;

		world.identity();
		world.translate(pstack[i].pos.x, pstack[i].pos.y, 0.0f);
		draw.SetModelMatrix(world * scale);

		switch(pstack[i].type) {
		case BULLET_PLAYER_W1:
		case BULLET_PLAYER_W2:
		case BULLET_PLAYER_W3:
			//mesh_draw(mesh::bullet_stick, false, olc::Colour::TANGERINE);
			mesh_draw(mesh::bullet_ball, false, olc::Colour::TANGERINE);
			break;

		case BULLET_ENEMY_PINK:
			mesh_draw(mesh::bullet_ball, false, olc::Pixel(239,41,115));
			break;

		case BULLET_ENEMY_BLUE:
			mesh_draw(mesh::bullet_ball, false, olc::Pixel(49,189,247));
			break;

		case PARTICLE_SPARK:
		case PARTICLE_EXPLOSION:
		case PARTICLE_SMOKE:
			break;

		default:
			break;
		}
	}
}