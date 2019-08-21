#pragma once
#include <vector>
#include "Building.hpp"
#include "Orb.hpp"
#include "Block.hpp"
#include <glm/glm.hpp>

#define NUM_BUILDINGS 3 // prime

class SceneNode;

class BuildingManager {
public:
	BuildingManager( 
		SceneNode* building_node, SceneNode* orb_node, SceneNode* block_node,
		float half_building, float gravity, float jump_speed, float aspect, float fov );

	const std::vector<Building>& getBuildings();
	const std::vector<Orb>& getOrbs();
	const std::vector<Block>& getBlocks();

	std::vector<Collider*> getBuildingColliders();
	const std::vector<Collider*>& getOrbColliders();

	void generateBuilding( float player_speed );
	void spawnIfPossible( float player_z, float player_speed );

private:
	const float orb_size = 4.0f;
	const float block_size_x = 15.0f;
	const float block_size_z = 10.0f;

	glm::vec3 generateOrb( const glm::mat4& trans );
	void generateBlock( const glm::mat4& trans );
	void generateBlock( const glm::mat4& trans, const glm::vec3& rand_vec );

	std::vector<Building> buildings;
	std::vector<Collider*> building_colliders;

	std::vector<Orb> orbs;
	std::vector<Collider*> orb_colliders;

	std::vector<Block> blocks;
	std::vector<Collider*> block_colliders;

	SceneNode* building_node, *orb_node, *block_node;
	int next_building = 0, next_orb = 0, next_block = 0;
	float half_building, air_time, aspect, fov;
};
