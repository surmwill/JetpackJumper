// Winter 2019

#include "JointNode.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/io.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

using namespace glm;
using namespace std;

//---------------------------------------------------------------------------------------
JointNode::JointNode(const std::string& name)
	: SceneNode(name)
{
	m_nodeType = NodeType::JointNode;
	clearAngle();
}

//---------------------------------------------------------------------------------------
JointNode::~JointNode() {

}
 //---------------------------------------------------------------------------------------
void JointNode::set_joint_x(double min, double init, double max) {
	m_joint_x.min = min;
	m_joint_x.init = init;
	m_joint_x.max = max;
	x_angle = init;
}

//---------------------------------------------------------------------------------------
void JointNode::set_joint_y(double min, double init, double max) {
	m_joint_y.min = min;
	m_joint_y.init = init;
	m_joint_y.max = max;
	y_angle = init;
}

void JointNode::rotate( double amount ) {
	rotate_x( amount );
	rotate_y( amount );
}

void JointNode::rotate_x( double amount ) {
	if( x_angle + amount >= m_joint_x.min && x_angle + amount <= m_joint_x.max ) {
		x_angle += amount;
	}
}

void JointNode::rotate_y( double amount ) {
	if( y_angle + amount >= m_joint_y.min && y_angle + amount <= m_joint_y.max ) {
		y_angle += amount;
	}
}

mat4 JointNode::getRotationMatrix() const {
	return glm::rotate( 
			glm::rotate( mat4{ 1.0f }, radians( (float)x_angle ), vec3( 1.0f, 0.0f, 0.0f ) ),
			radians( (float)y_angle ),
			vec3( 0.0f, 1.0f, 0.0f ) );
}

// ASSUME ONLY TRANSLATIONS ON JOINT NODES
const mat4 JointNode::get_inverse() const {
	mat4 invtrans{ 1.0f };
	invtrans[3][0] = -trans[3][0];
    invtrans[3][1] = -trans[3][1];
    invtrans[3][2] = -trans[3][2];
	return invtrans;
}

void JointNode::cutFromSequence( int seq ) {
	for( int i = 0; i < prev_x_angles.size(); i++ ) {
		if( prev_x_angles[i].first >= seq ) {
			prev_x_angles.erase( prev_x_angles.begin() + i, prev_x_angles.end() );
			prev_y_angles.erase( prev_y_angles.begin() + i, prev_y_angles.end() );
			break;
		}
	}
}

void JointNode::pushAngle( int seq ) {
	//prev_x_angles.erase( prev_x_angles.begin() + next_stack_pos, prev_x_angles.end() );
	//prev_y_angles.erase( prev_y_angles.begin() + next_stack_pos, prev_y_angles.end() );

	prev_x_angles.emplace_back( seq, x_angle );
	prev_y_angles.emplace_back( seq, y_angle );

	next_stack_pos++;

	// printStack();
}

bool JointNode::undo( int seq ) {
	if( seq == 0 ) return false;

	if( prev_x_angles[ next_stack_pos - 1 ].first == seq ) {
		x_angle = prev_x_angles[ next_stack_pos - 2 ].second;
		y_angle = prev_y_angles[ next_stack_pos - 2 ].second;
		next_stack_pos--;
		return true;
	}
	return false;
}

bool JointNode::redo( int seq ) {
	if( next_stack_pos == prev_x_angles.size() ) return false;

	if( prev_x_angles[ next_stack_pos ].first == seq ) {
		x_angle = prev_x_angles[ next_stack_pos ].second;
		y_angle = prev_y_angles[ next_stack_pos ].second;
		next_stack_pos++;
		return true;
	}
	return false;
}

void JointNode::clearAngle() {
	prev_x_angles.clear();
	prev_y_angles.clear();
	next_stack_pos = 0;
	x_angle = 0;
	y_angle = 0;
	pushAngle( 0.0f );
}

void JointNode::printStack() {
	cout << m_name << endl;
	cout << "next stack pos: " << next_stack_pos << endl;
	for( pair<int, double>& entry : prev_x_angles ) {
		cout << "seq: " << entry.first << " angle: " << entry.second << endl;
	}
	cout << "\n";
}



