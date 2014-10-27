#include "Agent.h"
#include <queue>
using namespace std;

struct lessThanF{				//this code is for priority queue if I can get it to work right
	bool operator()(GridNode *node1, GridNode *node2){
		return node1->getF()>=node2->getF();
	}
};

Agent::Agent(Ogre::SceneManager* SceneManager, std::string name, std::string filename, float height, float scale)
{
	using namespace Ogre;


	mSceneMgr = SceneManager; // keep a pointer to where this agent will be

	if (mSceneMgr == NULL)
	{
		std::cout << "ERROR: No valid scene manager in Agent constructor" << std::endl;
		return;
	}

	this->height = height;
	this->scale = scale;

	mBodyNode = mSceneMgr->getRootSceneNode()->createChildSceneNode(); // create a new scene node
	mBodyEntity = mSceneMgr->createEntity(name, filename); // load the model
	mBodyNode->attachObject(mBodyEntity);	// attach the model to the scene node

	mBodyNode->translate(0,height,0); // make the Ogre stand on the plane (almost)
	mBodyNode->scale(scale,scale,scale); // Scale the figure

	setupAnimations();  // load the animation for this character

	// configure walking parameters
	mWalkSpeed = 35.0f;	
	mDirection = Ogre::Vector3::ZERO;
}

Agent::~Agent(){
	// mSceneMgr->destroySceneNode(mBodyNode); // Note that OGRE does not recommend doing this. It prefers to use clear scene
	// mSceneMgr->destroyEntity(mBodyEntity);
}

void 
Agent::setPosition(float x, float y, float z)
{
	this->mBodyNode->setPosition(x,  y+this->getHeight(), z);		//the y+height is height bug discussed in class
}

// update is called at every frame from GameApplication::addTime
void
Agent::update(Ogre::Real deltaTime) 
{
	this->updateAnimations(deltaTime);	// Update animation playback
	this->updateLocomote(deltaTime);	// Update Locomotion
}


void 
Agent::setupAnimations()
{
	this->mTimer = 0;	// Start from the beginning
	this->mVerticalVelocity = 0;	// Not jumping

	// this is very important due to the nature of the exported animations
	mBodyEntity->getSkeleton()->setBlendMode(Ogre::ANIMBLEND_CUMULATIVE);

	// Name of the animations for this character
	Ogre::String animNames[] =
		{"IdleBase", "IdleTop", "RunBase", "RunTop", "HandsClosed", "HandsRelaxed", "DrawSwords",
		"SliceVertical", "SliceHorizontal", "Dance", "JumpStart", "JumpLoop", "JumpEnd"};

	// populate our animation list
	for (int i = 0; i < 13; i++)
	{
		mAnims[i] = mBodyEntity->getAnimationState(animNames[i]);
		mAnims[i]->setLoop(true);
		mFadingIn[i] = false;
		mFadingOut[i] = false;
	}

	// start off in the idle state (top and bottom together)
	setBaseAnimation(ANIM_IDLE_BASE);
	setTopAnimation(ANIM_IDLE_TOP);

	// relax the hands since we're not holding anything
	mAnims[ANIM_HANDS_RELAXED]->setEnabled(true);
}

void 
Agent::setBaseAnimation(AnimID id, bool reset)
{
	if (mBaseAnimID >= 0 && mBaseAnimID < 13)
	{
		// if we have an old animation, fade it out
		mFadingIn[mBaseAnimID] = false;
		mFadingOut[mBaseAnimID] = true;
	}

	mBaseAnimID = id; 

	if (id != ANIM_NONE)
	{
		// if we have a new animation, enable it and fade it in
		mAnims[id]->setEnabled(true);
		mAnims[id]->setWeight(0);
		mFadingOut[id] = false;
		mFadingIn[id] = true;
		if (reset) mAnims[id]->setTimePosition(0);
	}
}
	
void Agent::setTopAnimation(AnimID id, bool reset)
{
	if (mTopAnimID >= 0 && mTopAnimID < 13)
	{
		// if we have an old animation, fade it out
		mFadingIn[mTopAnimID] = false;
		mFadingOut[mTopAnimID] = true;
	}

	mTopAnimID = id;

	if (id != ANIM_NONE)
	{
		// if we have a new animation, enable it and fade it in
		mAnims[id]->setEnabled(true);
		mAnims[id]->setWeight(0);
		mFadingOut[id] = false;
		mFadingIn[id] = true;
		if (reset) mAnims[id]->setTimePosition(0);
	}
}

void 
Agent::updateAnimations(Ogre::Real deltaTime)
{
	using namespace Ogre;

	Real baseAnimSpeed = 1;
	Real topAnimSpeed = 1;

	mTimer += deltaTime; // how much time has passed since the last update

	if (mTopAnimID != ANIM_IDLE_TOP)
	if (mTopAnimID != ANIM_NONE)
	if (mTimer >= mAnims[mTopAnimID]->getLength())
		{
			setTopAnimation(ANIM_IDLE_TOP, true);
			setBaseAnimation(ANIM_IDLE_BASE, true);
			mTimer = 0;
		}
	
	// increment the current base and top animation times
	if (mBaseAnimID != ANIM_NONE) mAnims[mBaseAnimID]->addTime(deltaTime * baseAnimSpeed);
	if (mTopAnimID != ANIM_NONE) mAnims[mTopAnimID]->addTime(deltaTime * topAnimSpeed);

	// apply smooth transitioning between our animations
	fadeAnimations(deltaTime);
}

void 
Agent::fadeAnimations(Ogre::Real deltaTime)
{
	using namespace Ogre;

	for (int i = 0; i < 13; i++)
	{
		if (mFadingIn[i])
		{
			// slowly fade this animation in until it has full weight
			Real newWeight = mAnims[i]->getWeight() + deltaTime * 7.5f; //ANIM_FADE_SPEED;
			mAnims[i]->setWeight(Math::Clamp<Real>(newWeight, 0, 1));
			if (newWeight >= 1) mFadingIn[i] = false;
		}
		else if (mFadingOut[i])
		{
			// slowly fade this animation out until it has no weight, and then disable it
			Real newWeight = mAnims[i]->getWeight() - deltaTime * 7.5f; //ANIM_FADE_SPEED;
			mAnims[i]->setWeight(Math::Clamp<Real>(newWeight, 0, 1));
			if (newWeight <= 0)
			{
				mAnims[i]->setEnabled(false);
				mFadingOut[i] = false;
			}
		}
	}
}

void
Agent::moveTo(GridNode *node, Grid grid){
	//A* implemented in here
	//node is the goal node


	///////////////////////////////////////////////////////////////////////////////////////////////////
	vector<GridNode*>open;				//creates an open list
	vector<GridNode*>closed;			//creates closed list
	this->current->setCost(0);			//sets starting cost to 0
	//this->current->contains='S';		//sets starting node contains to S
	open.push_back(this->current);
	if (this->current->getID()!=node->getID() && node->isClear()){				//checks to see if the starting node is the same as the goal node
		while (this->current->getID()!=node->getID()){							//while the current node is not the goal node-- do A*
			if (grid.getNorthNode(this->current)){								//checks north neightbor
				GridNode* temp=grid.getNorthNode(this->current);				//creates temp node and makes it the north node
					open.push_back(temp);										//pushes that node onto the open list
					temp->setParent(this->current);								//sets the parent of neighbor to the current node
					temp->setCost(this->current->getCost()+10);					//updates cost of neighbor
					temp->setF(temp->getCost()+grid.getDistance(temp,node));	//updates F value of neighbor
			

			//The rest of these just do the same as above, but with the specified neighbor
			}
			if (grid.getEastNode(this->current)){
				GridNode* temp=grid.getEastNode(this->current);
				
					open.push_back(temp);
					temp->setParent(this->current);
					temp->setCost(this->current->getCost()+10);
					temp->setF(temp->getCost()+grid.getDistance(temp,node));
				
			}
			if (grid.getSouthNode(this->current)){
				GridNode* temp=grid.getSouthNode(this->current);
			
					open.push_back(temp);
					temp->setParent(this->current);
					temp->setCost(this->current->getCost()+10);
					temp->setF(temp->getCost()+grid.getDistance(temp,node));
				
			}
			if (grid.getWestNode(this->current)){
				GridNode* temp=grid.getWestNode(this->current);
			
					open.push_back(temp);
					temp->setParent(this->current);
					temp->setCost(this->current->getCost()+10);
					temp->setF(temp->getCost()+grid.getDistance(temp,node));
				
			}
			if (grid.getNENode(this->current)){
				GridNode* temp=grid.getNENode(this->current);
				
					open.push_back(temp);
					temp->setParent(this->current);
					temp->setCost(this->current->getCost()+14);
					temp->setF(temp->getCost()+grid.getDistance(temp,node));
				
			}
			if (grid.getSENode(this->current)){
				GridNode* temp=grid.getSENode(this->current);
			
					open.push_back(temp);
					temp->setParent(this->current);
					temp->setCost(this->current->getCost()+14);
					temp->setF(temp->getCost()+grid.getDistance(temp,node));
				
			}
			if (grid.getSWNode(this->current)){
				GridNode* temp=grid.getSWNode(this->current);
				/*bool chk=false;							//this code was used to check if neighbor is alread in open list, but as said in README, it wasnt working correctly
				for (int i=0;i<open.size();i++){			//commented out for future reference and implementation
					if (temp->getID()==open[i]->getID()){
						chk=true;
					}
				}
				if (chk==true){
					if (temp->getCost()>this->current->getCost()+14){
						cout<<"here"<<endl;
						temp->setParent(this->current);
						temp->setCost(this->current->getCost()+14);
						temp->setF(temp->getCost()+grid.getDistance(temp,node));
					}
				}
				else{*/
					open.push_back(temp);
					temp->setParent(this->current);
					temp->setCost(this->current->getCost()+14);
					temp->setF(temp->getCost()+grid.getDistance(temp,node));
			//	}
			}
			if (grid.getNWNode(this->current)){
				GridNode* temp=grid.getNWNode(this->current);				
					open.push_back(temp);
					temp->setParent(this->current);
					temp->setCost(this->current->getCost()+14);
					temp->setF(temp->getCost()+grid.getDistance(temp,node));
	
			}

			//this code finds the node in open with the minimum F val
			GridNode* min=open[0];							//sets min to the beginning of the vector
			int loc=0;										//keeps track of location in vector for use outside of for loop
			for(int i=1;i<open.size();i++){
				if (min->getF()>open[i]->getF()){			//if min F > another F val, set min to the other node
					min=open[i];
					loc=i;									//keep track of location in open
					i=closed.size();
				}
			}
			open.erase(open.begin()+loc);				//deletes min from open list and puts in closed list
			closed.push_back(min);

			Ogre::Vector3 next= grid.getPosition(min->getRow(), min->getColumn());		//gets coords from min and makes agent walk there
			this->addWalk(next);
			this->current=min;
 		}

	}

	cout<<"\n"<<closed.size()<<endl;
	//this code goes through the closed list and sets the contain

	int c=0;	
	int x=0;
	for (int i=1;i<closed.size()-1;i++){
		if (c>9) {c=0;}
		x=(c)+(int)'0';			//c is used to iterate through 0-9 and set contain as that valu
		closed[i]->contains=x;	
		c++;
	}
	closed[closed.size()-1]->contains='G';			//sets the goal node contain as G
	grid.getNode(closed[0]->getRow(),closed[0]->getColumn())->contains='S';		//this is the only way that setting first node as S works??
	grid.printToFile(grid.count);							//calls printToFile


	//this is used to reset all of the grid nodes so that it contains . and values are set back to 0
	for (int r=0;r<grid.nRows;r++){
		for (int c=0;c<grid.nCols;c++){
			GridNode* temp = grid.getNode(r,c);
			temp->setCost(0);
			temp->setParent(temp);
			temp->setF(0);
			if (temp->isClear()) temp->contains='.';
		}
	}
	open.clear();			//deletes open list
	closed.clear();			//deletes closed list


}

void
Agent::addWalk(Ogre::Vector3 vec){
	this->mWalkList.push_back(vec);
}

float
Agent::getHeight(){
	return this->height;
}

bool 
Agent::nextLocation()
{
	if (mWalkList.empty())
             return false;

	mDestination = mWalkList.front();  // this gets the front of the deque
        mWalkList.pop_front();             // this removes the front of the deque
 
        mDirection = mDestination - mBodyNode->getPosition();
        mDistance = mDirection.normalise();

	return true;
}

void 
Agent::updateLocomote(Ogre::Real deltaTime)
{
	if (mDirection == Ogre::Vector3::ZERO) 
			{
				if (nextLocation()) 
				{
					// Set walking animation
					setTopAnimation(ANIM_RUN_TOP);
					setBaseAnimation(ANIM_RUN_BASE);
				}
			}
	else
         {
			
             Ogre::Real move = mWalkSpeed* deltaTime;						//.1;//mWalkSpeed *mTime;   ********NEED TO GET THIS SPEED RIGHT**********
             mDistance -= move;

			 if (mDistance <= 0.0f)
             {
                 mBodyNode->setPosition(mDestination);
                 mDirection = Ogre::Vector3::ZERO;

				 // Set animation based on if the robot has another point to walk to. 
                if (! nextLocation())
                {
                    // Set Idle animation                     
                    setTopAnimation(ANIM_IDLE_TOP);
					setBaseAnimation(ANIM_IDLE_BASE);
                } 
                else
                {
                    Ogre::Vector3 src = mBodyNode->getOrientation() * Ogre::Vector3::UNIT_Z;
					if ((1.0f + src.dotProduct(mDirection)) < 0.0001f) 
					{
						mBodyNode->yaw(Ogre::Degree(180));
					}
					else
					{
						Ogre::Quaternion quat = src.getRotationTo(mDirection);
						mBodyNode->rotate(quat);
					} // else
                }
            }
			else
            {
                 mBodyNode->translate(mDirection * move);
            } // else
         } // if

}


