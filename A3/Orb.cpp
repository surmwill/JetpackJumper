#include "Orb.hpp"

using namespace glm;
using namespace std;

int Orb::orbs_collected = 0;

Orb::Orb( SceneNode* root, const mat4& trans ) : Collider{ root, trans } /* , root{ root }, trans{ trans } */ {}

//const mat4& Orb::getTrans() const { return trans; }

//SceneNode* Orb::getRoot() const { return root; }

void Orb::collided() { check_for_collision = false; orbs_collected++; }

