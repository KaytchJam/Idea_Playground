#pragma once

class BVHTree {
private:
	size_t size = 0;
public:
	BVHTree();
	~BVHTree();

	bool add();
};