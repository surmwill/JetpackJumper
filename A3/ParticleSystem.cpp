#include "ParticleSystem.hpp"

// #include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/io.hpp>
// #include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include <math.h>
#include <iostream>
#include <cstdlib>
#include <algorithm>

using namespace std;
using namespace glm;

ParticleSystem::ParticleSystem( int max_particles, int particles_per_second, float particle_life, const glm::vec3& origin ) : 
	max_particles{ max_particles}, particles_per_second{ particles_per_second }, particle_life{ particle_life }, origin{ origin }
{
	particle_container.resize( max_particles );
	particle_pos_size_data.resize( max_particles * 4 );
	particle_col_data.resize( max_particles * 4 );
}

int ParticleSystem::findUnusedParticle() {
	for( int i = last_used_particle; i < max_particles; i++ ) {
		if(	particle_container[i].life < 0 ) {
			last_used_particle = i;
			return i;
		}
	}
	for( int i = 0; i < last_used_particle; i++ ) {
		if(	particle_container[i].life < 0 ) {
			last_used_particle = i;
			return i;
		}
	}
	return 0;	// guess we'll just override the first one
}

void ParticleSystem::spawnNewParticles( double delta_time ) {
	if( delta_time > 0.016f ) delta_time = 0.016f;	// cap at 60fps so we dont spawn too many particles too fast
	int new_particles = (int)( delta_time * particles_per_second );
	// int new_particles = particles_per_second;
	for( int i = 0; i < new_particles; i++ ) {
		const int i_particle = findUnusedParticle();
		particle_container[i_particle].life = particle_life;
		particle_container[i_particle].pos = origin;

		const float spread = 1.5f;
		vec3 main_dir = vec3( 0.0f, -10.0f, 0.0f );
		const float alpha = rand() % 16;
		const float beta = rand() % 361;
		main_dir = rotate( main_dir, radians( alpha ), vec3( 0, 0, 1 ) );
		main_dir = rotate( main_dir, radians( beta ), vec3( 0, 1, 0 ) );

		/*
		const vec3 random_dir = vec3( 
			( rand() % 2000 - 1000.0f ) / 1000.0f,
			( rand() % 2000 - 1000.0f ) / 1000.0f,
			( rand() % 2000 - 1000.0f ) / 1000.0f
		);
		particle_container[i_particle].vel = main_dir + ( random_dir * spread );
		*/

		particle_container[i_particle].vel = main_dir;

		particle_container[i_particle].col = vec4( col_yellow, rand() / (double)(RAND_MAX) );
		
		/*	
		particle_container[i_particle].col = vec4( 
			rand() / (double)(RAND_MAX),
			rand() / (double)(RAND_MAX),
			rand() / (double)(RAND_MAX),
			rand() / (double)(RAND_MAX)
		);
		*/
		
		particle_container[i_particle].size = (rand()%1000) / 2000.0f + 0.1f;
	}
}

int ParticleSystem::updateParticles( double delta_time, const vec3& camera_pos ) {
	spawnNewParticles( delta_time );
	int particle_count = 0;	

	for( Particle& p : particle_container ) {
		if( p.life > 0 ) {
			p.life -= delta_time;
			if( p.life > 0 ) {
				p.vel += vec3( 0.0f, -9.81f, 0.0f ) * (float)delta_time * 0.5f;	// change in vel
				p.pos += p.vel * (float)delta_time;	// change in position
				p.camera_distance = pow( length( p.pos - camera_pos ), 2 );
				const vec3 trans_pos = vec3( trans * vec4( p.pos, 1.0f ) );
				const float t = p.life / particle_life;
				const vec3 new_col = t * col_yellow + ( 1 - t ) * col_red;
				p.col = vec4( new_col, p.col.w );

				particle_pos_size_data[4 * particle_count + 0] = trans_pos.x;
				particle_pos_size_data[4 * particle_count + 1] = trans_pos.y;
				particle_pos_size_data[4 * particle_count + 2] = trans_pos.z;
				particle_pos_size_data[4 * particle_count + 3] = p.size;

				particle_col_data[4 * particle_count + 0] = p.col.x;
				particle_col_data[4 * particle_count + 1] = p.col.y;
				particle_col_data[4 * particle_count + 2] = p.col.z;
				particle_col_data[4 * particle_count + 3] = p.col.w;
			}
			else p.camera_distance = -1.0f;		// just died
			particle_count++;
		}
	}
	sort( particle_container.begin(), particle_container.end() ); // -1 camera distances in back
	return particle_count;
}

const float* ParticleSystem::getParticlePosSizeData() const { return &particle_pos_size_data[0]; }

const float* ParticleSystem::getParticleColData() const { return &particle_col_data[0]; }

void ParticleSystem::updateTrans( const mat4& new_trans ) { trans = new_trans; }
