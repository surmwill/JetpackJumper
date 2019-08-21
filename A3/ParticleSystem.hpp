#pragma once
#include <glm/glm.hpp>
#include <vector>

class ParticleSystem {
public:
	ParticleSystem( int max_particles, int particles_per_second, float particle_life, const glm::vec3& origin );

	int updateParticles( double delta_time, const glm::vec3& camera_pos );
	const float* getParticlePosSizeData() const;
	const float* getParticleColData() const;
	void updateTrans( const glm::mat4& new_trans );

private:
	struct Particle {
		glm::vec3 pos, vel;
		glm::vec4 col;
		float size, angle, weight;
		float life = -1.0f;
		float camera_distance = -1.0f;	// for blending

		bool operator<( const Particle& other ) const {
			return this->camera_distance > other.camera_distance; // note need to change later
		}		
	};

	void spawnNewParticles( double delta_time );
	int findUnusedParticle();

	std::vector<Particle> particle_container;
	std::vector<float> particle_pos_size_data;
	std::vector<float> particle_col_data;

	glm::vec3 col_yellow{ 1, 1, 0.2 };
	glm::vec3 col_red{ 1, 0, 0 };

	int max_particles, particles_per_second;
	float particle_life;
	glm::vec3 origin;
	glm::mat4 trans{ 1.0f };
	int last_used_particle = 0;		// previously used and ready to be reborn
};
