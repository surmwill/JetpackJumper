#include "Block.hpp"
#include "SceneNode.hpp"
#include "Collider.hpp"

#include <iostream>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/io.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace std;
using namespace glm;

vector<glm::vec3> Block::tangent_polys;
vector<glm::vec3> Block::bitangent_polys;

Block::Block( SceneNode* root, const glm::mat4& trans ) : Collider{ root, trans } { 
	// fillTangentsBitangents();
}

void Block::fillTangentsBitangents( SceneNode* block ) {
	cout << "calculating tangents/bitangents for blocks" << endl;
	Block b{ block, mat4{ 1.0f } }; 

	for( unsigned int i = 0; i < b.polys.size(); i++ ) {
		const array<vec4, 3>& poly = b.polys[i];
		const array<vec2, 3>& uv = b.uv_polys[i];

		const vec3 edge1 = vec3( poly[1] - poly[0] );
		const vec3 edge2 = vec3( poly[2] - poly[0] );
		const vec2 deltaUV1 = uv[1] - uv[0];
		const vec2 deltaUV2 = uv[2] - uv[0];
	
		const float f = 1.0f / ( deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y );

		for( int j = 0; j < 3; j++ ) {
			tangent_polys.emplace_back( normalize( vec3 (
				f * ( deltaUV2.y * edge1.x - deltaUV1.y * edge2.x ),
				f * ( deltaUV2.y * edge1.y - deltaUV1.y * edge2.y ),
				f * ( deltaUV2.y * edge1.z - deltaUV1.y * edge2.z ) ) ) );
		}

		for( int j = 0; j < 3; j++ ) {
			bitangent_polys.emplace_back( normalize( vec3 (
				f * ( -deltaUV2.x * edge1.x + deltaUV1.x * edge2.x ),
				f * ( -deltaUV2.x * edge1.y + deltaUV1.x * edge2.y ),
				f * ( -deltaUV2.x * edge1.z + deltaUV1.x * edge2.z ) ) ) );
		}
	}
	// printTangentsBitangents();
}

void Block::printTangentsBitangents() {
	cout << "tangents" << endl;
	for( unsigned int i = 0; i < tangent_polys.size(); i++ ) {
		if( i && i % 3 == 0 ) cout << "\n";
		cout << tangent_polys[i] << " ";
		
	}
	cout << "\n";
	cout << "bitangents" << endl;
	for( unsigned int i = 0; i < bitangent_polys.size(); i++ ) {
		if( i && i % 3 == 0 ) cout << "\n";
		cout << bitangent_polys[i] << " ";
		
	}
	cout << "\n";
}
