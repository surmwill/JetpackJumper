// Winter 2019

#pragma once

#include "SceneNode.hpp"
#include <stack>
#include <vector>

class JointNode : public SceneNode {
public:
	JointNode(const std::string & name);
	virtual ~JointNode();

	void cutFromSequence( int seq );
	void printStack();
	void pushAngle( int seq );
	bool undo( int seq );
	bool redo( int seq );
	void clearAngle();
	void set_joint_x(double min, double init, double max);
	void set_joint_y(double min, double init, double max);
	void rotate( double amount );
	glm::mat4 getRotationMatrix() const;
	const glm::mat4 get_inverse() const override;	

	struct JointRange {
		double min, init, max;
	};
	JointRange m_joint_x, m_joint_y;

private:
	void rotate_x( double amount );
	void rotate_y( double amount );

	double x_angle, y_angle;
	int next_stack_pos = 0;
	std::vector< std::pair<int, double> > prev_x_angles;
	std::vector< std::pair<int, double> > prev_y_angles;	
};
