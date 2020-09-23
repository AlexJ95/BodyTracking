#include "TrainLevel.h"
#include "MainForm.h"

void TrainLevel::init() {
	animator = new Animator();

	controlsSetup();
	audioSetup();
	graphicsSetup();
	Level::init();
	form = new MainForm();
	offsets = 1;
	gameStart = false;
	tunnelCounter = 8;
}
void TrainLevel::controlsSetup()
{
	input = input->getInstance();
}
void TrainLevel::audioSetup()
{
	Kore::Sound* sound = new Kore::Sound("sound/train.wav");
	sound->setVolume(0.05f);
	sound->length = 4.8f;
	audio = audio->getInstanceAndAppend({
		{"traindrivingsound", sound}
		});
}
void TrainLevel::graphicsSetup() {
	//Load Shaders
	renderer->loadEnvironmentShader("shader_basic_shading.vert", "shader_basic_shading.frag");
	renderer->loadEntityShader("shader.vert", "shader.frag");

	//Set graphics variables
	const Kore::Graphics4::VertexStructure& entitySructure = renderer->entityGraphics->structure;
	const Kore::Graphics4::VertexStructure& environmentSructure = renderer->environmentGraphics->structure;


	//Load Avatar
	avatar = new TheAvatar("avatar/avatar_male.ogex", "avatar/", entitySructure, 1.0f, Kore::vec3(0, 0, 0), Kore::Quaternion(0, 0, 0, 0), true, false);

	//Load Enemy
	createEnemy(entitySructure);


	//Environment

	//Load Calibration
	roomInit(environmentSructure);

	//Load Skybox
	skyInit(environmentSructure);

	//Load Train
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

	//Load Signs
	//signInit(environmentSructure);
}

// updates
void TrainLevel::update(double deltaT){

	freeMemory();
	//updateFPS(deltaT);
	//write level-specific runtime logic here
	Level::update(deltaT);
	
	if (!avatar->entity->calibrated)
		runCalibrationRoom();
	else if (!loaded)
		loadTrainLevel();

	//Kore::vec3 pos = math->getCameraPos();
	//avatar->entity->position = locToGlob.Invert() * Kore::vec4(pos.x(), pos.y(), pos.z(), 1.0);

	if (gameStart) {
		updateBuilding(deltaT, 20);	
		gamePlay(deltaT);
	}
}

void TrainLevel::gamePlay(double deltaT) {

	updateAudio(deltaT);

	if (avatar->entity->position.y() < -16.0f * currentCarriage && !enemyExist)
		loadEnemies(deltaT, currentCarriage++);
	if (form->gameStarted() && enemySpawn)
	{
		updatePoints();
		checkStation(deltaT);
		checkEnemyCollision();
		checkHittingAvatar();
		checkHittingEnemy();
		checkingMoving();
	}
	if (!enemyExist && currentCarriage == 11)
		loadEnding();
}

void TrainLevel::updateBuilding(double deltaT, double speed)
{
	if (!form->isFormShown())
	{
		for (ALevelObject* object : environment)
			if (object->object->moveable && object->object->activated)
				if (object->object->position.x() > -420)
				{

					if (object->object->tag == "car") {
						object->object->position.x() -= deltaT * speed * 0.75f;
						if (object->object->position.z() > 6.5f)
							object->object->position.z() -= deltaT * 0.02f;
						else object->object->position.z() += deltaT * 0.001f;
					}
					else if (object->object->tag == "car1") {
						object->object->position.x() -= deltaT * speed * 2.0f;
						object->object->position.z() -= deltaT * speed * 0.02f;
					}
					else if (object->object->tag == "tunnelS") {
						object->object->position.x() -= deltaT * speed * 3.0f;
						object->object->position.z() -= deltaT * speed * 0.02f;
						if (object->object->position.x() < 0)
							enemyExist = false;
					}
					else if (object->object->tag == "airplane") {
						object->object->position.x() -= deltaT * speed * 6;
						airPlanePos = object->object->position;
					}
					else if (object->object->tag == "signl" || object->object->tag == "signr") {
						object->object->position.x() -= deltaT * speed * 2;
						object->object->position.z() -= deltaT * speed * 0.0075f * 2;
						if (object->object->position.x() < 0)
							enemyExist = false;
					}
					else {
						object->object->position.x() -= deltaT * speed;
						object->object->position.z() -= deltaT * speed * 0.0075f;
					}
				}
				else {
					if (object->object->tag == "floor") {
						setPosition(object, 580, object->object->position.y(), 4.25f);
					}
					else if (object->object->tag == "floorl") {
						setPosition(object, 580, object->object->position.y(), -3.3f);
					}
					else if (object->object->tag == "floorr") {
						setPosition(object, 580, object->object->position.y(), 12.7f);
					}
					else if (object->object->tag == "houseR") {
						setPosition(object, 454, object->object->position.y(), 19.21);
					}
					else if (object->object->tag == "houseL") {
						setPosition(object, 454, object->object->position.y(), -12.0f);
					}
					else if (object->object->tag == "car1") {
						setPosition(object, 454, object->object->position.y(), -2.6f);
					}
					else if (object->object->tag == "car") {
						setPosition(object, 454, object->object->position.y(), 10.3f);
					}
					else if (object->object->tag == "airplane") {
						object->object->activated = false;
						object->object->position = object->initPosition;
					}
					else if (object->object->tag == "signl") {
						object->object->activated = false;
						object->object->position = object->initPosition;
					}
					else if (object->object->tag == "signr") {
						object->object->activated = false;
						object->object->position = object->initPosition;
					}
					else if (object->object->tag == "tunnelS") {
						object->object->activated = false;
						object->object->position = object->initPosition;
					}
					else setPosition(object, 454, object->object->position.y(), object->object->position.z());
				}
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

void TrainLevel::updatePoints() {
		switch (currentEnemy) {
		case 0:
			if ((avatar->entity->lastMovement == 5|| avatar->entity->lastMovement == 1 || avatar->entity->lastMovement == 2) && true)
				form->addHighScore(2);
			break;
		case 1:
			if ((avatar->entity->lastMovement == 5 || avatar->entity->lastMovement == 1) && true)
				form->addHighScore(2);
			else if((avatar->entity->lastMovement == 2) && true)
				form->addHighScore(1);
			break;

		case 2:
			if ((avatar->entity->lastMovement == 5 || avatar->entity->lastMovement == 2) && true)
				form->addHighScore(2);
			else if ((avatar->entity->lastMovement == 1) && true)
				form->addHighScore(1);
			break;
		case 3:
			if ((avatar->entity->lastMovement == 1 || avatar->entity->lastMovement == 2) && true)
				form->addHighScore(2);
			else if ((avatar->entity->lastMovement == 5) && true)
				form->addHighScore(1);
			break;
		case 4:
			if ((avatar->entity->lastMovement == 1) && true)
				form->addHighScore(2);
			else if ((avatar->entity->lastMovement == 5 || avatar->entity->lastMovement == 1) && true)
				form->addHighScore(1);
			break;

		}
}

void TrainLevel::updateAudio(double deltaT) {
		audio->play("traindrivingsound",deltaT);
}

// Scene change
void TrainLevel::runCalibrationRoom() {
	renderer->mirror = true;
	for (ALevelObject* object : environment)
		if (object->object->tag == "room")
			object->object->activated = true;
}

void TrainLevel::deleteRoom() {
	for (ALevelObject* object : environment)
		if (object->object->tag == "room")
		{
			avatar->entity->renderAxisForEndEffectors = false;
			avatar->entity->renderTrackerAndControllers = false;
			object->object->activated = false;
			object->del = true;
		}
}
void TrainLevel::loadTrainLevel() {
		form->displayLoading();
		gameStart = true;
		deleteRoom();

		for (ALevelObject* object : environment) {
			if (object->object->tag != "tunnelS" && object->object->tag != "airplane" && object->object->tag != "signr" && object->object->tag != "tunnell" && object->object->tag != "signl" && object->object->tag != "signr" && object->object->tag != "room")
				object->object->activated = true;
		}
		renderer->mirror = false;
}

// Load Assets
void TrainLevel::loadEnemies(float deltaT, int carriage) {

	loaded = true;
	Kore::log(Kore::Info, "make enemy %d", carriage);
	switch (carriage) {
	case 1: loadEnemy(0, avatar->entity->position);	break; 
	case 2: loadSign(); break;
	case 3: loadEnemy(1, avatar->entity->position);	break;
	case 4: loadTunnel(carriage);	break;
	case 5: loadEnemy(2, avatar->entity->position);	break;
	case 6: loadSign();  break;
	case 7: loadEnemy(3, avatar->entity->position);	break;
	case 8: loadTunnel(carriage);	break;
	case 9: loadEnemy(4, avatar->entity->position);	break;
	default: break;
	}

	
}

//set the various game elements in the scene
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
		if (object->object->tag == "tunnelS") {
			object->object->activated = true;
			
		}
	}
	enemyExist = true;
}
void TrainLevel::loadAirplane() {
	Kore::log(Kore::Info, "call the airplane");
	for (ALevelObject* object : environment) {
		if (object->object->tag == "airplane") {
			object->object->activated = true;
		}
	}
}
void TrainLevel::loadSign() {
	Kore::log(Kore::Info, "make a sigh");
	for (ALevelObject* object : environment) {
		if (object->object->tag == "signl"|| object->object->tag == "signr") {
			object->object->activated = true;
		}
	}
	enemyExist = true;
}
void TrainLevel::loadEnemy(int range,Kore::vec3 pos) {
	Kore::log(Kore::Info, "call the enemy");
	loadAirplane();
	enemySpawn = true;
	currentEnemy = range;
}
void TrainLevel::loadEnding() {
	Kore::log(Kore::Info, "Game End");
	form->displayEnd();	
	gameStart = false;
}

// Help function
void TrainLevel::setPosition(ALevelObject* alo ,float x, float y, float z) {
	alo->object->position = Kore::vec3(x, y, z);
}

//checkStation check if an station with the enemies was completed. An station is complete if all enemies are beated. 
void TrainLevel::checkStation(double deltaT){
	if (!stationComplete & currentEnemyCount < maxEnemyCount)// & airPlanePos.x() <= Kore::abs(avatar->entity->position.y() - stationLength))
	{
		spawn(deltaT);//, AirPlanePos);		
	}
	if (StateMachineAI::beatedEnemyCount >= maxEnemyCount & !stationComplete)
	{
		stationComplete = true;
		enemyExist = false;
		enemySpawn = false;
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
	countDown += deltaT;
}

//Activate the enemies and insert in the scene. The startposition bases on the position of the last beated enemy and the stationLength. 
void TrainLevel::spawn(double deltaT){
	if (countDown > maxWaitingTime | (stationStarted & countDown > maxWaitingTime / 100.0))
	{
		stationStarted = false;
		int i = 0;
		for (NonPlayerCharacter* enemy : enemies)
		{
			if (i == currentEnemy) {
				
				if (!(enemy->entity->beated) & !(enemy->entity->activated))
				{
					enemyExist = true;
					Kore::vec3 globAirPlanePos = locToGlob * Kore::vec4(airPlanePos.x(), airPlanePos.y(), airPlanePos.z(), 1.0);
					float randomX_Pos = (float)(rand() % 2 - 1);
					if (randomX_Pos == 0.0)
						randomX_Pos += 0.1;
					enemy->entity->position = Kore::vec3(randomX_Pos, (avatar->entity->position.y() - stationLength), 0.0f);
					enemy->entity->rotation = Kore::Quaternion(Kore::vec3(0, 0, 1), Kore::pi);
					countDown = 0.0;
					enemy->ai->spawn();
					currentEnemyCount++;
					
				}
			}
			i++;
		}
	}
}

//This an simple implementation of the collision detection. We check wehther an endeffektor (left/right hand or left/right foot) enters in the bounding box. If it is about a certain height, the method hit() will be called. 
void TrainLevel::checkHittingAvatar(){
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

//Show the next attack in the gui based on the color of the enemy.
void TrainLevel::showAttackInUI(string colorTag){
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

void TrainLevel::checkingMoving(){
	if (avatar->entity->lastMovement == Avatar::Jogging && !avatar->entity->movementExpired)
	{
		Kore::vec3 dir = camForward;
		cameraPos += dir * camVelocity;
	}
}

//We use here the same prenceple as in checkHittingAvatar().
//In this case we increase the highscore if the motion recognition recognizes any attack-motion of the player. 
//If the right motion was recohnized the player get an bonus point.
void TrainLevel::checkHittingEnemy(){
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

//checking the collision between the enemies
void TrainLevel::checkEnemyCollision(){
	for (int i=0; i< maxEnemyCount; i++)
	{
		if (enemies[i]->entity->activated)
		{
			for (int k = 0; k < maxEnemyCount && i != k; k++)
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

// For Delete
void TrainLevel::freeMemory() {
	for (ALevelObject* object : environment) {
		if (object->del) {
			environment.erase(environment.begin() + object->object->iterator);
			delete object;
			object = nullptr;
			break;
		}
	}
	reIteratorVector();
}
void TrainLevel::reIteratorVector() {
	int i = 0;
	for (ALevelObject* object : environment) {
		object->object->iterator = i++;
	}

}

// Graphic Init 
void TrainLevel::createEnemy(Kore::Graphics4::VertexStructure entitySructure) {
	AnAnimatedEntity* reference;
	for (int i = 0; i < 5; i++) {
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

Level::ALevelObject* TrainLevel::createNewObject(String pfad, String pfad2, VertexStructure vstruct, float scale, Kore::vec3 pos, Kore::Quaternion rot) {
	ALevelObject* object = new ALevelObject(pfad, pfad2, vstruct, scale, pos, rot);
	object->initPosition = pos;
	object->object->iterator = environment.size();
	environment.emplace_back(object);
	return object;
}
Level::ALevelObject* TrainLevel::createObjectCopy(ALevelObject* object, Kore::vec3 pos, Kore::Quaternion rot) {
	ALevelObject* newobject = new ALevelObject(object, pos, rot);
	newobject->initPosition = pos;
	newobject->object->iterator = environment.size();
	environment.emplace_back(newobject);
	return newobject;
}

void TrainLevel::roomInit(Kore::Graphics4::VertexStructure environmentSructure) {
	ALevelObject* room = createNewObject("sherlock_living_room/sherlock_living_room.ogex", "sherlock_living_room/", environmentSructure, 1, Kore::vec3(0, 0, 0), Kore::Quaternion(-1, 0, 1, 0));
	renderer->setLights(*room->object, renderer->environmentGraphics->lightCount, renderer->environmentGraphics->lightPosLocation);
	room->object->tag = "room";
	room->object->moveable = false;
	Kore::Quaternion livingRoomRot = Kore::Quaternion(0, 0, 0, 1);
	livingRoomRot.rotate(Kore::Quaternion(Kore::vec3(1, 0, 0), -Kore::pi / 2.0));
	livingRoomRot.rotate(Kore::Quaternion(Kore::vec3(0, 0, 1), Kore::pi / 2.0));
	livingRoomRot.rotate(Kore::Quaternion(Kore::vec3(0, 0, 1), Kore::pi));
	Kore::mat4 mirrorMatrix = Kore::mat4::Identity();
	mirrorMatrix.Set(2, 2, -1);
	Kore::vec3 mirrorOver(6.057f, 0.0f, 0.04f);
	room->object->meshObject->Mmirror = mirrorMatrix * Kore::mat4::Translation(mirrorOver.x(), mirrorOver.y(), mirrorOver.z()) * livingRoomRot.matrix().Transpose();
	//ALevelObject* object = createObjectCopy(room, Kore::vec3(-2, 0, 0), Kore::Quaternion(1, 2, 3, 0));
	//renderer->setLights(*object->object, renderer->environmentGraphics->lightCount, renderer->environmentGraphics->lightPosLocation);
}
void TrainLevel::skyInit(Kore::Graphics4::VertexStructure environmentSructure) {
	ALevelObject* sky = createNewObject("skybox/skybox.ogex", "skybox/", environmentSructure, 1, Kore::vec3(0, 0, -75), Kore::Quaternion(3, 0, 1, 0));
	sky->object->moveable = false;
}
//Trainlenght: 1 = short = 3 waggons total; 2 = medium = 5 Waggons total; 3 = long = 10 waggons total
void TrainLevel::trainInit(Kore::Graphics4::VertexStructure environmentSructure, int trainLenght) {
	float yRot = 1.995f;
	float xoffset = 16.5;
	float zoffset = 0.11f;

	ALevelObject* trainBack = createNewObject("train/trainFront.ogex", "train/", environmentSructure, 1, Kore::vec3(7.2, -3, 0.08), Kore::Quaternion(3, 0, -0.005f, 0));
	trainBack->object->moveable = false;
	ALevelObject* trainMiddle1 = createNewObject("train/trainMiddle.ogex", "train/", environmentSructure, 1, Kore::vec3(7.2 + xoffset, -3, 0.08 + zoffset), Kore::Quaternion(3, 0, yRot, 0));
	trainMiddle1->object->moveable = false;
	ALevelObject* trains;

	switch (trainLenght) {
	case 1:	{
		trains = createObjectCopy(trainBack, Kore::vec3(trainBack->object->position.x() + xoffset * 2 + 0.3, trainBack->object->position.y(), trainBack->object->position.z() + zoffset * 2), Kore::Quaternion(3, 0, yRot, 0));
		break;
	}
	case 2:	{
		for (int i = 1; i < 3; i++)
			trains = createObjectCopy(trainMiddle1, Kore::vec3(trainMiddle1->object->position.x() + xoffset * i, trainMiddle1->object->position.y(), trainMiddle1->object->position.z() + zoffset * i), Kore::Quaternion(3, 0, yRot, 0));
		trains = createObjectCopy(trainBack, Kore::vec3(trainBack->object->position.x() + xoffset * 4 + 0.2, trainBack->object->position.y(), trainBack->object->position.z() + zoffset * 4), Kore::Quaternion(3, 0, yRot, 0));
		break;
	}
	case 3:	{
		for (int i = 1; i < 8; i++) 
		trains = createObjectCopy(trainMiddle1, Kore::vec3(trainMiddle1->object->position.x() + xoffset * i, trainMiddle1->object->position.y(), trainMiddle1->object->position.z() + zoffset * i), Kore::Quaternion(3, 0, yRot, 0));
		trains = createObjectCopy(trainBack, Kore::vec3(trainBack->object->position.x() + xoffset * 9 + 0.2, trainBack->object->position.y(), trainBack->object->position.z() + zoffset * 9), Kore::Quaternion(3, 0, yRot, 0));
		break;
	}
	default:		break;
	}
}
void TrainLevel::groundInit(Kore::Graphics4::VertexStructure environmentSructure) {
	ALevelObject* floor = createNewObject("floor/floor.ogex", "floor/", environmentSructure, 1, Kore::vec3(-410, -3, -2.75), Kore::Quaternion(-1, 0, 0, 0));
	floor->object->tag = "floor";
	ALevelObject* lfloor = createNewObject("floor/lfloor.ogex", "floor/", environmentSructure, 1, Kore::vec3(-410, -2.19, -13), Kore::Quaternion(3, 0, 0.993f, 0));
	lfloor->object->tag = "floorr";
	ALevelObject* rfloor = createNewObject("floor/lfloor.ogex", "floor/", environmentSructure, 1, Kore::vec3(-410, -2.19, 3), Kore::Quaternion(3, 0, 0.993f, 0));
	rfloor->object->tag = "floorl";
	ALevelObject* object;

	float xoffset = 25.0f;
	float offsetZfloor = 0.175f;

	float xoffset2 = 18;
	float zoffset2 = 0.175f;
	
	for (int x = 0; x < 56; x++) {		
		if (x < 40) object = createObjectCopy(floor,Kore::vec3(floor->object->position.x() + xoffset * x, floor->object->position.y(), floor->object->position.z() + offsetZfloor * x), floor->object->rotation);
		object = createObjectCopy(rfloor, Kore::vec3(rfloor->object->position.x() + xoffset2 * x, rfloor->object->position.y(), rfloor->object->position.z() + zoffset2 * x), rfloor->object->rotation);
		object = createObjectCopy(lfloor, Kore::vec3(lfloor->object->position.x() + xoffset2 * x, lfloor->object->position.y(), lfloor->object->position.z() + zoffset2 * x), lfloor->object->rotation);		
	}
}
void TrainLevel::houseInit(Kore::Graphics4::VertexStructure environmentSructure) {
	float xoffset = 13;
	float zoffset = 0.115f;
	ALevelObject* houseL = createNewObject("houseL/hausL.ogex", "houseL/", environmentSructure, 1, Kore::vec3(-410, -10, 11), Kore::Quaternion(3, 0, 1.02, 0));
	ALevelObject* houseS = createNewObject("houseS/haus.ogex", "houseS/", environmentSructure, 1, Kore::vec3(-402.3, -10, 15), Kore::Quaternion(3, 0, 1, 0));
	ALevelObject* houseM = createNewObject("houseM/hausM.ogex", "houseM/", environmentSructure, 1, Kore::vec3(-394.6, -10, 14 + zoffset*2), Kore::Quaternion(3, 0, 0, 0));
	ALevelObject* houseML = createNewObject("houseML/hausML.ogex", "houseML/", environmentSructure, 1, Kore::vec3(-386.9, -10, 14 + zoffset*3), Kore::Quaternion(3, 0, 0, 0));
	ALevelObject* object;
	int randNumb = 0;
	for (int x = 0; x < 67; x++) {
		randNumb = rand() % 4;
		switch (randNumb) {
		case 0:object = createObjectCopy(houseS, Kore::vec3(houseL->object->position.x() + xoffset*x, -3, houseL->object->position.z() + zoffset * x), houseL->object->rotation);break;
		case 1:object = createObjectCopy(houseM, Kore::vec3(houseL->object->position.x() + xoffset * x, -3, houseL->object->position.z() + zoffset * x), houseL->object->rotation);break;
		case 2:object = createObjectCopy(houseML, Kore::vec3(houseL->object->position.x() + xoffset * x, -3, houseL->object->position.z() + zoffset * x), houseL->object->rotation);break;
		case 3:object = createObjectCopy(houseL, Kore::vec3(houseL->object->position.x() + xoffset * x, -3, houseL->object->position.z() + zoffset * x), houseL->object->rotation);break;
		}
		object->object->tag = "houseR";
		randNumb = rand() % 4;
		switch (randNumb) {
		case 0:object = createObjectCopy(houseS, Kore::vec3(houseL->object->position.x() + xoffset * x, -5, -houseS->object->position.z() -5 + zoffset * x), houseS->object->rotation);break;
		case 1:object = createObjectCopy(houseM, Kore::vec3(houseL->object->position.x() + xoffset * x, -5, -houseS->object->position.z() - 5 + zoffset * x), houseS->object->rotation);break;
		case 2:object = createObjectCopy(houseML, Kore::vec3(houseL->object->position.x() + xoffset * x, -5, -houseS->object->position.z() - 5 + zoffset * x), houseS->object->rotation);break;
		case 3:object = createObjectCopy(houseL, Kore::vec3(houseL->object->position.x() + xoffset * x, -5, -houseS->object->position.z() - 5 + zoffset * x), houseS->object->rotation);break;
		}
		object->object->tag = "houseL";
	}

	houseL->del = true;
	houseS->del = true;
	houseM->del = true;
	houseML->del = true;
}
void TrainLevel::airplaneInit(Kore::Graphics4::VertexStructure environmentSructure) {
	ALevelObject* airplane = createNewObject("airplane/airplane.ogex", "airplane/", environmentSructure, 1, Kore::vec3(454,50,0), Kore::Quaternion(3, 0, 1, 0));
	airplane->object->tag = "airplane";
}
void TrainLevel::carInit(Kore::Graphics4::VertexStructure environmentSructure) {
	ALevelObject* car = createNewObject("cars/car.ogex", "cars/", environmentSructure, 1, Kore::vec3(78, -2, 6), Kore::Quaternion(3, 0, 1, 0));
	car->object->tag = "car";
	ALevelObject* object = createObjectCopy(car, Kore::vec3(car->object->position.x()*2, car->object->position.y(), car->object->position.z()-15), Kore::Quaternion(3, 0, 3, 0));
	object->object->tag = "car1";
}
void TrainLevel::tunnelInit(Kore::Graphics4::VertexStructure environmentSructure) {	
	float yRot = 1.996f;
	float xoffset = 15.0f;
	float zoffset = 0.1f;
	float yoffset = 0.023;
	ALevelObject* tunnel = createNewObject("tunnel/tunnelNew.ogex", "tunnel/", environmentSructure, 1, Kore::vec3(454 - xoffset*8, -11, 3 - zoffset*8), Kore::Quaternion(3, 0, 0, 0));
	tunnel->object->tag = "tunnelS";
	ALevelObject* object;
	for (int i = 1; i < 9; i++) {
		object = createObjectCopy(tunnel, Kore::vec3(tunnel->object->position.x() + xoffset * i, tunnel->object->position.y() + yoffset * i, tunnel->object->position.z() + zoffset * i), Kore::Quaternion(3, 0, yRot, 0));
	}
}
void TrainLevel::signInit(Kore::Graphics4::VertexStructure environmentSructure) {
	ALevelObject* signl = createNewObject("sign/sign3.ogex", "sign/", environmentSructure, 0.01, Kore::vec3(400, -2.18, 138.25), Kore::Quaternion(0, 2, 0, 0));
	signl->object->tag = "signl";
	ALevelObject* signr = createNewObject("sign/sign3.ogex", "sign/", environmentSructure, -0.01, Kore::vec3(415, -2.18, 140.75), Kore::Quaternion(2, 0, 0, 0));
	signr->object->tag = "signr";
}

//Buttons
void TrainLevel::x(){avatar->entity->calibrated = true;}

