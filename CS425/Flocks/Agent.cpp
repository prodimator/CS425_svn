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

Ogre::Vector3
Agent::getPos(){

	return this->mBodyNode->getPosition();

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

Ogre::Vector3
Agent::sepVelocity(list<Agent*> agentList){
	//this code finds the separation velocity
	Ogre::Vector3 c;
	c[0]=0; c[1]=0;c[2]=0;
	for (list<Agent*>::iterator it= agentList.begin(); it!=agentList.end();++it){		//loops through all agents
		if (this!=(*it)){
			if (sqrt(pow(this->getPos()[0]-(*it)->getPos()[0],2)+pow(this->getPos()[2]-(*it)->getPos()[2],2))<12){			//if distance between current agent and agent from list is < 12, move away
				c= c- ((*it)->getPos()-this->getPos());
			}
		}
	}
	return c;
}


Ogre::Vector3
Agent::alignVel(list<Agent*> agentList){
	//calculates average velocity of all of the agents and makes them all match it
	Ogre::Vector3 velocity;
	velocity[0]=0; velocity[1]=0;velocity[2]=0;
	for (list<Agent*>::iterator it= agentList.begin(); it!=agentList.end(); it++){
		if (this!=(*it)){			//loop through list, add currents velocity to sum velocity
			velocity[0]=velocity[0]+(*it)->velocity[0];
			velocity[2]=velocity[2]+(*it)->velocity[2];
		}
	}
	velocity=velocity/(agentList.size()-1);		//divide sum of all velocities by the number of agents -1
	return (velocity-this->velocity);
}


Ogre::Vector3
Agent::cohere(list<Agent*> agentList){
	//this finds the center of the flock and makes the agents move towards it
	Ogre::Vector3 center;
	center[0]=0; center[1]=0;center[2]=0;
	for (list<Agent*>::iterator it= agentList.begin(); it!=agentList.end(); it++){
		if (this!=(*it)){
			center[0]=center[0]+(*it)->getPos()[0];			//sums all positions
			center[2]=center[2]+(*it)->getPos()[2];
		}
	}
	center=center/(agentList.size()-1);		//divides sum by the number of agents - 1
	center=(center-this->getPos());///100;
	center[1]=0;
	return center;
}

Ogre::Vector3
Agent::goToGoal(list<Agent*> agentList, Ogre::Vector3 goal){
	//gets a velocity that aims agent towards goal
	Ogre::Vector3 path;
	path[0]=0; path[1]=0;path[2]=0;
	for (list<Agent*>::iterator it= agentList.begin(); it!=agentList.end(); it++){
		if (this!=(*it)){			//distance to goal / 100 makes the agent move 1% of the way to the goal each step
			path[0]=(goal[0]-this->getPos()[0])/100;
			path[2]=(goal[2]-this->getPos()[2])/100;
		}
	}
	return path;
}





void
Agent::moveTo(GridNode *node, Grid grid){
//A* was here
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


