/*
* Implements of EulerOp including :
* mvfs()
* mev()
* mef()
* kemr()
* kfmrh()
*/

#include "EulerOp.h"

//mvfs(): build a vertex and create data structure of solid face and loop for the new solid
Solid* EulerOp::mvfs(float p[3]) {

	Vertex* v = new Vertex(p);
	Solid* s = new Solid();
	Face* f = new Face();
	Loop* l = new Loop();

	//build the connectivity among elements;
	s->sface = f;
	f->fsolid = s;
	f->outer = l;
	l->lface = f;
	s->svertex = v;
	return s;
}

//mev(): connect the new vertex v2 with existed v1 with an edge
Halfedge* EulerOp::mev(Loop *lp, Vertex *v1, float p[3]) {

	Vertex* v2 = new Vertex(p);
	Halfedge* he1 = new Halfedge();
	Halfedge* he2 = new Halfedge();
	Edge* e = new Edge();

	he1->partner = he2;
	he2->partner = he1;

	he1->hedge = e;
	he2->hedge = e;
	he1->hloop = lp;
	he2->hloop = lp;
	he1->startv = v1;
	he1->endv = v2;
	he2->startv = v2;
	he2->endv = v1;

	//the end of this new edge is vertex v2,and two halfedges will form a sharing loop here
	he1->nxt = he2;
	he2->pre = he1;

	//extend existed loop in v1
	if (lp->lhe == NULL) {
		lp->lhe = he1;
		he1->pre = he2;
		he2->nxt = he1;
	}
	else{
		Halfedge* lhe = lp->lhe;
		while (lhe->endv != v1) lhe = lhe->nxt;
		he2->nxt = lhe->nxt;
		lhe->nxt->pre = he2;
		he1->pre = lhe;
		lhe->nxt = he1;
	}
	
	//add new vertex and new edge into list;
	Vertex* vtail = v1;
	while (vtail->nxt != NULL)vtail = vtail->nxt;
	vtail->nxt = v2;
	v2->pre = vtail;

	e->ehe = he1;
	Edge* tail = lp->lface->fsolid->sedge;
	if (tail == NULL) lp->lface->fsolid->sedge = e;
	else {
		while (tail->nxt != NULL) tail = tail->nxt;
		tail->nxt = e;
		e->pre = tail;
	}

	return he1;
}

Loop* EulerOp::mef(Vertex* v1, Vertex* v2, Loop* lp) {

	Halfedge* he1 = new Halfedge();
	Halfedge* he2 = new Halfedge();
	Edge* ne = new Edge();
	Loop* nlp = new Loop();
	Face* nf = new Face();

	he1->hedge = ne;
	he2->hedge = ne;
	he1->partner = he2;
	he2->partner = he1;
	he1->startv = v1;
	he1->endv = v2;
	he2->startv = v2;
	he2->endv = v1;
	
	ne->ehe = he1;

	//find exsited left halfedges whose endv is v1 and v2
	Halfedge* helist = lp->lhe;
	Halfedge* hev1, * hev2;
	while (helist->endv != v1) helist = helist->nxt;
	hev1 = helist;
	while (helist->endv != v2) helist = helist->nxt;
	hev2 = helist;

	//connect new halfedge with old loop
	he2->nxt = hev1->nxt;
	hev1->nxt->pre = he2;
	he1->nxt = hev2->nxt;
	hev2->nxt->pre = he1;
	hev1->nxt = he1;
	he1->pre = hev1;
	hev2->nxt = he2;
	he2->pre = hev2;

	//split the loop
	lp->lhe = he1;
	nlp->lhe = he2;

	he1->hloop = lp;
	while (he2->hloop != nlp) {
		he2->hloop = nlp;
		he2 = he2->nxt;
	}

	nlp->lface = nf;
	
	//add new loop int list
	if (nf->outer == NULL) nf->outer = nlp;
	else {
		Loop* ltail = nf->inner;
		if (ltail = NULL) nf->inner = nlp;
		else {
			while (ltail->nxt != NULL)ltail = ltail->nxt;
			ltail->nxt = nlp;
			nlp->pre = ltail;
		}
	}
	
	//add new edge into list
	Edge* etail = lp->lface->fsolid->sedge;
	if (etail == NULL) lp->lface->fsolid->sedge = ne;
	else {
		while (etail->nxt != NULL) etail = etail->nxt;
		etail->nxt = ne;
		ne->pre = etail;
	}
	//add new face into list
	Face* ftail = lp->lface;
	while (ftail->nxt != NULL)ftail = ftail->nxt;
	ftail->nxt = nf;
	nf->pre = ftail;
	nf->fsolid = ftail->fsolid;

	tobesweeped.push_back(he1->hloop->lface);

	return nlp;
}

//kemr():delete an edge then combine loops in its two sides into a new loop
Loop* EulerOp::kemr(Vertex* v1, Vertex* v2, Loop* lp) {
	Halfedge* he1 = lp->lhe;
	Halfedge* he2;
	Solid* s;
	Edge* de;
	while (he1->startv != v1 || he1->endv != v2) he1 = he1->nxt;
	he2 = he1->partner;
	de = he1->hedge;

	he1->pre->nxt = he2->nxt;
	he2->nxt->pre = he1->pre;
	he1->nxt->pre = he2->pre;
	he2->pre->nxt = he1->nxt;
	while (he2->hloop != lp) {
		he2->hloop = lp;
		he2 = he2->nxt;
	}
	lp->lhe = he1->nxt;

	//delete the edge from global list
	s = he1->hloop->lface->fsolid;
	Edge* etail = s->sedge;
	while (etail != de)etail = etail->nxt;
	etail->nxt->pre = etail->pre;
	etail->pre->nxt = etail->nxt;
	delete etail;
	
	delete he1;
	delete he2;
	
	return lp;
}

//kfmrh():delete a inner face and scratch the loop of it into a hole
void EulerOp::kfmrh(Face* f1, Face* f2) {
	Loop* lp = f2->outer;
	if (f1->outer == NULL) f1->outer = lp;
	else {
		Loop* ltail = f1->inner;
		if (ltail = NULL) f1->inner = lp;
		else {
			while (ltail->nxt != NULL)ltail = ltail->nxt;
			ltail->nxt = lp;
			lp->pre = ltail;
		}
	}

	Solid* s = f1->fsolid;
	Face* ftail = s->sface;
	if (ftail == f2) s->sface = s->sface->nxt;
	else {
		while (ftail != f2) ftail = ftail->nxt;
		ftail->nxt->pre = ftail->pre;
		ftail->pre->nxt = ftail->nxt;
	}
	delete f2;
}
