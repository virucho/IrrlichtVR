/************************************************************************************ 
 * Copyright (c) 2014, TU Ilmenau
 * 
 * Build with Irrlicht framework
 * VR_Medienprojekt use:
 *                  Vuzix SDK 3.3
 *					Irrlicht 1.8
 *                  Microsoft Kinect Framework
 *                  IrrEdit 4.5
  *					Directx 9
 * Viel Dank guys
 * ===================================================================================
 * Authors:  Luis Rojas (luis-alejandro.rojas-vargas@tu-ilmenau.de) 
 *************************************************************************************/

#ifndef __C_IWEAR_TRACKING_H_INCLUDED__
#define __C_IWEAR_TRACKING_H_INCLUDED__

/****************************************************************/
/*                        Include                               */
/****************************************************************/

//Zusätzliche Bibliotheken
#include <windows.h>

//Haupt Bibliotheken von Iwear VR920
#include <iweardrv.h>		//Steurung von Beschleunigungssensor
#include <iwrstdrv.h>		//steruung von Bildschrim
#include <iWearSDK.h>		//SDK - HilfesBibliothek

//Haupt Bibliotheken von Irrlicht
#include <irrlicht.h>


using namespace irr;			//Haupt Namespace von Irrlicht
using namespace core;
using namespace scene;

/****************************************************************/
/*                        Defines                               */
/****************************************************************/

enum						{ LEFT_EYE=0, RIGHT_EYE, MONO_EYES };

/****************************************************************/
/*                        Extra                                 */
/****************************************************************/


/****************************************************************/
/*                         Class                                */
/****************************************************************/

namespace Iwear
{
	namespace Tracking
	{
		class iWearTracker
		{
			/********** Member Field **********/
			private:
				//Tracker
				bool isAvailable;
				bool DataTrackerAvailable;
				BOOL FilterState;
				float yaw;
				float pitch;
				float roll;

				float Radyaw;
				float Radpitch;
				float Radroll;

				long y, p, r;

				int ProductID;
				DWORD iwr_Ret;
				//IWR_RETURNS iwr_Ret;
			/********** Constructor **********/
			public:
				iWearTracker();

			/********** Public Methods **********/
				IWR_RETURNS Initialize();
				bool SetFilterState(bool State);
				void UpdateSensor();
				void iWearDispose();
				vector3df CalViewVector(float Pitch, float Yaw);

			/********** Private Methods **********/
			private:

				float ConvertToRadians(long value);
				float ConvertToDeg(long value);

			/********** Propiedades **********/
			public:

			int getProductID()
			{
				return ProductID;
			}

			float getYaw()
			{
				return yaw;
			}

			float getPitch()
			{
				return pitch;
			}

			float getRoll()
			{
				return roll;
			}

			float getRadYaw()
			{
				return Radyaw;
			}

			float getRadPitch()
			{
				return Radpitch;
			}

			float getRadRoll()
			{
				return Radroll;
			}

			bool getIsOpen()
			{
				return isAvailable;
			}

			bool gettrackerAvailable()
			{
				return DataTrackerAvailable;
			}
		
			BOOL getFilterState()
			{
				return FilterState;
			}
		};
	}// End namespace Tracking
	
	namespace Stereo3D
	{
		//Estereo Defines
		#define DEFAULT_SEPARATION		0.35f
		#define DEFAULT_FOV_Y			45.0f
		#define DEFAULT_FOCAL_LENGTH	10.0f
		#define DEFAULT_NEAR_Z			1.0f
		#define DEFAULT_FAR_Z			1000.0f
		
		class iWearStereo3D
		{
			/********** Member Field **********/
			private:
				//Stereo
				HANDLE stereoHandle;
				bool stereoAvailable;

				int ProductID;
				DWORD iwr_Ret;

				vector3df UpVector;
			/********** Constructor **********/
			public:
				iWearStereo3D();

			/********** Public Methods **********/
				void ActiveStereo();
				bool SynchronizeEye(int Eye);
				void CalcStereoVectors(ICameraSceneNode* Camera, vector3df CameraPos, vector3df ViewVector, int Eye);

			/********** Private Methods **********/
			private:

			/********** Propiedades **********/

			bool getStereoAvailable()
			{
				return stereoAvailable;
			}
		};
	}// End namespace Stereo3D

}// End namespace Iwear

#endif // __C_IWEAR_TRACKING_H_INCLUDED__

/**************************** END *******************************/