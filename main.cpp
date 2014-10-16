/************************************************************************************ 
 * Copyright (c) 2014, TU Ilmenau
 * 
 * Build with Irrlicht framework
 * VR_Medienprojekt use:
 *                  Vuzix SDK 3.3
 *					Irrlicht 1.8
 *                  Microsoft Kinect Framework
 *                  IrrEdit 4.5
 *					DirectX 9
 * Viel Dank guys
 * ===================================================================================
 * Authors:  Luis Rojas (luis-alejandro.rojas-vargas@tu-ilmenau.de) 
 *************************************************************************************/

/****************************************************************/
/*                        Defines                               */
/****************************************************************/

#define INT_FULLSCREEN false

#define _QUAKE_MODEL_

//Iwear Defines
#define _IWEAR_ACTIVED_
#define _IWEAR_STEREO_
#define _IWEAR_TRACKING_

#define _DEFAULT_STEP_ 0.5f

/****************************************************************/
/*                        Include                               */
/****************************************************************/

//Haupt Bibliotheken von Irrlicht
#include <irrlicht.h>

//Zusätzliche Bibliotheken
#include <tchar.h>

#ifdef _IWEAR_ACTIVED_
//Haupt Bibliotheken von Iwear VR920
#include "IWear_Tracking.h"		//Bibliothek zu Tracking
#else
#include <windows.h>
#endif


/****************************************************************/
/*                        Namespace                             */
/****************************************************************/

using namespace irr;			//Haupt Namespace von Irrlicht

using namespace core;			//Zusätzliche Namespace
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

#ifdef _IWEAR_ACTIVED_
	using namespace Iwear;
	#ifdef _IWEAR_TRACKING_
		using namespace Tracking;
	#endif
	#ifdef _IWEAR_STEREO_
		using namespace Stereo3D;
	#endif
#endif


/****************************************************************/
/*                        Extra                                 */
/****************************************************************/

//Adicion de parametros en la compilacion apra la libreria (Solo en Windows)
#ifdef _IRR_WINDOWS_
	#pragma comment(lib, "Irrlicht.lib")
	#pragma comment(linker, "/subsystem:windows /ENTRY:mainCRTStartup")
#endif

/****************************************************************/
/*                       Mis Clases                             */
/****************************************************************/

class CAppReciever : public IEventReceiver
{
	private:
		bool KeyDown[KEY_KEY_CODES_COUNT];
	public:
		CAppReciever()
		{
			//Reset All Keys
			for (int i = 0; i < KEY_KEY_CODES_COUNT; i++)
			{
				KeyDown[i] = false;
			}
		}

		virtual bool OnEvent (const SEvent & evento)
		{
			switch (evento.EventType)
			{
				case EET_KEY_INPUT_EVENT:
				{
					KeyDown[evento.KeyInput.Key] = evento.KeyInput.PressedDown;
					break;
				}
				default:
					break;
			}
			return false;
		}

		virtual bool isKeyDown(EKEY_CODE keyCode) const
		{
			return KeyDown[keyCode];
		}

		virtual bool isKeyUp(EKEY_CODE keyCode) const
		{
			return !KeyDown[keyCode];
		}
};

/****************************************************************/
/*                       Extra Funktions                        */
/****************************************************************/

bool MoveCameraArrow (vector3df &CPos, vector3df ViewCamera, CAppReciever* Keys)
{
	vector3df UpVector = vector3df(0.0f,1.0f,0.0f);

	vector3df RightVector = UpVector.crossProduct(ViewCamera);
	
	if(Keys->isKeyDown(KEY_UP))
	{
		CPos += ViewCamera * _DEFAULT_STEP_;
		return true;
	}

	if(Keys->isKeyDown(KEY_DOWN))
	{
		CPos -= ViewCamera * _DEFAULT_STEP_;
		return true;
	}

	if(Keys->isKeyDown(KEY_RIGHT))
	{
		CPos += RightVector * _DEFAULT_STEP_;
		return true;
	}

	if(Keys->isKeyDown(KEY_LEFT))
	{
		CPos -= RightVector * _DEFAULT_STEP_;
		return true;
	}

	return false;

}

/****************************************************************/
/*                        Main                                  */
/****************************************************************/

void main()
{

#ifdef _IWEAR_ACTIVED_
	#ifdef _IWEAR_TRACKING_
		iWearTracker *VRTracker;
	#endif
	#ifdef _IWEAR_STEREO_
		iWearStereo3D *VRStereo;
	#endif
#endif

	//Instancia Evento Presion de Tecla
	CAppReciever* appReceiver = new CAppReciever();

	//Variables de manejo de la camara
	vector3df CamPos = vector3df(0.0f,5,0);
	vector3df ViewVector = vector3df(1.0f,-1.0f,1.0f);

	SIrrlichtCreationParameters Parameters;

	int lastFPS = -1;
	int fps = -1;

	core::stringw strName = L"MedienProjekt - IrrlichtVR [FPS: 0]";

	Parameters.DriverType = EDT_DIRECT3D9;
	Parameters.Vsync = false;
	Parameters.Stencilbuffer = true;
	Parameters.Stereobuffer = false;
	Parameters.Fullscreen = INT_FULLSCREEN;
	Parameters.EventReceiver = appReceiver;
	Parameters.WindowSize = dimension2d<u32>(800, 600);
	
	IrrlichtDevice *device = createDeviceEx( Parameters );

	if (!device)
		return;

	//Driver para trabajar
	IVideoDriver* driver = device->getVideoDriver();//driver->getExposedVideoData();
	//Manejador de Escena
	ISceneManager* smgr = device->getSceneManager();
	//Manejador de GUI
	IGUIEnvironment* guienv = device->getGUIEnvironment();
	
#ifdef _IWEAR_ACTIVED_
	#ifdef _IWEAR_TRACKING_
		//Inicio las gafas
		VRTracker = new iWearTracker();
		VRTracker->Initialize();
		VRTracker->SetFilterState(true);
	#endif

	#ifdef _IWEAR_STEREO_
		//Start Stereo
		VRStereo = new iWearStereo3D();
		VRStereo->ActiveStereo();
	#endif
#endif

	wchar_t Texto[128];
	wcsncpy (Texto, L"Hola Mundo", 10);

	IGUIStaticText *Mitexto = guienv->addStaticText(Texto,
		rect<s32>(10,10,260,22), false);

#ifdef _QUAKE_MODEL_
	//Load Quake Scene
	device->getFileSystem()->addFileArchive("Media/map-20kdm2.pk3");
	//Get Mesh from File
	scene::IAnimatedMesh* mesh = smgr->getMesh("20kdm2.bsp");
	//Add Mesh to Scene
	if (mesh)
	{
		scene::ISceneNode* node = smgr->addOctreeSceneNode(mesh->getMesh(0), 0, -1, 1024);
		//Move Mesh because Not in 0,0,0 Point
		if (node)
			node->setPosition(core::vector3df(-1300,-144,-1249));
	}
#else

	//Agrego el Terreno
	ITerrainSceneNode* terrain = smgr->addTerrainSceneNode("Media/heightmap.png");
	//Escalo el terreno
	terrain->setScale(vector3df(1.0f, 0.25f, 1.0f));
	//muevo el terreno
	terrain->setPosition(core::vector3df(0,-150,0));
	//Apago componente de Luz
	terrain->setMaterialFlag(video::EMF_LIGHTING, false);
	//Aplico una textura
	terrain->setMaterialTexture(0, driver->getTexture("Media/minimap.jpg"));
	//Aplico segunda textura
	//terrain->setMaterialTexture(1, driver->getTexture("Media/redrock.jpg"));
	//Activo tipo de texturizado
	//terrain->setMaterialType(video::EMT_DETAIL_MAP);

	//SkyDome
	driver->setTextureCreationFlag(video::ETCF_CREATE_MIP_MAPS, false);
	ISceneNode* skydome = smgr->addSkyDomeSceneNode(driver->getTexture("Media/skydome.jpg"),16,8,0.95f,2.0f);
	driver->setTextureCreationFlag(video::ETCF_CREATE_MIP_MAPS, true);
#endif

	//Add Camera in the Scene
	ICameraSceneNode* cameraRight = smgr->addCameraSceneNode(0, CamPos, ViewVector);
	cameraRight->setFOV(DEFAULT_FOV_Y);
	cameraRight->setNearValue(DEFAULT_NEAR_Z);
	cameraRight->setFarValue(DEFAULT_FAR_Z);  

	ICameraSceneNode* cameraLeft = smgr->addCameraSceneNode(0, CamPos, ViewVector);
	cameraLeft->setFOV(DEFAULT_FOV_Y);
	cameraLeft->setNearValue(DEFAULT_NEAR_Z);
	cameraLeft->setFarValue(DEFAULT_FAR_Z);
	
	//cameraLeft->bindTargetAndRotation(true);
	//cameraRight->bindTargetAndRotation(true);

	char Chachara[128];

	while(device->run())
	{
		//Valido si la Ventana esta en el Foco
		if(device->isWindowActive())
		{
			//Valido la tecla presionada
			if(appReceiver->isKeyDown(KEY_ESCAPE))
				break;

			//Change Position from Camera
			MoveCameraArrow(CamPos, ViewVector, appReceiver);

			if(appReceiver->isKeyDown(KEY_KEY_Q))
				VRTracker->setStereoSeparation(VRTracker->getStereoSeparation() - SEPARATION_STEP);
			if(appReceiver->isKeyDown(KEY_KEY_W))
				VRTracker->setStereoSeparation(VRTracker->getStereoSeparation() + SEPARATION_STEP);

#ifdef _IWEAR_ACTIVED_

	#ifdef _IWEAR_TRACKING_

			//Update Tracking Sensor
			VRTracker->UpdateSensor();

			ViewVector = VRTracker->CalViewVector(VRTracker->getRadPitch(), VRTracker->getRadYaw());

			//Update Position stereo Camera
			VRTracker->CalcCameraVectors(cameraLeft, CamPos, ViewVector, LEFT_EYE);
			VRTracker->CalcCameraVectors(cameraRight, CamPos, ViewVector, RIGHT_EYE);

			//Sensor Data
			sprintf(Chachara, "Pich: %f - Yaw: %f -- Stereo: %f", VRTracker->getPitch(), -VRTracker->getYaw(), VRTracker->getStereoSeparation());
			//wsprintf(Texto, Chachara);
			mbstowcs (Texto, Chachara, 64);

			Mitexto->setText(Texto);

			//cameraLeft->setRotation(vector3df(VRTracker->getPitch() * -1, VRTracker->getYaw() * -1, 0));
			//cameraRight->setRotation(vector3df(VRTracker->getPitch() * -1, VRTracker->getYaw() * -1, 0));

	#endif
#endif

			/****************** RENDER ***************/

			//Inicio la Escena a Renderizar
			driver->beginScene(true, true, SColor(255,255,255,255));

			// draw left camera
			smgr->setActiveCamera(cameraLeft);
			smgr->drawAll();
			guienv->drawAll();

			driver->endScene();
#ifdef _IWEAR_ACTIVED_

	#ifdef _IWEAR_STEREO_
			VRStereo->SynchronizeEye(IWRSTEREO_LEFT_EYE);

			/************** ojo *******/

			driver->beginScene(true, true, SColor(255,255,255,255));

			// draw right camera
			smgr->setActiveCamera(cameraRight);
			smgr->drawAll();
			guienv->drawAll();

			driver->endScene();

			VRStereo->SynchronizeEye(IWRSTEREO_RIGHT_EYE);
	#endif
#endif
			fps = driver->getFPS();

			if (lastFPS != fps)
			{
				strName = L"MedienProjekt - IrrlichtVR [FPS: ";
				strName += fps;
				strName += "]";

				device->setWindowCaption(strName.c_str());
				lastFPS = fps;
			}
		}
		else
			device->yield();
	}

	device->drop(); //Eliminar el dispositivo de video

#ifdef _IWEAR_ACTIVED_
	#ifdef _IWEAR_TRACKING_
		VRTracker->iWearDispose();
	#endif
#endif

	return;
}