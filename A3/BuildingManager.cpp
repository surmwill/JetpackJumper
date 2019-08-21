#include "BuildingManager.hpp"
#include "SceneNode.hpp"

#include <iostream>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/io.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace std;
using namespace glm;

BuildingManager::BuildingManager( 
	SceneNode* building_node, SceneNode* orb_node, SceneNode* block_node, float half_building, float gravity, 
	float jump_speed, float aspect, float fov ) : 
		building_node{ building_node }, orb_node{ orb_node }, block_node{ block_node },
		half_building{ half_building }, aspect{ aspect}, fov{ fov }
{
	air_time = jump_speed / ( -gravity / 2.0f );

	buildings.reserve( NUM_BUILDINGS );
	building_colliders.reserve( NUM_BUILDINGS );

	orbs.reserve( NUM_BUILDINGS );
	orb_colliders.reserve( NUM_BUILDINGS );
	
	blocks.reserve( NUM_BUILDINGS );
	block_colliders.reserve( NUM_BUILDINGS );

	buildings.emplace_back( Building{ building_node, mat4{ 1.0f } } );
	building_colliders.emplace_back( &buildings[0] ); 
	next_building = 1;
}

void BuildingManager::generateBuilding( const float player_speed ) {
	mat4 trans;
	if( next_building == 0 ) trans = buildings[NUM_BUILDINGS - 1].getTrans();
	else trans = buildings[next_building - 1].getTrans();

	const float z_displace = player_speed * air_time;
	// const float angle = ( rand() % 60 ) - 30;
	const float angle = rand() % 60 + 60;
	const float add_x = cos( radians( angle ) ) * z_displace; 
	const int add_z = rand() % (int)( half_building * 0.9f );

	/*
	const float y_t = tan( radians( fov / 2.0f ) ) * z_displace;
	const float x_r = ( y_t * aspect ) - half_building;
	const int add_x = rand() % ( (int)( x_r * 2 ) ) - (int)(x_r);
	const int add_z = rand() % (int)( half_building * 0.9f );
	*/

	trans = translate( trans, vec3( add_x, 0, -( z_displace + add_z ) ) );	

	if( rand() % 2 == 0 ) generateOrb( trans );
	if( rand() % 2 == 0 ) generateBlock( trans );
	// generateOrb( trans );

	if ( buildings.size() <= next_building ) {
		buildings.emplace_back( building_node, trans );
		building_colliders.emplace_back( &buildings[next_building] );
	}
	else {
		buildings[next_building] = Building{ building_node, trans };
		building_colliders[next_building] = &buildings[next_building];
	}

	// generateBlock( trans );
	next_building = ( next_building + 1 ) % NUM_BUILDINGS;
}

vec3 BuildingManager::generateOrb( const mat4& trans ) {
	vec3 rand_vec{
		rand() % ( 2 * (int)( half_building - orb_size ) ) - (int)( half_building - orb_size ),
		0,
		rand() % ( 2 * (int)( half_building - orb_size ) ) - (int)( half_building - orb_size ) };

	if( orbs.size() <= next_orb ) {
		orbs.emplace_back( orb_node, translate( trans, rand_vec ) );
		orb_colliders.emplace_back( &orbs[next_orb] );
	}
	else {
		orbs[next_orb] = Orb{ orb_node, translate( trans, rand_vec ) };
		orb_colliders[next_orb] = &orbs[next_orb];
	}
	next_orb = ( next_orb + 1 ) % NUM_BUILDINGS;
	return rand_vec;
}

void BuildingManager::generateBlock( const mat4& trans ) {
	vec3 rand_vec{
		rand() % ( 2 * (int)( half_building - block_size_x ) ) - (int)( half_building - block_size_x ),
		0,
		rand() % ( 2 * (int)( half_building - block_size_z ) ) - (int)( half_building - block_size_z ) };

	if( blocks.size() <= next_block ) {
		blocks.emplace_back( block_node, translate( trans, rand_vec ) );
		block_colliders.emplace_back( &blocks[next_block] );
	}
	else {
		blocks[next_block] = Block{ block_node, translate( trans, rand_vec ) };
		block_colliders[next_block] = &blocks[next_block];
	}
	next_block = ( next_block + 1 ) % NUM_BUILDINGS;
}

void BuildingManager::generateBlock( const mat4& trans, const vec3& rand_vec ) {
	vec3 rand_vec2{
		rand() % ( 2 * (int)( half_building - block_size_x ) ) - (int)( half_building - block_size_x ),
		0,
		rand() % ( 2 * (int)( half_building - block_size_z ) ) - (int)( half_building - block_size_z ) };

	/*
	check for intercepting spawn positions - do later
	if( rand_vec2.x + block_size_x >= rand_vec.x - orb_size && rand_vec2.x
	*/

	if( blocks.size() <= next_block ) {
		blocks.emplace_back( block_node, translate( trans, rand_vec2 ) );
		block_colliders.emplace_back( &blocks[next_block] );
	}
	else {
		blocks[next_block] = Block{ block_node, translate( trans, rand_vec2 ) };
		block_colliders[next_block] = &blocks[next_block];
	}
	next_block = ( next_block + 1 ) % NUM_BUILDINGS;
}

const vector<Building>& BuildingManager::getBuildings() { return buildings; }

const vector<Orb>& BuildingManager::getOrbs() { return orbs; }

const vector<Block>& BuildingManager::getBlocks() { return blocks; }

vector<Collider*> BuildingManager::getBuildingColliders() { 
	vector<Collider*>colliders( building_colliders );
	colliders.insert( colliders.end(), block_colliders.begin(), block_colliders.end() ); 
	return colliders;
}

const vector<Collider*>& BuildingManager::getOrbColliders() { return orb_colliders; }

void BuildingManager::spawnIfPossible( float player_z, float player_speed ) {
	if( buildings.size() != NUM_BUILDINGS ) return;
	if( abs( buildings[next_building].getTrans()[3][2] ) + ( half_building * 2 ) < abs( player_z ) )
		generateBuilding( player_speed ); // generate building also generates blocks and orbs

	for( Orb& orb: orbs ) {
		if( orb.checkForCollision() && abs( orb.getTrans()[3][2] ) + half_building < abs( player_z ) )
			orb.setCollisionCheck( false );	 
	}
}
