#include "TrainLevel.h"
#include "MainForm.h"

/*
	ALevelObject* object->moveable		: set Object should move or not, still render in window
				  object->activated		: set Object should render or not, still moveable	
				  object->render->tag	: set object with a tag, for compare
	
	ALevelObjects are saved in environment;
	NonPlayerCharacters are saved in enemies;

	
	*/


void TrainLevel::update(double deltaT)
{
	//code level-specific runtime logic here
	Level::update(deltaT);


	if (gameStart) {
		updateBuilding(deltaT, 20);
		updateFPS(deltaT);		
		starttime += deltaT;
		if(starttime > 3.0f)
			gamePlay(deltaT);
		else starttime += deltaT;
	}

}

void TrainLevel::gamePlay(double deltaT) {


	avatar->entity->position = locToGlob.Invert() * Kore::vec4(math->cameraPos.x(), math->cameraPos.y(), math->cameraPos.z(), 1.0);
	if (form->gameStarted())
	{
		checkStation(deltaT);
		checkEnemyCollision();
		checkHittingAvatar();
		checkHittingEnemy();
	}


}

void TrainLevel::updateFPS(double deltaT) {
	fps++;
	time += deltaT;
	if (time > 1.0f)
	{
		Kore::log(Kore::Info, "%d", fps);
		Kore::log(Kore::Info, "Player Position is %f %f %f", avatar->entity->position.x(), avatar->entity->position.y(), avatar->entity->position.z());
		fps = 0;
		time = 0;
	}
}


void TrainLevel::updateBuilding(double deltaT,double speed) {	
	if (!form->isFormShown())
	{
		for (ALevelObject* object : environment)
			if (object->moveable)
				if (object->render->position.x() > -420)
				{

					if (object->render->tag == "car") {
						object->render->position.x() -= deltaT * speed * 0.5f;
						object->render->position.z() -= 0.01 * deltaT;
					}
					else if (object->render->tag == "car1") {
						object->render->position.x() -= deltaT * speed * 2;
						object->render->position.z() -= deltaT * speed * 0.01f;
					}
					else if (object->render->tag == "tunnelS") {
						
						//object->render->activated = true;
						object->render->position.x() -= deltaT * speed * 3;
						object->render->position.z() -= deltaT * speed * 0.02f;
						
					}
					else if (object->render->tag == "airplane") {
						speed *= 0.4;
						object->render->position.x() -= deltaT * speed * 3;
						if (object->render->position.x() < 0)
							object->render->position.y() += deltaT * speed;
						else if (object->render->position.y() > 15)
						{
							object->render->position.y() -= deltaT * speed;

						}

						airPlanePos = object->render->position;
						
					}
					else {
						object->render->position.x() -= deltaT * speed;
						object->render->position.z() -= deltaT * speed * 0.0075f;
					}
				}
				else { 
					//object->render->position.z() = 8.325f;
					if (object->render->tag == "floor") {
						object->render->position.x() = 580;
						object->render->position.z() = 4.25f;
					}
					if (object->render->tag == "floorh") {
						object->render->position.x() = 580;
						object->render->position.z() = 0.075f;
					}
					if (object->render->tag == "floorl") {
						object->render->position.x() = 580;
						object->render->position.z() = -3.0f;
					}
					if (object->render->tag == "houseR") {
						object->render->position.x() = 454;
						object->render->position.z() = 20.21;
					}
					if (object->render->tag == "houseL") {
						object->render->position.x() = 454;
						object->render->position.z() = -14.0f;
					}
					if (object->render->tag == "car1") {
						object->render->position.x() = 454;
						object->render->position.z() = -6;
					}
					if (object->render->tag == "car") {
						object->render->position.x() = 454;
						object->render->position.z() = 12;
					}
					if (object->render->tag == "airplane") {
						object->render->position.x() = 454;
						object->render->position.y() = 50;
					}
					if (object->render->tag == "tunnelS") {
						tunnelCounter++;
						if (tunnelCounter == 9) {
							tunnelCounter = 0;
							
							for (ALevelObject* object : environment) {
								if (object->render->tag == "tunnelS") {
									object->render->activated = false;
									object->moveable = false;
								}
							}
						}
						object->render->position.x() = 454;
						object->render->position.z() = 3;
						
					}
						
				}
	}
}

void TrainLevel::init() {
	controlsSetup();
	audioSetup();
	graphicsSetup();
	Level::init();
	form = new MainForm();
	offsets = 1;
	gameStart = true;
	tunnelCounter = 8;
}

void TrainLevel::controlsSetup()
{
	input = input->getInstance();
}

void TrainLevel::audioSetup()
{
	audio = audio->getInstanceAndAppend({
		{"Hier koennte Ihre Werbung stehen!", new Kore::Sound("sound/start.wav")}
		});
}

void TrainLevel::graphicsSetup()
{
	//Load Shaders
	renderer->loadEnvironmentShader("shader_basic_shading.vert", "shader_basic_shading.frag");
	renderer->loadEntityShader("shader.vert", "shader.frag");
	
	//Set graphics variables
	const Kore::Graphics4::VertexStructure& entitySructure = renderer->entityGraphics->structure;
	const Kore::Graphics4::VertexStructure& environmentSructure = renderer->environmentGraphics->structure;
	
	//Load Skybox

	skyInit(environmentSructure);

	//Load Train
	//trainInit(environmentSructure);
	trainInit(environmentSructure, 3);

	//Load Avatar
	avatar = new TheAvatar("avatar/avatar_male.ogex", "avatar/", entitySructure, 1.0f, Kore::vec3(0, 0, 0), Kore::Quaternion(0, 0, 0, 0), true, true);

	//create Pool od entitys
	//Load Enemy
	createEnemy(entitySructure);

	//Load Ground
	groundInit(environmentSructure);

	//Load Houses
	houseInit(environmentSructure);

	//Load Airplane
	airplaneInit(environmentSructure);

	//Load Car

	carInit(environmentSructure);

	//Load Tunnel
	
	tunnelInit(environmentSructure);
	
}

//////////////////////////////interaction Methods

void TrainLevel::checkStation(double deltaT)//, Kore::vec3 AirPlanePos)
{
	if (!stationComplete & currentEnemyCount < maxEnemyCount)// & airPlanePos.x() <= Kore::abs(avatar->entity->position.y() - stationLength))
	{
		spawn(deltaT);//, AirPlanePos);
	}

	if (StateMachineAI::beatedEnemyCount >= maxEnemyCount & !stationComplete)
	{
		stationComplete = true;
		currentEnemyCount = 0;

		if (maxEnemyCount < poolSize)
			maxEnemyCount++;
		else
			maxEnemyCount = poolSize;
	}
	else if(stationComplete & avatar->entity->position.y() <= (StateMachineAI::lastDeadPos -  stationLength))
	{
		for (NonPlayerCharacter* enemy : enemies)
		{
			enemy->entity->beated = false;
		}
		StateMachineAI::beatedEnemyCount = 0;
		stationComplete = false;
		stationStarted=true;
		stationNr++;
	}
	countDown += deltaT;
}

void TrainLevel::spawn(double deltaT)//, Kore::vec3 AirPlanePos)
{

	if (countDown > maxWaitintTime | (stationStarted & countDown > maxWaitintTime / 100.0))
	{
		stationStarted = false;
		
		for (NonPlayerCharacter* enemy : enemies)
		{
			if (!(enemy->entity->beated) & !(enemy->entity->activated))
			{
				Kore::vec3 globAirPlanePos = locToGlob * Kore::vec4(airPlanePos.x(), airPlanePos.y(), airPlanePos.z(), 1.0);
				float randomX_Pos = (float)(rand() % 2 - 1);
				if (randomX_Pos == 0.0)
					randomX_Pos += 0.1;
				enemy->entity->position = Kore::vec3(randomX_Pos, (avatar->entity->position.y() - stationLength), 0.0f);
				enemy->entity->rotation = Kore::Quaternion(Kore::vec3(0, 0, 1), Kore::pi);
				countDown = 0.0;
				enemy->ai->spawn();
				currentEnemyCount++;
				break;
			}
		}
	}

	
}

void TrainLevel::checkHittingAvatar()
{
	for (NonPlayerCharacter* enemy : enemies)
	{

		Kore::vec3 leftFootPos = enemy->entity->meshObject->bones[leftFootBoneIndex]->getPosition();
		Kore::vec3 rightFootPos = enemy->entity->meshObject->bones[rightFootBoneIndex]->getPosition();
		 
		Kore::vec3 dir_L = leftFootPos - avatar->entity->position;
		Kore::vec3 dir_R = rightFootPos - avatar->entity->position;

		dir_L.z() = 0;
		dir_R.z() = 0;

		float a = leftFootPos.z();
		float b = dir_L.getLength();

		if (leftFootPos.z() > hittingHeight & dir_L.getLength() < hittingRadius & !enemy->entity->attackedSuccessfully)
		{
			enemy->entity->attackedSuccessfully = true;
			avatar->entity->hit();
		}
		else if (rightFootPos.z() > hittingHeight & dir_R.getLength() < hittingRadius & !enemy->entity->attackedSuccessfully)
		{
			enemy->entity->attackedSuccessfully = true;
			avatar->entity->hit();
		}
		else if (dir_L.getLength() > hittingRadius & dir_R.getLength() > hittingRadius & enemy->entity->attackedSuccessfully)
		{
			enemy->entity->attackedSuccessfully = false;
		}
	}
}

void TrainLevel::checkHittingEnemy()
{
	for (NonPlayerCharacter* enemy : enemies)
	{
		
		Kore::vec3 leftFootPos = avatar->entity->meshObject->bones[leftFootBoneIndex]->getPosition();
		Kore::vec3 rightFootPos = avatar->entity->meshObject->bones[leftFootBoneIndex]->getPosition();

		Kore::vec3 dir_L = leftFootPos - enemy->entity->position;
		Kore::vec3 dir_R = rightFootPos - enemy->entity->position;

		dir_L.z() = 0;
		dir_R.z() = 0;

		if (avatar->entity->endEffector[leftFoot]->getDesPosition().z() > hittingHeight & dir_L.getLength() < hittingRadius & !avatar->entity->attackedSuccessfully)
		{
			avatar->entity->attackedSuccessfully = true;
			enemy->entity->hit();
		}
		else if (avatar->entity->endEffector[rightFoot]->getDesPosition().z() > hittingHeight & dir_R.getLength() < hittingRadius & !avatar->entity->attackedSuccessfully)
		{
			avatar->entity->attackedSuccessfully = true;
			enemy->entity->hit();
		}
		else if (dir_L.getLength() > hittingRadius & dir_R.getLength() > hittingRadius & avatar->entity->attackedSuccessfully)
		{
			avatar->entity->attackedSuccessfully = false;
		}
	}
}


void TrainLevel::checkEnemyCollision()
{
	for (int i=0; i< maxEnemyCount; i++)
	{
		if (enemies[i]->entity->activated)
		{
			for (int k = 0; k < maxEnemyCount & i != k; k++)
			{
				if(!enemies[k]->entity->beated)
					enemies[i]->ai->checkCollision(enemies[k]->entity->position);
				else
					enemies[i]->ai->checkCollision(Kore::vec3(1000.0,0.0,0.0));
			}
		}
	}
}
///////////////////////////////////////////////////////////////////

void TrainLevel::createEnemy(Kore::Graphics4::VertexStructure entitySructure)
{
	AnAnimatedEntity* reference;
	for (int i = 0; i < poolSize; i++) {
		switch (i) {		
		case 4:reference = new AnAnimatedEntity("enemy/avatar_male.ogex", "enemy/", entitySructure, 1.0f, Kore::vec3(0, 0, -1000), Kore::Quaternion(0, 0, 0, 0)); reference->tag = "NinjaM"; break;
		case 1:reference = new AnAnimatedEntity("enemy/avatar_maleR.ogex", "enemy/", entitySructure, 1.0f, Kore::vec3(0, 0, -1000), Kore::Quaternion(0, 0, 0, 0)); reference->tag = "NinjaR"; break;
		case 2:reference = new AnAnimatedEntity("enemy/avatar_maleB.ogex", "enemy/", entitySructure, 1.0f, Kore::vec3(0, 0, -1000), Kore::Quaternion(0, 0, 0, 0)); reference->tag = "NinjaB"; break;
		case 3:reference = new AnAnimatedEntity("enemy/avatar_maleY.ogex", "enemy/", entitySructure, 1.0f, Kore::vec3(0, 0, -1000), Kore::Quaternion(0, 0, 0, 0)); reference->tag = "NinjaY"; break;
		case 0:reference = new AnAnimatedEntity("enemy/avatar_maleW.ogex", "enemy/", entitySructure, 1.0f, Kore::vec3(0, 0, -1000), Kore::Quaternion(0, 0, 0, 0)); reference->tag = "NinjaW"; break;
		default:reference = new AnAnimatedEntity("enemy/avatar_maleW.ogex", "enemy/", entitySructure, 1.0f, Kore::vec3(0, 0, -1000), Kore::Quaternion(0, 0, 0, 0)); reference->tag = "NinjaM"; break;
		}
		NonPlayerCharacter* enemy = new NonPlayerCharacter(reference, Kore::vec3(0, 0, -1000), Kore::Quaternion(0, 0, 0, 0));
		enemy->ai = new CyborgAI(enemy->entity, animator, avatar->entity);
		enemy->entity->activated = false;
		enemies.emplace_back(enemy);
	}
	
}

Level::ALevelObject* TrainLevel::createNewObject(String pfad, String pfad2, VertexStructure vstruct,float scale, Kore::vec3 pos, Kore::Quaternion rot) {

	ALevelObject* object = new ALevelObject(pfad, pfad2, vstruct,scale,pos,rot);
	environment.emplace_back(object);
	return object;
}

Level::ALevelObject* TrainLevel::createObjectCopy(ALevelObject* object, Kore::vec3 pos, Kore::Quaternion rot) {

	ALevelObject* newobject = new ALevelObject(object, pos, rot);
	environment.emplace_back(newobject);
	return newobject;
}

void TrainLevel::skyInit(Kore::Graphics4::VertexStructure environmentSructure) {

	ALevelObject* sky = createNewObject("skybox/skybox.ogex", "skybox/", environmentSructure, 1, Kore::vec3(0, 0, -75), Kore::Quaternion(3, 0, 1, 0));
	renderer->setLights(*sky->render, renderer->environmentGraphics->lightCount, renderer->environmentGraphics->lightPosLocation);
	sky->moveable = false;	
}
//Old
void TrainLevel::trainInit(Kore::Graphics4::VertexStructure environmentSructure) {
	ALevelObject* trainf = createNewObject("train/trainFront.ogex", "train/", environmentSructure, 1, Kore::vec3(40.8, -3, 0), Kore::Quaternion(3, 0, 2, 0));
	trainf->moveable = false;
	ALevelObject* trainm = createNewObject("train/trainMiddle.ogex", "train/", environmentSructure, 1, Kore::vec3(7.2, -3, 0), Kore::Quaternion(3, 0, 2, 0));
	renderer->setLights(*trainm->render, renderer->environmentGraphics->lightCount, renderer->environmentGraphics->lightPosLocation);
	trainm->moveable = false;

	float xoffset = 16.8;
	ALevelObject* object = createObjectCopy(trainm, Kore::vec3(trainm->render->position.x() + xoffset, trainm->render->position.y(), trainm->render->position.z()), trainm->render->rotation);
	object->moveable = false;
}

//Trainlenght: 1 = short = 3 waggons total; 2 = medium = 5 Waggons total; 3 = long = 10 waggons total
void TrainLevel::trainInit(Kore::Graphics4::VertexStructure environmentSructure, int trainLenght) {
	float yRot = 1.995f;
	//float xoffset = 16.8;
	float xoffset = 16.5;
	float zoffset = 0.11f;

	ALevelObject* trainBack = createNewObject("train/trainFront.ogex", "train/", environmentSructure, 1, Kore::vec3(7.2, -3, 0.08), Kore::Quaternion(3, 0, -0.005f, 0));
	trainBack->moveable = false;
	ALevelObject* trainMiddle1 = createNewObject("train/trainMiddle.ogex", "train/", environmentSructure, 1, Kore::vec3(7.2 + xoffset, -3, 0.08 + zoffset), Kore::Quaternion(3, 0, yRot, 0));
	trainMiddle1->moveable = false;

	switch (trainLenght) {
	case 1:
	{
		ALevelObject* trainFront = createObjectCopy(trainBack, Kore::vec3(trainBack->render->position.x() + xoffset * 2 + 0.3, trainBack->render->position.y(), trainBack->render->position.z() + zoffset * 2), Kore::Quaternion(3, 0, yRot, 0));
		trainFront->moveable = false;
		break;
	}
	case 2:
	{
		ALevelObject* trainMiddle2 = createObjectCopy(trainMiddle1, Kore::vec3(trainMiddle1->render->position.x() + xoffset, trainMiddle1->render->position.y(), trainMiddle1->render->position.z() + zoffset), Kore::Quaternion(3, 0, yRot, 0));
		ALevelObject* trainMiddle3 = createObjectCopy(trainMiddle1, Kore::vec3(trainMiddle1->render->position.x() + xoffset * 2, trainMiddle1->render->position.y(), trainMiddle1->render->position.z() + zoffset * 2), Kore::Quaternion(3, 0, yRot, 0));
		ALevelObject* trainFront2 = createObjectCopy(trainBack, Kore::vec3(trainBack->render->position.x() + xoffset * 4 + 0.2, trainBack->render->position.y(), trainBack->render->position.z() + zoffset * 4), Kore::Quaternion(3, 0, yRot, 0));
		trainMiddle2->moveable = false;
		trainMiddle3->moveable = false;
		trainFront2->moveable = false;
		break;
	}
	case 3:
	{
		ALevelObject* trainMiddle22 = createObjectCopy(trainMiddle1, Kore::vec3(trainMiddle1->render->position.x() + xoffset, trainMiddle1->render->position.y(), trainMiddle1->render->position.z() + zoffset), Kore::Quaternion(3, 0, yRot, 0));
		ALevelObject* trainMiddle33 = createObjectCopy(trainMiddle1, Kore::vec3(trainMiddle1->render->position.x() + xoffset * 2, trainMiddle1->render->position.y(), trainMiddle1->render->position.z() + zoffset * 2), Kore::Quaternion(3, 0, yRot, 0));
		ALevelObject* trainMiddle4 = createObjectCopy(trainMiddle1, Kore::vec3(trainMiddle1->render->position.x() + xoffset * 3, trainMiddle1->render->position.y(), trainMiddle1->render->position.z() + zoffset * 3), Kore::Quaternion(3, 0, yRot, 0));
		ALevelObject* trainMiddle5 = createObjectCopy(trainMiddle1, Kore::vec3(trainMiddle1->render->position.x() + xoffset * 4, trainMiddle1->render->position.y(), trainMiddle1->render->position.z() + zoffset * 4), Kore::Quaternion(3, 0, yRot, 0));
		ALevelObject* trainMiddle6 = createObjectCopy(trainMiddle1, Kore::vec3(trainMiddle1->render->position.x() + xoffset * 5, trainMiddle1->render->position.y(), trainMiddle1->render->position.z() + zoffset * 5), Kore::Quaternion(3, 0, yRot, 0));
		ALevelObject* trainMiddle7 = createObjectCopy(trainMiddle1, Kore::vec3(trainMiddle1->render->position.x() + xoffset * 6, trainMiddle1->render->position.y(), trainMiddle1->render->position.z() + zoffset * 6), Kore::Quaternion(3, 0, yRot, 0));
		ALevelObject* trainMiddle8 = createObjectCopy(trainMiddle1, Kore::vec3(trainMiddle1->render->position.x() + xoffset * 7, trainMiddle1->render->position.y(), trainMiddle1->render->position.z() + zoffset * 7), Kore::Quaternion(3, 0, yRot, 0));
		ALevelObject* trainFront3 = createObjectCopy(trainBack, Kore::vec3(trainBack->render->position.x() + xoffset * 9 + 0.2, trainBack->render->position.y(), trainBack->render->position.z() + zoffset * 9), Kore::Quaternion(3, 0, yRot, 0));
		trainMiddle22->moveable = false;	
		trainMiddle33->moveable = false;
		trainMiddle4->moveable = false;
		trainMiddle5->moveable = false;
		trainMiddle6->moveable = false;
		trainMiddle7->moveable = false;
		trainMiddle8->moveable = false;
		trainFront3->moveable = false;
		break;
	}
	default:
		break;
	}
}

void TrainLevel::groundInit(Kore::Graphics4::VertexStructure environmentSructure) {

	ALevelObject* floor = createNewObject("floor/floor.ogex", "floor/", environmentSructure, 1, Kore::vec3(-410, -3, -2.75), Kore::Quaternion(-1, 0, 0, 0));
	floor->render->tag = "floor";
	ALevelObject* hfloor = createNewObject("floor/hfloor.ogex", "floor/", environmentSructure, 1, Kore::vec3(-410, -2, -6), Kore::Quaternion(3, 0, 1, 0));
	ALevelObject* object = createObjectCopy(floor, Kore::vec3(floor->render->position.x(), floor->render->position.y(), -floor->render->position.z()), floor->render->rotation);
	object->render->tag = "floor";
	ALevelObject* lfloor = createNewObject("floor/lfloor.ogex", "floor/", environmentSructure, 1, Kore::vec3(-410, -2, 3), Kore::Quaternion(3, 0, 0.99, 0));
	lfloor->render->activated = false;
	//object = createObjectCopy(lfloor, Kore::vec3(lfloor->render->position.x(), lfloor->render->position.y(), -lfloor->render->position.z()), lfloor->render->rotation);

	

	float xoffset = 25.0f;
	float xoffset2 = 18;
	float zoffset2 = 0.15f;
	float offsetZfloor = 0.175f;

	for (int x = 0; x < 40; x++) {
		
		object = createObjectCopy(floor, Kore::vec3(floor->render->position.x()+xoffset*x, floor->render->position.y(), floor->render->position.z()+ offsetZfloor*x), floor->render->rotation);
		object->render->tag = "floor";
		//object = createObjectCopy(hfloor, Kore::vec3(hfloor->render->position.x() + xoffset2, hfloor->render->position.y(), -hfloor->render->position.z()), hfloor->render->rotation);

		
		//object = createObjectCopy(lfloor, Kore::vec3(lfloor->render->position.x() + xoffset2, lfloor->render->position.y(), -lfloor->render->position.z()), lfloor->render->rotation);

		//xoffset += 26.0f;
		//xoffset2 += 20;
	}


	for (int x = 0; x < 56; x++) {
		object = createObjectCopy(lfloor, Kore::vec3(lfloor->render->position.x() + xoffset2 * x, lfloor->render->position.y(), lfloor->render->position.z() + zoffset2 * x), lfloor->render->rotation);
		object->render->tag = "floorl";
	}


}

void TrainLevel::houseInit(Kore::Graphics4::VertexStructure environmentSructure) {
	//float xoffset = 7.7f;
	float xoffset = 13;
	float zoffset = 0.115f;
	ALevelObject* houseL = createNewObject("houseL/hausL.ogex", "houseL/", environmentSructure, 1, Kore::vec3(-410, -10, 11), Kore::Quaternion(3, 0, 0, 0));
	ALevelObject* houseS = createNewObject("houseS/haus.ogex", "houseS/", environmentSructure, 1, Kore::vec3(-402.3, -10, 15), Kore::Quaternion(3, 0, 1, 0));
	ALevelObject* houseM = createNewObject("houseM/hausM.ogex", "houseM/", environmentSructure, 1, Kore::vec3(-394.6, -10, 14 + zoffset*2), Kore::Quaternion(3, 0, 0, 0));
	ALevelObject* houseML = createNewObject("houseML/hausML.ogex", "houseML/", environmentSructure, 1, Kore::vec3(-386.9, -10, 14 + zoffset*3), Kore::Quaternion(3, 0, 0, 0));
	houseL->render->activated = false;
	houseML->render->activated = false;
	houseS->render->activated = false;
	houseM->render->activated = false;
	// Mirror
	ALevelObject* object = createObjectCopy(houseL, Kore::vec3(houseL->render->position.x(), houseL->render->position.y(), -houseL->render->position.z()), houseL->render->rotation);
	object->render->activated = false;
	//object = createObjectCopy(houseS, Kore::vec3(houseS->render->position.x(), houseS->render->position.y(), -houseS->render->position.z()), houseS->render->rotation);
	//object = createObjectCopy(houseM, Kore::vec3(houseM->render->position.x(), houseM->render->position.y(), -houseM->render->position.z()), houseM->render->rotation);
	//object = createObjectCopy(houseML, Kore::vec3(houseML->render->position.x(), houseML->render->position.y(), -houseML->render->position.z()), houseML->render->rotation);
	int randNumb = 0;
	//float xoffset = 30.8f;
	for (int x = 0; x < 67; x++) {
		randNumb = rand() % 4;
		switch (randNumb) {
		case 0:
		{
			object = createObjectCopy(houseS, Kore::vec3(houseL->render->position.x() + xoffset*x, -3, houseL->render->position.z() + zoffset*x), houseS->render->rotation);
			object->render->tag = "houseR";
			break;
		}
		case 1:
		{
			object = createObjectCopy(houseM, Kore::vec3(houseL->render->position.x() + xoffset * x, -3, houseL->render->position.z() + zoffset * x), houseS->render->rotation);
			object->render->tag = "houseR";
			break;
		}
		case 2:
		{
			object = createObjectCopy(houseML, Kore::vec3(houseL->render->position.x() + xoffset * x, -3, houseL->render->position.z() + zoffset * x), houseS->render->rotation);
			object->render->tag = "houseR";
			break;
		}
		case 3:
		{
			object = createObjectCopy(houseL, Kore::vec3(houseL->render->position.x() + xoffset * x, -3, houseL->render->position.z() + zoffset * x), houseS->render->rotation);
			object->render->tag = "houseR";
			break;
		}
		}
	}
	randNumb = rand() % 4;
	for (int x = 0; x < 67; x++) {
		int randNumb = rand() % 4;
		switch (randNumb) {
		case 0:
		{
			object = createObjectCopy(houseS, Kore::vec3(houseL->render->position.x() + xoffset * x, -3, -houseS->render->position.z() + zoffset * x), houseS->render->rotation);
			object->render->tag = "houseL";
			break;
		}
		case 1:
		{
			object = createObjectCopy(houseM, Kore::vec3(houseL->render->position.x() + xoffset * x, -3, -houseS->render->position.z() + zoffset * x), houseS->render->rotation);
			object->render->tag = "houseL";
			break;
		}
		case 2:
		{
			object = createObjectCopy(houseML, Kore::vec3(houseL->render->position.x() + xoffset * x, -3, -houseS->render->position.z() + zoffset * x), houseS->render->rotation);
			object->render->tag = "houseL";
			break;
		}
		case 3:
		{
			object = createObjectCopy(houseL, Kore::vec3(houseL->render->position.x() + xoffset * x, -3, -houseS->render->position.z() + zoffset * x), houseS->render->rotation);
			object->render->tag = "houseL";
			break;
		}
		}
	}
}
//Old
void TrainLevel::houseInit(Kore::Graphics4::VertexStructure environmentSructure, bool placeholder) {
	ALevelObject* houseL = createNewObject("houseL/hausL.ogex", "houseL/", environmentSructure, 1, Kore::vec3(-34.2, -3, 17), Kore::Quaternion(3, 0, 0, 0));
	ALevelObject* houseS = createNewObject("houseS/haus.ogex", "houseS/", environmentSructure, 1, Kore::vec3(-26.5, -3, 17), Kore::Quaternion(3, 0, 0, 0));	
	ALevelObject* houseM = createNewObject("houseM/hausM.ogex", "houseM/", environmentSructure, 1, Kore::vec3(-18.8, -3, 17), Kore::Quaternion(3, 0, 0, 0));
	ALevelObject* houseML = createNewObject("houseML/hausML.ogex", "houseML/", environmentSructure, 1, Kore::vec3(-11.1, -3, 17), Kore::Quaternion(3, 0, 0, 0));
	// Mirror
	ALevelObject* object = createObjectCopy(houseL, Kore::vec3(houseL->render->position.x(), houseL->render->position.y(), -houseL->render->position.z()), houseL->render->rotation);
	object = createObjectCopy(houseS, Kore::vec3(houseS->render->position.x(), houseS->render->position.y(), -houseS->render->position.z()), houseS->render->rotation);
	object = createObjectCopy(houseM, Kore::vec3(houseM->render->position.x(), houseM->render->position.y(), -houseM->render->position.z()), houseM->render->rotation);
	object = createObjectCopy(houseML, Kore::vec3(houseML->render->position.x(), houseML->render->position.y(), -houseML->render->position.z()), houseML->render->rotation);

	float xoffset = 30.8f;
	for (int i = 0; i < 5; i++) {
		 object = createObjectCopy(houseL, Kore::vec3(houseL->render->position.x()+xoffset,houseL->render->position.y(),houseL->render->position.z()),houseL->render->rotation);
		 object = createObjectCopy(houseL, Kore::vec3(houseL->render->position.x() + xoffset, houseL->render->position.y(), -houseL->render->position.z()), houseL->render->rotation);
		 if (i < 4) {
			 object = createObjectCopy(houseS, Kore::vec3(houseS->render->position.x() + xoffset, houseS->render->position.y(), houseS->render->position.z()), houseS->render->rotation);
			 object = createObjectCopy(houseS, Kore::vec3(houseS->render->position.x() + xoffset, houseS->render->position.y(), -houseS->render->position.z()), houseS->render->rotation);
			 object = createObjectCopy(houseM, Kore::vec3(houseM->render->position.x() + xoffset, houseM->render->position.y(), houseM->render->position.z()), houseM->render->rotation);
			 object = createObjectCopy(houseM, Kore::vec3(houseM->render->position.x() + xoffset, houseM->render->position.y(), -houseM->render->position.z()), houseM->render->rotation);
			 object = createObjectCopy(houseML, Kore::vec3(houseML->render->position.x() + xoffset, houseML->render->position.y(), houseML->render->position.z()), houseML->render->rotation);
			 object = createObjectCopy(houseML, Kore::vec3(houseML->render->position.x() + xoffset, houseML->render->position.y(), -houseML->render->position.z()), houseML->render->rotation);
		 }
		xoffset += 30.8f;
	}
}

void TrainLevel::airplaneInit(Kore::Graphics4::VertexStructure environmentSructure) {

	ALevelObject* airplane = createNewObject("airplane/airplane.ogex", "airplane/", environmentSructure, 1, Kore::vec3(78,50,0), Kore::Quaternion(3, 0, 1, 0));
	airplane->render->tag = "airplane";
	//airplane->render->activated = false;
}

void TrainLevel::carInit(Kore::Graphics4::VertexStructure environmentSructure) {

	ALevelObject* car = createNewObject("cars/car.ogex", "cars/", environmentSructure, 1, Kore::vec3(78, -2, 5), Kore::Quaternion(3, 0, 1, 0));
	car->render->tag = "car";
	ALevelObject* object = createObjectCopy(car, Kore::vec3(car->render->position.x()*2, car->render->position.y(), car->render->position.z()+3.5), car->render->rotation);
	object->render->tag = "car1";

}

void TrainLevel::tunnelInit(Kore::Graphics4::VertexStructure environmentSructure) {
	
	//float yRot = 1.995f;
	float yRot = 1.996f;
	//float xoffset = 16.0f;
	float xoffset = 15.0f;
	//float zoffset = 0.11f;
	float zoffset = 0.1f;
	float yoffset = 0.023;
	//Right height = -11
	//ALevelObject* tunnel = createNewObject("tunnel/tunnelNew.ogex", "tunnel/", environmentSructure, 1, Kore::vec3(0, -10, 0), Kore::Quaternion(3, 0, 0, 0));
	ALevelObject* tunnel = createNewObject("tunnel/tunnelNew.ogex", "tunnel/", environmentSructure, 1, Kore::vec3(454 - xoffset * 8, -10, 3 - zoffset * 8), Kore::Quaternion(3, 0, 0, 0));
	//ALevelObject* trainBack = createNewObject("train/trainFront.ogex", "train/", environmentSructure, 1, Kore::vec3(7.2, -3, 0.08), Kore::Quaternion(3, 0, -0.005f, 0));
	ALevelObject* tunnel2 = createObjectCopy(tunnel, Kore::vec3(tunnel->render->position.x() + xoffset, tunnel->render->position.y() + yoffset, tunnel->render->position.z() + zoffset), Kore::Quaternion(3, 0, yRot, 0));
	ALevelObject* tunnel3 = createObjectCopy(tunnel, Kore::vec3(tunnel->render->position.x() + xoffset * 2, tunnel->render->position.y() + yoffset * 2, tunnel->render->position.z() + zoffset * 2), Kore::Quaternion(3, 0, yRot, 0));
	ALevelObject* tunnel4 = createObjectCopy(tunnel, Kore::vec3(tunnel->render->position.x() + xoffset * 3, tunnel->render->position.y() + yoffset * 3, tunnel->render->position.z() + zoffset * 3), Kore::Quaternion(3, 0, yRot, 0));
	ALevelObject* tunnel5 = createObjectCopy(tunnel, Kore::vec3(tunnel->render->position.x() + xoffset * 4, tunnel->render->position.y() + yoffset * 4, tunnel->render->position.z() + zoffset * 4), Kore::Quaternion(3, 0, yRot, 0));
	ALevelObject* tunnel6 = createObjectCopy(tunnel, Kore::vec3(tunnel->render->position.x() + xoffset * 5, tunnel->render->position.y() + yoffset * 5, tunnel->render->position.z() + zoffset * 5), Kore::Quaternion(3, 0, yRot, 0));
	ALevelObject* tunnel7 = createObjectCopy(tunnel, Kore::vec3(tunnel->render->position.x() + xoffset * 6, tunnel->render->position.y() + yoffset * 6, tunnel->render->position.z() + zoffset * 6), Kore::Quaternion(3, 0, yRot, 0));
	ALevelObject* tunnel8 = createObjectCopy(tunnel, Kore::vec3(tunnel->render->position.x() + xoffset * 7, tunnel->render->position.y() + yoffset * 7, tunnel->render->position.z() + zoffset * 7), Kore::Quaternion(3, 0, yRot, 0));
	ALevelObject* tunnel9 = createObjectCopy(tunnel, Kore::vec3(tunnel->render->position.x() + xoffset * 8, tunnel->render->position.y() + yoffset * 8, tunnel->render->position.z() + zoffset * 8), Kore::Quaternion(3, 0, yRot, 0));
	tunnel2->render->activated = false;
	tunnel3->render->activated = false;
	tunnel4->render->activated = false;
	tunnel5->render->activated = false;
	tunnel6->render->activated = false;
	tunnel7->render->activated = false;
	tunnel8->render->activated = false;
	tunnel9->render->activated = false;
	tunnel->render->activated = false;
	tunnel2->moveable = false;
	tunnel3->moveable = false;
	tunnel4->moveable = false;
	tunnel5->moveable = false;
	tunnel6->moveable = false;
	tunnel7->moveable = false;
	tunnel8->moveable = false;
	tunnel9->moveable = false;
	tunnel->moveable = false;
	tunnel->render->tag = "tunnelS";
	tunnel2->render->tag = "tunnelS";
	tunnel3->render->tag = "tunnelS";
	tunnel4->render->tag = "tunnelS";
	tunnel5->render->tag = "tunnelS";
	tunnel6->render->tag = "tunnelS";
	tunnel7->render->tag = "tunnelS";
	tunnel8->render->tag = "tunnelS";
	tunnel9->render->tag = "tunnelS";
	
	//objects[0] = tunnel;
}

void TrainLevel::v()
{
	objects[0]->render->position.x() += offsets;
	Kore::log(Kore::Info, "X = %f", objects[0]->render->position.x());
}
void TrainLevel::h()
{
	objects[0]->render->position.x() -= offsets;
	Kore::log(Kore::Info, "X = %f", objects[0]->render->position.x());
}
void TrainLevel::l()
{
	objects[0]->render->position.z() += offsets;
	Kore::log(Kore::Info, "Z = %f", objects[0]->render->position.z());
}
void TrainLevel::r()
{
	objects[0]->render->position.z() -= offsets;
	Kore::log(Kore::Info, "Z = %f", objects[0]->render->position.z());
}
void TrainLevel::t()
{
	for (ALevelObject* object : environment) {
		if (object->render->tag == "tunnelS") {
			object->moveable = true;
			object->render->activated = true;
		}
	}
}
void TrainLevel::x()
{
	objects[0]->render->rotation.x += offsets;
	Kore::log(Kore::Info, "Xr = %f", objects[0]->render->rotation.x);
}
void TrainLevel::y()
{
	objects[0]->render->rotation.y += offsets;
	Kore::log(Kore::Info, "Yr = %f", objects[0]->render->rotation.y);
}
void TrainLevel::z()
{
	objects[0]->render->rotation.z += offsets;
	Kore::log(Kore::Info, "Zr = %f", objects[0]->render->rotation.z);
	objects[0]->render->activated = !objects[0]->render->activated;
}
