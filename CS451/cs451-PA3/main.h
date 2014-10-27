//------------------------------------------------------------------------------
//  Copyright 2007-2014 by Jyh-Ming Lien and George Mason University
//  See the file "LICENSE" for more information
//------------------------------------------------------------------------------


#pragma once

#include "objReader.h"
#include "model.h"
#include "Library.hpp" //motion capture library
#include "pose_utils.hpp"
#include <list>
#include <float.h>
using namespace std;
using namespace Character;

//-----------------------------------------------------------------------------
// INPUTS
list<string> input_filenames;
string mocap_dir; //motion capture dir

//-----------------------------------------------------------------------------
// Intermediate data
list<model> models; //NOTE: only the first model of the list is used in this code
float R=0;          //radius
Point3d COM;        //center of mass

int currentMotion=0; //current motion dataset, used to animate skeleton
int currentFrame=0;  //current frame in the dataset, used to animate skeleton

//-----------------------------------------------------------------------------

/////------ PA3 Variables START-----

bool BindToSkin = false;       // initially the skeleton is not binded to the skin (mesh)

Character::Pose BindingPose;   // at this pose that the skeleton binds to the skin

vector< vector<double> > SkinningWeights; //weights for each vertex and each bone
                                          //there are N vector<double>, where N is the number of vertices in the mesh
                                          //there are K double in vector<double>, where K is the number of bones in skeleton 

vector< vector<Vector3d> > BoneSpaceCoordinates; //local coordinates for each vertex in bone subspace
                                                //there are N vector<Point3d>, where N is the number of vertices in the mesh
                                                //there are K double in vector<Point3d>, where K is the number of bones in skeleton 

////------ PA3 Variables END-----

//-----------------------------------------------------------------------------

/////------ PA3 TODOs START-----

//TODO: implement this function to setup the binding pose.
//      See details below
void setupBindingPose();

//TODO: implement this function to bind the skeleton to the skin.
//      See details below
void bind2skin();

//TODO: skeleton-subspace deformation. perform SSD 
void SSD();

/////------ PA3 TODOs END-----

//-----------------------------------------------------------------------------
bool readfromfile();
void computeCOM_R();

//-----------------------------------------------------------------------------
bool parseArg(int argc, char ** argv)
{
    for(int i=1;i<argc;i++){
        if(argv[i][0]=='-')
        {
			if (string(argv[i]) == "-mocap")      mocap_dir = argv[++i];
			else
				return false; //unknown
        }
        else{
            input_filenames.push_back(argv[i]);
        }
    }

    return true;
}

void printUsage(char * name)
{
    //int offset=20;
    cerr<<"Usage: "<<name<<" [options] -mocap dir *.obj \n"
        <<"options:\n\n";
    cerr<<"\n-- Report bugs to: Jyh-Ming Lien jmlien@gmu.edu"<<endl;
}

//-----------------------------------------------------------------------------

bool readfromfiles()
{
	if (input_filenames.empty())
	{
		cerr << "! Error: No input model" << endl;
		return false;
	}

	if (mocap_dir.empty())
	{
		cerr << "! Error: No input motion capture data" << endl;
		return false;
	}

	//read obj model
    long vsize=0;
    long fsize=0;

    uint id=0;
    for(list<string>::iterator i=input_filenames.begin();i!=input_filenames.end();i++,id++){
        cout<<"- ["<<id<<"/"<<input_filenames.size()<<"] Start reading "<<*i<<endl;
        model m;
        if(!m.build(*i)) continue;
        cout<<"- ["<<id<<"/"<<input_filenames.size()<<"] Done reading "<<m.v_size<<" vertices and "<<m.t_size<<" facets"<<endl;
        vsize+=m.v_size;
        fsize+=m.t_size;
        models.push_back(m);
    }
    cout<<"- Total: "<<vsize<<" vertices, "<<fsize<<" triangles, and "<<input_filenames.size()<<" models"<<endl;
    computeCOM_R();
	
	//read mocap skeleton and animation
	Library::init(mocap_dir);

	//setup binding pose
	setupBindingPose();

    return true;
}

void computeCOM_R()
{
    //compute a bbox
    double box[6]={FLT_MAX,-FLT_MAX,FLT_MAX,-FLT_MAX,FLT_MAX,-FLT_MAX};
    //-------------------------------------------------------------------------
    for(list<model>::iterator i=models.begin();i!=models.end();i++){
        for(unsigned int j=0;j<i->v_size;j++){
            Point3d& p=i->vertices[j].p;
            if(p[0]<box[0]) box[0]=p[0];
            if(p[0]>box[1]) box[1]=p[0];
            if(p[1]<box[2]) box[2]=p[1];
            if(p[1]>box[3]) box[3]=p[1];
            if(p[2]<box[4]) box[4]=p[2];
            if(p[2]>box[5]) box[5]=p[2];
        }//j
    }//i

    //-------------------------------------------------------------------------
    // compute center of mass and R...
    COM.set( (box[1]+box[0])/2,(box[3]+box[2])/2,(box[5]+box[4])/2);

    //-------------------------------------------------------------------------
	R=0;
    for(list<model>::iterator i=models.begin();i!=models.end();i++){
        for(unsigned int j=0;j<i->v_size;j++){
            Point3d& p=i->vertices[j].p;
            float d=(float)(p-COM).normsqr();
            if(d>R) R=d;
        }//j
    }//i

    R=sqrt(R);
}

void setupBindingPose()
{
	//set binding pose to zero pose
	Library::Motion const &mo = Library::motion(0);
	mo.get_pose(0, BindingPose);

	BindingPose.root_position = Vector3d(0, 0, 0);
	BindingPose.root_orientation = Quaternion();
	for (int k = 0; k<BindingPose.bone_orientations.size(); k++) 
		BindingPose.bone_orientations[k] = Quaternion();

	//
	// TODO: Determine the Binding Pose, you can do it manually 
	//       or you can build a GUI to help you determine the binding pose
	//       The binding pose is a pose that matches the input mesh
	//

	BindingPose.bone_orientations[1] = Quaternion::get(-.30, Vector3d(0, 0, 1));		//baby's left leg--bone numbers are on sheet for reference
	//BindingPose.bone_orientations[3] = Quaternion::get(.30, Vector3d(1,0 , 1));		//ankle movement commented out because not sure if needed
	//BindingPose.bone_orientations[4] = Quaternion::get(.10, Vector3d(0, 1, 0));

	BindingPose.bone_orientations[26] = Quaternion::get(.30, Vector3d(0, 0, 1));		//baby's right leg
	//BindingPose.bone_orientations[28] = Quaternion::get(.30, Vector3d(1,0 , 1));		//ankle movement commented out because not sure if needed
	//BindingPose.bone_orientations[29] = Quaternion::get(.10, Vector3d(0, 1, 0));

	BindingPose.bone_orientations[8] = Quaternion::get(-.10, Vector3d(0, 0, 1));		//baby's left arm
	BindingPose.bone_orientations[9] = Quaternion::get(.08, Vector3d(0, 1, 0));
	BindingPose.bone_orientations[10] = Quaternion::get(-.75, Vector3d(0, 1, 0));
	BindingPose.bone_orientations[11] = Quaternion::get(.50, Vector3d(1, 0, 0));


	BindingPose.bone_orientations[18] = Quaternion::get(.10, Vector3d(0, 0, 1));		//baby's right arm
	BindingPose.bone_orientations[19] = Quaternion::get(-.08, Vector3d(0, 1, 0));
	BindingPose.bone_orientations[20] = Quaternion::get(.75, Vector3d(0, 1, 0));
	BindingPose.bone_orientations[21] = Quaternion::get(.50, Vector3d(1, 0, 0));

}

//
// This function will be called when "B" (captial b) is pressed
//
double distToBone(model model, WorldBones wb, int i, int j){
		Vector3d pa(model.vertices[j].p[0] - wb.bases[i][0], model.vertices[j].p[1] - wb.bases[i][1], model.vertices[j].p[2] - wb.bases[i][2]);
		Vector3d pb(model.vertices[j].p[0] - wb.tips[i][0], model.vertices[j].p[1] - wb.tips[i][1], model.vertices[j].p[2] - wb.tips[i][2]);
		Vector3d ab(wb.tips[i][0] - wb.bases[i][0], wb.tips[i][1] - wb.bases[i][1], wb.tips[i][2] - wb.bases[i][2]);
		double lenAB = sqrt(pow(ab[0], 2) + pow(ab[1], 2) + pow(ab[2], 2));

		double lenAPp = pa*(ab / lenAB);
		//double lenAPp = sqrt(pow(pa[0], 2) + pow(pa[1], 2) + pow(pa[2], 2));
		//cout << lenAPp << " " << lenAPpp << endl;

		Vector3d p = (ab / lenAB)*lenAPp + wb.bases[i];			//point p'
		Vector3d Pp(model.vertices[j].p[0]-p[0], model.vertices[j].p[1]-p[1], model.vertices[j].p[2]-p[2]);

		if (lenAPp<0){
			double lenPA = sqrt(pow(pa[0], 2) + pow(pa[1], 2) + pow(pa[2], 2));
			return lenPA;
		}
		if (lenAPp> lenAB){
			double lenPB = sqrt(pow(pb[0], 2) + pow(pb[1], 2) + pow(pb[2], 2));
			return lenPB;
		}
		else{
			double lenPp= sqrt(pow(Pp[0], 2) + pow(Pp[1], 2) + pow(Pp[2], 2));
			return lenPp;
		}
}

void bind2skin()
{
	if (BindToSkin) return; //already binded
	WorldBones wb;
	get_world_bones(BindingPose, wb);
	
	//work on the first model only
	model& model = models.front();

	vector <vector<double>> temp(model.v_size, vector<double>(30));				//creates an (num verts)x(num bones) vertex
	SkinningWeights = temp;

	cout << "Calculating weights may take some time..." <<model.v_size<< endl;
	for (int j = 0; j < model.v_size; j++){
		double sumDist = 0;
		for (int i=0; i < 30; i++){
			sumDist += pow(1.0/distToBone(model, wb, i, j), 3);
		}
		for (int i = 0; i < 30; i++){
			SkinningWeights[j][i] = pow((1.0 / distToBone(model, wb, i, j)),3) / sumDist;
		}
	}
	
	cout << "Done!" << endl;


	//BoneSpaceCoordinates

	vector <vector<Vector3d>> temp2(model.v_size, vector<Vector3d>(30));				//creates an (num verts)x(num bones) vertex
	BoneSpaceCoordinates = temp2;
	for (int j = 0; j < model.v_size; j++){
		for (int i = 0; i < 30; i++){
			Vector3d vec;
			vec[0] = model.vertices[j].p[0];
			vec[1] = model.vertices[j].p[1];
			vec[2] = model.vertices[j].p[2];
			Quaternion temp = -wb.orientations[i];
			Vector3d PB =temp.rotate(vec-wb.bases[i]);			//if errors in ssd -- look here!!!
			BoneSpaceCoordinates[j][i] = PB;
		}

	}

	//
	// TODO: compute SkinningWeights using BindingPose
	//
	//       Read amc_library/Lirary.hpp, amc_library/Skeleton.hpp
	//       to find out how to get all the bones
	//

	//
	// TODO: compute BoneSpaceCoordinates using BindingPose
	//       
	//       determine the cooridnates of each model vertex with respect to each bone
	//       in binding pose

	//
	BindToSkin = true;
}

//TODO: skeleton-subspace deformation. perform SSD 
void SSD()
{
	//
	//work on the first model only
	//
	WorldBones wb;
	Library::Motion const &mo = Library::motion(currentMotion);
	Character::Pose pos;
	mo.get_pose(currentFrame, pos);
	get_world_bones(pos, wb);
	model& model = models.front();
	
	for (int j = 0; j < model.v_size; j++){
		double sumx = 0;
		double sumy = 0;
		double sumz = 0;
		for (int i = 0; i < 30; i++){
			//Vector3d Pb = wb.orientations[i].rotate(BoneSpaceCoordinates[j][i] + wb.bases[i]);
			Vector3d Pb = wb.orientations[i].rotate(BoneSpaceCoordinates[j][i]) + wb.bases[i];
			sumx += Pb[0]*SkinningWeights[j][i];
			sumy += Pb[1]*SkinningWeights[j][i];
			sumz += Pb[2]*SkinningWeights[j][i];
		}
		model.vertices[j].p[0] = sumx;
		model.vertices[j].p[1] = sumy;
		model.vertices[j].p[2] = sumz;
	}

	//
	// recompute the position of each vertex in model
	// using BoneSpaceCoordinates and SkinningWeights
	//
}

//-----------------------------------------------------------------------------
//
//
//
//  Open GL stuff below
//
//
//-----------------------------------------------------------------------------

#include <draw.h>



