//#include "stdafx.h"
#include <cstdlib>
#include <iostream>
#include "dVector.h"
#include <Irrlicht.h>
#include "newton.h"
#include "irrKlang.h"
//#include "JointLibrary.h"
//#include "Entity.h"
//#include "Custom6DOF.h"
#include "MaterialManager.h"

irr::scene::ICameraSceneNode *camera;
irr::scene::ICameraSceneNode *cameraTop;
irr::IrrlichtDevice *irrDev;
irr::scene::ISceneNode* Model = 0;

NewtonBody *bodyWhite; 
NewtonBody *bodyGrey;
irrklang::ISoundEngine* engine;

// callback functions for the 2 colliding bodies.
static int beginCallback(const NewtonMaterial* material, const NewtonBody* body0, const NewtonBody* body1);
static int processCallback(const NewtonMaterial* material, const NewtonContact* contact);
static void endCallback(const NewtonMaterial* material);

// Values used to identify individual GUI elements
enum
{
	GUI_ID_DIALOG_ROOT_WINDOW  = 0x10000,

	GUI_ID_X_SCALE,
	GUI_ID_Y_SCALE,
	GUI_ID_Z_SCALE,

	GUI_ID_OPEN_MODEL,
	GUI_ID_SET_MODEL_ARCHIVE,
	GUI_ID_LOAD_AS_OCTREE,

	GUI_ID_SKY_BOX_VISIBLE,
	GUI_ID_TOGGLE_DEBUG_INFO,

	GUI_ID_DEBUG_OFF,
	GUI_ID_DEBUG_BOUNDING_BOX,
	GUI_ID_DEBUG_NORMALS,
	GUI_ID_DEBUG_SKELETON,
	GUI_ID_DEBUG_WIRE_OVERLAY,
	GUI_ID_DEBUG_HALF_TRANSPARENT,
	GUI_ID_DEBUG_BUFFERS_BOUNDING_BOXES,
	GUI_ID_DEBUG_ALL,

	GUI_ID_MODEL_MATERIAL_SOLID,
	GUI_ID_MODEL_MATERIAL_TRANSPARENT,
	GUI_ID_MODEL_MATERIAL_REFLECTION,

	GUI_ID_CAMERA_MAYA,
	GUI_ID_CAMERA_FIRST_PERSON,

	GUI_ID_POSITION_TEXT,

	GUI_ID_ABOUT,
	GUI_ID_QUIT,

	GUI_ID_TEXTUREFILTER,
	GUI_ID_SKIN_TRANSPARENCY,
	GUI_ID_SKIN_ANIMATION_FPS,

	GUI_ID_BUTTON_SET_SCALE,
	GUI_ID_BUTTON_SCALE_MUL10,
	GUI_ID_BUTTON_SCALE_DIV10,
	GUI_ID_BUTTON_OPEN_MODEL,
	GUI_ID_BUTTON_SHOW_ABOUT,
	GUI_ID_BUTTON_SHOW_TOOLBOX,
	GUI_ID_BUTTON_SELECT_ARCHIVE,

	GUI_ID_ANIMATION_INFO,

	// And some magic numbers
	MAX_FRAMERATE = 80,
	DEFAULT_FRAMERATE = 30
};