#pragma once
#include <glm/glm.hpp>
#include <memory>
#include "Collider.hpp"

class SceneNode;

class Building : public Collider  {
public:
	Building( SceneNode* root, const glm::mat4& trans );
//	const glm::mat4& getTrans() const;
//	SceneNode* getRoot() const;

private:
//	glm::mat4 trans;
//	SceneNode* root;
};
