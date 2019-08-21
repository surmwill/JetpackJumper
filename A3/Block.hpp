#pragma once
#include <glm/glm.hpp>
#include <memory>
#include "Collider.hpp"

class SceneNode;

class Block : public Collider  {
public:
	Block( SceneNode* root, const glm::mat4& trans );
	
	static void fillTangentsBitangents( SceneNode* block );
	static void printTangentsBitangents();
	static std::vector< glm::vec3 > tangent_polys;
	static std::vector< glm::vec3 > bitangent_polys;
};
