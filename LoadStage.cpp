//================================================//

#include "GameState.hpp"

//================================================//

void GameState::loadStage(void)
{
	Settings& settings = Settings::getSingleton();

	// Init DotSceneLoader
	DotSceneLoader* loader = new DotSceneLoader(m_pEventManager->getDynamicObjectManager(), m_physics);
	Ogre::SceneNode* scene = m_pSceneMgr->getRootSceneNode()->createChildSceneNode("MainScene");

	// Reset hydrax/skyx
	m_hydraxInitialised = m_skyXInitialised = false;

	// Create the scene based on current stage
	switch(m_profile->getStage()){
	default:
		m_pSceneMgr->setSkyBox(true, "Examples/MorningSkybox");
		break;

	// ======== //

	// Oil Rig
	case Profile::STAGE::DEV:
		{
			settings.graphics.sky = Settings::LOW;

			// Skybox
			if(settings.graphics.sky >= Settings::MEDIUM){
				m_skyXController = new SkyX::BasicController();
				m_skyX = new SkyX::SkyX(m_pSceneMgr, m_skyXController);
				m_skyX->create();

				Base::getSingletonPtr()->m_pRoot->addFrameListener(m_skyX);
				Base::getSingletonPtr()->m_pRenderWindow->addListener(m_skyX);

				// Add clouds
				SkyX::CloudLayer::Options options;

				options.WindDirection = Ogre::Vector2(10.0, 0.0);

				m_skyX->getCloudsManager()->add(SkyX::CloudLayer::Options(options));

				m_pSunlight = m_pSceneMgr->createLight("Sunlight");
				m_pSunlight->setType(Ogre::Light::LT_DIRECTIONAL);
				m_pSunlight->setDiffuseColour(Ogre::ColourValue(1.0, 1.0, 1.0));
				m_pSunlight->setSpecularColour(Ogre::ColourValue(1.0, 1.0, 1.0));

				m_skyXInitialised = true;

			}
			else{
				//m_pSceneMgr->setSkyBox(true, "Examples/CloudyNoonSkyBox");
			}

			Ogre::Entity* e;
			Ogre::Plane p;
			p.normal = Ogre::Vector3(1, 0, 0);
			p.d = 0;

			Ogre::MeshManager::getSingleton().createPlane("CityPlane", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
				p, 192000, 108000, 50, 50, true, 1, 1, 1, Ogre::Vector3::UNIT_Y);
			e = m_pSceneMgr->createEntity("City", "CityPlane");
			e->setMaterialName("2A/HotelCity");
			Ogre::SceneNode* plane = m_pSceneMgr->getRootSceneNode()->createChildSceneNode("City");
			plane->attachObject(e);
			plane->setPosition(-50000.0, -9000.0, 0.0);

			Ogre::Light* city = m_pSceneMgr->createLight("CityLight");
			city->setType(Ogre::Light::LT_SPOTLIGHT);
			city->setSpotlightInnerAngle(Ogre::Radian(5.0));
			city->setSpotlightOuterAngle(Ogre::Radian(100.0));
			city->setAttenuation(100500.0, 1.0, 0.0, 0.0);
			city->setPosition(-5000.0, 0.0, 0.0);
			city->setDirection(Ogre::Vector3(1.0, 0.0, 0.0));

			/*city = m_pSceneMgr->createLight("WindowLight");
			city->setType(Ogre::Light::LT_SPOTLIGHT);
			city->setSpotlightInnerAngle(Ogre::Radian(5.0));
			city->setSpotlightOuterAngle(Ogre::Radian(21.0));
			city->setDiffuseColour(Ogre::ColourValue(1.0, 1.0, 1.0));
			city->setSpecularColour(Ogre::ColourValue(0.8, 0.8, 0.8));
			city->setCastShadows(false);
			city->setAttenuation(1000.0, 0.5, 0.0, 0.0);
			city->setPosition(-120.0, 0.0, -5.0);
			city->setSpotlightFalloff(5.0);
			city->setDirection(Ogre::Vector3(-1.0, 0.0, 0.0));*/

			// Scale scene node and all child nodes
			scene->translate(0, 200.0, 0);
			scene->setInheritScale(true);

			loader->parseDotScene("Apartment.scene", "General", m_pSceneMgr, scene);

			

			m_pSceneMgr->setAmbientLight(Ogre::ColourValue(0.005, 0.005, 0.005));
		}
		break;

	// ======== //

	// Test Stage
	case Profile::STAGE::TEST_STAGE:
		{
			m_pSceneMgr->setSkyBox(true, "Examples/TrippySkyBox");

			// DEBUG
			settings.graphics.water = Settings::HIGH;

			// Water

			if(settings.graphics.water == Settings::LOW){																																																																																																																																																																																																																																																																																																																																																												
				Ogre::Entity* e;
				Ogre::Plane p;

				p.normal = Ogre::Vector3(0, 1, 0); 
				p.d = 0;

				Ogre::MeshManager::getSingleton().createPlane("FloorPlane", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
					p, 200000, 200000, 50, 50, true, 1, 200, 200, Ogre::Vector3::UNIT_Z);
				e = m_pSceneMgr->createEntity("Floor", "FloorPlane");
				e->setMaterialName("Examples/Water2");
				Ogre::SceneNode* node = m_pSceneMgr->getRootSceneNode()->createChildSceneNode("WaterPlane");
				node->attachObject(e);
				//e->getMesh()->buildEdgeList();
				//e->setCastShadows(false);

				// Plane underneath
				Ogre::Entity* underPlane;
				Ogre::Plane p2;

				p2.normal = Ogre::Vector3::UNIT_Y;
				p2.d = 0;

				Ogre::MeshManager::getSingleton().createPlane("UnderPlane", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
					p2, 200000, 200000, 50, 50, true, 1, 200, 200, Ogre::Vector3::UNIT_Z);
				underPlane = m_pSceneMgr->createEntity("Under", "UnderPlane");
				underPlane->setMaterialName("blue");
				Ogre::SceneNode* underPlaneNode = m_pSceneMgr->getRootSceneNode()->createChildSceneNode("UnderPlane");
				underPlaneNode->attachObject(underPlane);
				underPlaneNode->translate(0.0, -100.0, 0.0);
			}
			else if(settings.graphics.water >= Settings::HIGH){

				m_hydraxCamera = m_pSceneMgr->createCamera("HydraxCamera");
				
				m_hydraxCamera->setAutoAspectRatio(true);
				m_hydraxCamera->setNearClipDistance(m_player->getCamera()->getOgreCamera()->getNearClipDistance());
				m_hydraxCamera->setFarClipDistance(m_player->getCamera()->getOgreCamera()->getFarClipDistance());
				m_hydraxCamera->setAspectRatio(Ogre::Real(Base::getSingletonPtr()->m_pViewport->getActualWidth()) / 
					Ogre::Real(Base::getSingletonPtr()->m_pViewport->getActualHeight()));

				// Init Hydrax
				m_hydrax = new Hydrax::Hydrax(m_pSceneMgr, m_hydraxCamera, Base::getSingletonPtr()->m_pViewport);

				// Create projected grid module
				Hydrax::Module::ProjectedGrid* module
					= new Hydrax::Module::ProjectedGrid(m_hydrax,
														new Hydrax::Noise::Perlin(),
														Ogre::Plane(Ogre::Vector3(0, 1, 0), Ogre::Vector3(0, 0, 0)),
														Hydrax::MaterialManager::NM_VERTEX,
														Hydrax::Module::ProjectedGrid::Options());
				m_hydrax->setModule(module);

				m_hydrax->loadCfg("HydraxDemo.hdx");

				m_hydrax->create();
				m_hydraxInitialised = true;
			} // end water

			m_pSceneMgr->setAmbientLight(Ogre::ColourValue(0.005, 0.005, 0.005));
		}
		break;

	// ======== //

	// End
	case Profile::STAGE::END:
		{

		}
		break;
	}

	// Scene loaded, delete the loader
	delete loader;
}

//================================================//