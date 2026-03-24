#pragma once
#include <utility> 
using namespace std;
class Entity {
public:
	Entity(int);
	inline int getType() { return type; };
private:
	int type;
};
