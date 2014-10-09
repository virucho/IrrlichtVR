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

/****************************************************************/
/*                        Include                               */
/****************************************************************/

//Haupt Bibliotheken von Iwear VR920
#include "IWear_Tracking.h"		//Bibliothek zu Tracking

/****************************************************************/
/*                        Namespace                             */
/****************************************************************/

namespace Iwear
{

/****************************************************************/
/*                       Functions                              */
/****************************************************************/

namespace Tracking
{
	/********** Constructor **********/
	iWearTracker::iWearTracker()	
	{
		isAvailable = false;
		DataTrackerAvailable = false;
		FilterState = false;
		yaw = 0;
		pitch = 0;
		roll = 0;

		Radyaw = 0;
		Radpitch = 0;
		Radroll = 0;

		ProductID = 0;
		iwr_Ret = IWR_OK;
	}

	/********** Public Methods **********/
	IWR_RETURNS iWearTracker::Initialize()
	{
		//Macht Geräte auf
		iwr_Ret = IWROpenTracker();

		if(iwr_Ret != IWR_OK)
			return IWR_NO_TRACKER;

		//vergleicht das kennzeichnen des Gerätes
		ProductID = IWRGetProductID();

		if(ProductID != IWR_PROD_VR920)
			return IWR_NOTRACKER_INSTANCE;

		//Liest den Zustand von Filter
		FilterState = IWRGetFilterState();

		isAvailable = true;

		return IWR_OK;
	}

	bool iWearTracker::SetFilterState(bool State)
	{
		IWRSetFilterState(State);

		return true;
	}

	void iWearTracker::UpdateSensor()
	{
		//Abtastung von Sensor
		iwr_Ret = IWRGetTracking(&y, &p, &r);
		if(iwr_Ret == IWR_OK)
		{
			yaw = ConvertToDeg(y);
			pitch = ConvertToDeg(p);
			roll = ConvertToDeg(r);

			Radyaw = ConvertToRadians(y);
			Radpitch = ConvertToRadians(p);
			Radroll = ConvertToRadians(r);

			DataTrackerAvailable = true;
		}
		else
			DataTrackerAvailable = false;
	}

	void iWearTracker::iWearDispose()
	{
		IWRCloseTracker();

		isAvailable = false;
	}

	vector3df iWearTracker::CalViewVector(float Pitch, float Yaw)
	{
		float y = tan(Pitch);
		float x = tan(-Yaw);

		vector3df Target = vector3df(x, y, 1.0f);
		return Target;
	}

	/********** Private Methods **********/

	float iWearTracker::ConvertToRadians(long value)
	{
		return (float)value * IWR_RAWTORAD;
	}

	float iWearTracker::ConvertToDeg(long value)
	{
		return (float)value * IWR_RAWTODEG;
	}
}// End namespace Tracking

namespace Stereo3D
{
	/********** Constructor **********/
	iWearStereo3D::iWearStereo3D()	
	{
		stereoHandle = NULL;
		stereoAvailable = false;

		ProductID = 0;
		iwr_Ret = IWR_OK;

		UpVector = vector3df(0.0f,1.0f,0.0f);
	}
	
	void iWearStereo3D::ActiveStereo()
	{
		if (stereoAvailable)
		{
			IWRSTEREO_SetStereo(stereoHandle, true);
		}
		else
		{
			stereoHandle = IWRSTEREO_Open();
				
			if(stereoHandle == INVALID_HANDLE_VALUE)
			{
				if (GetLastError() == ERROR_INVALID_FUNCTION) 
				{
					//MessageBox( NULL, "Your VR920 firmware does not support stereoscopy.  Please update your firmware.", "VR920 STEREO WARNING", MB_OK );
					stereoAvailable	= false;
				}
				else 
				{
					//MessageBox( NULL, "NO VR920 Stereo Driver handle", "VR920 STEREO ERROR", MB_OK );
					stereoAvailable	= false;
				}
			}
			else //Alles OK
			{
				IWRSTEREO_SetStereo(stereoHandle, IWR_STEREO_MODE);
				stereoAvailable = true;
			}
		}
	}

	bool iWearStereo3D::SynchronizeEye(int Eye)
	{
		IWRSTEREO_WaitForAck(stereoHandle, Eye);

		// In windowed mode, we must poll for vSync.
		//if (!State.Graphics.IsFullScreen)
		//    while (State.Device.RasterStatus.ScanLine < windowBottomLine) ;

		IWRSTEREO_SetLR(stereoHandle, Eye);

		return true;
	}

	void iWearStereo3D::CalcStereoVectors(ICameraSceneNode* Camera, vector3df CameraPos, vector3df ViewVector, int Eye)
	{
		vector3df RightVector;
		vector3df vStereoAdj;

		vector3df vEyePt = CameraPos;
		vector3df vLookatPt = ViewVector + vEyePt;

		RightVector = UpVector.crossProduct(ViewVector);
	
		switch( Eye ) 
		{
			case LEFT_EYE:
				// Vector adjust:
				vStereoAdj	= RightVector * DEFAULT_SEPARATION;
				vEyePt		-= vStereoAdj;
				vLookatPt	-= vStereoAdj;

				Camera->setPosition(vEyePt);
				Camera->setTarget(vLookatPt);
			break;
			case RIGHT_EYE:
				// Vector adjust:
				vStereoAdj	= RightVector * DEFAULT_SEPARATION;
				vEyePt		+= vStereoAdj;
				vLookatPt	+= vStereoAdj;

				Camera->setPosition(vEyePt);
				Camera->setTarget(vLookatPt);
			break;
		}
	}
}// End namespace Stereo
	
}// End namespace Iwear

/**************************** END *******************************/
