////////////////////////////////////////////////////////
// Class to hold the grid layout of our environment
// Used for navigation and AI, not graphics
#pragma once
#ifndef GRID_H
#define GRID_H
#include <iostream>
#include <vector>
#include <assert.h>
#include "BaseApplication.h"

#define NODESIZE 10.0


class GridNode {
private:
	int nodeID;			// identify for the node
	int rCoord;			// row coordinate
	int cCoord;			// column coordinate
	bool clear;			// is the node walkable?
	int fValue;
	int cost;
	GridNode* parent;	//keeps track of parent node for A*
			
public:
	

	Ogre::Entity *entity;	// a pointer to the entity in this node
	char contains;		// For printing... B = blocked, S = start, G = goal, numbers = path
	GridNode();			// default constructor
	GridNode(int nID, int row, int column, bool isC = true);	// create a node
	~GridNode();		// destroy a node

	void setF(int f);
	int getF();
	void setCost(int c);
	int getCost();
	void setID(int id);		// set the node id
	int getID(){return nodeID;};			// get the node ID
	void setRow(int r);		// set the row coordinate
	void setColumn(int c);		// set the column coordinate
	void setParent(GridNode* node);
	GridNode* getParent();
	int getRow();				// get the row and column coordinate of the node
	int getColumn();
	Ogre::Vector3 getPosition(int rows, int cols);	// return the position of this node
	void setClear();		// set the node as walkable
	void setOccupied();		// set the node as occupied
	bool isClear();			// is the node walkable
};

class GridRow {  // helper class
public:
	std::vector<GridNode> data;
	GridRow(int size) {data.resize(size);};
	~GridRow(){};
};

class Grid {
private:
	Ogre::SceneManager* mSceneMgr;		// pointer to scene graph
	std::vector<GridRow> data;  // actually hold the grid data

public:
	Grid(Ogre::SceneManager* mSceneMgr, int numRows, int numCols);	// create a grid
	Grid();
	~Grid();					// destroy a grid

	int nRows;					// number of rows			look at me and my bad coding practices ):
	int nCols;					// number of columns
	int count;				//used for keeping track of what grid.txt to print out

	GridNode* getNode(int r, int c);  // get the node specified 

	GridNode* getNorthNode(GridNode* n);		  // get adjacent nodes;
	GridNode* getSouthNode(GridNode* n);
	GridNode* getEastNode(GridNode* n);
	GridNode* getWestNode(GridNode* n);
	GridNode* getNENode(GridNode* n);
	GridNode* getNWNode(GridNode* n);
	GridNode* getSENode(GridNode* n);
	GridNode* getSWNode(GridNode* n);

	GridNode* getAllNeighbors(GridNode* n);

	int getDistance(GridNode* node1, GridNode* node2);  // get Manhattan distance between between two nodes
	
	void printToFile(int count);				// Print a grid to a file.  Good for debugging
	void loadObject(std::string name, std::string filename, int row, int height, int col, float scale = 1); // load and place a model in a certain location.
	Ogre::Vector3 getPosition(int r, int c);	// return the position  
};

#endif