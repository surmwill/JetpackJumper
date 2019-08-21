#include "Player.hpp"
#include "SceneNode.hpp"
#include "Collider.hpp"
#include "polyroots.hpp"

#include <iostream>
#include <utility>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/io.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

using namespace glm;
using namespace std;
using namespace irrklang;

const int Player::MAX_PARTICLES = 2000; // 100000
const int Player::PARTICLES_PER_SECOND = 1000; // 10000
const float Player::PARTICLE_LIFE = 0.75f;

Player::Player( const mat4& init, float gravity ) : 
	left_foot_ps{ MAX_PARTICLES, PARTICLES_PER_SECOND, PARTICLE_LIFE, vec3( 0.0f, 8.4f, 1.525f ) },
	accel{ 0.0f, gravity, 0.0f } 
{
	init_pos = init * init_pos;
	engine = createIrrKlangDevice();
	if( !engine ) cout << "sound engine's fucked" << endl;
} 

Player::~Player() { } // engine->drop(); }

vec4 Player::getPosition() { 
	// cout << trans * init_pos << endl;
	return trans * init_pos; 
}

const mat4 Player::getModelMatrix() { return trans * rot; }

vec3& Player::getVelocity() { return vel; }

void Player::updateCameraStatus( double delta_time ) {
	vec3 pos = vec3( getPosition() );
	if( pos.y < 0 ) camera_status = falling;	
	else if( abs( vel.z ) <= 0.01f ) {
		camera_status = hitWall;
		camera_extra_dist += -move_speed * (float)delta_time;
	}
	else camera_status = normal;
}

vec3 Player::getCameraPosition() {
	vec3 pos = vec3 ( getPosition()	);
	if ( camera_status == normal || !game_mode ) return vec3( pos.x, pos.y + CAMERA_UP, pos.z + CAMERA_BACK );
	else if( camera_status == falling ) return vec3( pos.x, CAMERA_UP, pos.z + CAMERA_BACK );
	else return vec3( pos.x, pos.y + CAMERA_UP, pos.z + CAMERA_BACK + camera_extra_dist );
}

vec3 Player::getCameraLookAt() {
	vec3 pos = vec3 ( getPosition()	);
	if( camera_status == falling || camera_status == normal || !game_mode ) return pos;
	else return vec3( pos.x, pos.y, pos.z + camera_extra_dist );
}

void Player::toggleGameMode() {
	game_mode = !game_mode;
	if( game_mode ) {
		vel = vec3( 0, 0, -move_speed );
		rot = mat4{ 1.0f };
	}
	else {
		vel = vec3( 0, 0, 0 );
		rot = mat4{ 1.0f };
	}
}

bool Player::getGameMode(){ return game_mode; }

void Player::updatePosition( const vector<Collider*>& building_colliders, const vector<Collider*>& orb_colliders, int dir, float delta_time ) {
	const float speed = 10.0f;
	if( dir == 0 ) vel = vec3( -speed, 0, 0 );
	else if( dir == 1 ) vel = vec3( 0, 0, -speed );
	else if( dir == 2 ) vel = vec3( speed, 0, 0 );
	else if( dir == 3 ) vel = vec3( 0, 0, speed );
	else if( dir == 4 ) vel = vec3( 0, speed, 0 );
	else if( dir == 5 ) vel = vec3( 0, -speed, 0 );
	vel *= delta_time;

	if( checkCollisions( vel, building_colliders ) ) vel = vec3( 0.0f );
	if( checkCollisions( vel, orb_colliders, false ) ) vel = vec3( 0.0f );
	trans = translate( trans, vel );
    left_foot_ps.updateTrans( trans );
}

void Player::updatePosition( const vector<Collider*>& building_colliders, const vector<Collider*>& orb_colliders, float delta_time ) {
	/*
	if( vel.x > move_speed ) {
		vel.x = move_speed;
		accel.x = 0;
	}
	if( vel.x < -move_speed ) {
		vel.x = -move_speed;
		accel.x = 0;
	}
	*/
	if( vel_wind.x > wind_speed ) {
		vel_wind.x = wind_speed;
		accel_wind = vec3( 0.0f, 0.0f, 0.0f );
	}
	if( vel_wind.x < -wind_speed ) {
		vel_wind.x = -wind_speed;
		accel_wind = vec3( 0.0f, 0.0f, 0.0f );
	}

	
	vel += ( accel * delta_time );
	if( in_air ) vel_wind += ( accel_wind * delta_time );

	// needed for checkCollisions, checkCollision asks if we can move an entire velocity
	// vector forward and this is the max we want to move
	vec3 trim_vel = ( vel + vel_wind ) * delta_time;
	
	vec3 vel_x = vec3( trim_vel.x, 0.0f, 0.0f );
	vec3 vel_y = vec3( 0.0f, trim_vel.y, 0.0f );
	vec3 vel_z = vec3( 0.0f, 0.0f, trim_vel.z );

	if( checkCollisions( vel_x, building_colliders ) ) {
		trim_vel.x = 0.0f;
		vel.x = 0.0f;
	}
	else trans = translate( trans, vel_x );

	if( checkCollisions( vel_y, building_colliders ) ) {
		in_air = false;
		trim_vel.y = 0.0f;
		vel.y = 0.0f;
		vel_wind = vec3( 0.0f, 0.0f, 0.0f );
		if( engine ) engine->stopAllSounds();
	}
	else trans = translate( trans, vel_y );
	
	if( checkCollisions( vel_z, building_colliders ) ) {
		trim_vel.z = 0.0f;
		vel.z = 0.0f;
	}
	else trans = translate( trans, vel_z );

	//trans = translate( trans, trim_vel );
    left_foot_ps.updateTrans( trans );

	if( checkCollisions( trim_vel, orb_colliders ) ) reduceWindSpeed();
}

bool Player::checkCollisions( vec3 vel, const vector<Collider*>& colliders, bool remove ) {
	vec3 pos = vec3( getPosition() );

	cob_r3 = mat4{ 1.0f };
	cob_r3[1][1] = BOUNDING_R_Y;
	cob_r3[0][0] = sqrt( pow( BOUNDING_R_X, 2 ) * pow( cos( xz_rot ), 2 ) +
			pow( BOUNDING_R_Z, 2 ) * pow( sin( xz_rot ), 2 ) );
	cob_r3[2][2] = sqrt( pow( BOUNDING_R_X, 2 ) * pow( sin( xz_rot ), 2 ) +
			pow( BOUNDING_R_Z, 2 ) * pow( cos( xz_rot ), 2 ) );
	cob_ellipse = inverse( cob_r3 );

	// Transform velocity vector and polygons by player's bb ellipse cob matrix. Now we are working with a sphere
	pos = mat3( cob_ellipse ) * pos;
	vel = mat3( cob_ellipse ) * vel;

	float min_t = -1, t0, t1;	// range [0, 1] range in which the sphere intersects a polygon
	vec3 min_collision_point;

	int i = 0;
	for( Collider* collider : colliders ) {
		if( !collider->checkForCollision() ) continue;
		bool collider_found_collision = false;
		collider->transformPolys( cob_ellipse );
		const vector< array<vec4, 3> >& polys = collider->getPolys(); 
		// const vector< array<vec3, 3> >& norm_polys = collider->getPolyNorms(); 
		//building_collider.printPolys();

		for( unsigned int i = 0; i < polys.size(); i++ ) {
			// enough to represent a plane
			// const vec3 plane_norm = norm_polys[i][0];
			const vec3 plane_norm = normalize( cross( vec3( polys[i][1] - polys[i][0] ), vec3( polys[i][2] - polys[i][1] ) ) ); 
			const vec3 plane_point = vec3( polys[i][0] );
			bool in_plane = false;

			// test if swept sphere intersects triangle plane	
			float signed_distance = dot( plane_norm, pos - plane_point );

			if( dot( plane_norm, vel ) == 0 ) {
				// special case: we are in the same plane as the polygon
				if( abs( signed_distance ) > 1.0f ) continue;	// no intersection
				else {
					in_plane = true;
					t0 = 0;	// being in the same plane we are always intersecting the polygon
					t1 = 1;
				}
			}
			else {
				t0 = ( 1 - signed_distance ) / dot( plane_norm, vel );
				t1 = -1 - signed_distance / dot( plane_norm, vel );
				if( ( t0 < 0 || t0 > 1 ) && ( t1 < 0 || t1 > 1 ) ) continue;	// no intersection
				if( t0 > t1 ) swap( t0, t1 );
			}

			// clamp		
			if( t0 < 0 ) t0 = 0;
			if( t0 > 1 ) t0 = 1;
			if( t1 < 0 ) t1 = 0;
			if( t1 > 1 ) t1 = 1;

			vec3 collision_point;
			bool found_collision = false;
			float t = -1;

			if( !in_plane ) {
				// intersecion point: the point on the plane that the sphere will make first contact with when moving along the velocity vector
				const vec3 i_point = ( pos + t0 * vel ) - plane_norm;

				// check if the point is in the polygon
				const vec3 a = vec3( polys[i][0] );
				const vec3 b = vec3( polys[i][1] );
				const vec3 c = vec3( polys[i][2] );

				const vec3 u = b - a;
				const vec3 v = c - a;
				const vec3 n = cross( u, v );
				const vec3 w = i_point - a;
				const float gamma = dot( cross( u, w ), n ) / dot( n, n );
				const float beta = dot( cross( w, v ), n ) / dot( n, n ); 
				const float alpha = 1 - gamma - beta;

				if( alpha >= 0 && alpha <= 1 && beta >=0 && beta <= 1 && gamma >=0 && gamma <= 1 ) {	// in polygon
					t = t0;
					found_collision = true;
					collision_point = i_point;
				}
			}

			if( !found_collision ) {
				float a, b, c;
				double roots[2];
				double min_root = -1;
				int num_roots;
				float vel_sqr_len = pow( length( vel ), 2 );

				// check collision against each *vertex* in the polygon
				// collision occurs any time the squared distance between the swept sphere center and the vertex is 1
				// ( pos + vel * t - poly ) . ( pos + vel * t - poly ) = 0. Solve for t 
				a = vel_sqr_len;		
				for( int j = 0; j < 3; j++ ) {
					vec3 p = vec3( polys[i][j] );
					float b = 2 * dot( vel, pos - p );
					float c = pow( length( p - pos ), 2 ) - 1;
					num_roots = quadraticRoots( a, b, c, roots );
					if( num_roots == 1 ) min_root = roots[0]; 
					else if( num_roots == 2 ) {
						if( roots[0] < 0 ) min_root = roots[1];
						else if( roots[1] < 0 ) min_root = roots[0];
						else min_root = std::min( roots[0], roots[1] );
					}
					if( num_roots > 0 && min_root >= 0 && ( t < 0 || min_root < t ) ) {
						t = min_root;
						found_collision = true;
						collision_point = p;
					}
				}

				// check collision against each *edge* in the polygon
				vec3 edge, base_to_vertex;
				float edge_sqr_len, edge_dot_vel, edge_dot_base_to_vertex;

				/* goes: p1 -> p2, p2 -> p3, p3 -> p1 */
				for( int j = 0; j < 3; j++ ) {
					vec3 p1, p2;
					if( j != 2 ) {	// p1 -> p2, p2 -> p3
						p1 = vec3( polys[i][j] );
						p2 = vec3( polys[i][j+1] );
					}
					else {	// p3 -> p1
						p1 = vec3( polys[i][2] );
						p2 = vec3( polys[i][0] );
					}

					edge = vec3( p2 - p1 );
					base_to_vertex = p1 - pos;
					edge_sqr_len = pow( length( edge ), 2 );
					edge_dot_vel = dot( edge, vel );	
					edge_dot_base_to_vertex = dot( edge, base_to_vertex );

					a = edge_sqr_len * -vel_sqr_len + pow( edge_dot_vel, 2 );
					b = edge_sqr_len * ( 2 * dot( vel, base_to_vertex ) ) - 2 * edge_dot_vel * edge_dot_base_to_vertex;
					c = edge_sqr_len * ( 1 - pow( length( base_to_vertex ), 2 ) ) + pow( edge_dot_base_to_vertex, 2 );
					num_roots = quadraticRoots( a, b, c, roots );
					if( num_roots != 0 ) {
						if( num_roots == 1 ) min_root = roots[0];
						else if( num_roots == 2 ) {
							if( roots[0] < 0 ) min_root = roots[1];
							else if( roots[1] < 0 ) min_root = roots[0];
							else min_root = std::min( roots[0], roots[1] );
						}

						const float f = ( edge_dot_vel * min_root - edge_dot_base_to_vertex ) / edge_sqr_len;
						if( f >= 0 && f <= 1 ) {
							if( min_root >= 0 && ( t < 0 || min_root < t ) ) {
								t = min_root;
								found_collision = true;
								collision_point = vec3( polys[i][0] ) + f * edge;
							}
						}
					}
				}// end for (edge collisions)

			}//end if( !found_collision )

			if( found_collision ) {
				if( min_t < 0 || t < min_t ) {
					min_t = t;
					min_collision_point = collision_point;
				}
				if( t >= 0 && t <= 1 ) collider_found_collision = true;
			}
		}//end for (all polygon collisions in current collider)
		if( remove && collider_found_collision ) collider->collided();
	} // end for (all colliders)

	if( min_t >= 0 && min_t <= 1 ) return true;
	else return false;
}

ParticleSystem& Player::getLeftFootParticleSystem() { return left_foot_ps; }

void Player::moveLeft() {
	cout << "left" << endl;
	// accel.x = -move_speed / 2.0f;
	// vel = vec3( -move_speed, vel.y, 0 );
	// vel = vec3( -move_speed, 0, 0 );
	// accel = vec3( -move_speed * scale_accel, accel.y, 0.0f );
}

void Player::moveRight() {
	vel = vec3( move_speed, vel.y, 0 );
	// vel = vec3( move_speed, 0, 0 );
	// accel = vec3( move_speed * scale_accel, accel.y, 0.0f );
}

void Player::moveForward() {
	vel = vec3( 0, vel.y, -move_speed );
	// vel = vec3( 0, 0, -move_speed );
	// accel = vec3( -move_speed * scale_accel, accel.y, 0.0f );
}

void Player::moveBackward() {
	vel = vec3( 0, vel.y, move_speed );
	//vel = vec3( 0, 0, move_speed );
	// accel = vec3( -move_speed * scale_accel, accel.y, 0.0f );
}

void Player::moveDown() {
	vel = vec3( 0, -move_speed, 0 );
}

void Player::moveUp() {
	vel = vec3( 0, move_speed, 0 );
}

void Player::reduceWindSpeed() {
	wind_speed -= wind_increase * 2;
	if( wind_speed < 0 ) wind_speed = 0.0f;
	move_speed += speed_increase;
	vec3 incVel( 0.0f, 0.0f, -speed_increase );

	if( prev_face == right ) incVel = rotate( incVel, radians( -45.0f ), vec3( 0, 1, 0 ) );
	if( prev_face == left ) incVel = rotate( incVel, radians( 45.0f ), vec3( 0, 1, 0 ) );

	vel += incVel;
}

void Player::jump() {
	if( !in_air ) {
		vel.y += jump_speed;
		in_air = true;
		if( rand() % 3 == 0 || toggle_always_wind ) {
			wind_speed += wind_increase;
			accel_wind = vec3( wind_increase, 0.0f, 0.0f );
			if( engine ) engine->play2D( "Assets/Sounds/wind.mp3", false );
		}
	}
	// vel = vec3( vel.x, jump_speed, -move_speed /* vel.z */ );
}

void Player::stop() {
	vel = vec3( 0.0f );
	accel = vec3( 0.0f );
}

void Player::faceLeft() {
	rot = rotate( mat4{ 1.0f }, radians( 45.0f ), vec3( 0, 1, 0 ) );
	if( prev_face == right ) vel = rotate( vel, radians( 90.0f ), vec3( 0, 1, 0 ) );
	else if( prev_face == forward ) vel = rotate( vel, radians( 45.0f ), vec3( 0, 1, 0 ) );
	xz_rot = 45.0f;
	prev_face = left;
}

void Player::faceRight() {
	rot = rotate( mat4{ 1.0f }, radians( -45.0f ), vec3( 0, 1, 0 ) );
	if( prev_face == left ) vel = rotate( vel, radians( -90.0f ), vec3( 0, 1, 0 ) );
	else if( prev_face == forward ) vel = rotate( vel, radians( -45.0f ), vec3( 0, 1, 0 ) );
	xz_rot = -45.0f;
	prev_face = right;
}

void Player::faceForward() {
	rot = mat4{ 1.0f };
	if( prev_face == right ) vel = rotate( vel, radians( 45.0f ), vec3( 0, 1, 0 ) );
	else if( prev_face == left ) vel = rotate( vel, radians( -45.0f ), vec3( 0, 1, 0 ) );
	vel.z = -move_speed;
	vel.x = 0;
	xz_rot = 0.0f;
	prev_face = forward;
}

float Player::getMoveSpeed() { return move_speed; }

float Player::getJumpSpeed() { return jump_speed; }

// Works, but I don't understand it
/* 
typedef unsigned int uint32;
#define in(a) ((uint32&) a)

bool checkPointInTriangle( const vec3& point, const vec3& pa, const vec3& pb, const vec3& pc) {
	vec3 e10 = pb-pa;
	vec3 e20 = pc-pa;
	float a = dot( e10, e10 );
	float b = dot( e10, e20 );
	float c = dot( e20, e20 );
	float ac_bb = (a*c)-(b*b);
	vec3 vp = vec3(point.x - pa.x, point.y - pa.y, point.z - pa.z );
	float d = dot( vp, e10 );
	float e = dot( vp, e20 );
	float x = (d*c)-(e*b);
	float y = (e*a)-(d*b);
	float z = x+y-ac_bb;
	return (( in(z)& ~(in(x)|in(y)) ) & 0x80000000);
}
*/
