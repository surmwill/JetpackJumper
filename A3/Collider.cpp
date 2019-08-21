#include <iostream>
#include "Collider.hpp"
#include "SceneNode.hpp"
#include "GeometryNode.hpp"
#include <glm/gtx/io.hpp>

using namespace glm;
using namespace std;

BatchInfoMap* Collider::batch_info_map = nullptr;
MeshConsolidator* Collider::mesh_consolidator = nullptr;

Collider::Collider( SceneNode* collider, const mat4& trans ) : node{ collider}, trans{ trans } {
	if( !batch_info_map || !mesh_consolidator ) {
		cerr << "ERROR: call init() first" << endl;
		return;
	}
	fillPolys( collider );
	trans_polys = polys;
	trans_norm_polys = norm_polys;
}

Collider::Collider( 
	const vector<array<vec4, 3>>& polys,
	const vector<array<vec3, 3>>& norm_polys,
	const vector<array<vec2, 3>>& uv_polys,
	const mat4& trans ) :
		polys{ polys }, norm_polys{ norm_polys }, uv_polys{ uv_polys }, trans{ trans } 
{
	trans_polys = polys;
	trans_norm_polys = norm_polys;
}

void Collider::init( BatchInfoMap* bim, MeshConsolidator* mc ) {
	batch_info_map = bim;
	mesh_consolidator = mc;
}

void Collider::fillPolys( SceneNode* node ) {
	if( node->m_nodeType == NodeType::GeometryNode ) {
		GeometryNode* geo_node = static_cast<GeometryNode*>( node );
		BatchInfo batch_info = (*batch_info_map)[geo_node->meshId]; 

		// extract vertices		
		for( int i = batch_info.startIndex; i < batch_info.numIndices * 3; i+=9 ) {
			const float* start_vert = mesh_consolidator->getVertexPositionDataPtr() + i;
			models.emplace_back( trans * geo_node->trans );
			
			polys.emplace_back( array<vec4, 3> {
				vec4( *( start_vert ), *( start_vert + 1 ), *( start_vert + 2 ), 1 ),
				vec4( *( start_vert + 3 ), *( start_vert + 4 ), *( start_vert + 5 ), 1 ),
				vec4( *( start_vert + 6 ), *( start_vert + 7 ), *( start_vert + 8 ), 1 )
			} );
		}

		// extract normals
		for( int i = batch_info.startIndex; i < batch_info.numIndices * 3; i+=9 ) {
			const float* start_norm = mesh_consolidator->getVertexNormalDataPtr() + i;
			
			norm_polys.emplace_back( array<vec3, 3> {
				normalize( vec3( *( start_norm ), *( start_norm + 1 ), *( start_norm + 2 ) ) ),
				normalize( vec3( *( start_norm + 3 ), *( start_norm + 4 ), *( start_norm + 5 ) ) ),
				normalize( vec3( *( start_norm + 6 ), *( start_norm + 7 ), *( start_norm + 8 ) ) )
			} );
		}

		// extract uvs
		for( int i = batch_info.startIndex; i < batch_info.numIndices * 2; i+=6 ) {
			const float* start_uv = mesh_consolidator->getVertexUVDataPtr() + i;
			
			uv_polys.emplace_back( array<vec2, 3> {
				vec2( *( start_uv ), *( start_uv + 1 ) ),
				vec2( *( start_uv + 2 ), *( start_uv + 3 ) ),
				vec2( *( start_uv + 4 ), *( start_uv + 5 ) )
			} );
		}
	}
	for( SceneNode* node2 : node->children ) { 
		fillPolys( node2 ); 
	}
}

void Collider::printPolys() {
	cout << "faces" << endl;
	for( const array<vec4, 3>& poly: trans_polys ) {
		cout << poly[0] << " " << poly[1] << " " << poly[2] << endl;
	}
	cout << "normals" << endl;
	for( const array<vec3, 3>& norm_poly: trans_norm_polys ) {
		cout << norm_poly[0] << " " << norm_poly[1] << " " << norm_poly[2] << endl;
	}
	cout << "uvs" << endl;
	for( const array<vec2, 3>& uv_poly: uv_polys ) {
		cout << uv_poly[0] << " " << uv_poly[1] << " " << uv_poly[2] <<  endl;
	}
}

void Collider::transformPolys( const mat4& trans ) { 
	for( unsigned int i = 0; i < polys.size(); i++ ) {
		const mat4 m = trans * models[i];
		trans_polys[i][0] = m * polys[i][0];
		trans_polys[i][1] = m * polys[i][1];
		trans_polys[i][2] = m * polys[i][2];
	}
	for( unsigned int i = 0; i < polys.size(); i++ ) {
		const mat3 norm = transpose( inverse( mat3( trans * models[i] ) ) );
		trans_norm_polys[i][0] = normalize( norm * norm_polys[i][0] );
		trans_norm_polys[i][1] = normalize( norm * norm_polys[i][1] );
		trans_norm_polys[i][2] = normalize( norm * norm_polys[i][2] );
	}
}

void Collider::setCollisionCheck( bool collision ) { check_for_collision = collision; }

void Collider::collided() { }

bool Collider::checkForCollision() const { return check_for_collision; }

const std::vector< std::array<glm::vec4, 3> >& Collider::getPolys() { return trans_polys; }

const std::vector< std::array<glm::vec3, 3> >& Collider::getPolyNorms() { return trans_norm_polys; }

const mat4& Collider::getTrans() const { return trans; }

SceneNode* Collider::getRoot() const { return node; }
