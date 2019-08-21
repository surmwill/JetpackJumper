// Winter 2019

#include "GeometryNode.hpp"

//---------------------------------------------------------------------------------------
GeometryNode::GeometryNode(
		const std::string & meshId,
		const std::string & name
)
	: SceneNode(name),
	  meshId(meshId)
{
	m_nodeType = NodeType::GeometryNode;
}

void GeometryNode::setBackupKD() {
	if( !backup_set ) {
		backup_kd = material.kd;
		backup_set = true;
	}
}

void GeometryNode::setKDToBackup() { material.kd = backup_kd; }
