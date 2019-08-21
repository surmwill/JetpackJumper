#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <array>
#include "cs488-framework/MeshConsolidator.hpp"

class SceneNode;

class Collider {
public:
	Collider( SceneNode* collider, const glm::mat4& trans );
	Collider(
		const std::vector<std::array<glm::vec4, 3>>& polys,
		const std::vector<std::array<glm::vec3, 3>>& norm_polys,
		const std::vector<std::array<glm::vec2, 3>>& uv_polys,
		const glm::mat4& trans );

	static void init( BatchInfoMap* bim, MeshConsolidator* mc );
	void printPolys();
	const std::vector< std::array<glm::vec4, 3> >& getPolys();
	const std::vector< std::array<glm::vec3, 3> >& getPolyNorms();
	void transformPolys( const glm::mat4& trans ); 

	void setCollisionCheck( bool collision );
    bool checkForCollision() const;
	virtual void collided();

	const glm::mat4& getTrans() const;
	SceneNode* getRoot() const;


protected:
	void fillPolys( SceneNode* node );

	bool check_for_collision = true;
	SceneNode* node;
	glm::mat4 trans;
	std::vector<glm::mat4> models;

	// all the polygons in the object
	std::vector< std::array<glm::vec4, 3> > polys;
	std::vector< std::array<glm::vec4, 3> > trans_polys;
	std::vector< std::array<glm::vec3, 3> > norm_polys;
	std::vector< std::array<glm::vec3, 3> > trans_norm_polys;
	std::vector <std::array<glm::vec2, 3> > uv_polys;


	static BatchInfoMap* batch_info_map;
	static MeshConsolidator* mesh_consolidator;	
};
