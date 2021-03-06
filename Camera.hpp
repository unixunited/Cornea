//================================================//

#ifndef __CAMERA_HPP__
#define __CAMERA_HPP__

//================================================//

#include "stdafx.h"
#include "Base.hpp"
#include "Boots.hpp"
#include "Sparks.hpp"
#include "Physics.hpp"
#include "TextRenderer.hpp"
#include "Settings.hpp"

//================================================//

#define NODE_CAMERA			"CameraNode"
#define NODE_CAMERA_YAW		"CameraYawNode"
#define NODE_CAMERA_PITCH	"CameraPitchNode"
#define NODE_CAMERA_ROLL	"CameraRollNode"

//================================================//

typedef struct{
	bool				hasHit;
	Ogre::SceneNode*	node;
	Ogre::Vector3		hitPoint;
	Ogre::Real			distance;
} Rayhit;

//================================================//

namespace Sparks
{

//================================================//

class Camera
{
public:
	Camera(Ogre::SceneManager* mgr);
	~Camera(void);

	void init(Physics* physics);

	void pitch(Ogre::Degree x);
	void yaw(Ogre::Degree y);
	void roll(Ogre::Degree z);

	void getRayhit(Ogre::Vector3& to, Rayhit* rayhit);
	void getRayhit(Ogre::Vector3& from, Ogre::Vector3& to, Rayhit* rayhit);

	// Setter functions
	void setMode(unsigned mode);
	void setMaxVelocity(Ogre::Real maxVelocity);
	void setPosition(Ogre::Vector3 pos);
	void setOrientation(Ogre::Quaternion q);
	
	// Getter functions
	Ogre::Camera*		getOgreCamera(void) const;
	Ogre::SceneNode*	getSceneNode(void) const;
	Ogre::SceneNode*	getPitchNode(void) const;
	Ogre::SceneNode*	getYawNode(void) const;
	Ogre::SceneNode*	getRollNode(void) const;
	Ogre::Vector3		getDirection(void) const;
	btRigidBody*		getRigidBody(void) const;
	Ogre::Real			getCapsuleHeightOffset(void) const;
	const Ogre::Vector3 getTranslateVector(void) const;
	const unsigned		getMode(void) const;
	Physics*			getPhysics(void) const;

	Rayhit*				getNegativeYRayhit(void) const; 
	Rayhit*				getNegativeZRayhit(void) const;

	void update(double timeSinceLastFrame);	

	// --- //

	// modes
	enum{
		MODE_FIRST_PERSON = 0,
		MODE_SPECTATOR
	};

	// key pressed values
	bool	m_moveForwardsPressed, m_moveBackwardsPressed,
			m_moveLeftPressed, m_moveRightPressed,
			m_moveUpPressed, m_moveDownPressed,
			m_shiftPressed,
			m_spacePressed;

private:
	void createRigidBody(void);
	void createAltRigidBody(void);
	void removeRigidBody(void);

	void moveFirstPerson(double timeSinceLastFrame);
	void updateJump(double timeSinceLastFrame); 
	void moveSpectator(double timeSinceLastFrame);
	void updateRays(void);

	// --- //

	Ogre::SceneManager*		m_pSceneMgr;
	Ogre::Camera*			m_pCamera;
	Ogre::SceneNode*		m_pCameraNode;
	Ogre::SceneNode*		m_pCameraYawNode;
	Ogre::SceneNode*		m_pCameraPitchNode;
	Ogre::SceneNode*		m_pCameraRollNode;

	Rayhit*					m_negativeYRayhit;
	Rayhit*					m_negativeZRayhit;

	Ogre::Real				m_farClipDistance;
	Ogre::Vector3			m_translateVector;
	Ogre::Vector3			m_velocityVector;
	Ogre::Real				m_maxVelocity;
	Ogre::Real				m_sprintVelocity;
	Ogre::Real				m_maxSpecVelocity;
	Ogre::Real				m_acceleration;
	Ogre::Real				m_moveSpeed;
	Ogre::Real				m_specMoveSpeed;
	Ogre::Degree			m_rotateSpeed;
	unsigned				m_mode;
	Ogre::Real				m_cameraHeight;
	Ogre::Real				m_capsuleHeightOffset;
	Ogre::Real				m_capsuleRadius;
	Ogre::Real				m_maxYOffset;

	Physics*				m_physics;
	btRigidBody*			m_btCamera;
	btVector3				m_defGravity;
	btScalar				m_mass;
	Physics::CAMERA_DATA*	m_physicsData;

	Ogre::Real				m_jmpConstant;
	bool					m_jumping;
};

//================================================//

inline void Camera::setMaxVelocity(Ogre::Real maxVelocity)
{ m_maxVelocity = maxVelocity; }

inline Ogre::Camera* Camera::getOgreCamera(void) const
{ return m_pCamera; }

inline Ogre::SceneNode* Camera::getSceneNode(void) const
{ return m_pCameraNode; }

inline Ogre::SceneNode* Camera::getPitchNode(void) const
{ return m_pCameraPitchNode; }

inline Ogre::SceneNode* Camera::getYawNode(void) const
{ return m_pCameraYawNode; }

inline Ogre::SceneNode* Camera::getRollNode(void) const
{ return m_pCameraRollNode; }

inline Ogre::Vector3 Camera::getDirection(void) const
{ return (m_pCameraYawNode->getOrientation() * m_pCameraPitchNode->getOrientation() * Ogre::Vector3(0, 0, -1)); }
//{ return (m_pCamera->getDerivedDirection()); }
//{ return (m_pCameraNode->_getDerivedOrientation() * Ogre::Vector3(0, 0, -1)); }


inline btRigidBody* Camera::getRigidBody(void) const
{ return m_btCamera; }

inline Ogre::Real Camera::getCapsuleHeightOffset(void) const
{ return m_capsuleHeightOffset; }

inline const Ogre::Vector3 Camera::getTranslateVector(void) const
{ return m_translateVector; }

inline const unsigned Camera::getMode(void) const
{ return m_mode; }

inline Rayhit* Camera::getNegativeYRayhit(void) const
{ return m_negativeYRayhit; }

inline Rayhit* Camera::getNegativeZRayhit(void) const
{ return m_negativeZRayhit; }

inline Physics* Camera::getPhysics(void) const
{ return m_physics; }

//================================================//

} // namespace Sparks

//================================================//

#endif

//================================================//