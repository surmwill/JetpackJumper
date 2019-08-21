#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <array>
#include "ParticleSystem.hpp"
#include <irrKlang.h>

class SceneNode;
class Collider;

class Player {
public:
	bool toggle_always_wind = false;

	// jet particles
	static const int MAX_PARTICLES;
	static const int PARTICLES_PER_SECOND;
	static const float PARTICLE_LIFE;

	Player( const glm::mat4& init, float gravity );
	~Player();
	glm::vec4 getPosition();
	const glm::mat4 getModelMatrix();
	void updateCameraStatus( double delta_time );
	glm::vec3 getCameraPosition();
	glm::vec3 getCameraLookAt();

	glm::vec3& getVelocity();


	void faceLeft();
	void faceRight();
	void faceForward();

	void reduceWindSpeed();
	void moveLeft();
	void moveRight();
	void moveForward();
	void moveBackward();
	void moveUp();
	void moveDown();
	void jump();
	void stop();
	float getMoveSpeed();
	void toggleGameMode();
	bool getGameMode();
	void updatePosition( const std::vector<Collider*>& building_colliders, const std::vector<Collider*>& orb_colliders, int dir, float delta_time );
	float getJumpSpeed();
	
	void updatePosition( const std::vector<Collider*>& building_colliders, const std::vector<Collider*>& orb_colliders, float delta_time );
	ParticleSystem& getLeftFootParticleSystem();

private:
	enum Facing{ left, forward, right };
	enum CameraStatus{ normal, falling, hitWall };
	Facing prev_face = forward;
	CameraStatus camera_status = normal;

	irrklang::ISoundEngine* engine;

	// player
	const float CAMERA_UP = 5.0f, CAMERA_BACK = 25.0f; // 10.0f up
	const float tol = 0.01f;
	const float speed_increase = 5.0f;
	const float wind_increase = 2.5f;

	bool in_air = false, game_mode = true;
	float move_speed = 40.0f;
	float jump_speed = 50.0f;
	float wind_speed = 2.5f;
	float xz_rot = 0.0f;
	
	glm::vec4 init_pos{ 0, 0, 0, 1 };	
	glm::mat4 trans{ 1.0f };
	glm::mat4 rot{ 1.0f };

	glm::vec3 vel{ 0.0f, 0.0f, 0.0f -move_speed };
	glm::vec3 vel_wind{ 0.0f, 0.0f, 0.0f };
	glm::vec3 accel;
	glm::vec3 accel_wind;
	float camera_extra_dist = 0.0f;

	const float BOUNDING_R_X = 2.1,BOUNDING_R_Y = 5.8, BOUNDING_R_Z = 1.15;	
	glm::mat4 cob_ellipse;
	glm::mat4 cob_r3;

	ParticleSystem left_foot_ps;

	bool checkCollisions( glm::vec3 vel, const std::vector<Collider*>& colliders, bool remove = true );
};
