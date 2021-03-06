//================================================//

#ifndef __DYNAMICOBJECT_HPP__
#define __DYNAMICOBJECT_HPP__

#define USE_KINEMATIC_GROUND 1

//================================================//

#include "stdafx.h"
#include "Base.hpp"
#include "Sparks.hpp"
#include "Camera.hpp"
#include "Sound.hpp"
	
//================================================//

class Switch;
class Trigger;

//================================================//

/* A class for interactive/moving objects in the world */
class DynamicObject{
public:

	// Data structures for dynamic objects
	typedef struct{

		struct{
			Ogre::Real		step;
			Ogre::Real		length;
			btScalar		friction;
			bool			spline;
			bool			loop;
			bool			active;
			std::vector<Ogre::Vector3> vectors;
		} animation;

		struct{
			bool			enabled;
			unsigned int	type;
			unsigned int	actionCode;
			bool			loop;
			Ogre::Real		timeout;
			Ogre::String	str;
			int				x;
			bool			hasNext;
			Ogre::String	next;
			bool			invisible;
			Ogre::Real		range;
		} trigger;

		Ogre::ColourValue	colour;
		Ogre::Vector3		offset;
		Ogre::Quaternion	rotationOffset;
		unsigned int		buffer;

	} DYNAMIC_OBJECT_DATA, *PDYNAMIC_OBJECT_DATA;

	typedef struct{
		int type;
		Ogre::Real range;
		Ogre::Real inner, outer;
		Ogre::ColourValue colour;
		bool shadows;
		bool hideNode;
		Ogre::Real buffer;
	} LIGHT_DATA, *PLIGHT_DATA;

	enum{
		STATE_IDLE = 0,
		STATE_ACTIVATED = 1
	};

	enum{
		ARG_NULL = 0,
		ARG_ACTION = 1, 
		ARG_ACTIVATE,
		ARG_DEACTIVATE,
		ARG_OVERRIDE
	};

	enum{
		TYPE_MOVING_OBJECT = 0,
		TYPE_MOVING_KINEMATIC_OBJECT,
		TYPE_ELEVATOR,
		TYPE_ROTATING_DOOR,
		TYPE_SLIDING_DOOR,
		TYPE_SWITCH,
		TYPE_NPC,

		TYPE_STATIC_LIGHT,
		TYPE_PULSE_LIGHT,
		TYPE_FLICKER_LIGHT,

		TYPE_MAGIC_CUBE,

		TYPE_TRIGGER,

		END
	};

	// --- //

	DynamicObject(void);
	virtual ~DynamicObject(void);

	virtual void init(Ogre::SceneNode* node, btCollisionObject* colObj);
	virtual void init(Ogre::SceneManager* mgr, Physics* physics, Ogre::SceneNode* node, btCollisionObject* colObj);
	virtual void initTrigger(Ogre::SceneManager* mgr, Ogre::SceneNode* node, Sparks::Camera* camera){} 
	virtual void initLight(Ogre::SceneManager* mgr, Ogre::SceneNode* node){}

	virtual void initSound(const char* file, bool loop = false);

	virtual unsigned send(unsigned arg);		// send a command
	virtual unsigned recv(void);
	virtual unsigned recv(unsigned arg);

	// Data functions
	virtual DYNAMIC_OBJECT_DATA* getData(void) const;
	virtual void deleteData(void);

	void setUserData(int n, void* data);
	void freeUserData(void);

	// Misc. functions
	virtual void retrieve(void);

	// Some virtual functions to be used by children
	virtual void setupAnimation(void){}
	virtual void setTriggerData(DYNAMIC_OBJECT_DATA* data){}
	virtual void setLinkedObject(DynamicObject* obj){}
	virtual void setLinkedObject(void* obj){}
	virtual void setTimeout(Ogre::Real timeout){}
	virtual void attachSwitch(Switch* _switch){}
	virtual void setNextTrigger(Trigger* next){}

	// Getter functions
	const bool isActive(void) const;
	const unsigned getState(void) const;
	Ogre::SceneNode* getSceneNode(void) const;
	const bool isRetrievable(void) const;
	const bool isRetrieved(void) const;

	static int findType(Ogre::SceneNode* node);
	static unsigned int getTier(int type);

	// Setter functions
	void setState(const unsigned state){ m_state = state; }
	void activate(void);

	bool needsUpdate(void);

	virtual void update(double timeSinceLastFrame) = 0;

protected:

#define USERDATA_SIZE 10

	// This data is used for misc. values needed for certain dynamic objects
	struct{
		void* data[USERDATA_SIZE];
		bool flags[USERDATA_SIZE];
	} m_userData;

	Ogre::SceneManager* m_pSceneMgr;
	Ogre::SceneNode*	m_pSceneNode;
	btCollisionObject*  m_collisionObject;

	bool				m_retrievable;
	bool				m_retrieved;

	bool				m_needsUpdate;
	Ogre::Real			m_updateRange;
	unsigned			m_state;
	
	Sound*				m_pSound;
	bool				m_hasSound;

	Physics*			m_physics;
};

//================================================//

typedef DynamicObject::DYNAMIC_OBJECT_DATA	DynamicObjectData;
typedef DynamicObject::LIGHT_DATA			LightData;

//================================================//

inline const bool DynamicObject::isActive(void) const
{ return (m_state > STATE_IDLE) ? true : false; }

inline const unsigned DynamicObject::getState(void) const
{ return m_state; }

inline Ogre::SceneNode* DynamicObject::getSceneNode(void) const
{ return m_pSceneNode; }

inline const bool DynamicObject::isRetrievable(void) const
{ return m_retrievable; }

inline const bool DynamicObject::isRetrieved(void) const
{ return m_retrieved; }

inline void DynamicObject::activate(void)
{ m_state = STATE_ACTIVATED; }

//================================================//

/* Here the inherited classes will be defined, such as door, elevator, lever, etc. */

//================================================//
//================================================//

/* Moving object, moves along a track of points in an infinite loop */
class MovingObject : public DynamicObject{
public:
	MovingObject(void);
	virtual ~MovingObject(void);

	virtual void setupAnimation(void);

	virtual void update(double timeSinceLastFrame);

protected:
	Ogre::AnimationState*		m_pAnimState;
	bool						m_loop;
};

//================================================//
//================================================//

/* Moving kinematic object, same as MovingObject, but applies friction to rigid bodies, e.g. a moving platform that holds the player */
class MovingKinematicObject : public MovingObject{
public:
	MovingKinematicObject(void);

	void setupAnimation(void);

	void update(double timeSinceLastFrame);

protected:
	btRigidBody*		m_rigidBody;
};

//================================================//
//================================================//

class Elevator : public MovingObject
{
public:
	Elevator(void);

	unsigned send(unsigned arg); 

	void update(double timeSinceLastFrame);

protected:

};

//================================================//
//================================================//

/* Switch that can be turned on or off */

class Switch : public DynamicObject
{
public:
	Switch(void);

	void init(Ogre::SceneManager* mgr, Physics* physics, Ogre::SceneNode* node, btCollisionObject* colObj);

	unsigned send(unsigned arg);
	void setLinkedObject(DynamicObject* obj);

	void deleteData(void);

	void update(double timeSinceLastFrame);

protected:
	DynamicObject*	m_linkedObject;
	bool			m_linked;
};

//================================================//

inline void Switch::setLinkedObject(DynamicObject* obj)
{ m_linkedObject = obj; m_linked = true; }

//================================================//


//================================================//

#endif

//================================================//