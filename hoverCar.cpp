// Racing Cunts.cpp: A program using the TL-Engine

#include <sstream>
#include <TL-Engine.h>	// TL-Engine include file and namespace
using namespace tle;
I3DEngine* myEngine = New3DEngine(kTLX);


const int gamestateLength = 5;
enum EGameState { menu, countdown, lapOne, lapTwo, finish };
EGameState gamestate[6] = { menu, countdown, lapOne, lapTwo, finish };

const int camerastateLength = 2;
enum ECameraState { defaultcamera, firstperson };
ECameraState camerastate[3] = { defaultcamera, firstperson };

float frameTime = 0.0f;

void CameraMovement(ICamera* myCamera, IModel* dummy, EKeyCode CameraUp, EKeyCode CameraDown, EKeyCode CameraLeft, EKeyCode CameraRight, EKeyCode CameraReset, EKeyCode Camera1stPerson,
	float cameraSpeed, float cameraupperLimit, float cameralowerLimit);

void PlayerMovement(IModel*& hovercar, IModel* dummy, ICamera* myCamera, IFont* pressstartFont, float null, float& hovercarSpeed, float kSpeedUpperLimit, float kSpeedLowerLimit, float& hoverspeedcounterFloat,
	float hoverspeedcounterupperLimit, float hoverspeedcounterlowerLimit, float hoverturnSpeed, float kHoriCentre, float topuiCoord, EKeyCode ForwardKey, EKeyCode BackwardKey, EKeyCode LeftKey,
	EKeyCode RightKey);

void sphtoboxCollision(IModel* object, IModel* hoverCar, float width, float length, float hovercarRadius, float hovercarX, float hovercarZ, float& hovercarSpeed);

void sphtosphcheckpCollision(IModel* object, IModel*& hoverCar, float dummyRadius, float hovercarRadius, float hovercarX, float hovercarZ, int& checkpointState)
{
	float x = hovercarX - object->GetX();
	float z = hovercarZ - object->GetZ();
	float collisionDist = sqrt(x*x + z*z);

	if (collisionDist < dummyRadius + hovercarRadius)
	{
		checkpointState++;
	}

}

void laponeCollision(IModel* object, IModel*& hoverCar, float dummyRadius, float hovercarRadius, float hovercarX, float hovercarZ, int& checkpointState)
{
	float x = hovercarX - object->GetX();
	float z = hovercarZ - object->GetZ();
	float collisionDist = sqrt(x*x + z*z);

	if (collisionDist < dummyRadius + hovercarRadius)
	{
		checkpointState = 0;
		gamestate[gamestateLength] = { lapTwo };
	}

}

void laptwoCollision(IModel* object, IModel*& hoverCar, float dummyRadius, float hovercarRadius, float hovercarX, float hovercarZ, int& checkpointState)
{
	float x = hovercarX - object->GetX();
	float z = hovercarZ - object->GetZ();
	float collisionDist = sqrt(x*x + z*z);

	if (collisionDist < dummyRadius + hovercarRadius)
	{
		checkpointState = 0;
		gamestate[gamestateLength] = { finish };
	}

}

void sphtosphCollision(IModel* object, IModel*& hoverCar, float objectRadius, float hovercarRadius, float hovercarX, float hovercarZ, float& hovercarSpeed)
{
	float x = hovercarX - object->GetX();
	float z = hovercarZ - object->GetZ();
	float collisionDist = sqrt(x*x + z*z);

	if (hovercarSpeed >= 0)
	{
		if (collisionDist < hovercarRadius + objectRadius)
		{
			hoverCar->MoveLocalZ(-5.0f);
		}
	}

	if (hovercarSpeed <= 0)
	{
		if (collisionDist < hovercarRadius + objectRadius)
		{
			hoverCar->MoveLocalZ(5.0f);
		}
	}
}

//--------------------------------------------------------------------------------------MAIN-------------------------------------------------------------------------------------//
void main()
{
	// Create a 3D engine (using TLX engine here) and open a window for it
	myEngine->StartWindowed();

	// Add default folder for meshes and other media
	myEngine->AddMediaFolder(".\\Media");

	/**** Set up your scene here ****/
	//***********************************************************************************************
	//---------------------------------------------Camera------------------------------------------//
	ICamera* myCamera = myEngine->CreateCamera(kManual, 0, 5, -30);
	myCamera->RotateX(10);
	float cameraupperLimit = 30.0f;
	float cameralowerLimit = 2.0f;
	//***********************************************************************************************
	//--------------------------------------------User keys----------------------------------------//
	const EKeyCode EscapeKey = Key_Escape;							//Exits game in any game "state"
	const EKeyCode Spacebar = Key_Space;
	//***********************************************************************************************
	//---------------------------------------Hovercraft Control------------------------------------//
	const EKeyCode ForwardKey = Key_W;								//Forward
	const EKeyCode BackwardKey = Key_S;								//Backwards
	const EKeyCode LeftKey = Key_A;									//Left
	const EKeyCode RightKey = Key_D;								//Right
																	//***********************************************************************************************
																	//----------------------------------------------Camera-----------------------------------------//
	const EKeyCode CameraUp = Key_Up;
	const EKeyCode CameraDown = Key_Down;
	const EKeyCode CameraLeft = Key_Left;
	const EKeyCode CameraRight = Key_Right;
	const EKeyCode CameraReset = Key_1;
	const EKeyCode Camera1stPerson = Key_2;

	const int cameracoordLength = 3;
	float defaultcameraCoord[3]{ 0.0f, 15.0f, -30.0f };
	float firstpersoncameraCoord[3]{ 0.0f, 2.0f, 2.0f };
	int cameraxCoord = 0;
	int camerayCoord = 1;
	int camerazCoord = 2;
	//***********************************************************************************************
	//-----------------------------------------------Font------------------------------------------//
	IFont* myFont = myEngine->LoadFont("Arial Black", 20);			//Console text font
	IFont* countdownFont = myEngine->LoadFont("Arial Black", 80);	//Countdown text
	IFont* pressstartFont = myEngine->LoadFont("Arial Black", 35);	//Press start font
	IFont* endFont = myEngine->LoadFont("Arial Black", 30);
	const int kHoriCentre = 640;
	const int kVertiCentre = 0;
	const int bottomuiCoord = 705;
	const int topuiCoord = 50;
	const int checkpointuiCoord = 92;

	//NULL
	float null = 0.0f;

	//************************************
	//-----------------UI---------------//
	ISprite* userInterface = myEngine->CreateSprite("custombackdrop4.png", 0, 0, 0);
	ISprite* redLight = myEngine->CreateSprite("lightred.png", 0, 0, 0);
	ISprite* oneyellowLight;
	ISprite* twoyellowLight;
	ISprite* threeyellowLight;
	ISprite* greenLight;

	string redLights = "lightred.png";
	string oneyellowLights = "lightyellow1.png";
	string twoyellowLights = "lightyellow2.png";
	string threeyellowLights = "lightyellow3.png";
	string greenLights = "lightgreen.png";

	//************************************
	//--------------COUNTDOWN-----------//
	float countDown = 3.0f;
	float goCount = 2.0f;

	//************************************
	//----------SKYBOX & GROUND---------//

	IMesh* skyboxMesh = myEngine->LoadMesh("Skybox 07.x");
	IModel* skybox = skyboxMesh->CreateModel(null, -960.0f, null);

	IMesh* floorMesh = myEngine->LoadMesh("ground.x");
	IModel* floor = floorMesh->CreateModel(null, null, null);

	//************************************
	//----------PHYSICAL MODELS---------//
	//Meshes
	IMesh* checkpointMesh = myEngine->LoadMesh("Checkpoint.x");
	IMesh* isleMesh = myEngine->LoadMesh("IsleStraight.x");
	IMesh* wallMesh = myEngine->LoadMesh("Wall.x");
	IMesh* tankMesh = myEngine->LoadMesh("TankSmall1.x");

	//************************************
	//------------CHECKPOINTS-----------//
	const int KNumCheckpoints = 7;
	const int KCheckpArrLength = 7;
	float checkpcoordXArray[KCheckpArrLength]{ 0.0f, 0.0f, 0.0f, 75.0f, 513.5f, 513.5f, 480.0f };
	float checkpcoordZArray[KCheckpArrLength]{ 0.0f, 100.0f, 200.0f, 425.0f, 340.0f, -50.0f, -40.0f };
	IModel* checkpoint[KNumCheckpoints];

	for (int i = 0; i < KNumCheckpoints; i++)
	{
		checkpoint[i] = checkpointMesh->CreateModel(checkpcoordXArray[i], null, checkpcoordZArray[i]);
	}

	checkpoint[3]->RotateLocalY(90);

	//************************************
	//--------------COUNTDOWN-----------//
	const int KNumIsle = 12;
	const int KIsleArrLength = 12;
	const int isleWidth = 2;
	const int isleLength = 2;
	float islecoordXArray[KIsleArrLength]{ -10.0f, 10.0f, 10.0f, -10.0f, -10.0f, 10.0f, -10.0f, 10.0f, 153.0f, 166.0f,
		153.0f, 166.0f };
	float islecoordZArray[KIsleArrLength]{ 40.0f, 40.0f, 53.0f, 53.0f, 140.0f, 140.0f, 153.0f, 153.0f, 440.0f, 440.0f,
		410.0f, 410.0f };
	IModel* isles[KNumIsle];

	//For loop to create isles
	for (int i = 0; i < KNumIsle; i++)
	{
		isles[i] = isleMesh->CreateModel(islecoordXArray[i], null, islecoordZArray[i]);
	}
	//************************************
	//---------------WALLS--------------//
	const int KNumWall = 86;
	const int KWallArrLength = 86;
	const int wallLength = 2;
	const float wallWidth = 0.5;
	float wallcoordXArray[KWallArrLength]{ 9.5f, -10.5f, 9.5f, -10.5f, 160.0f, 160.0f, // FIRST WALLS AT START OF GAME 
		520.0f, 520.0f, 520.0f, 520.0f, 520.0f, 520.0f, 520.0f, 520.0f, 520.0f, 520.0f,
		520.0f, 520.0f, 520.0f, 520.0f, 520.0f, 520.0f, 520.0f, 520.0f, 520.0f, 520.0f,
		520.0f, 520.0f, 520.0f, 520.0f, 520.0f, 520.0f, 520.0f, 520.0f, 520.0f, 520.0f,
		520.0f, 520.0f, 520.0f, 520.0f, 520.0f, 520.0f, 520.0f, 520.0f, 520.0f, 520.0f, //LEFT HAND SIDE WALLS

		507.0f, 507.0f, 507.0f, 507.0f, 507.0f, 507.0f, 507.0f, 507.0f, 507.0f, 507.0f,
		507.0f, 507.0f, 507.0f, 507.0f, 507.0f, 507.0f, 507.0f, 507.0f, 507.0f, 507.0f,
		507.0f, 507.0f, 507.0f, 507.0f, 507.0f, 507.0f, 507.0f, 507.0f, 507.0f, 507.0f,
		507.0f, 507.0f, 507.0f, 507.0f, 507.0f, 507.0f, 507.0f, 507.0f, 507.0f, 507.0f }; //RIGHT HAND SIDE WALLS

	float wallcoordZArray[KWallArrLength]{ 46.0f, 46.0f, 146.0f, 146.0f, 440.0f, 410.0f, //FIRST WALLS AT START OF GAME

		340.0f, 330.0f, 320.0f, 310.0f, 300.0f, 290.0f, 280.0f, 270.0f, 260.0f, 250.0f,
		240.0f, 230.0f, 220.0f, 210.0f, 200.0f, 190.0f, 180.0f, 170.0f, 160.0f, 150.0f,
		140.0f, 130.0f, 120.0f, 110.0f, 100.0f, 90.0f, 80.0f, 70.0f, 60.0f, 50.0f,
		40.0f, 30.0f, 20.0f, 10.0f, 0.0f, -10.0f, -20.0f, -30.0f, -40.0f, -50.0f, // LEFT HAND SIDE WALLS

		340.0f, 330.0f, 320.0f, 310.0f, 300.0f, 290.0f, 280.0f, 270.0f, 260.0f, 250.0f,
		240.0f, 230.0f, 220.0f, 210.0f, 200.0f, 190.0f, 180.0f, 170.0f, 160.0f, 150.0f,
		140.0f, 130.0f, 120.0f, 110.0f, 100.0f, 90.0f, 80.0f, 70.0f, 60.0f, 50.0f,
		40.0f, 30.0f, 20.0f, 10.0f, 0.0f, -10.0f, -20.0f, -30.0f, -40.0f, -50.0f }; //RIGHT HAND SIDE WALLS
	IModel* walls[KNumWall];

	for (int i = 0; i < KNumWall; i++)
	{
		walls[i] = wallMesh->CreateModel(wallcoordXArray[i], null, wallcoordZArray[i]);
	}

	walls[4]->RotateLocalY(90);
	walls[5]->RotateLocalY(90);

	//************************************
	//----------------TANKS-------------//
	const int KNumTank = 52;
	const int KTankArrLength = 52;
	const int tankRadius = 3;
	float tankcoordXArray[KTankArrLength]{ 20.0f, 15.0f, 10.0f, 0.0f, -10.0f, -10.0f, -20.0f, -25.0f, -30.0f, -40.0f,
		-50.0f, -60.0f, -60.0f, -60.0f, -60.0f, -60.0f, -60.0f, -55.0f, -50.0f, -45.0f,
		-40.0f, -35.0f, -30.0f, -20.0f,	-10.0f, 0.0f, 10.0f, 20.0f, -10.0f, -10.0f,
		-5.0f, 0.0f, //CURVE ROUND 1ST CORNER

		300.0f, 350.0f, 400.0f,

		450.0f, 460.0f, 470.0f, 480.0f, 490.0f, 500.0f, 510.0f, 520.0f, 520.0f, 520.0f, //SECOND CURVE TO STRAIGHT

		520.0f, 510.0f, 500.0f, 490.0f, 480.0f, 470.0f, 460.0f };//CURVE THROUGH TO PORTAL

	float tankcoordYArray[KTankArrLength]{ 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f ,0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, //CURVE ROUND 1ST CORNER

		-5.0f, -5.0f, -5.0f, //UNDER GROUND TANKS

		0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, // SECOND CURVE TO STRAIGHT

		0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };//CURVE THROUGH TO PORTAL

	float tankcoordZArray[KTankArrLength]{ 350.0f, 365.0f, 375.0f, 380.0f, 385.0f, 300.0f, 310.0f, 320.0f, 330.0f, 330.0f,
		330.0f, 330.0f, 330.0f, 340.0f, 350.0f, 360.0f, 370.0f, 385.0f, 400.0f, 410.0f,
		420.0f, 430.0f, 435.0f, 440.0f,	440.0f, 440.0f, 440.0f, 440.0f, 400.0f, 392.5f,
		405.0f,	410.0f, //CURVE ROUND 1ST CORNER

		425.0f, 425.0f, 425.0f,

		440.0f, 440.0f, 440.0f, 430.0f, 420.0f, 410.0f, 400.0f, 390.0f, 380.0f, 365.0f,//SECOND CURVE TO STRAIGHT

		-60.0f, -70.0f, -80.0f, -80.0f, -80.0f, -70.0f, -60.0f };//CURVE THROUGH TO PORTAL

	IModel* tanks[KNumTank];

	for (int i = 0; i < KNumTank; i++)
	{
		tanks[i] = tankMesh->CreateModel(tankcoordXArray[i], tankcoordYArray[i], tankcoordZArray[i]);
	}

	tanks[32]->RotateLocalX(-30);
	tanks[33]->RotateLocalX(30);
	tanks[34]->RotateLocalX(-30);
	//************************************
	//--------------HOVERCAR------------//
	IMesh* hovercarMesh = myEngine->LoadMesh("race2.x");
	IModel* hovercar = hovercarMesh->CreateModel(null, null, null);
	float hovercarRadius = 3.0f;

	//HOVER CAR SPEEDS
	const float kSpeedUpperLimit = 0.040f;
	const float kSpeedLowerLimit = -0.005f;
	float hovercarSpeed = 0.0f;
	int hoverspeedcounterupperLimit = 100;
	int hoverspeedcounterlowerLimit = -10;
	float hoverspeedcounterFloat = null;


	//************************************
	//---------------DUMMYS-------------//
	IMesh* dummyMesh = myEngine->LoadMesh("Dummy.x");
	IModel* dummy = dummyMesh->CreateModel(0, 5, 10);
	myCamera->LookAt(dummy);

	//Checkpoint Dummys
	const int KNumCheckpDummy = 7;
	const int KCheckPArrLength = 7;
	float checkpdummyRadius = 3.0f;
	float checkpdummycoordXArray[KCheckPArrLength]{ 0.0f, 0.0f, 75.0f, 513.0f, 513.0f, 480.0f, 0.0f };
	float checkpdummycoordZArray[KCheckPArrLength]{ 100.0f, 200.0f, 425.0f, 340.0f, -50.0f, -40.0f, -10.0f };
	IModel* checkpDummy[KNumCheckpDummy];

	for (int i = 0; i < KNumCheckpDummy; i++)
	{
		checkpDummy[i] = dummyMesh->CreateModel(checkpdummycoordXArray[i], null, checkpdummycoordZArray[i]);
	}

	//HOVER CAR ROTATIONS
	float hovercarRotation = null;
	float hovercarmaxrightRotation = -2;
	float hovercarmaxleftRotation = 2;

	//STATES
	int countdownState = 0;
	int goState = 0;
	int checkpointState = 0;

	//------- SWITCH CASES -------//
	gamestate[gamestateLength] = menu;
	camerastate[camerastateLength] = defaultcamera;

	int frameCounter = 0;

	//--------------------------------------------------------------------------------------------------- MAIN GAME -----------------------------------------------------------------------------------------------------//
	// The main game loop, repeat until engine is stopped
	while (myEngine->IsRunning())
	{
		// Draw the scene
		myEngine->DrawScene();
		frameTime = myEngine->Timer();


		/**** Update your scene each frame here ****/
		switch (gamestate[gamestateLength])
		{
		case menu:
		{
			stringstream outText;
			outText << "PRESS SPACE TO START!";
			pressstartFont->Draw(outText.str(), kHoriCentre, topuiCoord, kBlack, kCentre, kVCentre);
			outText.str(""); // Clear myStream

			stringstream outText2;
			outText2 << "MENU";
			pressstartFont->Draw(outText2.str(), kHoriCentre, bottomuiCoord, kBlack, kCentre, kVCentre);
			outText2.str(""); // Clear myStream

			if (myEngine->KeyHit(Spacebar))
			{
				myEngine->RemoveSprite(redLight);
				gamestate[gamestateLength] = { countdown };
			}

			break;
		}//End of case menu
		case countdown:
		{
			stringstream outText2;
			outText2 << "COUNTDOWN";
			pressstartFont->Draw(outText2.str(), kHoriCentre, bottomuiCoord, kBlack, kCentre, kVCentre);
			outText2.str(""); // Clear myStream

			countDown -= frameTime;
			stringstream outText;

			switch (countdownState)
			{
			case 0:
			{
				if (countDown <= 3.0f)
				{
					countdownState = 3;
					oneyellowLight = myEngine->CreateSprite(oneyellowLights, 0, 0, 0);
				}
				break;
			}
			case 3:
			{
				outText << "3";

				if (countDown <= 2.0f)
				{
					countdownState = 2;
					myEngine->RemoveSprite(oneyellowLight);
					oneyellowLight = myEngine->CreateSprite(twoyellowLights, 0, 0, 0);
				}
				break;
			}
			case 2:
			{
				outText << "2";

				if (countDown <= 1.0f)
				{
					countdownState = 1;
					myEngine->RemoveSprite(oneyellowLight);
					oneyellowLight = myEngine->CreateSprite(threeyellowLights, 0, 0, 0);
				}
				break;
			}

			case 1:
			{
				outText << "1";

				if (countDown <= 0.0f)
				{
					countdownState = 0;
					myEngine->RemoveSprite(oneyellowLight);
					oneyellowLight = myEngine->CreateSprite(greenLights, 0, 0, 0);
					gamestate[gamestateLength] = { lapOne };
				}
				break;
			}
			}
			countdownFont->Draw(outText.str(), kHoriCentre, 190, kYellow, kCentre, kVCentre);

			break;
		}//----------------------------------------------------------------------------------END OF CASE COUNTDOWN------------------------------------------------------------------------------------------//

		 //-----------------------------------------------------------------------------------START OF RACING CASES------------------------------------------------------------------------------------------//
		case lapOne:
		{
			//PRE-REQUISITE CHECKS FOR ENTIRE LAP REGARDLESS OF CHECKPOINTS
			stringstream outText;
			outText << "LAP ONE";
			pressstartFont->Draw(outText.str(), kHoriCentre, bottomuiCoord, kBlack, kCentre, kVCentre);
			outText.str(""); // Clear myStream

			float cameraSpeed = frameTime * 40;
			float hoverturnSpeed = frameTime * 100;
			float hovercarX = hovercar->GetX();
			float hovercarZ = hovercar->GetZ();

			for (int i = 0; i < KNumWall; i++)
			{
				sphtoboxCollision(walls[i], hovercar, wallWidth, wallLength, hovercarRadius, hovercarX, hovercarZ, hovercarSpeed);
			}

			for (int i = 0; i < KNumIsle; i++)
			{
				sphtoboxCollision(isles[i], hovercar, isleWidth, isleLength, hovercarRadius, hovercarX, hovercarZ, hovercarSpeed);
			}

			for (int i = 0; i < KNumTank; i++)
			{
				sphtosphCollision(tanks[i], hovercar, tankRadius, hovercarRadius, hovercarX, hovercarZ, hovercarSpeed);
			}

			stringstream outText2;
			outText2 << "CHECKPOINT " << checkpointState + 1;
			myFont->Draw(outText2.str(), kHoriCentre, checkpointuiCoord, kBlack, kCentre, kVCentre);
			outText2.str(""); // Clear myStream

			PlayerMovement(hovercar, dummy, myCamera, pressstartFont, null, hovercarSpeed, kSpeedUpperLimit, kSpeedLowerLimit, hoverspeedcounterFloat, hoverspeedcounterupperLimit, hoverspeedcounterlowerLimit,
				hoverturnSpeed, kHoriCentre, topuiCoord, ForwardKey, BackwardKey, LeftKey, RightKey);

			CameraMovement(myCamera, dummy, CameraUp, CameraDown, CameraLeft, CameraRight, CameraReset, Camera1stPerson, cameraSpeed, cameraupperLimit, cameralowerLimit);

			//---------------------------------------------------------------START OF CHECKPOINTS------------------------------------------------------------------//
			switch (checkpointState)
			{
			case 0:		//CHECKPOINT START
			{
				switch (goState)
				{
				case 0:
				{
					myEngine->RemoveSprite(oneyellowLight);
					goState = 1;
				}
				case 1:
				{
					if (goCount > null)
					{
						goCount -= frameTime;
						stringstream outText;
						outText << "GO!";
						countdownFont->Draw(outText.str(), kHoriCentre, 300, kGreen, kCentre, kVCentre);
						outText.str(""); // Clear myStream
					}
				}
				}

				sphtosphcheckpCollision(checkpDummy[0], hovercar, checkpdummyRadius, hovercarRadius, hovercarX, hovercarZ, checkpointState);
				break;
			}
			case 1:		//CHECKPOINT ONE
			{
				sphtosphcheckpCollision(checkpDummy[1], hovercar, checkpdummyRadius, hovercarRadius, hovercarX, hovercarZ, checkpointState);
				break;
			}
			case 2:		//CHECKPOINT TWO
			{
				sphtosphcheckpCollision(checkpDummy[2], hovercar, checkpdummyRadius, hovercarRadius, hovercarX, hovercarZ, checkpointState);
				break;
			}
			case 3:		//CHECKPOINT THREE
			{
				sphtosphcheckpCollision(checkpDummy[3], hovercar, checkpdummyRadius, hovercarRadius, hovercarX, hovercarZ, checkpointState);
				break;
			}
			case 4:
			{
				sphtosphcheckpCollision(checkpDummy[4], hovercar, checkpdummyRadius, hovercarRadius, hovercarX, hovercarZ, checkpointState);
				break;
			}
			case 5:
			{
				sphtosphcheckpCollision(checkpDummy[5], hovercar, checkpdummyRadius, hovercarRadius, hovercarX, hovercarZ, checkpointState);
				break;
			}
			case 6:
			{
				hovercar->SetX(0);
				laponeCollision(checkpDummy[6], hovercar, checkpdummyRadius, hovercarRadius, hovercarX, hovercarZ, checkpointState);
				break;
			}
			}
			break;
		}//-----------------------------------------------------------------END OF LAP ONE--------------------------------------------------------------------//
		case lapTwo:
		{
			stringstream outText;
			outText << "LAP TWO";
			pressstartFont->Draw(outText.str(), kHoriCentre, bottomuiCoord, kBlack, kCentre, kVCentre);
			outText.str(""); // Clear myStream

			float cameraSpeed = frameTime * 40;
			float hoverturnSpeed = frameTime * 100;
			float hovercarX = hovercar->GetX();
			float hovercarZ = hovercar->GetZ();

			for (int i = 0; i < KNumWall; i++)
			{
				sphtoboxCollision(walls[i], hovercar, wallWidth, wallLength, hovercarRadius, hovercarX, hovercarZ, hovercarSpeed);
			}

			for (int i = 0; i < KNumIsle; i++)
			{
				sphtoboxCollision(isles[i], hovercar, isleWidth, isleLength, hovercarRadius, hovercarX, hovercarZ, hovercarSpeed);
			}

			for (int i = 0; i < KNumTank; i++)
			{
				sphtosphCollision(tanks[i], hovercar, tankRadius, hovercarRadius, hovercarX, hovercarZ, hovercarSpeed);
			}

			stringstream outText2;
			outText2 << "CHECKPOINT " << checkpointState + 1;
			myFont->Draw(outText2.str(), kHoriCentre, checkpointuiCoord, kBlack, kCentre, kVCentre);
			outText2.str(""); // Clear myStream

			PlayerMovement(hovercar, dummy, myCamera, pressstartFont, null, hovercarSpeed, kSpeedUpperLimit, kSpeedLowerLimit, hoverspeedcounterFloat, hoverspeedcounterupperLimit, hoverspeedcounterlowerLimit,
				hoverturnSpeed, kHoriCentre, topuiCoord, ForwardKey, BackwardKey, LeftKey, RightKey);

			CameraMovement(myCamera, dummy, CameraUp, CameraDown, CameraLeft, CameraRight, CameraReset, Camera1stPerson, cameraSpeed, cameraupperLimit, cameralowerLimit);


			switch (checkpointState)
			{
			case 0:		//CHECKPOINT START
			{
				sphtosphcheckpCollision(checkpDummy[0], hovercar, checkpdummyRadius, hovercarRadius, hovercarX, hovercarZ, checkpointState);
			}
			case 1:		//CHECKPOINT ONE
			{
				sphtosphcheckpCollision(checkpDummy[1], hovercar, checkpdummyRadius, hovercarRadius, hovercarX, hovercarZ, checkpointState);
				break;
			}
			case 2:		//CHECKPOINT TWO
			{
				sphtosphcheckpCollision(checkpDummy[2], hovercar, checkpdummyRadius, hovercarRadius, hovercarX, hovercarZ, checkpointState);
				break;
			}
			case 3:		//CHECKPOINT THREE
			{
				sphtosphcheckpCollision(checkpDummy[3], hovercar, checkpdummyRadius, hovercarRadius, hovercarX, hovercarZ, checkpointState);
				break;
			}
			case 4:
			{
				sphtosphcheckpCollision(checkpDummy[4], hovercar, checkpdummyRadius, hovercarRadius, hovercarX, hovercarZ, checkpointState);
				break;
			}
			case 5:
			{
				sphtosphcheckpCollision(checkpDummy[5], hovercar, checkpdummyRadius, hovercarRadius, hovercarX, hovercarZ, checkpointState);
				break;
			}
			case 6:
			{
				hovercar->SetX(0);
				laptwoCollision(checkpDummy[6], hovercar, checkpdummyRadius, hovercarRadius, hovercarX, hovercarZ, checkpointState);
			}
			}
			break;
		}
		case finish:
		{
			stringstream outText;
			outText << "CONGRATULATIONS! YOU FINISHED!";
			endFont->Draw(outText.str(), kHoriCentre, topuiCoord, kBlack, kCentre, kVCentre);
			outText.str(""); // Clear myStream

			stringstream exitText;
			exitText << "PRESS ESC TO EXIT";
			myFont->Draw(exitText.str(), kHoriCentre, checkpointuiCoord, kBlack, kCentre, kVCentre);
			exitText.str(""); // Clear myStream
			break;
		}
		}//End of switch case



		 //WILL WORK REGARDLESS OF WHETHER PAUSED OR NOT
		if (myEngine->KeyHit(EscapeKey))
		{
			myEngine->Stop();					//Exit game from any state
		}

	}//End of engine running

	 // Delete the 3D engine now we are finished with it
	myEngine->Delete();
} // End of main
  //---------------------------------------------------------------------------------------------end of main-----------------------------------------------------------------------------------------------------//




void CameraMovement(ICamera* myCamera, IModel* dummy, EKeyCode CameraUp, EKeyCode CameraDown, EKeyCode CameraLeft, EKeyCode CameraRight, EKeyCode CameraReset, EKeyCode Camera1stPerson,
	float cameraSpeed, float cameraupperLimit, float cameralowerLimit)
{
	switch (camerastate[camerastateLength])
	{
	case defaultcamera:
	{
		myCamera->LookAt(dummy);

		//Camera Movement Up
		if (myEngine->KeyHeld(CameraUp))						//Holding the up key on the keyboard will allow to player to tilt the camera up, centred on the frog
		{
			myCamera->MoveLocalY(cameraSpeed);
		}
		//Limit Upwards Movement of Camera
		if (myCamera->GetLocalY() > cameraupperLimit)			//Enforcing an upper limit for the camera so the player cannot go out of bounds, the camera turning up side down and breaking the game
		{
			myCamera->MoveLocalY(-cameraSpeed);					//Moves the camera at the same speed in the opposite direction in order to give the look of no movement
		}

		//Camera Movement Down
		if (myEngine->KeyHeld(CameraDown))					//Holding the up key on the keyboard will allow to player to tilt the camera down, centred on the frog
		{
			myCamera->MoveLocalY(-cameraSpeed);
		}
		//Limit Downwards Movement of Camera so it doesn't go through the floor
		if (myCamera->GetLocalY() < cameralowerLimit)			//Enforcing an lower limit for the camera so the player cannot go out of bounds, the camera turning up side down and breaking the game
		{
			myCamera->MoveLocalY(cameraSpeed);					//Moves the camera at the same speed in the opposite direction in order to give the look of no movement
		}

		//Camera Left Movement
		if (myEngine->KeyHeld(CameraLeft))						//Holding the up key on the keyboard will allow to player to tilt the camera up, centred on the frog
		{
			myCamera->MoveLocalX(-cameraSpeed);
		}
		//Camera Right Movement
		if (myEngine->KeyHeld(CameraRight))					//Holding the up key on the keyboard will allow to player to tilt the camera down, centred on the frog
		{
			myCamera->MoveLocalX(cameraSpeed);
		}
		break;
	} // End of default camera
	case firstperson:
	{
		break;
	}//End of first person camera
	}

	//When  1 pressed will reset the camera to the default third person position
	if (myEngine->KeyHit(CameraReset))
	{
		myCamera->SetLocalPosition(0, 5, -30);
		camerastate[camerastateLength] = defaultcamera;
	}

	//When  2 pressed will reset the camera to the default third person position
	if (myEngine->KeyHit(Camera1stPerson))
	{
		myCamera->SetLocalPosition(0, 2, -10);
		//myCamera->RotateLocalY(180);
		camerastate[camerastateLength] = firstperson;
	}
}

void PlayerMovement(IModel*& hovercar, IModel* dummy, ICamera* myCamera, IFont* pressstartFont, float null, float& hovercarSpeed, float kSpeedUpperLimit, float kSpeedLowerLimit, float& hoverspeedcounterFloat,
	float hoverspeedcounterupperLimit, float hoverspeedcounterlowerLimit, float hoverturnSpeed, float kHoriCentre, float topuiCoord, EKeyCode ForwardKey, EKeyCode BackwardKey, EKeyCode LeftKey,
	EKeyCode RightKey)
{
	myCamera->AttachToParent(dummy);
	dummy->AttachToParent(hovercar);
	myCamera->LookAt(dummy);

	//HOVERCRAFT CONTROLS
	/////////////////////
	//----------FORWARD----------//
	hovercar->MoveLocalZ(hovercarSpeed);

	if (myEngine->KeyHeld(ForwardKey))
	{
		hovercar->MoveLocalZ(hovercarSpeed);
		if (hovercarSpeed < kSpeedUpperLimit)
		{
			hovercarSpeed += frameTime / 25;					// get faster
			if (hoverspeedcounterFloat < hoverspeedcounterupperLimit)
			{
				hoverspeedcounterFloat += frameTime * 10;
			}

		}
	}
	//Forward Decay
	if (myEngine->KeyHeld(ForwardKey) == false)
	{
		if (hovercarSpeed > null)
		{
			hovercarSpeed -= frameTime / 50;
			if (hoverspeedcounterFloat > null)
			{
				hoverspeedcounterFloat -= frameTime * 5;
			}

		}
	}

	//----------BACKWARD----------//
	if (myEngine->KeyHeld(BackwardKey))
	{
		hovercar->MoveLocalZ(hovercarSpeed);
		if (hovercarSpeed > kSpeedLowerLimit)
		{
			hovercarSpeed -= frameTime / 100;					// get faster
			if (hoverspeedcounterFloat > hoverspeedcounterlowerLimit)
			{
				hoverspeedcounterFloat -= frameTime * 20;
			}

		}
	}

	//Backward decay
	if (myEngine->KeyHeld(BackwardKey) == false)
	{
		if (hovercarSpeed < null)
		{
			hovercarSpeed += frameTime / 100;
			if (hoverspeedcounterFloat < null)
			{
				hoverspeedcounterFloat += frameTime * 20;
			}

		}
	}

	//Right
	if (myEngine->KeyHeld(RightKey))
	{
		hovercar->RotateLocalY(hoverturnSpeed);
		//hovercar->RotateLocalZ(hovercarRotation);
		/*if (hovercarRotation < hovercarmaxrightRotation)
		{
		hovercarRotation += frameTime;
		}*/
	}
	//Left
	if (myEngine->KeyHeld(LeftKey))
	{
		hovercar->RotateLocalY(-hoverturnSpeed);
	}

	int hoverspeedCounter = (int)hoverspeedcounterFloat * 10;

	stringstream outText;
	outText << "Speed: " << hoverspeedCounter;
	pressstartFont->Draw(outText.str(), kHoriCentre, topuiCoord, kBlack, kCentre, kVCentre);
	outText.str(""); // Clear myStream
}

void sphtoboxCollision(IModel* object, IModel* hoverCar, float width, float length, float hovercarRadius, float hovercarX, float hovercarZ, float& hovercarSpeed)
{
	float minX = object->GetX() - width - hovercarRadius;
	float maxX = object->GetX() + width + hovercarRadius;
	float minZ = object->GetZ() - length - hovercarRadius;
	float maxZ = object->GetZ() + length + hovercarRadius;

	if (hovercarSpeed >= 0)
	{
		if (hovercarX > minX && hovercarX < maxX && hovercarZ > minZ && hovercarZ < maxZ)
		{
			hoverCar->MoveLocalZ(-5.0f);
		}
	}

	if (hovercarSpeed <= 0)
	{
		if (hovercarX > minX && hovercarX < maxX && hovercarZ > minZ && hovercarZ < maxZ)
		{
			hoverCar->MoveLocalZ(5.0f);
		}
	}
}