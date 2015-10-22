#include "main.h"
// use scale factor between Newton and IRR
const float NewtonToIrr = 32.0f;
const float IrrToNewton = (1.0f / NewtonToIrr);
const int NEWTON_GRAVITY = 500;
const int offset = 100;
float lastTimer;
float lastTimerBallIn;
float lastTimerDisplay;
const float zero[] = {0.0f, 0.0f, 0.0f};
//int activeCamera = 1;

#define NUM_BALLS 10
static int ballMaterial;

#define INIT_BCA_X	103
#define INIT_BCA_Y	222
#define INIT_BCA_Z	70
#define INIT_DIST_BCA_NEGRA	83
#define MIN_SPEED .5

using namespace std;
using namespace irr;
using namespace core;
using namespace gui;
using namespace io;
using namespace scene;
using namespace video;


// nivel
scene::IAnimatedMesh* g_map;
scene::IMesh* cueMesh;
scene::IMesh* tempMesh;
scene::ISceneNode* g_mapnode;
scene::ITriangleSelector *g_selector;
NewtonCollision* g_newtonmap;
NewtonBody* g_newtonmapbody;
IVideoDriver* driver;
IGUIEnvironment* env;
IGUIImage* ballImage[NUM_BALLS];
int sinkedIn[NUM_BALLS];
int sinkedInQty = 0;
const float radio = 3.2f;
const int materialBall = 1;


#pragma comment(lib, "Irrlicht.lib")
#pragma comment(lib, "Newton.lib")
#pragma comment(lib, "irrKlang.lib")
//pragma comment(lib, "C:\\Documents and Settings\\irrklang_sound_library\\irrKlang-1.1.3\\lib\\Win32-visualStudio\\irrKlang.lib") // link with irrKlang.dll

// struct for colliding bodies.
struct CollidingBodies
{
   const NewtonBody *colBody0, *colBody1;
   bool SphereWithSphere;
} geomCollision;

// struct for balls
struct GameBallsStructure
{
	NewtonBody *ballBody;
} gameBalls[NUM_BALLS];

class MyEventReceiver : public IEventReceiver
{
public:
	int KeyPressed;
	// We'll create a struct to record info on the mouse state
	struct SMouseState
	{
		core::position2di Position;
		bool LeftButtonDown;
		bool RightButtonDown;
		SMouseState() : LeftButtonDown(false) { }
	} MouseState;

	// This is the one method that we have to implement
	virtual bool OnEvent(const SEvent& event)
	{
		MouseState.RightButtonDown = false;
		// Remember the mouse state
		if (event.EventType == irr::EET_MOUSE_INPUT_EVENT)
		{
			switch(event.MouseInput.Event)
			{
			case EMIE_LMOUSE_PRESSED_DOWN:
				MouseState.LeftButtonDown = true;
				break;
			case EMIE_RMOUSE_PRESSED_DOWN:
				MouseState.RightButtonDown = true;
				break;
			case EMIE_LMOUSE_LEFT_UP:
				MouseState.LeftButtonDown = false;
				break;
			case EMIE_RMOUSE_LEFT_UP:
				MouseState.RightButtonDown = false;
				break;
			case EMIE_MOUSE_MOVED:
				MouseState.Position.X = event.MouseInput.X;
				MouseState.Position.Y = event.MouseInput.Y;
				break;

			default:
				// We won't use the wheel
				break;
			}
		}

		if (event.EventType == irr::EET_JOYSTICK_INPUT_EVENT
			&& event.JoystickEvent.Joystick == 0)
		{
			JoystickState = event.JoystickEvent;
		}

		if (event.EventType == irr::EET_KEY_INPUT_EVENT)
			KeyIsDown[event.KeyInput.Key] = event.KeyInput.PressedDown;

		return false;
	}

	const SEvent::SJoystickEvent & GetJoystickState(void) const
	{
		return JoystickState;
	}

	const SMouseState & GetMouseState(void) const
	{
		return MouseState;
	}

	const int GetKeyPressed(void)
	{
		return KeyPressed;
	}

	virtual bool IsKeyDown(EKEY_CODE keyCode) const
	{
		return KeyIsDown[keyCode];
	}
	
	MyEventReceiver()
	{
		for (u32 i=0; i<KEY_KEY_CODES_COUNT; ++i)
			KeyIsDown[i] = false;
	}

private:
	SEvent::SJoystickEvent JoystickState;
	bool KeyIsDown[KEY_KEY_CODES_COUNT];
};

void stopBall(NewtonBody *theBody)
{
	NewtonBodySetOmega(theBody, &zero[0]);
	NewtonBodySetTorque(theBody, &zero[0]);
	NewtonBodySetVelocity(theBody, &zero[0]);
}

void playBallInSound()
{
	if (irrDev->getTimer()->getTime() - lastTimerBallIn > 100)
	{
		lastTimerBallIn = irrDev->getTimer()->getTime();
		engine->play2D("data/in.wav", false);
	}
}

void addBallToSinkedInList(int ballNumber)
{
	// check if ball already in
	bool ballAlreadyIn = false;
	for (int cont = 0; cont < sinkedInQty; cont++)
		if (sinkedIn[cont] ==  ballNumber)
			ballAlreadyIn = true;

	if (!ballAlreadyIn)
		sinkedIn[sinkedInQty++] = ballNumber;
}

void displaySinkedInBalls()
{
	char *ballsInImages[NUM_BALLS] = {"data/balls/1_small.jpg", "data/balls/2_small.jpg", "data/balls/3_small.jpg", "data/balls/4_small.jpg", "data/balls/5_small.jpg", "data/balls/6_small.jpg", "data/balls/7_small.jpg", "data/balls/8_small.jpg", "data/balls/9_small.jpg", "data/balls/10_small.jpg"};	
	for (int cont = 0; cont < sinkedInQty; cont++)
		ballImage[cont] = env->addImage(driver->getTexture(ballsInImages[sinkedIn[cont]]), core::vector2d<s32>(10, 24 * (cont + 1)));

}

void checkIn(NewtonWorld *nWorld, GameBallsStructure *ballsArray)
{
	ISceneNode *tempNode;
	NewtonBody *tempBody;
	for (int cont = 0; cont < NUM_BALLS; cont++)
	{
		tempBody = ballsArray[cont].ballBody;
		tempNode = (ISceneNode *)NewtonBodyGetUserData(tempBody);
		if (tempNode->getPosition().Y < 180 && tempNode->getPosition().Y > 150)
		{
			playBallInSound();
			addBallToSinkedInList(cont);
//			displaySinkedInBalls();
		}

		// debug
		float ballVel[3] = {0, 0, 0 };
		NewtonBodyGetVelocity(tempBody, (float *)ballVel);
		if (abs(ballVel[0]) < MIN_SPEED && abs(ballVel[1]) < MIN_SPEED && abs(ballVel[2]) < MIN_SPEED)
		{
			stopBall(tempBody);
		}
	}
}

void _cdecl SetMeshTransformEvent(const NewtonBody* body, const float* matrix)
{
	// copy the matrix into an irrlicht matrix4
	matrix4 mat;
	memcpy(mat.pointer(), matrix, sizeof(float)*16);

	// Retreive the user data attached to the newton body
	ISceneNode *tmp = (ISceneNode *)NewtonBodyGetUserData(body);
	if (tmp)
	{
		// Position the node
		tmp->setPosition(mat.getTranslation());		// set position
		tmp->setRotation(mat.getRotationDegrees());	// and rotation
	}
}

void _cdecl ApplyForceAndTorqueEvent (const NewtonBody* body) 
{ 
   float mass; 
   float Ixx; 
   float Iyy; 
   float Izz; 
   float force[3]; 
   float torque[3]; 

   NewtonBodyGetMassMatrix (body, &mass, &Ixx, &Iyy, &Izz); 

   force[0] = 0.0f; 
   force[1] = -NEWTON_GRAVITY * mass;
   force[2] = 0.0f; 

   torque[0] = 0.0f; 
   torque[1] = 0.0f; 
   torque[2] = 0.0f; 

   NewtonBodyAddForce (body, force); 
   NewtonBodyAddTorque (body, torque); 
}

void _cdecl ApplyForceAndTorquePivotEvent (const NewtonBody* body) 
{ 
   float mass; 
   float Ixx; 
   float Iyy; 
   float Izz; 
   float force[3]; 
   float torque[3]; 

   NewtonBodyGetMassMatrix (body, &mass, &Ixx, &Iyy, &Izz); 

   force[0] = 0.0f; 
   force[1] = 10.0f;
   force[2] = 0.0f; 

   torque[0] = 0.0f; 
   torque[1] = 0.0f; 
   torque[2] = 0.0f; 

   NewtonBodyAddForce (body, force); 
   NewtonBodyAddTorque (body, torque); 
}


NewtonBody *createCube(NewtonWorld *nWorld, ISceneManager *smgr, float x, float y, float z, float ladox, float ladoy, float ladoz, IVideoDriver *driver, bool show)
{
	NewtonBody *body3;
	body3 = NewtonCreateBody(nWorld,NewtonCreateBox(nWorld,ladox,ladoy,ladoz,0)) ;
	matrix4 mat3 ;
	mat3.setTranslation(vector3df(x,y,z));
	NewtonBodySetMatrix(body3,mat3.pointer());
	NewtonBodySetMassMatrix(body3,0,2,2,2);
	NewtonBodySetTransformCallback(body3, SetMeshTransformEvent);
	NewtonBodySetForceAndTorqueCallback(body3, ApplyForceAndTorqueEvent);
	NewtonBodyGetMatrix(body3,mat3.pointer());

	if (show)
	{
		ISceneNode*cube3 = smgr->addCubeSceneNode(1,0,-1,core::vector3df(x, y, z),core::vector3df(0, 0, 0),core::vector3df(ladox, ladoy, ladoz));
		cube3->setMaterialFlag(EMF_LIGHTING, false);
		cube3->setMaterialTexture(0, driver->getTexture("data/t351sml.jpg"));
		NewtonBodySetUserData(body3, cube3);
	}
	//cube3->setRotation(mat3.getRotationDegrees());
	//cube3->setPosition(mat3.getTranslation());
	return body3;
}


NewtonBody *createSphere(NewtonWorld *nWorld, ISceneManager *smgr, float x, float y, float z)
{
	dFloat mass;
	dFloat Ixx;
	dFloat Iyy;
	dFloat Izz;
	
	// calculate a (almost) accurate moment of inertia
	vector3df size(radio, radio, radio);
	mass = (dFloat)10;
	Ixx = 0.7f * mass * (size.Y * size.Y + size.Z * size.Z) / 12.0f; // the higher the less inertia
	Iyy = 0.7f * mass * (size.X * size.X + size.Z * size.Z) / 120.0f;
	Izz = 0.7f * mass * (size.X * size.X + size.Y * size.Y) / 12.0f;

	NewtonBody *bodySphere ;
	bodySphere = NewtonCreateBody(nWorld,NewtonCreateSphere(nWorld,radio,radio,radio,0)) ;
	matrix4 matSphere ;
	matSphere.setTranslation(vector3df(x, y, z));
	NewtonBodySetMatrix(bodySphere,matSphere.pointer());
	
	scene::ISceneNode *sphere = smgr->addSphereSceneNode(radio);
	sphere->setMaterialFlag(EMF_LIGHTING, false);
	
	//float omega[] = {-25.0f, 5.0f, -15.0f};
	//NewtonBodySetOmega (bodySphere, &omega[0]); 

	//Set Body mass and Inertia matrix
	NewtonBodySetMassMatrix(bodySphere, mass, Ixx, Iyy, Izz);

	NewtonBodySetUserData(bodySphere, sphere);
	NewtonBodySetTransformCallback(bodySphere, SetMeshTransformEvent);
	NewtonBodySetForceAndTorqueCallback(bodySphere, ApplyForceAndTorqueEvent);
	
	//NewtonBodySetMaterialGroupID(bodySphere, ballMaterial);

	return bodySphere;
}

ISceneNode *createCue(NewtonWorld *nWorld, ISceneManager *smgr, float x, float y, float z, ISceneNode *parent)
{
	cueMesh = smgr->getMesh("data/taco.3ds");
	scene::ISceneNode * cueNode = smgr->addMeshSceneNode(cueMesh, (ISceneNode *)parent, -1, vector3df(x, y, z));
	return cueNode;
}

void createTriangle(NewtonWorld *nWorld, ISceneManager *smgr, int x, float y, int z, ISceneNode *bolas[], IVideoDriver* driver)
{
	char *bolaLink[NUM_BALLS] = {"textures/bola1.jpg", "textures/bola2.jpg", "textures/bola3.jpg", "textures/bola4.jpg", "textures/bola8.jpg", "textures/bola6.jpg", "textures/bola7.jpg", "textures/bola5.jpg", "textures/bola9.jpg","textures/bola10.jpg"};
	NewtonBody *bodySphere[NUM_BALLS];
	int i = NewtonMaterialGetDefaultGroupID(nWorld);
	float ballOffset = 1;
	/*y = 179.5f;*/
	int numBola = 0;
	float posX = 0, posZ = 0;
	int signe = 1;
	for (int i = 1; i < 5; i++)
	{
		posX = x + i * 2 * (radio + ballOffset);
		for (int j = 0; j < i; j++)
		{
			posZ = z + (j * 2 - i) * (radio + ballOffset);
			bodySphere[numBola] = createSphere(nWorld, smgr, posX, y, posZ);
			gameBalls[numBola].ballBody = bodySphere[numBola];

			bolas[numBola] = (ISceneNode *)NewtonBodyGetUserData(bodySphere[numBola]);
			bolas[numBola]->setMaterialTexture(0, driver->getTexture(bolaLink[numBola]));
			numBola++;
		}
	}
}

void resetTriangle(NewtonWorld *nWorld, ISceneManager *smgr, int x, float y, int z, ISceneNode *bolas[], IVideoDriver* driver, GameBallsStructure *ballsArray)
{

	int ballOffset = 1;
	float posX = 0, posZ = 0;
	int signe = 1;
	int numBola = 0;

	ISceneNode *tempNode;
	NewtonBody *tempBody;

	for (int i = 1; i < 5; i++)
	{
		posX = x + i * 2 * (radio + ballOffset);
		for (int j = 0; j < i; j++)
		{
			posZ = z + (j * 2 - i) * (radio + ballOffset);
			tempBody = ballsArray[numBola].ballBody;
			tempNode = (ISceneNode *)NewtonBodyGetUserData(tempBody);
			
			matrix4 matSphere ;
			matSphere.setTranslation(vector3df(posX, y, posZ));
			NewtonBodySetMatrix(tempBody,matSphere.pointer());
			tempNode->setPosition(vector3df(posX, y, posZ));
			float zero[] = {0.0f, 0.0f, 0.0f};
			NewtonBodySetOmega(tempBody, &zero[0]);
			NewtonBodySetTorque(tempBody, &zero[0]);
			NewtonBodySetVelocity(tempBody, &zero[0]);

			numBola++;
		}
	}
}


//void createToolBox()
//{
//	// remove tool box if already there
//	IGUIEnvironment* env = irrDev->getGUIEnvironment();
//	IGUIElement* root = env->getRootGUIElement();
//	IGUIElement* e = root->getElementFromId(GUI_ID_DIALOG_ROOT_WINDOW, true);
//	if (e)
//		e->remove();
//
//	// create the toolbox window
//	IGUIWindow* wnd = env->addWindow(core::rect<s32>(600,45,800,480),
//		false, L"Toolset XYXYXY", 0, GUI_ID_DIALOG_ROOT_WINDOW);
//	IGUITabControl* tab = env->addTabControl(
//		core::rect<s32>(2,20,800-602,480-7), wnd, true, true);
//	IGUITab* t1 = tab->addTab(L"Info");
//	// add some edit boxes and a button to tab one
//	env->addStaticText(L"Campos:",
//			core::rect<s32>(10,20,60,45), false, false, t1);
//	env->addStaticText(L"X:", core::rect<s32>(22,48,40,66), false, false, t1);
//	env->addEditBox(L"1.0", core::rect<s32>(40,46,130,66), true, t1, GUI_ID_X_SCALE);
//	env->addStaticText(L"Y:", core::rect<s32>(22,82,40,96), false, false, t1);
//	env->addEditBox(L"1.0", core::rect<s32>(40,76,130,96), true, t1, GUI_ID_Y_SCALE);
//	env->addStaticText(L"Z:", core::rect<s32>(22,108,40,126), false, false, t1);
//	env->addEditBox(L"1.0", core::rect<s32>(40,106,130,126), true, t1, GUI_ID_Z_SCALE);
//
//}


void updateToolBox()
{
	IGUIEnvironment* env = irrDev->getGUIEnvironment();
	//IGUIElement* root = env->getRootGUIElement();
	//IGUIElement* dlg = root->getElementFromId(GUI_ID_DIALOG_ROOT_WINDOW, true);
	/*if (!dlg )
		return;*/

	IGUIStaticText* postext = env->addStaticText(L"",
			core::rect<s32>(10,80,470,80),false, false, 0 );
	//postext->setVisible(false);

	core::stringw str;//( (s32)core::round_(animatedModel->getAnimationSpeed()) );
	str += L" Frame: ";
	postext->setText(L"test");
}

// check if bodies are almost colliding
static int beginCallback(const NewtonMaterial* material, const NewtonBody* body0, const NewtonBody* body1)
{
   geomCollision.colBody0 = body0;
   geomCollision.colBody1 = body1;
   return true; // telling newton we accept this collision
}

// process the collided bodies
static int processCallback(const NewtonMaterial* material, const NewtonContact* contact)
{
	bool ballTouching1 = false, ballTouching2 = false;
   	
	for (int cont = 0; cont < NUM_BALLS; cont++)
	{
		if (geomCollision.colBody0 == gameBalls[cont].ballBody)
			ballTouching1 = true;
		if (geomCollision.colBody1 == gameBalls[cont].ballBody)
			ballTouching2 = true;
	}

   if(geomCollision.colBody0 == bodyGrey || geomCollision.colBody0 == bodyWhite || ballTouching1)
   {
      if(geomCollision.colBody1 == bodyGrey || geomCollision.colBody1 == bodyWhite || ballTouching2)
      {
         geomCollision.SphereWithSphere = true;
      }
   }
   return true; // telling newton we accept this collision
}

// end the callback and play sound
static void endCallback(const NewtonMaterial* material)
{
   if(geomCollision.SphereWithSphere)
   {
       if (irrDev->getTimer()->getTime() - lastTimer > 200)
	   {
		   lastTimer = irrDev->getTimer()->getTime();
		   engine->play2D("data/balls.wav", false);
       }
	  
      // set boolean to false or you'll hear the sound the whole time.
      geomCollision.SphereWithSphere = false;
   }
}

int main(int argc, char **argv)
{
	bool aiming = true;
	//float newpos[3];
	float cameraAngle = 91.14f;
   // Initialisation Newtoniénne  // ---------------------------------------------------------
	NewtonWorld *nWorld = NewtonCreate(0,0);

	engine = irrklang::createIrrKlangDevice();
    if (!engine)
	{
		printf("Could not startup engine\n");
		return 0; // error starting up the engine
	}

	float maxBox[3];// = box.getMaximum();
	float minBox[3];// = box.getMinimum();
	// add some extra padding
	minBox[0] = 0.0f;
	minBox[1] = 0.0f;
	minBox[2] = 0.0f;
	
	maxBox[0] =  1000.0f;
	maxBox[1] =  1000.0f;
	maxBox[2] =  1000.0f;

	 // set the new world size
	NewtonSetWorldSize (nWorld, &minBox[0], &maxBox[0]);

	//http://irrlicht.sourceforge.net/forum/viewtopic.php?t=25924

	//int i= NewtonMaterialGetDefaultGroupID(nWorld);
	//NewtonMaterialSetDefaultElasticity(nWorld,i,i, 0.01f);
	//NewtonMaterialSetDefaultFriction(nWorld, i, i, .0001f, .0003f);
	//NewtonMaterialSetDefaultSoftness(nWorld, i, i, 0.0005f);
	//NewtonMaterialSetCollisionCallback(nWorld, materialBall, materialBall, NULL,NULL,NULL,NULL);

	//ballMaterial = NewtonMaterialCreateGroupID(nWorld);
	
	bool fullscreen = true;

	// create a NULL device to detect screen resolution
	IrrlichtDevice *nulldevice = createDevice(video::EDT_NULL);
	core::dimension2d<u32> deskres = nulldevice->getVideoModeList()->getDesktopResolution();
	nulldevice -> drop();

	// now the dimensions can be used to create the real device
	//IrrlichtDevice *device = createDevice(video::EDT_DIRECT3D9, deskres, 32, true, false, true, &receiver);

	// Initialisation Irrlichiénne // ---------------------------------------------------------
	irrDev = createDevice (EDT_OPENGL, deskres, 32, fullscreen, true, false, 0);
//	irrDev = createDevice (EDT_OPENGL, dimension2d<u32>(800,600), 32, fullscreen, true, false, 0);
	//int fullscr=0;
	//int x=800; int y = 600;
	//irrDev = createDevice (EDT_OPENGL, dimension2d<u32>(x, y), 32, fullscr, true, false, 0);
	//IrrlichtDevice *irrDev = createDevice (EDT_OPENGL, dimension2d<u32>(1920,1080), 32, true, true, false, 0);
	driver = irrDev->getVideoDriver ();
	ISceneManager *smgr = irrDev->getSceneManager ();
	
	// linar solver model for faster speed
	NewtonSetSolverModel (nWorld, 1);

	// set the adaptive friction model for faster speed
	NewtonSetFrictionModel (nWorld, 1);
    
	SMaterial mat;
      mat.Lighting=false;
      driver->setMaterial(mat);

	MyEventReceiver rv;
	irrDev->setEventReceiver(&rv);

	env = irrDev->getGUIEnvironment();
//	createToolBox();
	
	gui::IGUIToolBar* bar = env->addToolBar();
	IGUIStaticText* fpstext = env->addStaticText(L"",
			core::rect<s32>(10,4,250,40), true, false, bar);
	IGUIStaticText* strengthtext = env->addStaticText(L"",
			core::rect<s32>(260,4,360,40), true, false, bar);
	
	
	IGUIStaticText* whitetext = env->addStaticText(L"",
			core::rect<s32>(380,4,800,40), true, false, bar);
	
	// Creation d'une lumiere ambiente // 
	driver->setAmbientLight(video::SColorf(1.0, 1.0, 1.0,0.0));
	
	// Creation d'une camera FPS // 
	camera = smgr->addCameraSceneNodeFPS(0,100,.0018f);
	cameraTop = smgr->addCameraSceneNode(0, vector3df(INIT_BCA_X + INIT_DIST_BCA_NEGRA / 2, INIT_BCA_Y+200, INIT_BCA_Z), vector3df(INIT_BCA_X + INIT_DIST_BCA_NEGRA / 2, 0, INIT_BCA_Z));
	//	cameraTop->setTarget(core::vector3df (45,0,0));
	//smgr->setActiveCamera(
	
	//camera->setTarget(bolaBlanca->getPosition());	
	//camera = smgr->addCameraSceneNode (0, core::vector3df (0,100,.18f), core::vector3df (90,offset,120));
	
	//camera->setPosition(vector3df(80,offset,120));
	//camera->setTarget(vector3df(90,offset,120));
	
	//const int radio = 15;

	// load map
	irrDev->getFileSystem()->addZipFileArchive("data/poolMesa.pk3");
	g_map = smgr->getMesh("poolMesa.bsp");
	g_mapnode = smgr->addOctTreeSceneNode(g_map->getMesh(0));
	g_newtonmap = NewtonCreateTreeCollision(nWorld, NULL);
	NewtonTreeCollisionBeginBuild(g_newtonmap);
	//int cMeshBuffer, j;
	irr::u32 cMeshBuffer, j;
	int v1i, v2i, v3i;
	IMeshBuffer *mb;

	float vArray[9]; // vertex array (3*3 floats)

	int tmpCount = 0;

	for (cMeshBuffer=0; cMeshBuffer<g_map->getMesh(0)->getMeshBufferCount(); cMeshBuffer++)
	{	
		mb = g_map->getMesh(0)->getMeshBuffer(cMeshBuffer);

		video::S3DVertex2TCoords* mb_vertices = (irr::video::S3DVertex2TCoords*)mb->getVertices();

		u16* mb_indices  = mb->getIndices();

		// add each triangle from the mesh
		for (j=0; j<mb->getIndexCount(); j+=3)
		{
			v1i = mb_indices[j];
			v2i = mb_indices[j+1];
			v3i = mb_indices[j+2];
	
			vArray[0] = mb_vertices[v1i].Pos.X;
			vArray[1] = mb_vertices[v1i].Pos.Y;
			vArray[2] = mb_vertices[v1i].Pos.Z;
			vArray[3] = mb_vertices[v2i].Pos.X;
			vArray[4] = mb_vertices[v2i].Pos.Y;
			vArray[5] = mb_vertices[v2i].Pos.Z;
			vArray[6] = mb_vertices[v3i].Pos.X;
			vArray[7] = mb_vertices[v3i].Pos.Y;
			vArray[8] = mb_vertices[v3i].Pos.Z;

			NewtonTreeCollisionAddFace(g_newtonmap, 3, (float*)vArray, 12, 1);
		}

	}
	NewtonTreeCollisionEndBuild(g_newtonmap, 0);
	g_newtonmapbody = NewtonCreateBody(nWorld, g_newtonmap);
	

	// set the newton world size based on the bsp size
	float boxP0[3]; 
	float boxP1[3]; 
	float matrix[4][4]; 
	NewtonBodyGetMatrix (g_newtonmapbody, &matrix[0][0]); 
	NewtonCollisionCalculateAABB (g_newtonmap, &matrix[0][0],  &boxP0[0], &boxP1[0]); 

	matrix4 matr;
	memcpy(matr.pointer(), (const float*)matrix, sizeof(float)*16);

	ISceneNode *bolas[NUM_BALLS];
	int i = NewtonMaterialGetDefaultGroupID(nWorld);
	createTriangle(nWorld, smgr, INIT_BCA_X + INIT_DIST_BCA_NEGRA, INIT_BCA_Y + 1, INIT_BCA_Z + 3, bolas, driver);
	NewtonBody *tmpBody;
	for (int cont = 0; cont < NUM_BALLS; cont++)
	{
		tmpBody = gameBalls[cont].ballBody;
		NewtonMaterialSetCollisionCallback (nWorld, i, i, tmpBody, beginCallback, processCallback, endCallback);
	}

	// a ghost wall so that the ball does not take off
	createCube(nWorld, smgr,  100, 229, 115, 500, 0, 350, driver, false);

	// white ball
	//NewtonBody *bodyWhite = createSphere(nWorld, smgr, 108, 380, 95);

	bodyWhite = createSphere(nWorld, smgr, INIT_BCA_X, INIT_BCA_Y, INIT_BCA_Z);
	ISceneNode *bolaBlanca  = (ISceneNode *)NewtonBodyGetUserData(bodyWhite);
	bolaBlanca->setMaterialTexture(0, driver->getTexture("textures/bolaBlanca.jpg"));
	bolaBlanca->setPosition(vector3df(INIT_BCA_X, INIT_BCA_Y, INIT_BCA_Z));

	// Set up default material properties for Newton
    // white ball (for knowing when ball collides)
	NewtonMaterialSetCollisionCallback (nWorld, i, i, bodyWhite, beginCallback, processCallback, endCallback);
	
	ISceneNode *cue_node = createCue(nWorld, smgr, 158, 260, 115, 0);
	cue_node->setMaterialTexture(0, driver->getTexture("textures/taco.jpg"));
	cue_node->setMaterialFlag(video::EMF_LIGHTING, false);

	core::stringw str = "";
	
	float rot;
	irr::u32 lastTimer = 0;
	int counterBallStopped = 0;
	int strength = 0;
	int RMBClick = 0;
	matrix4 matCue;

	float envion = 0;
	int lastTimerShot = 0;
	vector3df shootPosition;
	
	bool shootPositionSet = false;
	int dist = 30;	// distance from the cue to the ball
	float rad = 80.0;

	camera->setPosition(bolaBlanca->getPosition() + vector3df(rad * cos((float)cameraAngle), rad, rad * sin((float)cameraAngle)));

	camera->setTarget(bolaBlanca->getPosition());
	cue_node->setPosition(bolaBlanca->getPosition() + vector3df(dist * cos((float)cameraAngle), 0, dist * sin((float)cameraAngle)));
	rot = atan2f((bolaBlanca->getPosition().X - camera->getPosition().X), (bolaBlanca->getPosition().Z - camera->getPosition().Z))*180.0f/3.14159f;
	cue_node->setRotation(vector3df(90, rot, 0));

	// La boucle principale du rendu // -----------------------------------------
	while (irrDev->run())
	{
		checkIn(nWorld, gameBalls);
		if (bolaBlanca->getPosition().Y < 180)
		{
			playBallInSound();
			matrix4 matSphere ;
			matSphere.setTranslation(vector3df(INIT_BCA_X, INIT_BCA_Y, INIT_BCA_Z));
			NewtonBodySetMatrix(bodyWhite,matSphere.pointer());
			bolaBlanca->setPosition(vector3df(INIT_BCA_X, INIT_BCA_Y, INIT_BCA_Z));
			
			NewtonBodySetOmega(bodyWhite, &zero[0]);
			NewtonBodySetTorque(bodyWhite, &zero[0]);
			NewtonBodySetVelocity(bodyWhite, &zero[0]);

			rad = 80;
			cameraAngle = 91.14f;

			camera->setPosition(bolaBlanca->getPosition() + vector3df(rad * cos((float)cameraAngle), rad, -rad * sin((float)cameraAngle)));
			camera->setTarget(bolaBlanca->getPosition());
			cue_node->setPosition(bolaBlanca->getPosition() + vector3df((dist + envion) * cos((float)cameraAngle), 0, (dist + envion) * sin((float)cameraAngle)));
			float rot = atan2f((bolaBlanca->getPosition().X - camera->getPosition().X), (bolaBlanca->getPosition().Z - camera->getPosition().Z))*180.0f/3.14159f;
			cue_node->setRotation(vector3df(90, rot, 0));
		}

		if (camera->getPosition().Y < 100)
		{
			camera->setPosition(bolaBlanca->getPosition() + vector3df(rad * cos((float)cameraAngle), rad, rad * sin((float)cameraAngle)));
			camera->setTarget(bolaBlanca->getPosition());
			cue_node->setPosition(bolaBlanca->getPosition() + vector3df((dist + envion) * cos((float)cameraAngle), 0, (dist + envion) * sin((float)cameraAngle)));
			float rot = atan2f((bolaBlanca->getPosition().X - camera->getPosition().X), (bolaBlanca->getPosition().Z - camera->getPosition().Z))*180.0f/3.14159f;
			cue_node->setRotation(vector3df(90, rot, 0));
		}

		if (rv.IsKeyDown(irr::KEY_KEY_T))
			smgr->setActiveCamera(cameraTop);

		if (rv.IsKeyDown(irr::KEY_KEY_C))
			smgr->setActiveCamera(camera);

		if (rv.IsKeyDown(irr::KEY_SPACE))
		{
			if (irrDev->getTimer()->getTime() - RMBClick > 200)
			{
				matrix4 matSphere ;
				matSphere.setTranslation(vector3df(INIT_BCA_X, INIT_BCA_Y, INIT_BCA_Z));
				NewtonBodySetMatrix(bodyWhite,matSphere.pointer());
				RMBClick = irrDev->getTimer()->getTime();
			}
		}

		if (rv.IsKeyDown(irr::KEY_ESCAPE))
		{
			return 0;
		}

		if (rv.IsKeyDown(irr::KEY_KEY_R))
		{
			resetTriangle(nWorld, smgr, INIT_BCA_X + INIT_DIST_BCA_NEGRA, INIT_BCA_Y + 1, INIT_BCA_Z + 3, bolas, driver, gameBalls);
		}
		
		if (rv.IsKeyDown(irr::KEY_LEFT) && !rv.GetMouseState().LeftButtonDown)
		{
			cameraAngle = cameraAngle + 0.003f;
			camera->setPosition(bolaBlanca->getPosition() + vector3df(rad * cos((float)cameraAngle), rad, rad * sin((float)cameraAngle)));
			cue_node->setPosition(bolaBlanca->getPosition() + vector3df((dist + envion) * cos((float)cameraAngle), 0, (dist + envion) * sin((float)cameraAngle)));
			float rot = atan2f((bolaBlanca->getPosition().X - camera->getPosition().X), (bolaBlanca->getPosition().Z - camera->getPosition().Z))*180.0f/3.14159f;
			cue_node->setRotation(vector3df(90, rot, 0));
			camera->setRotation(vector3df(camera->getRotation().X, rot, 0));
		}

		if (rv.IsKeyDown(irr::KEY_RIGHT) && !rv.GetMouseState().LeftButtonDown)
		{
			cameraAngle = cameraAngle - 0.003f;
			camera->setPosition(bolaBlanca->getPosition() + vector3df(rad * cos((float)cameraAngle), rad, rad * sin((float)cameraAngle)));
			cue_node->setPosition(bolaBlanca->getPosition() + vector3df((dist + envion) * cos((float)cameraAngle), 0, (dist + envion) * sin((float)cameraAngle)));
			float rot = atan2f((bolaBlanca->getPosition().X - camera->getPosition().X), (bolaBlanca->getPosition().Z - camera->getPosition().Z))*180.0f/3.14159f;
			cue_node->setRotation(vector3df(90, rot, 0));
			camera->setRotation(vector3df(camera->getRotation().X, rot, 0));
		}

		if (rv.IsKeyDown(irr::KEY_UP) && !rv.GetMouseState().LeftButtonDown)
		{
			rad = rad - .8f;
			camera->setPosition(bolaBlanca->getPosition() + vector3df(rad * cos((float)cameraAngle), rad, rad * sin((float)cameraAngle)));
			float rot = atan2f((bolaBlanca->getPosition().X - camera->getPosition().X), (bolaBlanca->getPosition().Z - camera->getPosition().Z))*180.0f/3.14159f;
			camera->setRotation(vector3df(camera->getRotation().X, rot, 0));
		}

		if (rv.IsKeyDown(irr::KEY_DOWN) && !rv.GetMouseState().LeftButtonDown)
		{
			rad = rad + .8f;
			camera->setPosition(bolaBlanca->getPosition() + vector3df(rad * cos((float)cameraAngle), rad, rad * sin((float)cameraAngle)));
			float rot = atan2f((bolaBlanca->getPosition().X - camera->getPosition().X), (bolaBlanca->getPosition().Z - camera->getPosition().Z))*180.0f/3.14159f;
			camera->setRotation(vector3df(camera->getRotation().X, rot, 0));
		}

		str = "Fuerza: ";	// debug: hardcoded!!
		if (lastTimer == 0)
			str += "0";
        else
			if (round32(2*(irrDev->getTimer()->getTime() - lastTimer)/100) < 50)
				str += round32(2*(irrDev->getTimer()->getTime() - lastTimer)/100);
			else
				str += 50;

		strengthtext->setText(str.c_str());
		vector3df camvec1 = (camera->getTarget() - camera->getPosition()).normalize();

		if (rv.GetMouseState().LeftButtonDown && aiming)
		{
			if (!shootPositionSet)
			{
				shootPosition = camera->getPosition();
				shootPositionSet = true;
			}

			if (envion < 40)
			{
				envion += 1;
				cue_node->setPosition(bolaBlanca->getPosition() + vector3df((dist + envion) * cos((float)cameraAngle), 0, (dist + envion) * sin((float)cameraAngle)));
			}

			if (lastTimer == 0)
				lastTimer = irrDev->getTimer()->getTime();
		} else
			// leftButton released
		{
			if (lastTimer != 0)
			{
				strength = (irrDev->getTimer()->getTime() - lastTimer);
				if (strength > 5000)
					strength = 5000;
	
				// hit the ball
				envion = 0;
				cue_node->setPosition(bolaBlanca->getPosition() + vector3df((dist + envion) * cos((float)cameraAngle), 0, (dist + envion) * sin((float)cameraAngle)));

				vector3df camvec = (bolaBlanca->getPosition() - shootPosition).normalize() * strength;
				float newpos[3] = { camvec.X, 0, camvec.Z };
				
				NewtonBodySetVelocity(bodyWhite, (float*)newpos);
				engine->play2D("data/cue.wav", false);
				lastTimer = 0;
				aiming = false;
				shootPositionSet = false;
				lastTimerShot = irrDev->getTimer()->getTime();;
			}
		}

		if (aiming == false)
		{
			if (irrDev->getTimer()->getTime() - lastTimerShot > 300)
				cue_node->setVisible(false);
		} else
			cue_node->setVisible(true);

		float whiteVel[3] = {0, 0, 0 };
		NewtonBodyGetVelocity(bodyWhite, (float *)whiteVel);

		str = "Whitevel: ";	// debug
		str += round32(whiteVel[0]);
        str += ", ";
		str += round32(bolaBlanca->getPosition().Y);
        str += ", ";
		str += round32(whiteVel[2]);
		irrDev->setWindowCaption(str.c_str());   


		//if (abs(whiteVel[0]) < .5 && abs(whiteVel[1]) < .5 && abs(whiteVel[2]) < .5)
		//	aiming = true;

		if (abs(whiteVel[0]) < MIN_SPEED && abs(whiteVel[1]) < MIN_SPEED && abs(whiteVel[2]) < MIN_SPEED)
			aiming = true;

		if (whiteVel[0] < MIN_SPEED && whiteVel[0] > -MIN_SPEED)
		{
			
			if (whiteVel[0] < 0)
				whiteVel[0] =  .1f;
			else
				whiteVel[0] =  -.1f;
			NewtonBodySetVelocity(bodyWhite, (float *)whiteVel);
		}

		str = "Usar las flechas para mover la camara/apuntar. Tirar con el boton izquierdo del mouse\n(mantener apretado para mas fuerza).";
		whitetext->setText(str.c_str());

		int fps = driver->getFPS();
		str = "R: Rearmar el triangulo.    |    ESC: Salir.\nT: Top view    |    éC: Camera view";

		fpstext->setText(str.c_str());
		
		str = "LA Pool Champ";
		NewtonUpdate ( nWorld, (0.95f / fps ));
		
		//On indique qu'on démarre la scène
		driver->beginScene (true, true, video::SColor (255,255,255,255));
		smgr->drawAll();
		env->drawAll();
//	   updateToolBox();
		driver->endScene();
	}
	return 0 ;
}
