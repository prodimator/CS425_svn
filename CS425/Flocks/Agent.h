
#include "BaseApplication.h"
#include "Grid.h"
#include <vector>

#include <deque>



class Agent
{
private:
	Ogre::SceneManager* mSceneMgr;		// pointer to scene graph
	Ogre::SceneNode* mBodyNode;			
	Ogre::Entity* mBodyEntity;
	//float sepVel;
	float height;						// height the character should be moved up
	float scale;						// scale of character from original model



	// all of the animations our character has, and a null ID
	// some of these affect separate body parts and will be blended together
	enum AnimID
	{
		ANIM_IDLE_BASE,
		ANIM_IDLE_TOP,
		ANIM_RUN_BASE,
		ANIM_RUN_TOP,
		ANIM_HANDS_CLOSED,
		ANIM_HANDS_RELAXED,
		ANIM_DRAW_SWORDS,
		ANIM_SLICE_VERTICAL,
		ANIM_SLICE_HORIZONTAL,
		ANIM_DANCE,
		ANIM_JUMP_START,
		ANIM_JUMP_LOOP,
		ANIM_JUMP_END,
		ANIM_NONE
	};

	Ogre::AnimationState* mAnims[13];		// master animation list
	AnimID mBaseAnimID;						// current base (full- or lower-body) animation
	AnimID mTopAnimID;						// current top (upper-body) animation
	bool mFadingIn[13];						// which animations are fading in
	bool mFadingOut[13];					// which animations are fading out
	Ogre::Real mTimer;						// general timer to see how long animations have been playing
	Ogre::Real mVerticalVelocity;			// for jumping

	void setupAnimations();					// load this character's animations
	void fadeAnimations(Ogre::Real deltaTime);				// blend from one animation to another
	void updateAnimations(Ogre::Real deltaTime);			// update the animation frame

	// for locomotion
	Ogre::Real mDistance;					// The distance the agent has left to travel
	Ogre::Vector3 mDirection;				// The direction the object is moving
	Ogre::Vector3 mDestination;				// The destination the object is moving towards
	std::deque<Ogre::Vector3> mWalkList;	// The list of points we are walking to
	Ogre::Real mWalkSpeed;					// The speed at which the object is moving
	bool nextLocation();					// Is there another destination?
	void updateLocomote(Ogre::Real deltaTime);			// update the character's walking

	//////////////////////////////////////////////
	// Lecture 4
	bool procedural;						// Is this character performing a procedural animation
    //////////////////////////////////////////////
public:
	Agent(Ogre::SceneManager* SceneManager, std::string name, std::string filename, float height, float scale);
	~Agent();
	void setPosition(float x, float y, float z);
	GridNode* current;

	float getHeight();
	Ogre::Vector3 getPos();
	Ogre::Vector3 velocity;
	float x;
	float y; 
	float z;

	void update(Ogre::Real deltaTime);		// update the agent
	void addWalk(Ogre::Vector3 vec);		//adds node to walklist
	void moveTo(GridNode* node, Grid grid);
		
	//float getSepVel();
	Ogre::Vector3 sepVelocity(std::list<Agent*> agentList);
	Ogre::Vector3 alignVel(std::list<Agent*> agentList);
	Ogre::Vector3 cohere(std::list<Agent*> agentList);
	Ogre::Vector3 goToGoal(std::list<Agent*> agentList, Ogre::Vector3 goal);
	void setBaseAnimation(AnimID id, bool reset = false);	// choose animation to display
	void setTopAnimation(AnimID id, bool reset = false);

};