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
	//write level-specific runtime logic here
	Level::update(deltaT);

	if (gameStart) {
		updateBuilding(deltaT, 20);
		updateFPS(deltaT);		
		initialCountdown += deltaT;
		if(initialCountdown > 3.0f)
			gamePlay(deltaT);
		else initialCountdown += deltaT;
	}
}

void TrainLevel::gamePlay(double deltaT)
{
	avatar->entity->position = locToGlob.Invert() * Kore::vec4(cameraPos.x(), cameraPos.y(), cameraPos.z(), 1.0);

	if (form->gameStarted())
	{
		checkStation(deltaT);
		checkEnemyCollision();
		checkHittingAvatar();
		checkHittingEnemy();
		checkingMoving();
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


void TrainLevel::updateBuilding(double deltaT, double speed)
{	
	if (!form->isFormShown())
	{
		for (ALevelObject* object : environment)
			if (object->moveable)
				if (object->object->position.x() > -420)
				{

					if (object->object->tag == "car") {
						object->object->position.x() -= deltaT * speed * 0.5f;
						object->object->position.z() -= 0.01 * deltaT;
					}
					else if (object->object->tag == "car1") {
						object->object->position.x() -= deltaT * speed * 2;
						object->object->position.z() -= deltaT * speed * 0.01f;
					}
					else if (object->object->tag == "tunnelS") {
						
						//object->render->activated = true;
						object->object->position.x() -= deltaT * speed * 3;
						object->object->position.z() -= deltaT * speed * 0.02f;
						
					}
					else if (object->object->tag == "airplane") {
						speed *= 0.4;
						object->object->position.x() -= deltaT * speed * 3;
						if (object->object->position.x() < 0)
							object->object->position.y() += deltaT * speed;
						else if (object->object->position.y() > 15)
						{
							object->object->position.y() -= deltaT * speed;

						}

						airPlanePos = object->object->position;
						
					}
					else {
						object->object->position.x() -= deltaT * speed;
						object->object->position.z() -= deltaT * speed * 0.0075f;
					}
				}
				else { 
					//object->render->position.z() = 8.325f;
					if (object->object->tag == "floor") {
						object->object->position.x() = 580;
						object->object->position.z() = 4.25f;
					}
					if (object->object->tag == "floorh") {
						object->object->position.x() = 580;
						object->object->position.z() = 0.075f;
					}
					if (object->object->tag == "floorl") {
						object->object->position.x() = 580;
						object->object->position.z() = -3.0f;
					}
					if (object->object->tag == "houseR") {
						object->object->position.x() = 454;
						object->object->position.z() = 20.21;
					}
					if (object->object->tag == "houseL") {
						object->object->position.x() = 454;
						object->object->position.z() = -14.0f;
					}
					if (object->object->tag == "car1") {
						object->object->position.x() = 454;
						object->object->position.z() = -6;
					}
					if (object->object->tag == "car") {
						object->object->position.x() = 454;
						object->object->position.z() = 12;
					}
					if (object->object->tag == "airplane") {
						object->object->position.x() = 454;
						object->object->position.y() = 50;
					}
					if (object->object->tag == "tunnelS") {
						tunnelCounter++;
						if (tunnelCounter == 9) {
							tunnelCounter = 0;
							
							for (ALevelObject* object : environment) {
								if (object->object->tag == "tunnelS") {
									object->object->activated = false;
									object->moveable = false;
								}
							}
						}
						object->object->position.x() = 454;
						object->object->position.z() = 3;
						
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
	
	//Load Avatar
	avatar = new TheAvatar("avatar/avatar_male.ogex", "avatar/", entitySructure, 1.0f, Kore::vec3(0, 0, 0), Kore::Quaternion(0, 0, 0, 0), false, true);

	animator = new Animator(avatar->entity);

	//create Pool of entities
	//Load Enemy
	createEnemy(entitySructure);

	//Load Skybox
	skyInit(environmentSructure);

	//Load Train
	//trainInit(environmentSructure);
	trainInit(environmentSructure, 3);

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

	if (countDown > maxWaitingTime | (stationStarted & countDown > maxWaitingTime / 100.0))
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
		Kore::vec3 leftHandPos = enemy->entity->meshObject->bones[leftHandBoneIndex]->getPosition();
		Kore::vec3 rightHandPos = enemy->entity->meshObject->bones[rightHandBoneIndex]->getPosition();
		 
		Kore::vec3 dir_L_Foot = leftFootPos - avatar->entity->position;
		Kore::vec3 dir_R_Foot = rightFootPos - avatar->entity->position;
		Kore::vec3 dir_L_Hand = leftHandPos - avatar->entity->position;
		Kore::vec3 dir_R_Hand = rightHandPos - avatar->entity->position;

		dir_L_Foot.z() = 0;
		dir_R_Foot.z() = 0;
		dir_L_Hand.z() = 0;
		dir_R_Hand.z() = 0;

		if (leftFootPos.z() > hittingHeightFoot & dir_L_Foot.getLength() < hittingRadius & !enemy->entity->attackedSuccessfully)
		{
			enemy->entity->attackedSuccessfully = true;
			avatar->entity->hit();
		}
		else if (rightFootPos.z() > hittingHeightFoot & dir_R_Foot.getLength() < hittingRadius & !enemy->entity->attackedSuccessfully)
		{
			enemy->entity->attackedSuccessfully = true;
			avatar->entity->hit();
		}
		else if (leftHandPos.z() > hittingHeightHand & dir_L_Hand.getLength() < hittingRadius & !enemy->entity->attackedSuccessfully)
		{
			enemy->entity->attackedSuccessfully = true;
			avatar->entity->hit();
		}
		else if (rightHandPos.z() > hittingHeightHand & dir_R_Hand.getLength() < hittingRadius & !enemy->entity->attackedSuccessfully)
		{
			enemy->entity->attackedSuccessfully = true;
			avatar->entity->hit();
		}
		else if (dir_L_Foot.getLength() > hittingRadius & dir_R_Foot.getLength() > hittingRadius & enemy->entity->attackedSuccessfully)
		{
			enemy->entity->attackedSuccessfully = false;
		}


	}
}

void TrainLevel::showAttackInUI(string colorTag)
{
	MainForm* mainForm = (MainForm*)form;

	if (colorTag == "NinjaW")
		mainForm->kindOfAttacking = "Punch, Kick or Punch-Kick";
	else if(colorTag == "NinjaY")
		mainForm->kindOfAttacking = "Punch or Kick";
	else if (colorTag == "NinjaB")
		mainForm->kindOfAttacking = "Punch or Punch-Kick";
	else if (colorTag == "NinjaR")
		mainForm->kindOfAttacking = "Kick or Punch-Kick";
	else if (colorTag == "NinjaM")
		mainForm->kindOfAttacking = "Kick";
	else
		mainForm->kindOfAttacking = NULL;
}

void TrainLevel::checkingMoving()
{
	if (avatar->entity->lastMovement == Avatar::Jogging && !avatar->entity->movementExpired)
	{
		Kore::vec3 dir = camForward;
		cameraPos += dir * camVelocity;
	}
}

void TrainLevel::checkHittingEnemy()
{
	float crossValue = 100000;
	float distance = 100000;
	string colorTag = "";
	bool allDead = true;

	for (NonPlayerCharacter* enemy : enemies)
	{
		Kore::vec3 entityPosGlob = locToGlob * Kore::vec4(enemy->entity->position.x(), enemy->entity->position.y(), enemy->entity->position.z(), 1.0);
		Kore::vec3 avatarPosGlob = locToGlob * Kore::vec4(avatar->entity->position.x(), avatar->entity->position.y(), avatar->entity->position.z(), 1.0);
		Kore::vec3 playerToEntDir = (entityPosGlob - avatarPosGlob);

		Kore::vec3 avatarDir = camForward;
		//avatarDir *= -1;

		playerToEntDir.y() = 0;
		avatarDir.y() = 0;
		float currentDistance = playerToEntDir.getLength();
		float currentCrossValue =Kore::abs( playerToEntDir.cross(avatarDir).y());
		float skalarProdukt = playerToEntDir * avatarDir;

		if (currentCrossValue < crossValue && enemy->entity->activated && skalarProdukt > 0) 
		{
			allDead = false;
			crossValue = currentCrossValue;
			distance = currentDistance;

			if (distance < minAttackingDistance & crossValue < 0.5)
				colorTag = enemy->entity->colorTag;
			else
				colorTag = "N";
		}

		//Kollision
		Kore::vec3 leftFootPos = avatar->entity->meshObject->bones[leftFootBoneIndex]->getPosition();
		Kore::vec3 rightFootPos = avatar->entity->meshObject->bones[leftFootBoneIndex]->getPosition();
		Kore::vec3 leftHandPos = enemy->entity->meshObject->bones[leftHandBoneIndex]->getPosition();
		Kore::vec3 rightHandPos = enemy->entity->meshObject->bones[rightHandBoneIndex]->getPosition();

		Kore::vec3 dir_L_Foot = leftFootPos - enemy->entity->position;
		Kore::vec3 dir_R_Foot = rightFootPos - enemy->entity->position;
		Kore::vec3 dir_L_Hand = leftHandPos - avatar->entity->position;
		Kore::vec3 dir_R_Hand = rightHandPos - avatar->entity->position;

		dir_L_Foot.z() = 0;
		dir_R_Foot.z() = 0;
		dir_L_Hand.z() = 0;
		dir_R_Hand.z() = 0;

		if (avatar->entity->endEffector[leftFoot]->getDesPosition().z() > hittingHeightFoot & dir_L_Foot.getLength() < hittingRadius & !avatar->entity->attackedSuccessfully)
		{
			avatar->entity->attackedSuccessfully = true;
			enemy->entity->hit();
			
			if (avatar->entity->lastMovement == Avatar::Kick && !avatar->entity->movementExpired)
			{
				((MainForm*)form)->highScore++;

				if (enemy->entity->colorTag == "NinjaW" | enemy->entity->colorTag == "NinjaY"| enemy->entity->colorTag == "NinjaR" | enemy->entity->colorTag == "NinjaM")
				{
					((MainForm*)form)->highScore++;
				}
			}
		}
		else if (avatar->entity->endEffector[rightFoot]->getDesPosition().z() > hittingHeightFoot & dir_R_Foot.getLength() < hittingRadius & !avatar->entity->attackedSuccessfully)
		{
			avatar->entity->attackedSuccessfully = true;
			enemy->entity->hit();

			if (avatar->entity->lastMovement == Avatar::Kick && !avatar->entity->movementExpired)
			{
				((MainForm*)form)->highScore++;

				if (enemy->entity->colorTag == "NinjaW" | enemy->entity->colorTag == "NinjaY" | enemy->entity->colorTag == "NinjaR" | enemy->entity->colorTag == "NinjaM")
				{
					((MainForm*)form)->highScore++;
				}
			}
		}
		else if (avatar->entity->endEffector[leftHand]->getDesPosition().z() > hittingHeightHand & dir_L_Hand.getLength() < hittingRadius & !enemy->entity->attackedSuccessfully)
		{
			avatar->entity->attackedSuccessfully = true;
			enemy->entity->hit();

			if (avatar->entity->lastMovement == Avatar::Punch && !avatar->entity->movementExpired)
			{
				((MainForm*)form)->highScore++;

				if (enemy->entity->colorTag == "NinjaW" | enemy->entity->colorTag == "NinjaY" | enemy->entity->colorTag == "NinjaB")
				{
					((MainForm*)form)->highScore++;
				}
			}
		}
		else if (avatar->entity->endEffector[rightHand]->getDesPosition().z() > hittingHeightHand & dir_R_Hand.getLength() < hittingRadius & !enemy->entity->attackedSuccessfully)
		{
			avatar->entity->attackedSuccessfully = true;
			enemy->entity->hit();

			if (avatar->entity->lastMovement == Avatar::Punch && !avatar->entity->movementExpired)
			{
				((MainForm*)form)->highScore++;

				if (enemy->entity->colorTag == "NinjaW" | enemy->entity->colorTag == "NinjaY" | enemy->entity->colorTag == "NinjaB")
				{
					((MainForm*)form)->highScore++;
				}
			}
		}
		else if (dir_L_Foot.getLength() > hittingRadius & dir_R_Foot.getLength() > hittingRadius & avatar->entity->attackedSuccessfully)
		{
			avatar->entity->attackedSuccessfully = false;
		}
	}

	if (allDead)
		colorTag = "N";

	if(colorTag != "")
		showAttackInUI(colorTag);
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
	renderer->setLights(*sky->object, renderer->environmentGraphics->lightCount, renderer->environmentGraphics->lightPosLocation);
	sky->moveable = false;	
}
//Old
void TrainLevel::trainInit(Kore::Graphics4::VertexStructure environmentSructure) {
	ALevelObject* trainf = createNewObject("train/trainFront.ogex", "train/", environmentSructure, 1, Kore::vec3(40.8, -3, 0), Kore::Quaternion(3, 0, 2, 0));
	trainf->moveable = false;
	ALevelObject* trainm = createNewObject("train/trainMiddle.ogex", "train/", environmentSructure, 1, Kore::vec3(7.2, -3, 0), Kore::Quaternion(3, 0, 2, 0));
	renderer->setLights(*trainm->object, renderer->environmentGraphics->lightCount, renderer->environmentGraphics->lightPosLocation);
	trainm->moveable = false;

	float xoffset = 16.8;
	ALevelObject* object = createObjectCopy(trainm, Kore::vec3(trainm->object->position.x() + xoffset, trainm->object->position.y(), trainm->object->position.z()), trainm->object->rotation);
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
		ALevelObject* trainFront = createObjectCopy(trainBack, Kore::vec3(trainBack->object->position.x() + xoffset * 2 + 0.3, trainBack->object->position.y(), trainBack->object->position.z() + zoffset * 2), Kore::Quaternion(3, 0, yRot, 0));
		trainFront->moveable = false;
		break;
	}
	case 2:
	{
		ALevelObject* trainMiddle2 = createObjectCopy(trainMiddle1, Kore::vec3(trainMiddle1->object->position.x() + xoffset, trainMiddle1->object->position.y(), trainMiddle1->object->position.z() + zoffset), Kore::Quaternion(3, 0, yRot, 0));
		ALevelObject* trainMiddle3 = createObjectCopy(trainMiddle1, Kore::vec3(trainMiddle1->object->position.x() + xoffset * 2, trainMiddle1->object->position.y(), trainMiddle1->object->position.z() + zoffset * 2), Kore::Quaternion(3, 0, yRot, 0));
		ALevelObject* trainFront2 = createObjectCopy(trainBack, Kore::vec3(trainBack->object->position.x() + xoffset * 4 + 0.2, trainBack->object->position.y(), trainBack->object->position.z() + zoffset * 4), Kore::Quaternion(3, 0, yRot, 0));
		trainMiddle2->moveable = false;
		trainMiddle3->moveable = false;
		trainFront2->moveable = false;
		break;
	}
	case 3:
	{
		ALevelObject* trainMiddle22 = createObjectCopy(trainMiddle1, Kore::vec3(trainMiddle1->object->position.x() + xoffset, trainMiddle1->object->position.y(), trainMiddle1->object->position.z() + zoffset), Kore::Quaternion(3, 0, yRot, 0));
		ALevelObject* trainMiddle33 = createObjectCopy(trainMiddle1, Kore::vec3(trainMiddle1->object->position.x() + xoffset * 2, trainMiddle1->object->position.y(), trainMiddle1->object->position.z() + zoffset * 2), Kore::Quaternion(3, 0, yRot, 0));
		ALevelObject* trainMiddle4 = createObjectCopy(trainMiddle1, Kore::vec3(trainMiddle1->object->position.x() + xoffset * 3, trainMiddle1->object->position.y(), trainMiddle1->object->position.z() + zoffset * 3), Kore::Quaternion(3, 0, yRot, 0));
		ALevelObject* trainMiddle5 = createObjectCopy(trainMiddle1, Kore::vec3(trainMiddle1->object->position.x() + xoffset * 4, trainMiddle1->object->position.y(), trainMiddle1->object->position.z() + zoffset * 4), Kore::Quaternion(3, 0, yRot, 0));
		ALevelObject* trainMiddle6 = createObjectCopy(trainMiddle1, Kore::vec3(trainMiddle1->object->position.x() + xoffset * 5, trainMiddle1->object->position.y(), trainMiddle1->object->position.z() + zoffset * 5), Kore::Quaternion(3, 0, yRot, 0));
		ALevelObject* trainMiddle7 = createObjectCopy(trainMiddle1, Kore::vec3(trainMiddle1->object->position.x() + xoffset * 6, trainMiddle1->object->position.y(), trainMiddle1->object->position.z() + zoffset * 6), Kore::Quaternion(3, 0, yRot, 0));
		ALevelObject* trainMiddle8 = createObjectCopy(trainMiddle1, Kore::vec3(trainMiddle1->object->position.x() + xoffset * 7, trainMiddle1->object->position.y(), trainMiddle1->object->position.z() + zoffset * 7), Kore::Quaternion(3, 0, yRot, 0));
		ALevelObject* trainFront3 = createObjectCopy(trainBack, Kore::vec3(trainBack->object->position.x() + xoffset * 9 + 0.2, trainBack->object->position.y(), trainBack->object->position.z() + zoffset * 9), Kore::Quaternion(3, 0, yRot, 0));
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
	floor->object->tag = "floor";
	ALevelObject* hfloor = createNewObject("floor/hfloor.ogex", "floor/", environmentSructure, 1, Kore::vec3(-410, -2, -6), Kore::Quaternion(3, 0, 1, 0));
	ALevelObject* object = createObjectCopy(floor, Kore::vec3(floor->object->position.x(), floor->object->position.y(), -floor->object->position.z()), floor->object->rotation);
	object->object->tag = "floor";
	ALevelObject* lfloor = createNewObject("floor/lfloor.ogex", "floor/", environmentSructure, 1, Kore::vec3(-410, -2, 3), Kore::Quaternion(3, 0, 0.99, 0));
	lfloor->object->activated = false;
	//object = createObjectCopy(lfloor, Kore::vec3(lfloor->render->position.x(), lfloor->render->position.y(), -lfloor->render->position.z()), lfloor->render->rotation);

	

	float xoffset = 25.0f;
	float xoffset2 = 18;
	float zoffset2 = 0.15f;
	float offsetZfloor = 0.175f;

	for (int x = 0; x < 40; x++) {
		
		object = createObjectCopy(floor, Kore::vec3(floor->object->position.x()+xoffset*x, floor->object->position.y(), floor->object->position.z()+ offsetZfloor*x), floor->object->rotation);
		object->object->tag = "floor";
		//object = createObjectCopy(hfloor, Kore::vec3(hfloor->render->position.x() + xoffset2, hfloor->render->position.y(), -hfloor->render->position.z()), hfloor->render->rotation);

		
		//object = createObjectCopy(lfloor, Kore::vec3(lfloor->render->position.x() + xoffset2, lfloor->render->position.y(), -lfloor->render->position.z()), lfloor->render->rotation);

		//xoffset += 26.0f;
		//xoffset2 += 20;
	}


	for (int x = 0; x < 56; x++) {
		object = createObjectCopy(lfloor, Kore::vec3(lfloor->object->position.x() + xoffset2 * x, lfloor->object->position.y(), lfloor->object->position.z() + zoffset2 * x), lfloor->object->rotation);
		object->object->tag = "floorl";
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
	houseL->object->activated = false;
	houseML->object->activated = false;
	houseS->object->activated = false;
	houseM->object->activated = false;
	// Mirror
	ALevelObject* object = createObjectCopy(houseL, Kore::vec3(houseL->object->position.x(), houseL->object->position.y(), -houseL->object->position.z()), houseL->object->rotation);
	object->object->activated = false;
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
			object = createObjectCopy(houseS, Kore::vec3(houseL->object->position.x() + xoffset*x, -3, houseL->object->position.z() + zoffset*x), houseS->object->rotation);
			object->object->tag = "houseR";
			break;
		}
		case 1:
		{
			object = createObjectCopy(houseM, Kore::vec3(houseL->object->position.x() + xoffset * x, -3, houseL->object->position.z() + zoffset * x), houseS->object->rotation);
			object->object->tag = "houseR";
			break;
		}
		case 2:
		{
			object = createObjectCopy(houseML, Kore::vec3(houseL->object->position.x() + xoffset * x, -3, houseL->object->position.z() + zoffset * x), houseS->object->rotation);
			object->object->tag = "houseR";
			break;
		}
		case 3:
		{
			object = createObjectCopy(houseL, Kore::vec3(houseL->object->position.x() + xoffset * x, -3, houseL->object->position.z() + zoffset * x), houseS->object->rotation);
			object->object->tag = "houseR";
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
			object = createObjectCopy(houseS, Kore::vec3(houseL->object->position.x() + xoffset * x, -3, -houseS->object->position.z() + zoffset * x), houseS->object->rotation);
			object->object->tag = "houseL";
			break;
		}
		case 1:
		{
			object = createObjectCopy(houseM, Kore::vec3(houseL->object->position.x() + xoffset * x, -3, -houseS->object->position.z() + zoffset * x), houseS->object->rotation);
			object->object->tag = "houseL";
			break;
		}
		case 2:
		{
			object = createObjectCopy(houseML, Kore::vec3(houseL->object->position.x() + xoffset * x, -3, -houseS->object->position.z() + zoffset * x), houseS->object->rotation);
			object->object->tag = "houseL";
			break;
		}
		case 3:
		{
			object = createObjectCopy(houseL, Kore::vec3(houseL->object->position.x() + xoffset * x, -3, -houseS->object->position.z() + zoffset * x), houseS->object->rotation);
			object->object->tag = "houseL";
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
	ALevelObject* object = createObjectCopy(houseL, Kore::vec3(houseL->object->position.x(), houseL->object->position.y(), -houseL->object->position.z()), houseL->object->rotation);
	object = createObjectCopy(houseS, Kore::vec3(houseS->object->position.x(), houseS->object->position.y(), -houseS->object->position.z()), houseS->object->rotation);
	object = createObjectCopy(houseM, Kore::vec3(houseM->object->position.x(), houseM->object->position.y(), -houseM->object->position.z()), houseM->object->rotation);
	object = createObjectCopy(houseML, Kore::vec3(houseML->object->position.x(), houseML->object->position.y(), -houseML->object->position.z()), houseML->object->rotation);

	float xoffset = 30.8f;
	for (int i = 0; i < 5; i++) {
		 object = createObjectCopy(houseL, Kore::vec3(houseL->object->position.x()+xoffset,houseL->object->position.y(),houseL->object->position.z()),houseL->object->rotation);
		 object = createObjectCopy(houseL, Kore::vec3(houseL->object->position.x() + xoffset, houseL->object->position.y(), -houseL->object->position.z()), houseL->object->rotation);
		 if (i < 4) {
			 object = createObjectCopy(houseS, Kore::vec3(houseS->object->position.x() + xoffset, houseS->object->position.y(), houseS->object->position.z()), houseS->object->rotation);
			 object = createObjectCopy(houseS, Kore::vec3(houseS->object->position.x() + xoffset, houseS->object->position.y(), -houseS->object->position.z()), houseS->object->rotation);
			 object = createObjectCopy(houseM, Kore::vec3(houseM->object->position.x() + xoffset, houseM->object->position.y(), houseM->object->position.z()), houseM->object->rotation);
			 object = createObjectCopy(houseM, Kore::vec3(houseM->object->position.x() + xoffset, houseM->object->position.y(), -houseM->object->position.z()), houseM->object->rotation);
			 object = createObjectCopy(houseML, Kore::vec3(houseML->object->position.x() + xoffset, houseML->object->position.y(), houseML->object->position.z()), houseML->object->rotation);
			 object = createObjectCopy(houseML, Kore::vec3(houseML->object->position.x() + xoffset, houseML->object->position.y(), -houseML->object->position.z()), houseML->object->rotation);
		 }
		xoffset += 30.8f;
	}
}

void TrainLevel::airplaneInit(Kore::Graphics4::VertexStructure environmentSructure)
{
	ALevelObject* airplane = createNewObject("airplane/airplane.ogex", "airplane/", environmentSructure, 1, Kore::vec3(78,50,0), Kore::Quaternion(3, 0, 1, 0));
	airplane->object->tag = "airplane";
	//airplane->render->activated = false;
}

void TrainLevel::carInit(Kore::Graphics4::VertexStructure environmentSructure)
{
	ALevelObject* car = createNewObject("cars/car.ogex", "cars/", environmentSructure, 1, Kore::vec3(78, -2, 5), Kore::Quaternion(3, 0, 1, 0));
	car->object->tag = "car";
	ALevelObject* object = createObjectCopy(car, Kore::vec3(car->object->position.x()*2, car->object->position.y(), car->object->position.z()+3.5), car->object->rotation);
	object->object->tag = "car1";
}

//why no array?!?
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
	ALevelObject* tunnel2 = createObjectCopy(tunnel, Kore::vec3(tunnel->object->position.x() + xoffset, tunnel->object->position.y() + yoffset, tunnel->object->position.z() + zoffset), Kore::Quaternion(3, 0, yRot, 0));
	ALevelObject* tunnel3 = createObjectCopy(tunnel, Kore::vec3(tunnel->object->position.x() + xoffset * 2, tunnel->object->position.y() + yoffset * 2, tunnel->object->position.z() + zoffset * 2), Kore::Quaternion(3, 0, yRot, 0));
	ALevelObject* tunnel4 = createObjectCopy(tunnel, Kore::vec3(tunnel->object->position.x() + xoffset * 3, tunnel->object->position.y() + yoffset * 3, tunnel->object->position.z() + zoffset * 3), Kore::Quaternion(3, 0, yRot, 0));
	ALevelObject* tunnel5 = createObjectCopy(tunnel, Kore::vec3(tunnel->object->position.x() + xoffset * 4, tunnel->object->position.y() + yoffset * 4, tunnel->object->position.z() + zoffset * 4), Kore::Quaternion(3, 0, yRot, 0));
	ALevelObject* tunnel6 = createObjectCopy(tunnel, Kore::vec3(tunnel->object->position.x() + xoffset * 5, tunnel->object->position.y() + yoffset * 5, tunnel->object->position.z() + zoffset * 5), Kore::Quaternion(3, 0, yRot, 0));
	ALevelObject* tunnel7 = createObjectCopy(tunnel, Kore::vec3(tunnel->object->position.x() + xoffset * 6, tunnel->object->position.y() + yoffset * 6, tunnel->object->position.z() + zoffset * 6), Kore::Quaternion(3, 0, yRot, 0));
	ALevelObject* tunnel8 = createObjectCopy(tunnel, Kore::vec3(tunnel->object->position.x() + xoffset * 7, tunnel->object->position.y() + yoffset * 7, tunnel->object->position.z() + zoffset * 7), Kore::Quaternion(3, 0, yRot, 0));
	ALevelObject* tunnel9 = createObjectCopy(tunnel, Kore::vec3(tunnel->object->position.x() + xoffset * 8, tunnel->object->position.y() + yoffset * 8, tunnel->object->position.z() + zoffset * 8), Kore::Quaternion(3, 0, yRot, 0));
	tunnel2->object->activated = false;
	tunnel3->object->activated = false;
	tunnel4->object->activated = false;
	tunnel5->object->activated = false;
	tunnel6->object->activated = false;
	tunnel7->object->activated = false;
	tunnel8->object->activated = false;
	tunnel9->object->activated = false;
	tunnel->object->activated = false;
	tunnel2->moveable = false;
	tunnel3->moveable = false;
	tunnel4->moveable = false;
	tunnel5->moveable = false;
	tunnel6->moveable = false;
	tunnel7->moveable = false;
	tunnel8->moveable = false;
	tunnel9->moveable = false;
	tunnel->moveable = false;
	tunnel->object->tag = "tunnelS";
	tunnel2->object->tag = "tunnelS";
	tunnel3->object->tag = "tunnelS";
	tunnel4->object->tag = "tunnelS";
	tunnel5->object->tag = "tunnelS";
	tunnel6->object->tag = "tunnelS";
	tunnel7->object->tag = "tunnelS";
	tunnel8->object->tag = "tunnelS";
	tunnel9->object->tag = "tunnelS";
	//objects[0] = tunnel;
}

void TrainLevel::v()
{
	objects[0]->object->position.x() += offsets;
	Kore::log(Kore::Info, "X = %f", objects[0]->object->position.x());
}
void TrainLevel::h()
{
	objects[0]->object->position.x() -= offsets;
	Kore::log(Kore::Info, "X = %f", objects[0]->object->position.x());
}
void TrainLevel::l()
{
	objects[0]->object->position.z() += offsets;
	Kore::log(Kore::Info, "Z = %f", objects[0]->object->position.z());
}
void TrainLevel::r()
{
	objects[0]->object->position.z() -= offsets;
	Kore::log(Kore::Info, "Z = %f", objects[0]->object->position.z());
}
void TrainLevel::t()
{
	for (ALevelObject* object : environment) {
		if (object->object->tag == "tunnelS") {
			object->moveable = true;
			object->object->activated = true;
		}
	}
}
void TrainLevel::x()
{
	objects[0]->object->rotation.x += offsets;
	Kore::log(Kore::Info, "Xr = %f", objects[0]->object->rotation.x);
}
void TrainLevel::y()
{
	objects[0]->object->rotation.y += offsets;
	Kore::log(Kore::Info, "Yr = %f", objects[0]->object->rotation.y);
}
void TrainLevel::z()
{
	objects[0]->object->rotation.z += offsets;
	Kore::log(Kore::Info, "Zr = %f", objects[0]->object->rotation.z);
	objects[0]->object->activated = !objects[0]->object->activated;
}
