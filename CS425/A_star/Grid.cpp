#include "Grid.h"
#include <iostream>
#include <fstream>


////////////////////////////////////////////////////////////////
// create a node
GridNode::GridNode(int nID, int row, int column, bool isC)
{
	this->clear = isC;

	this->rCoord = row;
	this->cCoord = column;

	this->entity = NULL;

	if (isC)
		this->contains = '.';
	else
		this->contains = 'B';
}

// default constructor
GridNode::GridNode()
{
	nodeID = -999;			// mark these as currently invalid
	this->clear = true;
	this->contains = '.';
} 

////////////////////////////////////////////////////////////////
// destroy a node
GridNode::~GridNode()
{}  // doesn't contain any pointers, so it is just empty

////////////////////////////////////////////////////////////////
// set the node id
void 
GridNode::setID(int id)
{
	this->nodeID = id;
}

////////////////////////////////////////////////////////////////
// set the x coordinate
void 
GridNode::setRow(int r)
{
	this->rCoord = r;
}

////////////////////////////////////////////////////////////////
// set the y coordinate
void 
GridNode::setColumn(int c)
{
	this->cCoord = c;
}

////////////////////////////////////////////////////////////////
// get the x and y coordinate of the node
int 
GridNode::getRow()
{
	return rCoord;
}

int 
GridNode::getColumn()
{
	return cCoord;
}

// return the position of this node
Ogre::Vector3 
GridNode::getPosition(int rows, int cols)
{
	Ogre::Vector3 t;
	t.z = (rCoord * NODESIZE) - (rows * NODESIZE)/2.0 + (NODESIZE/2.0); 
	t.y = 0; 
	t.x = (cCoord * NODESIZE) - (cols * NODESIZE)/2.0 + (NODESIZE/2.0); 
	return t;
}

////////////////////////////////////////////////////////////////
// set the node as walkable
void 
GridNode::setClear()
{
	this->clear = true;
	this->contains = '.';
}

////////////////////////////////////////////////////////////////
// set the node as occupied
void 
GridNode::setOccupied()
{
	this->clear = false;
	this->contains = 'B';
}

////////////////////////////////////////////////////////////////
// is the node walkable
bool 
GridNode::isClear()
{
	return this->clear;
}


////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
// create a grid
Grid::Grid(Ogre::SceneManager* mSceneMgr, int numRows, int numCols)
{
	this->mSceneMgr = mSceneMgr; 

	assert(numRows > 0 && numCols > 0);
	this->nRows = numRows;
	this->nCols = numCols;

	data.resize(numCols, GridRow(numRows));
		
	// put the coordinates in each node
	int count = 0;
	for (int i = 0; i < numRows; i++)
		for (int j = 0; j < numCols; j++)
		{
			GridNode *n = this->getNode(i,j);
			n->setRow(i);
			n->setColumn(j);
			n->setID(count);
			count++;
		}
}

/////////////////////////////////////////
// destroy a grid
Grid::~Grid(){};  														

////////////////////////////////////////////////////////////////
// get the node specified 
GridNode* 
Grid::getNode(int r, int c)
{
	if (r >= nRows || c >= nCols || r < 0 || c < 0)
		return NULL;

	return &this->data[c].data[r];
}

////////////////////////////////////////////////////////////////
// get adjacent nodes;
GridNode* 
Grid::getNorthNode(GridNode *n)
{

	if (n->getRow()-1>=0){                        //checks to see if the row above it is out of bounds									
		GridNode* temp=this->getNode(n->getRow()-1, n->getColumn());		//sets a temp pointer node to the node above it
		if (temp->isClear()==1){                                            //checks to see if it is clear or not--returns temp if it is
			//std::cout<<"North node is available!"<<std::endl;		
			return temp;

		}
		else{
			//std::cout<<"North node is not available! "<<std::endl;      //else returns null
			return NULL;
		}
	}
	else{
		//std::cout<<"North node out of bounds!"<<std::endl;			//returns null if temp is out of bounds
		return NULL;
	}
}
///////////The rest of these are just copy paste from above, but changed to fit the requirements of the specific node
GridNode* 
Grid::getSouthNode(GridNode* n)
{
	if (n->getRow()+1<=this->nRows){
		GridNode* temp=this->getNode(n->getRow()+1, n->getColumn());
		if (temp->isClear()==1){
			//std::cout<<"South node is available!"<<std::endl;
			return temp;

		}
		else{
			//std::cout<<"South node is not available! "<<std::endl;
			return NULL;
		}
	}
	else{
		//std::cout<<"South node out of bounds!"<<std::endl;
		return NULL;
	}

}

GridNode* 
Grid::getEastNode(GridNode* n)
{
	if (n->getColumn()+1<=this->nCols){
		GridNode* temp=this->getNode(n->getRow(), n->getColumn()+1);
		if (temp->isClear()==1){
			//std::cout<<"East node is available!"<<std::endl;
			return temp;

		}
		else{
			//std::cout<<"East node is not available! "<<std::endl;
			return NULL;
		}
	}
	else{
		//std::cout<<"East node out of bounds!"<<std::endl;
		return NULL;
	}
}

GridNode* 
Grid::getWestNode(GridNode* n)
{
	if (n->getColumn()-1>=0){
		GridNode* temp=this->getNode(n->getRow(), n->getColumn()-1);
		if (temp->isClear()==1){
			//std::cout<<"West node is available!"<<std::endl;
			return temp;

		}
		else{
			//std::cout<<"West node is not available! "<<std::endl;
			return NULL;
		}
	}
	else{
		//std::cout<<"West node out of bounds!"<<std::endl;
		return NULL;
	}

}

GridNode* 
Grid::getNENode(GridNode* n)  
{
	if (n->getRow()-1>=0 && n->getColumn()+1<=this->nCols){
		GridNode* temp=this->getNode(n->getRow()-1, n->getColumn()+1);
		if (temp->isClear()==1){
			//std::cout<<"NE node is available!"<<std::endl;
			return temp;

		}
		else{
			//std::cout<<"NE node is not available! "<<std::endl;
			return NULL;
		}
	}
	else{
		//std::cout<<"NE node out of bounds!"<<std::endl;
		return NULL;
	}
}

GridNode* 
Grid::getNWNode(GridNode* n) 
{
	if (n->getRow()-1>=0 && n->getColumn()-1>=0){
		GridNode* temp=this->getNode(n->getRow()-1, n->getColumn()-1);
		if (temp->isClear()==1){
			//std::cout<<"NW node is available!"<<std::endl;
			return temp;

		}
		else{
			//std::cout<<"NW node is not available! "<<std::endl;
			return NULL;
		}
	}
	else{
		//std::cout<<"NW node out of bounds!"<<std::endl;
		return NULL;
	}
}

GridNode* 
Grid::getSENode(GridNode* n) 
{
	if (n->getRow()+1<=this->nRows && n->getColumn()+1<=this->nCols){
		GridNode* temp=this->getNode(n->getRow()+1, n->getColumn()+1);
		if (temp->isClear()==1){
			//std::cout<<"SE node is available!"<<std::endl;
			return temp;

		}
		else{
			//std::cout<<"SE node is not available! "<<std::endl;
			return NULL;
		}
	}
	else{
		//std::cout<<"SE node out of bounds!"<<std::endl;
		return NULL;
	}
}

GridNode* 
Grid::getSWNode(GridNode* n) 
{
	if (n->getRow()+1<=this->nRows && n->getColumn()-1>=0){
		GridNode* temp=this->getNode(n->getRow()+1, n->getColumn()-1);
		if (temp->isClear()==1){
			//std::cout<<"SW node is available!"<<std::endl;
			return temp;
		}
		else{
			//std::cout<<"SW node is not available! "<<std::endl;
			return NULL;
		}
	}
	else{
		//std::cout<<"SW node out of bounds!"<<std::endl;
		return NULL;
	}
}

GridNode* Grid::getAllNeighbors(GridNode* n){
		//simple method call that calls all neighbor methods instead of individually
	//GridNode* temp=this->getNode(0, 4);	//this is just example code to test that it works.
	//GridNode* temp2=this->getNode(8, 14);

	//std::cout<<getDistance(temp,temp2)<<std::endl;
	
	getNorthNode(n);		  
	getSouthNode(n);
	getEastNode(n);
	getWestNode(n);
	getNENode(n);
	getNWNode(n);
	getSENode(n);
	getSWNode(n);

	return NULL;

}

////////////////////////////////////////////////////////////////
//get distance between between two nodes
int 
Grid::getDistance(GridNode* node1, GridNode* node2)
{
	int manhat= abs(node1->getColumn()-node2->getColumn())+abs(node1->getRow()-node2->getRow());
	return manhat*10;
	//|x1 - x2| + |y1 - y2|
	//then returns above statement multiplied by 10 to get value
}

///////////////////////////////////////////////////////////////////////////////
// Print out the grid in ASCII
void 
Grid::printToFile()
{
	std::string path = __FILE__; //gets the current cpp file's path with the cpp file
	path = path.substr(0,1+path.find_last_of('\\')); //removes filename to leave path
	path+= "Grid.txt"; //if txt file is in the same directory as cpp file
	std::ofstream outFile;
	outFile.open(path);

	if (!outFile.is_open()) // oops. there was a problem opening the file
	{
		std::cout << "ERROR, FILE COULD NOT BE OPENED" << std::endl;	
		return;
	}

	for (int i = 0; i < nRows; i++)
	{
		for (int j = 0; j < nCols; j++)
		{
			outFile << this->getNode(i, j)->contains << " ";
		}
		outFile << std::endl;
	}
	outFile.close();
}

void // load and place a model in a certain location.
Grid::loadObject(std::string name, std::string filename, int row, int height, int col, float scale)
{
	using namespace Ogre;

	if (row >= nRows || col >= nCols || row < 0 || col < 0)
		return;

	Entity *ent = mSceneMgr->createEntity(name, filename);
    SceneNode *node = mSceneMgr->getRootSceneNode()->createChildSceneNode(name,
        Ogre::Vector3(0.0f, 0.0f,  0.0f));
    node->attachObject(ent);
    node->setScale(scale, scale, scale);


	GridNode* gn = this->getNode(row, col);
	node->setPosition(getPosition(row, col)); 
	node->setPosition(getPosition(row, col).x, height, getPosition(row, col).z);
	gn->setOccupied();
	gn->entity = ent;
}

////////////////////////////////////////////////////////////////////////////
// Added this method and changed GridNode version to account for varying floor 
// plane dimensions. Assumes each grid is centered at the origin.
// It returns the center of each square. 
Ogre::Vector3 
Grid::getPosition(int r, int c)	
{
	Ogre::Vector3 t;
	t.z = (r * NODESIZE) - (this->nRows * NODESIZE)/2.0 + NODESIZE/2.0; 
	t.y = 0; 
	t.x = (c * NODESIZE) - (this->nCols * NODESIZE)/2.0 + NODESIZE/2.0; 
	return t;
}