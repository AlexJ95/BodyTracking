#include "TrainLevel.h"
#include "MainForm.h"

/*
	ALevelObject* object->moveable		: set Object should move or not, still render in window
				  object->activated		: set Object should render or not, still moveable	
				  object->render->tag	: set object with a tag, for compare
	
	ALevelObjects are saved in environment;
	NonPlayerCharacters are saved in enemies;

	
	*/

void TrainLevel::init() {
	controlsSetup();
	audioSetup();
	graphicsSetup();
	Level::init();
	form = new MainForm();
	offsets = 1;
	gameStart = false;
	tunnelCounter = 8;
}


void TrainLevel::update(double deltaT)
{
	//code level-specific runtime logic here
	Level::update(deltaT);

	avatar->entity->position = Kore::vec4(math->cameraPos.x(), math->cameraPos.y(), math->cameraPos.z(), 1.0);

	avatar->entity->calibrated = true;

	if (!avatar->entity->calibrated)
		runCalibrationRoom();
	else {	gameStart = true; 
			deleteRoom();
			loadTrainLevel();
	}

	if (gameStart) {
		updateBuilding(deltaT, 50);
		updateFPS(deltaT);		
		starttime += deltaT;
		if(starttime > 10.0f)
			gamePlay(deltaT);
		else starttime += deltaT;
	}

}

void TrainLevel::runCalibrationRoom() {
	for (ALevelObject* object : environment)
		if (object->render->tag == "room") {
			object->render->activated = true;
		}

}

void TrainLevel::deleteRoom() {
	for (ALevelObject* object : environment)
		if (object->render->tag == "room")
		{
			object->render->activated = false;
			freeMemory(object);
		}
}

void TrainLevel::loadTrainLevel() {
	for (ALevelObject* object : environment) {
		if(object->render->tag != "tunnelS" && object->render->tag != "airplane")
			object->render->activated = true;
	}
}

void TrainLevel::gamePlay(double deltaT) {

	if (avatar->entity->position.x() > 15.0f * currentCarriage &&  !enemyExist)
				loadEnemies(deltaT,currentCarriage++);

	if (form->gameStarted())
	{
		checkEnemyCollision();
		checkHittingAvatar();
		checkHittingEnemy();
		checkingMoving();
	}

	if (!enemyExist && currentCarriage == 9)
		loadEnding();
}

void TrainLevel::loadEnemies(float deltaT, int carriage) {

	Kore::log(Kore::Info, "make enemy %d", carriage);
	switch (carriage) {
	case 1: loadAirplane();	break;
	case 2: loadTunnel(carriage);	break;
	default: spawn(deltaT);
	}

	
}

void TrainLevel::loadEnemyRandom(float deltaT, int carriage) {

	Kore::log(Kore::Info, "make enemy");
	int num = 1;

	switch (num) {
	case 0: checkStation(deltaT); break;
	case 1: loadTunnel(carriage); break;
	default: checkStation(deltaT);
	}
}

void TrainLevel::loadTunnel(int range) {

	Kore::log(Kore::Info, "make tunnel");
	for (ALevelObject* object : environment) {
		if (object->render->tag == "tunnelS") {
			object->render->activated = true;
			
		}
	}
	enemyExist = true;
}

void TrainLevel::loadAirplane() {
	Kore::log(Kore::Info, "call the airplane");
	for (ALevelObject* object : environment) {
		if (object->render->tag == "airplane") {
			object->render->activated = true;
		}
	}
}

void TrainLevel::loadEnemy(int range,Kore::vec3 pos) {

	Kore::log(Kore::Info, "drop the enemy");
	NonPlayerCharacter* enemy = enemies[range];		
	enemy->entity->position = pos;
	enemy->entity->rotation = Kore::Quaternion(Kore::vec3(0, 0, 1), Kore::pi);		
	enemy->ai->spawn();
	enemy->entity->activated = true;
	enemyExist = true;
}


void TrainLevel::loadEnding() {



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
			if (object->render->moveable && object->render->activated)
				if (object->render->position.x() > -420)
				{

					if (object->render->tag == "car") {
						object->render->position.x() -= deltaT * speed * 1.5f;
						object->render->position.z() -= 0.23 * deltaT;
					}
					else if (object->render->tag == "car1") {
						object->render->position.x() -= deltaT * speed * 4;
						object->render->position.z() -= deltaT * speed * 0.03f;
					}
					else if (object->render->tag == "tunnelS") {
						object->render->position.x() -= deltaT * speed * 3;
						object->render->position.z() -= deltaT * speed * 0.02f;
						if (object->render->position.x() < 0)
							enemyExist = false;					
					}
					else if (object->render->tag == "airplane") {

						if (object->render->position.x() < avatar->entity->position.x() + 30 && object->render->position.x() > 0 && !enemyExist)
							loadEnemy(currentCarriage,object->render->position);

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
					if (object->render->tag == "floor") {
						setPosition(object, 580, object->render->position.y(), 4.25f);
					}
					else if (object->render->tag == "floorl") {
						setPosition(object, 580, object->render->position.y(), -3.3f);
					}
					else if (object->render->tag == "floorr") {
						setPosition(object, 580, object->render->position.y(), 12.7f);
					}
					else if (object->render->tag == "houseR") {
						setPosition(object, 454, object->render->position.y(), 19.21);
					}
					else if (object->render->tag == "houseL") {
						setPosition(object, 454, object->render->position.y(), -12.0f);
					}
					else if (object->render->tag == "car1") {
						setPosition(object, 454, object->render->position.y(), -2.6f);
					}
					else if (object->render->tag == "car") {
						setPosition(object, 454, object->render->position.y(), 12.3f);
					}
					else if (object->render->tag == "airplane") {
						object->render->activated = false;
						object->render->position = object->initPosition;
					}
					else if (object->render->tag == "tunnelS") {
						object->render->activated = false;
						object->render->position = object->initPosition;
					}
					else setPosition(object, 454, object->render->position.y(), object->render->position.z());
				}
	}
						
}

void TrainLevel::setPosition(ALevelObject* alo ,float x, float y, float z) {
	alo->render->position = Kore::vec3(x, y, z);
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
	
	//Load Calibration
	roomInit(environmentSructure);


	//Load Skybox
	skyInit(environmentSructure);

	//Load Train
	trainInit(environmentSructure, 3);

	//Load Avatar
	avatar = new TheAvatar("avatar/avatar_male.ogex", "avatar/", entitySructure, 1.0f, Kore::vec3(0, 0, 0), Kore::Quaternion(0, 0, 0, 0), true, false);

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
		stationStarted = true;
		stationNr++;
	}
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
		Kore::vec3 dir = math->camForward;
		math->cameraPos += dir * camVelocity;
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

		Kore::vec3 avatarDir =  math->camForward;
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

void TrainLevel::freeMemory(ALevelObject* alo) {
	environment.erase(environment.begin()+alo->render->iterator);
	delete alo;
	alo = nullptr;
	reIteratorVector();
}

void TrainLevel::reIteratorVector() {
	int i = 0;
	for (ALevelObject* object : environment) {
		object->render->iterator = i++;
	}

}

Level::ALevelObject* TrainLevel::createNewObject(String pfad, String pfad2, VertexStructure vstruct,float scale, Kore::vec3 pos, Kore::Quaternion rot) {

	ALevelObject* object = new ALevelObject(pfad, pfad2, vstruct,scale,pos,rot);
	object->initPosition = pos;
	object->render->iterator = environment.size();
	environment.emplace_back(object);
	return object;
}

Level::ALevelObject* TrainLevel::createObjectCopy(ALevelObject* object, Kore::vec3 pos, Kore::Quaternion rot) {
	ALevelObject* newobject = new ALevelObject(object, pos, rot);
	newobject->initPosition = pos;
	newobject->render->iterator = environment.size();
	environment.emplace_back(newobject);
	return newobject;
}

void TrainLevel::roomInit(Kore::Graphics4::VertexStructure environmentSructure) {
	ALevelObject* room = createNewObject("sherlock_living_room/sherlock_living_room.ogex", "sherlock_living_room/", environmentSructure, 1, Kore::vec3(4, 0, 0), Kore::Quaternion(1, 2, 1, 0));
	renderer->setLights(*room->render, renderer->environmentGraphics->lightCount, renderer->environmentGraphics->lightPosLocation);
	room->render->tag = "room";
	room->render->moveable = false;
	ALevelObject* object = createObjectCopy(room, Kore::vec3(-2, 0, 0), Kore::Quaternion(1, 2, 3, 0));
	renderer->setLights(*object->render, renderer->environmentGraphics->lightCount, renderer->environmentGraphics->lightPosLocation);

}

void TrainLevel::skyInit(Kore::Graphics4::VertexStructure environmentSructure) {

	ALevelObject* sky = createNewObject("skybox/skybox.ogex", "skybox/", environmentSructure, 1, Kore::vec3(0, 0, -75), Kore::Quaternion(3, 0, 1, 0));
	sky->render->moveable = false;
}

//Trainlenght: 1 = short = 3 waggons total; 2 = medium = 5 Waggons total; 3 = long = 10 waggons total
void TrainLevel::trainInit(Kore::Graphics4::VertexStructure environmentSructure, int trainLenght) {
	float yRot = 1.995f;
	//float xoffset = 16.8;
	float xoffset = 16.5;
	float zoffset = 0.11f;

	ALevelObject* trainBack = createNewObject("train/trainFront.ogex", "train/", environmentSructure, 1, Kore::vec3(7.2, -3, 0.08), Kore::Quaternion(3, 0, -0.005f, 0));
	trainBack->render->moveable = false;
	ALevelObject* trainMiddle1 = createNewObject("train/trainMiddle.ogex", "train/", environmentSructure, 1, Kore::vec3(7.2 + xoffset, -3, 0.08 + zoffset), Kore::Quaternion(3, 0, yRot, 0));
	trainMiddle1->render->moveable = false;

	switch (trainLenght) {
	case 1:
	{
		ALevelObject* trainFront = createObjectCopy(trainBack, Kore::vec3(trainBack->render->position.x() + xoffset * 2 + 0.3, trainBack->render->position.y(), trainBack->render->position.z() + zoffset * 2), Kore::Quaternion(3, 0, yRot, 0));
		break;
	}
	case 2:
	{
		ALevelObject* trainMiddle2 = createObjectCopy(trainMiddle1, Kore::vec3(trainMiddle1->render->position.x() + xoffset, trainMiddle1->render->position.y(), trainMiddle1->render->position.z() + zoffset), Kore::Quaternion(3, 0, yRot, 0));
		ALevelObject* trainMiddle3 = createObjectCopy(trainMiddle1, Kore::vec3(trainMiddle1->render->position.x() + xoffset * 2, trainMiddle1->render->position.y(), trainMiddle1->render->position.z() + zoffset * 2), Kore::Quaternion(3, 0, yRot, 0));
		ALevelObject* trainFront2 = createObjectCopy(trainBack, Kore::vec3(trainBack->render->position.x() + xoffset * 4 + 0.2, trainBack->render->position.y(), trainBack->render->position.z() + zoffset * 4), Kore::Quaternion(3, 0, yRot, 0));
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
		break;
	}
	default:
		break;
	}
}

void TrainLevel::groundInit(Kore::Graphics4::VertexStructure environmentSructure) {

	ALevelObject* floor = createNewObject("floor/floor.ogex", "floor/", environmentSructure, 1, Kore::vec3(-410, -3, -2.75), Kore::Quaternion(-1, 0, 0, 0));
	floor->render->tag = "floor";
	ALevelObject* lfloor = createNewObject("floor/lfloor.ogex", "floor/", environmentSructure, 1, Kore::vec3(-410, -2.19, -13), Kore::Quaternion(3, 0, 0.993f, 0));
	lfloor->render->tag = "floorr";
	ALevelObject* rfloor = createNewObject("floor/lfloor.ogex", "floor/", environmentSructure, 1, Kore::vec3(-410, -2.19, 3), Kore::Quaternion(3, 0, 0.993f, 0));
	rfloor->render->tag = "floorl";
	ALevelObject* object;

	float xoffset = 25.0f;
	float offsetZfloor = 0.175f;

	float xoffset2 = 18;
	float zoffset2 = 0.175f;
	
	for (int x = 0; x < 56; x++) {		
		if (x < 40) object = createObjectCopy(floor,Kore::vec3(floor->render->position.x() + xoffset * x, floor->render->position.y(), floor->render->position.z() + offsetZfloor * x), floor->render->rotation);
		object = createObjectCopy(rfloor, Kore::vec3(rfloor->render->position.x() + xoffset2 * x, rfloor->render->position.y(), rfloor->render->position.z() + zoffset2 * x), rfloor->render->rotation);
		object = createObjectCopy(lfloor, Kore::vec3(lfloor->render->position.x() + xoffset2 * x, lfloor->render->position.y(), lfloor->render->position.z() + zoffset2 * x), lfloor->render->rotation);		
	}
}

void TrainLevel::houseInit(Kore::Graphics4::VertexStructure environmentSructure) {
	//float xoffset = 7.7f;
	float xoffset = 13;
	float zoffset = 0.115f;
	ALevelObject* houseL = createNewObject("houseL/hausL.ogex", "houseL/", environmentSructure, 1, Kore::vec3(-410, -10, 15), Kore::Quaternion(3, 0, 1.02, 0));
	ALevelObject* houseS = createNewObject("houseS/haus.ogex", "houseS/", environmentSructure, 1, Kore::vec3(-402.3, -10, 15), Kore::Quaternion(3, 0, 1, 0));
	ALevelObject* houseM = createNewObject("houseM/hausM.ogex", "houseM/", environmentSructure, 1, Kore::vec3(-394.6, -10, 14 + zoffset*2), Kore::Quaternion(3, 0, 0, 0));
	ALevelObject* houseML = createNewObject("houseML/hausML.ogex", "houseML/", environmentSructure, 1, Kore::vec3(-386.9, -10, 14 + zoffset*3), Kore::Quaternion(3, 0, 0, 0));
	ALevelObject* object;
	int randNumb = 0;
	//float xoffset = 30.8f;
	for (int x = 0; x < 67; x++) {
		randNumb = rand() % 4;
		switch (randNumb) {
		case 0:object = createObjectCopy(houseS, Kore::vec3(houseL->render->position.x() + xoffset*x, -3, houseL->render->position.z() + zoffset * x), houseL->render->rotation);break;
		case 1:object = createObjectCopy(houseM, Kore::vec3(houseL->render->position.x() + xoffset * x, -3, houseL->render->position.z() + zoffset * x), houseL->render->rotation);break;
		case 2:object = createObjectCopy(houseML, Kore::vec3(houseL->render->position.x() + xoffset * x, -3, houseL->render->position.z() + zoffset * x), houseL->render->rotation);break;
		case 3:object = createObjectCopy(houseL, Kore::vec3(houseL->render->position.x() + xoffset * x, -3, houseL->render->position.z() + zoffset * x), houseL->render->rotation);break;
		}
		object->render->tag = "houseR";
		randNumb = rand() % 4;
		switch (randNumb) {
		case 0:object = createObjectCopy(houseS, Kore::vec3(houseL->render->position.x() + xoffset * x, -5, -houseS->render->position.z() -5 + zoffset * x), houseS->render->rotation);break;
		case 1:object = createObjectCopy(houseM, Kore::vec3(houseL->render->position.x() + xoffset * x, -5, -houseS->render->position.z() - 5 + zoffset * x), houseS->render->rotation);break;
		case 2:object = createObjectCopy(houseML, Kore::vec3(houseL->render->position.x() + xoffset * x, -5, -houseS->render->position.z() - 5 + zoffset * x), houseS->render->rotation);break;
		case 3:object = createObjectCopy(houseL, Kore::vec3(houseL->render->position.x() + xoffset * x, -5, -houseS->render->position.z() - 5 + zoffset * x), houseS->render->rotation);break;
		}
		object->render->tag = "houseL";
	}

	freeMemory(houseL);
	freeMemory(houseS);
	freeMemory(houseM);
	freeMemory(houseML);
}

void TrainLevel::airplaneInit(Kore::Graphics4::VertexStructure environmentSructure) {

	ALevelObject* airplane = createNewObject("airplane/airplane.ogex", "airplane/", environmentSructure, 1, Kore::vec3(454,50,0), Kore::Quaternion(3, 0, 1, 0));
	airplane->render->tag = "airplane";
}

void TrainLevel::carInit(Kore::Graphics4::VertexStructure environmentSructure) {

	ALevelObject* car = createNewObject("cars/car.ogex", "cars/", environmentSructure, 1, Kore::vec3(78, -2, 6), Kore::Quaternion(3, 0, 1, 0));
	car->render->tag = "car";
	ALevelObject* object = createObjectCopy(car, Kore::vec3(car->render->position.x()*2, car->render->position.y(), car->render->position.z()-15), Kore::Quaternion(3, 0, 3, 0));
	object->render->tag = "car1";

}

void TrainLevel::tunnelInit(Kore::Graphics4::VertexStructure environmentSructure) {
	
	float yRot = 1.996f;
	float xoffset = 15.0f;
	float zoffset = 0.1f;
	float yoffset = 0.023;
	ALevelObject* tunnel = createNewObject("tunnel/tunnelNew.ogex", "tunnel/", environmentSructure, 1, Kore::vec3(454 - xoffset*8, -10, 3 - zoffset*8), Kore::Quaternion(3, 0, 0, 0));
	tunnel->render->tag = "tunnelS";
	ALevelObject* object;
	for (int i = 1; i < 9; i++) {
		object = createObjectCopy(tunnel, Kore::vec3(tunnel->render->position.x() + xoffset * i, tunnel->render->position.y() + yoffset * i, tunnel->render->position.z() + zoffset * i), Kore::Quaternion(3, 0, yRot, 0));
	}
}

void TrainLevel::t()
{	
	objects[0]->render->position.x() += offsets;
	Kore::log(Kore::Info, "X = %f", objects[0]->render->position.x());
}
void TrainLevel::g()
{
	objects[0]->render->position.x() -= offsets;
	Kore::log(Kore::Info, "X = %f", objects[0]->render->position.x());
}
void TrainLevel::f()
{
	objects[0]->render->position.z() += offsets;
	Kore::log(Kore::Info, "Z = %f", objects[0]->render->position.z());
}
void TrainLevel::h()
{
	objects[0]->render->position.z() -= offsets;
	Kore::log(Kore::Info, "Z = %f", objects[0]->render->position.z());
}
void TrainLevel::x()
{
	avatar->entity->calibrated = true;
	/*
	for (ALevelObject* object : environment) {
		if (object->render->tag == "tunnelS") {
			object->render->moveable = true;
			object->render->activated = true;
		}
	}*/
}
void TrainLevel::v()
{
	objects[0]->render->rotation.x += offsets;
	Kore::log(Kore::Info, "Xr = %f", objects[0]->render->rotation.x);
}
void TrainLevel::b()
{
	objects[0]->render->rotation.y += offsets;
	Kore::log(Kore::Info, "Yr = %f", objects[0]->render->rotation.y);
}
void TrainLevel::n()
{
	objects[0]->render->rotation.z += offsets;
	Kore::log(Kore::Info, "Zr = %f", objects[0]->render->rotation.z);
}
