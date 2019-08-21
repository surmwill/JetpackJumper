#include "Building.hpp"
#include "SceneNode.hpp"
#include "Collider.hpp"

#include <iostream>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/io.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace std;
using namespace glm;

Building::Building( SceneNode* root, const glm::mat4& trans ) 
	: Collider{ root, trans } /* , root{ root }, trans{ trans } */ { }


//const mat4& Building::getTrans() const { return trans; }

//SceneNode* Building::getRoot() const { return root; }
