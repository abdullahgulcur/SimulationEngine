#pragma once

#include <vector>

class Transform {

public:

	int id;
	Transform* parent;
	std::vector<Transform*> children;

	struct position
	{
		float x, y, z;
	};

	struct rotation
	{
		float x, y, z;
	};

	struct scale
	{
		float x, y, z;
	};

	position position;
	rotation rotation;
	scale scale;

};