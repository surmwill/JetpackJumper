#pragma once
#include <glm/glm.hpp>
#include "Collider.hpp"
#include "SceneNode.hpp"
#include <iostream>

class Orb: public Collider {
public:
	Orb( SceneNode* root, const glm::mat4& trans );

//	const glm::mat4& getTrans() const;
//	SceneNode* getRoot() const;
	void collided() override;

	static int orbs_collected;

private:
//	glm::mat4 trans;
//	SceneNode* root;
};

