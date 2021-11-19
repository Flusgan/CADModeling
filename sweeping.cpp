#define INF 0xfffff
#include <cstdio>
#include <string.h>
#include <fstream>
#include <iostream>
#include "EulerOp.h"
#include <GL/glut.h>

char buf[1024];
float v[1024][3] = { 0 };
float f[100] = { 0 };
float hole[10][100][3] = { INF };
int vnum = 0, fnum = 0, hnum = 0;
float dir[3] = { 0,0,0 };
float dis = 0;
Solid* solid;

void myDisplay()
{
	Face* f = solid->sface;
	glClearColor(0.1, 0.1, 0.4, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glLineWidth(3.0f);
	glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);

	while (f != NULL) {
		glBegin(GL_POLYGON);
		glColor4ub(255, 255, 255, 255);

		Loop* lp = f->outer;
		Halfedge* he = lp->lhe;
		Vertex* firstv = he->startv;

		while (he->endv!= firstv) {
			glVertex3f(he->startv->xyz[0], he->startv->xyz[1], he->startv->xyz[2]);
			he = he->nxt;
		}
		glVertex3f(he->startv->xyz[0], he->startv->xyz[1], he->startv->xyz[2]);
		glEnd();
		f = f->nxt;
	}

	glFlush();
}


void init(void)
{
	glClearColor(0.1, 0.1, 0.4, 0.0);
	glShadeModel(GL_SMOOTH);
}

void reshape(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, (GLfloat)w / (GLfloat)h, 0.1, 100000.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(40, 70, 150, 35, 0, 35, 0, 1, 0);
}

using namespace std;
int main(int argc, char* argv[]) {

	memset(buf, '\0', sizeof(buf));
	ifstream fin;
	fin.open("\sample.cobj", ios::in);


	//data reading
	while (fin.getline(buf, sizeof(buf))) {
		if (buf[0] == 'v') {
			int index = 2;
			for (int i = 0; i < 3; i++) {
				string s="";
				while (buf[index++] != ' ') s += buf[index-1];
				v[vnum][i] = atof(s.c_str());
			}
			vnum++;
		}
		if (buf[0] == 'f') {
			int index = 2;
			while (buf[index]) {
				string s = "";
				while (buf[index++] != ' ')  s += buf[index - 1];
				int temp = atoi(s.c_str());
				if (temp) f[fnum++] = temp;
				else
					break;
			}
		}

		if (buf[0] == 'h') {
			int pnum = 1;
			while (fin.getline(buf, sizeof(buf))) {
				if (buf[0] < '0' || buf[0]>'9') break;
				int index = 0;
				for (int i = 0; i < 3; i++) {
					string s = "";
					while (buf[index++] != ' ') s += buf[index - 1];
					hole[hnum][pnum][i] = atof(s.c_str());
				}
				pnum++;
				memset(buf, ' ', sizeof(buf));
			}
			hole[hnum][0][0] = pnum-1;
			hnum++;
		}
		
		if (buf[0] == 'd') {
			int index = 2;
			for (int i = 0; i < 4; i++) {
				string s = "";
				while (buf[index++] != ' ') s += buf[index - 1];
				if (i == 3) {
					dis = atof(s.c_str()); continue;
				}
				dir[i] = atof(s.c_str());
			}
		}
		memset(buf, ' ', sizeof(buf));
	}

	//modeling
	//draw the face to be sweeped first
	EulerOp Op;
	solid = Op.mvfs(v[0]);
	Vertex* vertex = solid->svertex;
	for (int i = 1; i < vnum; i++) {
		Op.mev(solid->sface->outer, vertex, v[i]);
		vertex = vertex->nxt;
	}
	Loop *nlp = Op.mef(vertex, solid->svertex, solid->sface->outer);

	for (int i = 0; i < hnum; i++) {
		Op.mev(nlp, solid->svertex, hole[i][1]);
		while (vertex->nxt != NULL) vertex = vertex->nxt;
		Vertex* first = vertex;
		for (int j = 2; j <= hole[i][0][0]; j++) {
			Op.mev(nlp, vertex, hole[i][j]);
			vertex = vertex->nxt;
		}
		Op.mef(vertex, first, nlp);
		Op.kemr(solid->svertex, first, nlp);
	}
	vector<Face*> sweeping = Op.tobesweeped;

	//sweeping
	for (auto it= sweeping.begin(); it != sweeping.end(); ++it) {

		Vertex* firstv, * nxtv;
		Halfedge* nowhe, * lastup, * nowup, * firstup;
		Loop* lp = (*it)->outer;
		
		nowhe = lp->lhe;
		firstv = nowhe->startv;
		float dv[3] = { 0 };
		dv[0] = firstv->xyz[0] + dir[0] * dis;
		dv[1] = firstv->xyz[1] + dir[1] * dis;
		dv[2] = firstv->xyz[2] + dir[2] * dis;
		
		//get the first vertex sweeped
		firstup = Op.mev(lp, firstv, dv);
		lastup = firstup;
		nowhe = nowhe->nxt;
		nxtv = nowhe->startv;
		Loop* nlp = lp;

		while (nxtv != firstv) {
			dv[0] = nxtv->xyz[0] + dir[0] * dis;
			dv[1] = nxtv->xyz[1] + dir[1] * dis;
			dv[2] = nxtv->xyz[2] + dir[2] * dis;
			nowup = Op.mev(lp, nxtv, dv);

			nlp = Op.mef(lastup->endv, nowup->endv, lp);

			lastup = nowup;
			nowhe = nowhe->nxt;
			nxtv = nowhe->startv;
		}
		Op.mef(lastup->endv, firstup->endv, lp);

	}
	//display
	glutInit(&argc, argv); 
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);

	//init display window
	glutInitWindowSize(1000, 1000);
	glutInitWindowPosition(100, 100);
	glutCreateWindow(argv[0]);

	init();
	glutReshapeFunc(reshape);
	glutDisplayFunc(myDisplay);

	//loop
	glutMainLoop();
	return 0;
}