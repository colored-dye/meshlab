#pragma once

#include "Mesh.h"

class Butterfly
{
public:
    subdiv::Mesh* run(subdiv::Mesh* org);
	bool needsTriangulation() { return true; }
protected:
    Point getEdgePoint(subdiv::Edge * e);
};

