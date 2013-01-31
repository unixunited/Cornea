//================================================//

#include "GameState.hpp"

//================================================//

GameState::GameState(void)
{
	//m_GUIEventId = new int(0);
	m_bQuit = false;
	m_GUIEventId = 0;
}

//================================================//

void GameState::enter(void)
{
	Base::getSingletonPtr()->m_pLog->logMessage("[S] Entering GameState...");
	m_bQuit = false;

	// Create Ogre octree scene manager
	m_pSceneMgr = Base::getSingletonPtr()->m_pRoot->createSceneManager("OctreeSceneManager", "GameSceneMgr");

	// Allow update loop to load the game
	m_state = STATE_LOADING_FIRST_ENTRY;
	m_loadingStep = STEP_FIRST;

	m_stage = Profile::getSingletonPtr()->getStage();

	new Boots();
}

//================================================//

void GameState::exit(void)
{
	Base::getSingletonPtr()->m_pLog->logMessage("[S] Leaving GameState...");

	destroyGUI();

	// Free environment
	delete m_pEventManager;

	// Free the player's boots
	delete Boots::getSingletonPtr();
	delete m_player;
	
	// Clear physics
	Base::getSingletonPtr()->destroyPhysicsWorld();

	// Cleanup scene manager
	/*m_pSceneMgr->getSceneNode("ogrenode")->detachAllObjects();
	m_pSceneMgr->getEntity("ogre")->getMesh()->unload();
	m_pSceneMgr->destroyEntity("ogre");*/
	
	m_pSceneMgr->clearScene();
	m_pSceneMgr->destroyAllCameras();

	//Ogre::MeshManager::getSingleton().removeAll();
	//Ogre::ResourceGroupManager::getSingleton().destroyResourceGroup("Popular");
	
	//Ogre::MaterialManager::getSingleton().unloadAll();
	//Ogre::TextureManager::getSingleton().unloadAll();
	//Ogre::GpuProgramManager::getSingleton().unloadAll();

	Base::getSingletonPtr()->m_pRoot->destroySceneManager(m_pSceneMgr);

	
}

//================================================//

bool GameState::pause(void)
{
	Base::getSingletonPtr()->m_pLog->logMessage("[S] Pausing GameState...");

	destroyGUI();

	return true;
}

//================================================//

void GameState::resume(void)
{
	Base::getSingletonPtr()->m_pLog->logMessage("[S] Resuming GameState...");
	m_bQuit = false;

	createGUI();

	// set the camera again
	Base::getSingletonPtr()->m_pViewport->setCamera(m_player->getCamera()->getOgreCamera());
}

//================================================//

void GameState::createScene(void)
{
	Settings& settings = Settings::getSingleton();

	// Environment
	m_pSceneMgr->setSkyBox(true, "Examples/SpaceSkyBox");

	// Soft shadows
	if(settings.graphics.shadows.enabled){
		m_pSceneMgr->setShadowTextureSelfShadow(true);
		m_pSceneMgr->setShadowTextureCasterMaterial("Sparks/shadow_caster");
		m_pSceneMgr->setShadowTextureCount(settings.graphics.shadows.textureCount);
		m_pSceneMgr->setShadowTextureSize(settings.graphics.shadows.textureSize);
		m_pSceneMgr->setShadowTexturePixelFormat(Ogre::PF_FLOAT16_RGB);
		m_pSceneMgr->setShadowCasterRenderBackFaces(false);
		m_pSceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_TEXTURE_ADDITIVE_INTEGRATED);
		m_pSceneMgr->setShadowTextureFSAA(settings.graphics.shadows.fsaa);
	}
	
	// Player flashlight
	m_player->initFlashlight();

	// plane
	Ogre::Entity* e;
	Ogre::Plane p;

	p.normal = Ogre::Vector3(0, 1, 0); 
	p.d = 0;

	Ogre::MeshManager::getSingleton().createPlane("FloorPlane", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		p, 200000, 200000, 50, 50, true, 1, 200, 200, Ogre::Vector3::UNIT_Z);
	e = m_pSceneMgr->createEntity("Floor", "FloorPlane");
	e->setMaterialName("metal");
	Ogre::SceneNode* node = m_pSceneMgr->getRootSceneNode()->createChildSceneNode("MofoPlane");
	node->attachObject(e);
	//e->getMesh()->buildEdgeList();
	e->setCastShadows(false);

	// Scene
	DotSceneLoader* loader = new DotSceneLoader();
	Ogre::SceneNode* scene = m_pSceneMgr->getRootSceneNode()->createChildSceneNode("Scene");
	loader->parseDotScene("level-test6.scene", "General", m_pSceneMgr, scene);
	delete loader;

	// Scale it and all child nodes
	scene->setInheritScale(true);
	scene->scale(20.0, 20.0, 20.0);

	/*Ogre::Entity* entity;
	entity = m_pSceneMgr->createEntity("ogre", "ogrehead.mesh", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	Ogre::SceneNode* node2 = m_pSceneMgr->getRootSceneNode()->createChildSceneNode("ogrenode");
	node2->attachObject(entity);
	entity->setCastShadows(true);

	node2->setPosition(50, 100, 0);*/

	// Reset ambient light
	//m_pSceneMgr->setAmbientLight(Ogre::ColourValue(0.005, 0.005, 0.005));
}

//================================================//

void GameState::createGUI(void)
{
	Base::getSingletonPtr()->m_GUI_Platform->getRenderManagerPtr()->setSceneManager(m_pSceneMgr);

	// HUD
	m_GUIHudLayer = new GUIGameState::GUIHudLayer(&m_GUIEventId);
	m_GUIHudLayer->create();


	/**/

	MyGUI::PointerManager::getInstancePtr()->setVisible(false);

	// Crosshair
	/*const MyGUI::IntSize size(80, 26);
	m_crosshairs = Base::getSingletonPtr()->m_GUI->createWidget<MyGUI::ImageBox>("ImageBox", (screenWidth / 2) - (size.width / 2), 
		(screenHeight / 2) - (size.height / 2), size.width, size.height, MyGUI::Align::Default, "Main");
	m_crosshairs->setImageTexture("D:/OgreSDK/media/materials/textures/Crosshairs.png");
	m_crosshairs->setVisible(true);*/
}

//================================================//

void GameState::createLoadingGUI(void)
{
	Base::getSingletonPtr()->m_GUI_Platform->getRenderManagerPtr()->setSceneManager(m_pSceneMgr);

	// Loading Layer
	m_GUILoadingLayer = new GUIGameState::GUILoadingLayer(&m_GUIEventId);
	m_GUILoadingLayer->create();


	Base::getSingletonPtr()->m_pViewport->setBackgroundColour(Ogre::ColourValue::Black);
}

//================================================//

void GameState::destroyGUI(void)
{
	m_GUIHudLayer->destroy();
	m_GUILoadingLayer->destroy();

	Base::getSingletonPtr()->m_GUI_Platform->getRenderManagerPtr()->setSceneManager(nullptr);
}

//================================================//

void GameState::preloadMeshData(void)
{
	// Load certain mesh data before scene is created to avoid lag spikes during gameplay
	Ogre::Entity* temp;
	
	temp = m_pSceneMgr->createEntity("temp", "br.mesh");
	m_pSceneMgr->destroyEntity(temp);

	temp = m_pSceneMgr->createEntity("temp", "sword.mesh");
	m_pSceneMgr->destroyEntity(temp);
}

//================================================//

bool GameState::keyPressed(const OIS::KeyEvent& arg)
{
	if(m_state != STATE_ACTIVE){
		return true;
	}

	switch(arg.key){
	case OIS::KC_W:
		if(!m_player->getCamera()->m_moveForwardsPressed){
			m_player->getCamera()->m_moveForwardsPressed = true;
		}
		break;

	case OIS::KC_S:
		if(!m_player->getCamera()->m_moveBackwardsPressed){
			m_player->getCamera()->m_moveBackwardsPressed = true;
		}
		break;

	case OIS::KC_A:
		if(!m_player->getCamera()->m_moveLeftPressed){
			m_player->getCamera()->m_moveLeftPressed = true;
		}
		break;

	case OIS::KC_D:
		if(!m_player->getCamera()->m_moveRightPressed){
			m_player->getCamera()->m_moveRightPressed = true;
		}
		break;

	case OIS::KC_SPACE:
		if(!m_player->getCamera()->m_spacePressed){
			m_player->getCamera()->m_spacePressed = true;
		}
		break;

	// Action key
	case OIS::KC_E:
		m_player->action(m_pEventManager);
		break;

	// Keys for switching boots
	case OIS::KC_1:
		//Boots::getSingletonPtr()->equipType(Boots::BOOTS_TYPE_NORMAL);
		m_player->setWeapon(Profile::getSingletonPtr()->getInventory()->getWeapon(0), m_pEventManager);
		break;

	case OIS::KC_2:
		//Boots::getSingletonPtr()->equipType(Boots::BOOTS_TYPE_VELOCITY);
		m_player->setWeapon(Profile::getSingletonPtr()->getInventory()->getWeapon(1), m_pEventManager);
		break;

	case OIS::KC_3:
		//Boots::getSingletonPtr()->equipType(Boots::BOOTS_TYPE_HIGH);
		m_player->setWeapon(Profile::getSingletonPtr()->getInventory()->getWeapon(2), m_pEventManager);
		break;

	case OIS::KC_4:
		m_player->setWeapon(Profile::getSingletonPtr()->getInventory()->getWeapon(3), m_pEventManager);
		break;

	case OIS::KC_TAB:

		break;

	case OIS::KC_CAPITAL:
		//m_pSceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_NONE);
		break;

	// NOTE: Below are debugging keys only, delete later

	case OIS::KC_B:
		{
			static bool bb = false;

			bb = !bb;

			Ogre::SceneNode::ChildNodeIterator itr = m_pSceneMgr->getSceneNode("Scene")->getChildIterator();
			for(;itr.hasMoreElements();){
				Ogre::SceneNode* child = static_cast<Ogre::SceneNode*>(itr.getNext());
				child->showBoundingBox(bb);
			}
		}
		break;

	case OIS::KC_P:
		{
			m_debugDrawer->setDebugMode(!m_debugDrawer->getDebugMode());
		}
		break;

	case OIS::KC_T:
		{
			static bool wireframe = false;

			if(wireframe)
				m_player->getCamera()->getOgreCamera()->setPolygonMode(Ogre::PM_SOLID);
			else
				m_player->getCamera()->getOgreCamera()->setPolygonMode(Ogre::PM_WIREFRAME);

			wireframe = !wireframe;
		}
		break;

	case OIS::KC_F:
		m_player->getFlashlight()->switchState();
		break;

	case OIS::KC_L:
		{
			static bool lighting = false;
			if(lighting){
				m_pSceneMgr->setAmbientLight(Ogre::ColourValue(0.005, 0.005, 0.005));
				lighting = false;
			}
			else{
				m_pSceneMgr->setAmbientLight(Ogre::ColourValue(0.8, 0.8, 0.8));
				lighting = true;
			}
		}
		break;

	case OIS::KC_V:
		Base::getSingletonPtr()->m_pRenderWindow->setVSyncEnabled(!Base::getSingletonPtr()->m_pRenderWindow->isVSyncEnabled());
		break;

	case OIS::KC_F1:
		m_player->getCamera()->setMode(Sparks::Camera::MODE_FIRST_PERSON);
		break;

	case OIS::KC_F2:
		m_player->getCamera()->setMode(Sparks::Camera::MODE_SPECTATOR);
		break;

	case OIS::KC_F5:
		Profile::getSingletonPtr()->save();
		break;

	case OIS::KC_F9:
		if(Profile::getSingletonPtr()->load("TestProfile")){
			printf("Profile loaded!\n");
		}
		else{
			printf("Invalid profile data.\n");
		}
		break;

	case OIS::KC_UP:
		{
			Ogre::Vector3 offset = m_player->getWeapon()->getOffset();

			offset.z -= 0.1;

			m_player->getWeapon()->setOffset(offset);
		}
		break;

	case OIS::KC_DOWN:
		{
			Ogre::Vector3 offset = m_player->getWeapon()->getOffset();

			offset.z += 0.1;

			m_player->getWeapon()->setOffset(offset);
		}
		break;

	case OIS::KC_RIGHT:
		{
			Ogre::Vector3 offset = m_player->getWeapon()->getOffset();

			offset.x += 0.1;

			m_player->getWeapon()->setOffset(offset);
		}
		break;

	case OIS::KC_LEFT:
		{
			Ogre::Vector3 offset = m_player->getWeapon()->getOffset();

			offset.x -= 0.1;

			m_player->getWeapon()->setOffset(offset);
		}
		break;

	case OIS::KC_ADD:
		{
			Ogre::Vector3 offset = m_player->getWeapon()->getOffset();

			offset.y += 0.1;

			m_player->getWeapon()->setOffset(offset);
		}
		break;

	case OIS::KC_SUBTRACT:
		{
			Ogre::Vector3 offset = m_player->getWeapon()->getOffset();

			offset.y -= 0.1;

			m_player->getWeapon()->setOffset(offset);
		}
		break;

	case OIS::KC_ESCAPE:
		m_bQuit = true;
		break;

	default:
		break;
	}

	Base::getSingletonPtr()->keyPressed(arg);

	return true;
}

//================================================//

bool GameState::keyReleased(const OIS::KeyEvent& arg)
{
	if(m_state != STATE_ACTIVE){
		return true;
	}

	switch(arg.key){
		case OIS::KC_W:
		if(m_player->getCamera()->m_moveForwardsPressed){
			m_player->getCamera()->m_moveForwardsPressed = false;
		}
		break;

	case OIS::KC_S:
		if(m_player->getCamera()->m_moveBackwardsPressed){
			m_player->getCamera()->m_moveBackwardsPressed = false;
		}
		break;

	case OIS::KC_A:
		if(m_player->getCamera()->m_moveLeftPressed){
			m_player->getCamera()->m_moveLeftPressed = false;
		}
		break;

	case OIS::KC_D:
		if(m_player->getCamera()->m_moveRightPressed){
			m_player->getCamera()->m_moveRightPressed = false;
		}
		break;

	case OIS::KC_SPACE:
		if(m_player->getCamera()->m_spacePressed){
			m_player->getCamera()->m_spacePressed = false;
		}
		break;

	//// NOTE: Below are only debugging keys, delete later
	//case OIS::KC_3:
	//	if(Boots::getSingletonPtr()->getEquippedType() == Boots::BOOTS_TYPE_DEBUG){
	//		Boots::getSingletonPtr()->setXMultiplier(Boots::getSingletonPtr()->getXMultiplier() - 0.1);
	//		Boots::getSingletonPtr()->setZMultiplier(Boots::getSingletonPtr()->getZMultiplier() - 0.1);
	//	}
	//	break;

	//case OIS::KC_4:
	//	if(Boots::getSingletonPtr()->getEquippedType() == Boots::BOOTS_TYPE_DEBUG){
	//		Boots::getSingletonPtr()->setXMultiplier(Boots::getSingletonPtr()->getXMultiplier() + 0.1);
	//		Boots::getSingletonPtr()->setZMultiplier(Boots::getSingletonPtr()->getZMultiplier() + 0.1);
	//	}
	//	break;

	//case OIS::KC_5:
	//	if(Boots::getSingletonPtr()->getEquippedType() == Boots::BOOTS_TYPE_DEBUG){
	//		Boots::getSingletonPtr()->setYMultiplier(Boots::getSingletonPtr()->getYMultiplier() - 10.0);
	//	}
	//	break;

	//case OIS::KC_6:
	//	if(Boots::getSingletonPtr()->getEquippedType() == Boots::BOOTS_TYPE_DEBUG){
	//		Boots::getSingletonPtr()->setYMultiplier(Boots::getSingletonPtr()->getYMultiplier() + 10.0);
	//	}
	//	break;

	//case OIS::KC_7:
	//	if(Boots::getSingletonPtr()->getEquippedType() == Boots::BOOTS_TYPE_DEBUG){
	//		Boots::getSingletonPtr()->setXAirMultiplier(Boots::getSingletonPtr()->getXAirMultiplier() - 0.005);
	//		Boots::getSingletonPtr()->setZAirMultiplier(Boots::getSingletonPtr()->getZAirMultiplier() - 0.005);
	//	}
	//	break;

	//case OIS::KC_8:
	//	if(Boots::getSingletonPtr()->getEquippedType() == Boots::BOOTS_TYPE_DEBUG){
	//		Boots::getSingletonPtr()->setXAirMultiplier(Boots::getSingletonPtr()->getXAirMultiplier() + 0.005);
	//		Boots::getSingletonPtr()->setZAirMultiplier(Boots::getSingletonPtr()->getZAirMultiplier() + 0.005);
	//	}
	//	break;

	//case OIS::KC_9:
	//	if(Boots::getSingletonPtr()->getEquippedType() == Boots::BOOTS_TYPE_DEBUG){
	//		btVector3 gravity = Boots::getSingletonPtr()->getGravity();

	//		gravity.setY(gravity.getY() - 0.1);
	//		Boots::getSingletonPtr()->setGravity(gravity);
	//	}
	//	break;

	//case OIS::KC_0:
	//	if(Boots::getSingletonPtr()->getEquippedType() == Boots::BOOTS_TYPE_DEBUG){
	//		btVector3 gravity = Boots::getSingletonPtr()->getGravity();

	//		gravity.setY(gravity.getY() + 0.1);
	//		Boots::getSingletonPtr()->setGravity(gravity);
	//	}
	//	break;

	//default:
	//	break;
	}

	Base::getSingletonPtr()->keyReleased(arg);

	return true;
}

//================================================//

bool GameState::mouseMoved(const OIS::MouseEvent& arg)
{
	if(m_state != STATE_ACTIVE){
		return false;
	}

	m_player->getCamera()->pitch(Ogre::Degree(arg.state.Y.rel)); // * Profile::sensitivity
	m_player->getCamera()->yaw(Ogre::Degree(arg.state.X.rel));
	return true;
}

//================================================//

bool GameState::mousePressed(const OIS::MouseEvent& arg, OIS::MouseButtonID id)
{
	if(m_state != STATE_ACTIVE){
		return false;
	}

	if(id == OIS::MB_Left){
		m_player->getWeapon()->attack(m_player->getCamera());
	}
	else if(id == OIS::MB_Right){
		m_player->getWeapon()->attackAlt(m_player->getCamera());
	}

	return true;
}

//================================================//

bool GameState::mouseReleased(const OIS::MouseEvent& arg, OIS::MouseButtonID id)
{
	if(m_state != STATE_ACTIVE){
		return false;
	}

	return true;
}

//================================================//

void GameState::update(double timeSinceLastFrame)
{
	if(m_bQuit){
		this->popAppState();
		return;
	}

	// Update according to sub-state
	switch(m_state){
	default:
		break;

	case STATE_ACTIVE:
		// Update player
		//printf("Player...\n");
		m_player->update(timeSinceLastFrame);

		// Update events
		//printf("Events...\n");
		m_pEventManager->update(timeSinceLastFrame);

		// Update UI
		//printf("UI...\n");
		updateUI();

		// Update physics
		//printf("Bullet...\n");
		updateBullet(timeSinceLastFrame);

		break;

	case STATE_LOADING_FIRST_ENTRY:
		switch(m_loadingStep){
		default:
			
			break;

		case STEP_FIRST:
			// Setup loading screen
			createLoadingGUI();

			// Init player so we can use the camera
			m_player = new Player(m_pSceneMgr);
			break;

		case STEP_CREATE_PHYSICS_WORLD:
			Base::getSingletonPtr()->createPhysicsWorld();
			m_player->getCamera()->init(); // must have physics world to init
			Base::getSingletonPtr()->m_pViewport->setCamera(m_player->getCamera()->getOgreCamera());
			break;

		case STEP_INIT_EVENT_MANAGER:
			m_pEventManager = new EventManager(m_pSceneMgr, m_player->getCamera());
			break;

		case STEP_PRELOAD_MESH_DATA:
			preloadMeshData();
			break;

		case STEP_CREATE_SCENE:
			createScene();
			break;

		case STEP_SETUP_COLLISION_WORLD:
			// Weird gray screen here after moving this code from createScene()
			// Add entities to the collision world
			BtOgre::registerAllEntitiesAsColliders(m_pSceneMgr, Base::getSingletonPtr()->m_btWorld);

			// Add dynamic objects
			m_pEventManager->getDynamicObjectManager()->registerAllObjectsInScene();

			// Debug drawer for Bullet
			m_debugDrawer = new BtOgre::DebugDrawer(m_pSceneMgr->getRootSceneNode(), Base::getSingletonPtr()->m_btWorld);
			Base::getSingletonPtr()->m_btWorld->setDebugDrawer(m_debugDrawer);
			m_debugDrawer->setDebugMode(0);

			m_pSceneMgr->setAmbientLight(Ogre::ColourValue(0.005, 0.005, 0.005));
			break;

		case STEP_CREATE_GUI:
			createGUI();
			break;

		case STEP_FINAL:
			// Final steps before game begins
			m_player->setWeapon(Profile::getSingletonPtr()->getInventory()->getWeapon(0), m_pEventManager);
			m_GUILoadingLayer->setVisible(false);
			m_state = STATE_ACTIVE;
			break;
		}

		// Increment loading step
		m_loadingStep++;

		// Update progress bar
		m_GUILoadingLayer->incrementProgressBar(GUIGameState::GUILoadingLayer::PROGRESSBAR_STATUS, 100);

		break; // STATE_LOADING_FIRST_ENTRY

	case STATE_LOADING_NEXT_STAGE:

		break; // STATE_LOADING_NEXT_STAGE
	}

	

	// Display boot info (DEBUG)
	//char buf[1024];
	//sprintf(buf, "XZ: %.2f // Y:%.2f // A: %.2f // G: %.2f // Offset: <%.2f, %.2f, %.2f>",
	//	Boots::getSingletonPtr()->getXMultiplier(), Boots::getSingletonPtr()->getYMultiplier(),
	//	Boots::getSingletonPtr()->getXAirMultiplier(), Boots::getSingletonPtr()->getGravity().getY(),
	//	m_player->getWeapon()->getOffset().x, m_player->getWeapon()->getOffset().y, m_player->getWeapon()->getOffset().z);
	//MyGUI::UString str((const char*)buf);
	//m_button->setCaption(str);
}

//================================================//

void GameState::updateUI(void)
{
	Rayhit* rayhit = m_player->getCamera()->getNegativeZRayhit();
	bool npcFound = false;
	Ogre::String name = rayhit->node->getName();
	Ogre::StringUtil strUtil;

	if(rayhit->hasHit){
		// Check distance
		if(rayhit->distance <= m_player->getWeapon()->getReticuleRange()){
			// Check if hit point is an NPC
			if(strUtil.startsWith(name, "NPC_", false)){
				std::vector<NPC*> NPCs = m_pEventManager->getNPCManager()->getNPCs();

				if(NPCs.size() > 0){
					// Find the NPC's scene node
					for(std::vector<NPC*>::iterator itr = NPCs.begin();
						itr != NPCs.end();
						++itr){
						Ogre::String npcName = static_cast<Ogre::String>((*itr)->getSceneNode()->getName());

						// Check the name for a match
						if(strUtil.match(name, npcName, true)){
							if((*itr)->isFriendly()){
								m_GUIHudLayer->setOverlayContainerMaterialName(GUIGameState::GUIHudLayer::OVERLAY_RETICULE, 
									m_player->getWeapon()->getReticuleMaterialName(Weapon::RETICULE_FRIENDLY));
							}
							else{
								m_GUIHudLayer->setOverlayContainerMaterialName(GUIGameState::GUIHudLayer::OVERLAY_RETICULE, 
									m_player->getWeapon()->getReticuleMaterialName(Weapon::RETICULE_ENEMY));
							}

							npcFound = true;
							break;
						}
					}
				}
			}
		}
	}

	if(!npcFound){
		m_GUIHudLayer->setOverlayContainerMaterialName(GUIGameState::GUIHudLayer::OVERLAY_RETICULE, 
									m_player->getWeapon()->getReticuleMaterialName(Weapon::RETICULE_DEFAULT));
	}
}

//================================================//

void GameState::updateBullet(double timeSinceLastFrame)
{
	Base::getSingletonPtr()->m_btWorld->stepSimulation(timeSinceLastFrame * 0.015f, 60);
	//Base::getSingletonPtr()->m_btWorld->stepSimulation(1.0f / 60.0f, 60);

	m_debugDrawer->step();

	// Update all rigid bodies
	btRigidBody* body;

	for(std::vector<btRigidBody*>::iterator itr = Base::getSingletonPtr()->m_btObjects.begin(); itr!=Base::getSingletonPtr()->m_btObjects.end(); ++itr){
		Ogre::SceneNode* node = static_cast<Ogre::SceneNode*>((*itr)->getUserPointer());
		body = *itr;

		btVector3 pos = body->getCenterOfMassPosition();
		node->setPosition(Ogre::Vector3((float)pos[0], (float)pos[1], (float)pos[2]));

		btQuaternion orientation = body->getOrientation();
		node->setOrientation(Ogre::Quaternion(orientation.w(), orientation.x(), orientation.y(), orientation.z()));
	}

	// Update the camera's rigid body
	if(m_player->getCamera()->getMode() == Sparks::Camera::MODE_FIRST_PERSON){
		body = m_player->getCamera()->getRigidBody();

		btVector3 pos = body->getCenterOfMassPosition();
		m_player->getCamera()->getSceneNode()->setPosition(Ogre::Vector3((float)pos[0], (float)pos[1], (float)pos[2]));

		btQuaternion orientation = body->getOrientation();
		m_player->getCamera()->getSceneNode()->setOrientation(Ogre::Quaternion(orientation.w(), orientation.x(), orientation.y(), orientation.z()));
	}
}

//================================================//