//------------------------------------------------------------------------------
//  Copyright 2007-2014 by Jyh-Ming Lien and George Mason University
//  See the file "LICENSE" for more information
//------------------------------------------------------------------------------

#pragma once

#include <GL/gli.h>
#include <time.h>

#include <map>
using namespace std;

#include "Basic.h"
#include "model.h"
#include "vector.h"

using namespace mathtool;

//#include "UserSpring.h"

//-----------------------------------------------------------------------------
//variables used in rendering
int selected=100000;
bool showWire=false; //on/off wireframe
bool showGrid = true; //toggle grid
bool showSharpEdgeOnly=false;
bool randomColor=false;
bool background=true; //on/off background
bool light=true; //on/off background

//Store display IDs and model colors
map<model*,int> model_solid_gids;
map<model*,int> model_wire_gids;
map<model*,Vector3d> model_colors;

//CUserSpring user_spring;

//-----------------------------------------------------------------------------
//this defines the size of the lattice
extern unsigned int lattice_nx, lattice_ny, lattice_nz;
extern vector<Point3d> FFD_lattice; //This stores all lattice nodes, FFD_lattice has size = (lattice_nx X lattice_ny X lattice_nz)
extern vector<Point3d> FFD_parameterization; //This stores all parameterized coordinates of all vertices from the model
                                             //FFD_parameterization has size = models.front().v_size
				//number ov values is nz*nx*ny*number of verts in model

//-----------------------------------------------------------------------------
inline void DisplayLattice(model& M)
{

	glDisable(GL_LIGHTING);

	////draw nodes
	//glPointSize(8);
	//glBegin(GL_POINTS);
	//glColor3d(1, 0, 0);
	//int i = 0;
	//for (int ix = 0; ix < lattice_nx; ix++)
	//{
	//	for (int iy = 0; iy < lattice_ny; iy++)
	//	{
	//		for (int iz = 0; iz < lattice_nz; iz++)
	//		{
	//			glVertex3dv(FFD_lattice[i++].get());
	//		}//end for z
	//	}//end for y
	//}//end for x
	//glEnd();
	//glPointSize(1);

	////draw edges
	//glBegin(GL_LINES);
	//glColor3d(0, 0, 1);
	//for (int ix = 0; ix < lattice_nx; ix++)
	//{
	//	for (int iy = 0; iy < lattice_ny; iy++)
	//	{
	//		for (int iz = 0; iz < lattice_nz; iz++)
	//		{
	//			//draw an edge along x-axis if (ix!=lattice_nx-1)
	//			if (ix != lattice_nx - 1)
	//			{
	//				glVertex3dv(FFD_lattice[ix*lattice_ny*lattice_nz + iy*lattice_nz + iz].get());
	//				glVertex3dv(FFD_lattice[(ix + 1)*lattice_ny*lattice_nz + iy*lattice_nz + iz].get());
	//			}

	//			//draw an edge along y-axis if (iy!=lattice_ny-1)
	//			if (iy != lattice_ny - 1)
	//			{
	//				glVertex3dv(FFD_lattice[ix*lattice_ny*lattice_nz + iy*lattice_nz + iz].get());
	//				glVertex3dv(FFD_lattice[ix*lattice_ny*lattice_nz + (iy + 1)*lattice_nz + iz].get());
	//			}

	//			//draw an edge along x-axis if (iz!=lattice_nz-1)
	//			if (iz != lattice_nz - 1)
	//			{
	//				glVertex3dv(FFD_lattice[ix*lattice_ny*lattice_nz + iy*lattice_nz + iz].get());
	//				glVertex3dv(FFD_lattice[ix*lattice_ny*lattice_nz + iy*lattice_nz + iz + 1].get());
	//			}
	//		}//end for z
	//	}//end for y
	//}//end for x

	//glEnd();









	//glDisable(GL_LIGHTING); //disable lighting

	if (showGrid){
		for (int i = 0; i < FFD_lattice.size(); i++){
			glPointSize(6.0);
			glBegin(GL_POINTS);
			if (i==selected){glColor3f(0.0f, 1.0f, 0.0f);}
			else{ glColor3f(1.0f, 0.0f, 0.0f); }
			glVertex3f(FFD_lattice[i][0], FFD_lattice[i][1], FFD_lattice[i][2]);
			glEnd();
		}
		
		glBegin(GL_LINES);
		glColor3f(0.0f, 0.0f, 1.0f);
		glLineWidth(1.5);
		glColor3f(0.0, 0.0, 1.0);
		glBegin(GL_LINE_LOOP);

		int lx = 0;
		for (int i = 0; i < lattice_nx; i++){
			glVertex3d(FFD_lattice[lx][0], FFD_lattice[lx][1], FFD_lattice[lx][2]);
			glVertex3d(FFD_lattice[lx+1][0], FFD_lattice[lx+1][1], FFD_lattice[lx+1][2]);
			glVertex3d(FFD_lattice[lx + 1][0], FFD_lattice[lx + 1][1], FFD_lattice[lx + 1][2]);
			glVertex3d(FFD_lattice[lx + 2][0], FFD_lattice[lx + 2][1], FFD_lattice[lx + 2][2]);
			glVertex3d(FFD_lattice[lx+2][0], FFD_lattice[lx+2][1], FFD_lattice[lx+2][2]);
			glVertex3d(FFD_lattice[lx + 3][0], FFD_lattice[lx + 3][1], FFD_lattice[lx + 3][2]);
			lx += 4;
		}

		lx = 12;
		for (int i = 0; i < lattice_nx; i++){
			glVertex3d(FFD_lattice[lx][0], FFD_lattice[lx][1], FFD_lattice[lx][2]);
			glVertex3d(FFD_lattice[lx + 1][0], FFD_lattice[lx + 1][1], FFD_lattice[lx + 1][2]);
			glVertex3d(FFD_lattice[lx + 1][0], FFD_lattice[lx + 1][1], FFD_lattice[lx + 1][2]);
			glVertex3d(FFD_lattice[lx + 2][0], FFD_lattice[lx + 2][1], FFD_lattice[lx + 2][2]);
			glVertex3d(FFD_lattice[lx + 2][0], FFD_lattice[lx + 2][1], FFD_lattice[lx + 2][2]);
			glVertex3d(FFD_lattice[lx + 3][0], FFD_lattice[lx + 3][1], FFD_lattice[lx + 3][2]);
			lx += 4;
		}
		lx = 24;
		for (int i = 0; i < lattice_nx; i++){
			glVertex3d(FFD_lattice[lx][0], FFD_lattice[lx][1], FFD_lattice[lx][2]);
			glVertex3d(FFD_lattice[lx + 1][0], FFD_lattice[lx + 1][1], FFD_lattice[lx + 1][2]);
			glVertex3d(FFD_lattice[lx + 1][0], FFD_lattice[lx + 1][1], FFD_lattice[lx + 1][2]);
			glVertex3d(FFD_lattice[lx + 2][0], FFD_lattice[lx + 2][1], FFD_lattice[lx + 2][2]);
			glVertex3d(FFD_lattice[lx + 2][0], FFD_lattice[lx + 2][1], FFD_lattice[lx + 2][2]);
			glVertex3d(FFD_lattice[lx + 3][0], FFD_lattice[lx + 3][1], FFD_lattice[lx + 3][2]);
			lx += 4;
		}
		lx = 36;
		for (int i = 0; i < lattice_nx; i++){
			glVertex3d(FFD_lattice[lx][0], FFD_lattice[lx][1], FFD_lattice[lx][2]);
			glVertex3d(FFD_lattice[lx + 1][0], FFD_lattice[lx + 1][1], FFD_lattice[lx + 1][2]);
			glVertex3d(FFD_lattice[lx + 1][0], FFD_lattice[lx + 1][1], FFD_lattice[lx + 1][2]);
			glVertex3d(FFD_lattice[lx + 2][0], FFD_lattice[lx + 2][1], FFD_lattice[lx + 2][2]);
			glVertex3d(FFD_lattice[lx + 2][0], FFD_lattice[lx + 2][1], FFD_lattice[lx + 2][2]);
			glVertex3d(FFD_lattice[lx + 3][0], FFD_lattice[lx + 3][1], FFD_lattice[lx + 3][2]);
			lx += 4;
		}
		///////////////////////end lines on z axis for grid//////////////
		int lz = 0;
		for (int i = 0; i < lattice_nz; i++){
			glVertex3d(FFD_lattice[lz][0], FFD_lattice[lz][1], FFD_lattice[lz][2]);
			glVertex3d(FFD_lattice[lz+4][0], FFD_lattice[lz+4][1], FFD_lattice[lz+4][2]);
			glVertex3d(FFD_lattice[lz + 4][0], FFD_lattice[lz + 4][1], FFD_lattice[lz + 4][2]);
			glVertex3d(FFD_lattice[lz + 8][0], FFD_lattice[lz + 8][1], FFD_lattice[lz + 8][2]);
			lz += 1;
		}
		lz = 12;
		for (int i = 0; i < lattice_nz; i++){
			glVertex3d(FFD_lattice[lz][0], FFD_lattice[lz][1], FFD_lattice[lz][2]);
			glVertex3d(FFD_lattice[lz + 4][0], FFD_lattice[lz + 4][1], FFD_lattice[lz + 4][2]);
			glVertex3d(FFD_lattice[lz + 4][0], FFD_lattice[lz + 4][1], FFD_lattice[lz + 4][2]);
			glVertex3d(FFD_lattice[lz + 8][0], FFD_lattice[lz + 8][1], FFD_lattice[lz + 8][2]);
			lz += 1;
		}
		lz = 24;
		for (int i = 0; i < lattice_nz; i++){
			glVertex3d(FFD_lattice[lz][0], FFD_lattice[lz][1], FFD_lattice[lz][2]);
			glVertex3d(FFD_lattice[lz + 4][0], FFD_lattice[lz + 4][1], FFD_lattice[lz + 4][2]);
			glVertex3d(FFD_lattice[lz + 4][0], FFD_lattice[lz + 4][1], FFD_lattice[lz + 4][2]);
			glVertex3d(FFD_lattice[lz + 8][0], FFD_lattice[lz + 8][1], FFD_lattice[lz + 8][2]);
			lz += 1;
		}
		lz = 36;
		for (int i = 0; i < lattice_nz; i++){
			glVertex3d(FFD_lattice[lz][0], FFD_lattice[lz][1], FFD_lattice[lz][2]);
			glVertex3d(FFD_lattice[lz + 4][0], FFD_lattice[lz + 4][1], FFD_lattice[lz + 4][2]);
			glVertex3d(FFD_lattice[lz + 4][0], FFD_lattice[lz + 4][1], FFD_lattice[lz + 4][2]);
			glVertex3d(FFD_lattice[lz + 8][0], FFD_lattice[lz + 8][1], FFD_lattice[lz + 8][2]);
			lz += 1;
		}
		/////////////////////end lines on x axis for grid///////////////
		int ly = 0;
		for (int i = 0; i < lattice_ny; i++){
			glVertex3d(FFD_lattice[ly][0], FFD_lattice[ly][1], FFD_lattice[ly][2]);
			glVertex3d(FFD_lattice[ly+12][0], FFD_lattice[ly+12][1], FFD_lattice[ly+12][2]);
			glVertex3d(FFD_lattice[ly + 12][0], FFD_lattice[ly + 12][1], FFD_lattice[ly + 12][2]);
			glVertex3d(FFD_lattice[ly + 24][0], FFD_lattice[ly + 24][1], FFD_lattice[ly + 24][2]);
			glVertex3d(FFD_lattice[ly + 24][0], FFD_lattice[ly + 24][1], FFD_lattice[ly + 24][2]);
			glVertex3d(FFD_lattice[ly + 36][0], FFD_lattice[ly +36][1], FFD_lattice[ly + 36][2]);
			ly += 1;
		}
		ly = 4;
		for (int i = 0; i < lattice_ny; i++){
			glVertex3d(FFD_lattice[ly][0], FFD_lattice[ly][1], FFD_lattice[ly][2]);
			glVertex3d(FFD_lattice[ly + 12][0], FFD_lattice[ly + 12][1], FFD_lattice[ly + 12][2]);
			glVertex3d(FFD_lattice[ly + 12][0], FFD_lattice[ly + 12][1], FFD_lattice[ly + 12][2]);
			glVertex3d(FFD_lattice[ly + 24][0], FFD_lattice[ly + 24][1], FFD_lattice[ly + 24][2]);
			glVertex3d(FFD_lattice[ly + 24][0], FFD_lattice[ly + 24][1], FFD_lattice[ly + 24][2]);
			glVertex3d(FFD_lattice[ly + 36][0], FFD_lattice[ly + 36][1], FFD_lattice[ly + 36][2]);
			ly += 1;
		}
		ly = 8;
		for (int i = 0; i < lattice_ny; i++){
			glVertex3d(FFD_lattice[ly][0], FFD_lattice[ly][1], FFD_lattice[ly][2]);
			glVertex3d(FFD_lattice[ly + 12][0], FFD_lattice[ly + 12][1], FFD_lattice[ly + 12][2]);
			glVertex3d(FFD_lattice[ly + 12][0], FFD_lattice[ly + 12][1], FFD_lattice[ly + 12][2]);
			glVertex3d(FFD_lattice[ly + 24][0], FFD_lattice[ly + 24][1], FFD_lattice[ly + 24][2]);
			glVertex3d(FFD_lattice[ly + 24][0], FFD_lattice[ly + 24][1], FFD_lattice[ly + 24][2]);
			glVertex3d(FFD_lattice[ly + 36][0], FFD_lattice[ly + 36][1], FFD_lattice[ly + 36][2]);
			ly += 1;
		}
		/////////////////ugh sorry for this horrible algorithm///////////////////

		
		glEnd();
	}

}

inline void DisplayModel(model& M, bool randcolor=false)
{
	//draw
	if (randcolor){
		if (model_colors.find(&M) == model_colors.end())
			model_colors[&M] = Vector3d(drand48() + 0.5, drand48() + 0.5, drand48(), +0.5).normalize() + Vector3d(0.25, 0.25, 0.25);
		glColor3dv(model_colors[&M].get());
	}
	
	//Draw facets
	glEnable( GL_POLYGON_OFFSET_FILL );
	glPolygonOffset( 0.5f, 0.5f );
	//for(list<polygon>::iterator i=M.polys.begin();i!=M.polys.end();i++)
	glBegin(GL_TRIANGLES);
	for(unsigned int i=0;i<M.t_size;i++)
	{
        const triangle & t=M.tris[i];
        glNormal3dv(M.tris[i].n.get());
        for(int k=0;k<3;k++)
		{
            const Point3d& pt=M.vertices[t.v[k]].p;

			//send pt to OpenGL
            glVertex3d(pt[0],pt[1],pt[2]);
        }
	}
	glEnd();
	glDisable( GL_POLYGON_OFFSET_FILL );
}

inline void DisplayModelWireFrame(model& M, bool randcolor=false)
{
    //Draw Edges
    if(showWire)
	{
		glBegin(GL_LINES);
        for(uint i=0;i<M.e_size;i++){
            glColor3f(0,0,0);
            const edge & e=M.edges[i];
            if(e.fid.size()==2){//normal case, check if e is sharp
                triangle& f1=M.tris[e.fid.front()];
                triangle& f2=M.tris[e.fid.back()];
                if(fabs(1-f1.n*f2.n)<1e-2){
                    if(showSharpEdgeOnly) continue; //not sharp
                    else
                        glColor3f(0.7f,0.7f,0.7f);
                }
            }

            Point3d& p1=M.vertices[e.vid[0]].p;
            Point3d& p2=M.vertices[e.vid[1]].p;
            glVertex3d(p1[0],p1[1],p1[2]);
            glVertex3d(p2[0],p2[1],p2[2]);
        }
        glEnd();
    }
}


//copied from meshlab
void DisplayBackground(void)
{
	float topcolor[]={1,1,1};
	float bottomcolor[]={1,1,0.5};
	
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(-1,1,-1,1,-1,1);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glPushAttrib(GL_ENABLE_BIT);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glBegin(GL_TRIANGLE_STRIP);
		glColor3fv(topcolor);  	glVertex2f(-1, 1);
		glColor3fv(bottomcolor);	glVertex2f(-1,-1);
		glColor3fv(topcolor);	glVertex2f( 1, 1);
		glColor3fv(bottomcolor);	glVertex2f( 1,-1);
	glEnd();
	
	glPopAttrib();
	glPopMatrix(); // restore modelview
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}

void drawAll()
{
    glEnable(GL_LIGHTING);



	//show the inputs
    glColor3f(1,1,1);
    if(light) glEnable(GL_LIGHTING);
    else glDisable(GL_LIGHTING);

    for(list<model>::iterator i=models.begin();i!=models.end();i++){
        DisplayModel(*i,randomColor);
		DisplayLattice(*i);
    }
    for(list<model>::iterator i=models.begin();i!=models.end();i++){
        DisplayModelWireFrame(*i);
    }

	//draw lattice
	//DisplayLattice(*i);
	//user_spring.doDraw();
}

//-----------------------------------------------------------------------------
void Display( void )
{
    //Init Draw
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    if(background) DisplayBackground();
    
    
    glPushMatrix();
    glLoadIdentity();
    static GLfloat light_position1[] = {  100, 100, 100.0f, 1.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, light_position1);
    static GLfloat light_position2[] = { -100, -100, 50.0f, 1.0f };
    glLightfv(GL_LIGHT1, GL_POSITION, light_position2);
    glPopMatrix();

    drawAll();

    glDisable(GL_LIGHTING);
}


//-----------------------------------------------------------------------------
// regular openGL callback functions
bool InitGL()
{
    // transparent
    glShadeModel(GL_SMOOTH);
    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	glEnable( GL_LINE_SMOOTH );
    glHint( GL_LINE_SMOOTH_HINT, GL_NICEST );

    // others
    glEnable( GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);
    glClearColor(1.0, 1.0, 1.0, 0.0);

    //Let's have light!
    GLfloat Diffuse[] =  { 0.9f, 0.9f, 0.9f, 1.0f };
    glMaterialfv(GL_FRONT, GL_DIFFUSE, Diffuse);
    glColorMaterial(GL_FRONT, GL_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);

    GLfloat WhiteLight[] =  { 0.75f, 0.75f, 0.75f, 1.0f };
    glLightfv(GL_LIGHT0,GL_DIFFUSE,WhiteLight);
    glLightfv(GL_LIGHT1,GL_DIFFUSE,WhiteLight);

    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);

    return true;
}

void Reshape( int w, int h)
{
    glViewport( 0, 0, (GLsizei)w, (GLsizei)h );

    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();

	//Perspective view
    gluPerspective( 60, 1.0f*w/h, R/100, R*100 );

	//Othogonal view
	//glOrtho(-R * 1.5, R * 1.5, -R * 1.5, R * 1.5, -R * 100, R * 100);


    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

	//user_spring.ReComputeInfomation();
}

void Mouse(int button, int state, int x, int y)
{









	//if (selected != 100000){ selected = 100000; }

	////selected = 0;
	//GLint viewport[4];
	//GLdouble modelview[16];
	//GLdouble projection[16];
	//GLdouble winX, winY, winZ;
	//GLdouble posX, posY, posZ;

	//glGetIntegerv(GL_VIEWPORT, viewport);
	//glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
	//glGetDoublev(GL_PROJECTION_MATRIX, projection);

	//winX = (float)x;
	//winY = (float)viewport[3] - (float)y-10;  // Subtract The Current Mouse Y Coordinate 
	//glReadPixels(x, winY, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ);//Reads the depth buffer
	//for (int i = 0; i < FFD_lattice.size(); i++){
	//	gluProject(FFD_lattice[i][0], FFD_lattice[i][1], FFD_lattice[i][2], modelview, projection, viewport, &posX, &posY, &posZ);
	//	if ((winX - 7 <= posX) && (posX <= winX + 7) && (winY - 7 <= posY) && (posY <= winY + 7)){
	//		selected = i;
	//	}

	//}



	// This handles mouse click 
	// 
	// TODO: check if the user clicks on one of the lattice nodes
	//
}


void Motion(int x, int y)
{

	if (selected != 100000){
		GLint viewport[4];
		GLdouble modelview[16];
		GLdouble projection[16];
		GLdouble winX, winY, winZ;
		GLdouble posX, posY, posZ;

		glGetIntegerv(GL_VIEWPORT, viewport);
		glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
		glGetDoublev(GL_PROJECTION_MATRIX, projection);

		winX = (float)x;
		winY = (float)viewport[3] - (float)y - 10;  // Subtract The Current Mouse Y Coordinate 
		//glReadPixels(x, winY, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ);//Reads the depth buffer

		for (int i = 0; i < FFD_lattice.size(); i++){
			if (selected == i){
				gluUnProject(winX, winY, winZ, modelview, projection, viewport, &FFD_lattice[i][0], &FFD_lattice[i][1], &FFD_lattice[i][2]);
			}
		}
	}

	//
	// This handles mouse motion when a button is pressed 
	// 
	// TODO: check if the user is moving a selected lattice node
	//       if so move the node to a new location
	//

	// TODO: recompute the position of every vertex in the model 
	//       i.e., using FFD_parameterization and FFD_lattice 
	//       Note, you only need to do this to the first model in "models"
	//

	model& m = models.front();

	glutPostRedisplay();
}

void PassiveMotion(int x, int y)
{
	// This handles mouse motion when mouse button is NOT pressed
	// does nothing now...

	glutPostRedisplay();
}

//Used for simulation/anitmation. 
void TimerCallback(int value)
{
    //in simuation state
    glutPostRedisplay();
    glutTimerFunc(30, TimerCallback,value);
}

//Handle keyboard events
void Keyboard( unsigned char key, int x, int y )
{
    // find closest colorPt3D if ctrl is pressed...
    switch( key ){
        case 27: exit(0);
		case 'g' : showGrid = !showGrid; break;
        case 'w' : showWire=!showWire; break;
        case 'r' : randomColor=!randomColor; break;
		case 'R' : model_colors.clear(); break;
		case 'L' : light=!light; break;
		case 'b' : background=!background; break;
		case 'S' : showSharpEdgeOnly=!showSharpEdgeOnly;
		           for(map<model*,int>::iterator i=model_wire_gids.begin();i!=model_wire_gids.end();i++) glDeleteLists(i->second,1);
		           model_wire_gids.clear();
		           break;
    }
    glutPostRedisplay();
}



