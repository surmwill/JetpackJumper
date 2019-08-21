// Winter 2019

#pragma once

#include "SceneNode.hpp"

class GeometryNode : public SceneNode {
public:
	GeometryNode(
		const std::string & meshId,
		const std::string & name
	);

	void setBackupKD();
	void setKDToBackup();

	Material material;

	// Mesh Identifier. This must correspond to an object name of
	// a loaded .obj file.
	std::string meshId;

private:
	bool backup_set = false;
	glm::vec3 backup_kd;
};
