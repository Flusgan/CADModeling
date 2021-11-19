#pragma once
/*
* Definition of Halfedge Structure including:
* Solid
* Face
* Loop
* Halfedge
* Edge
* Vertex
*/

#include <stdlib.h>

struct Solid;
struct Face;
struct Loop;
struct Halfedge;
struct Edge;
struct Vertex;

struct Solid {

	Solid* nxt;
	Solid* pre;
	
	Face* sface;
	Edge* sedge;
	Vertex* svertex;
	Solid() {
		nxt = NULL;
		pre = NULL;
		sface = NULL;
		sedge = NULL;
		svertex = NULL;
	}
};

struct Face {

	Face* nxt;
	Face* pre;

	Solid* fsolid;
	Loop* outer;
	Loop* inner;

	Face() {
		nxt = NULL;
		pre = NULL;
		fsolid = NULL;
		outer = NULL;
		inner = NULL;
	}
};

struct Loop {

	Loop* nxt;
	Loop* pre;

	Face* lface;
	Halfedge* lhe;

	Loop() {
		nxt = NULL;
		pre = NULL;
		lface = NULL;
		lhe = NULL;
	}
};

struct Halfedge {

	Halfedge* nxt;
	Halfedge* pre;
	Halfedge* partner;

	Loop* hloop;
	Vertex* startv;
	Vertex* endv;
	Edge* hedge;

	Halfedge() {
		nxt = NULL;
		pre = NULL;
		partner = NULL;
		hloop = NULL;
		startv = NULL;
		endv = NULL;
		hedge = NULL;
	}
};

struct Edge {

	Edge* nxt;
	Edge* pre;

	Halfedge* ehe;

	Edge() {
		nxt = NULL;
		pre = NULL;
		ehe = NULL;
	}
};

struct Vertex {

	Vertex* nxt;
	Vertex* pre;

	float xyz[3];

	Vertex(float p[3]) {
		nxt = NULL;
		pre = NULL;
		
		xyz[0] = p[0];
		xyz[1] = p[1];
		xyz[2] = p[2];
	}
};