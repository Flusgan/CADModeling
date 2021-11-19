/*
Definition of Eulerop class
implements of methods -> EulerOp.cpp
*/

#include "DefofHalfedge.h"
#include <vector>
using namespace std;

class EulerOp {

public:

	// 5 EulerOps
	Solid* mvfs(float p[3]);
	Halfedge* mev(Loop* lp, Vertex* v1, float p[3]);
	Loop* mef(Vertex* v1, Vertex* v2, Loop* lp);
	Loop* kemr(Vertex* v1, Vertex* v2, Loop* lp);
	void kfmrh(Face* f1, Face* f2);
	vector<Face*> tobesweeped;
};