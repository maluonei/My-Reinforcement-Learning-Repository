#pragma once
#ifndef POLICY_H
#define POLICY_H
#include "Map.h"

class policy {
public:
	virtual void train() = 0;
	virtual void printPath() = 0;
};

#endif // !POLICY_H



