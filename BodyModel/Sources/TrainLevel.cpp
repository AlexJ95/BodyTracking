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
	updateBuilding(deltaT,20);
	updateFPS(deltaT);
	Level::update(deltaT);

	avatar->entity->position = locToGlob.Invert() * Kore::vec4(math->cameraPos.x(), math->cameraPos.y(), math->cameraPos.z(), 1.0);
	spawn(deltaT);
	checkEnemyCollision();
	checkHittingAvatar();
	checkHittingEnemy();
}

void TrainLevel::updateFPS(double deltaT) {
	fps++;
	time += deltaT;
	if (time > 1.0f)
	{
		Kore::log(Kore::Info, "%d", fps);

		fps = 0;
		time = 0;
	}
}


void TrainLevel::updateBuilding(double deltaT,double speed) {	
	if (!form->isFormShown())
	{
		for (ALevelObject* object : environment)
			if (object->moveable)
				if (object->render->position.x() > -78)
				{

					if (object->render->tag == "car1")
						object->render->position.x() -= deltaT * speed * 2;
					else if (object->render->tag == "airplane")
						object->render->position.x() -= deltaT * speed * 3;
					else object->render->position.x() -= deltaT * speed;
				}
				else object->render->position.x() = 78;
	}
}

void TrainLevel::init() {
	controlsSetup();
	audioSetup();
	graphicsSetup();
	Level::init();
	form = new MainForm();
	offsets = 1;
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
	trainInit(environmentSructure);

	//Load Avatar
	avatar = new TheAvatar("avatar/avatar_male.ogex", "avatar/", entitySructure, 1.0f, Kore::vec3(0, 0, 0), Kore::Quaternion(0, 0, 0, 0), true, true);

	//create Pool od entitys
	for (int i = 0; i < poolSize; i++)
	{
		//Load Enemy
		createEnemy(new AnAnimatedEntity("enemy/avatar_male.ogex", "enemy/", entitySructure, 1.0f, Kore::vec3(0, 0, -1000), Kore::Quaternion(0, 0, 0, 0)), Kore::vec3(0, 0, -1000), Kore::Quaternion(0, 0, 0, 0));
	}

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
void TrainLevel::spawn(double deltaT)
{

	if (countDown > maxWaitintTime | (levelStarted & countDown > maxWaitintTime / 10.0))
	{
		levelStarted = false;
		for (NonPlayerCharacter* enemy : enemies)
		{
			if (!(enemy->entity->activated))
			{
				countDown = 0.0;
				enemy->ai->spawn();
				break;
			}
		}
	}

	countDown += deltaT;
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

		if (leftFootPos.z() > hittingHeight & dir_L.getLength() < hittingRadius & !enemy->entity->attackingSucceed)
		{
			enemy->entity->attackingSucceed = true;
			avatar->entity->hit();
		}
		else if (rightFootPos.z() > hittingHeight & dir_R.getLength() < hittingRadius & !enemy->entity->attackingSucceed)
		{
			enemy->entity->attackingSucceed = true;
			avatar->entity->hit();
		}
		else if (dir_L.getLength() > hittingRadius & dir_R.getLength() > hittingRadius & enemy->entity->attackingSucceed)
		{
			enemy->entity->attackingSucceed = false;
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

		if (avatar->entity->endEffector[leftFoot]->getDesPosition().z() > hittingHeight & dir_L.getLength() < hittingRadius & !avatar->entity->attackingSucceed)
		{
			avatar->entity->attackingSucceed = true;
			enemy->entity->hit();
		}
		else if (avatar->entity->endEffector[rightFoot]->getDesPosition().z() > hittingHeight & dir_R.getLength() < hittingRadius & !avatar->entity->attackingSucceed)
		{
			avatar->entity->attackingSucceed = true;
			enemy->entity->hit();
		}
		else if (dir_L.getLength() > hittingRadius & dir_R.getLength() > hittingRadius & avatar->entity->attackingSucceed)
		{
			avatar->entity->attackingSucceed = false;
		}
	}
}


void TrainLevel::checkEnemyCollision()
{
	for (int i=0; i< poolSize; i++)
	{
		if (enemies[i]->entity->activated)
		{
			for (int k = 0; k < poolSize & i != k; k++)
			{
				enemies[i]->ai->checkColision(enemies[k]->entity->position);
			}
		}
	}
}
///////////////////////////////////////////////////////////////////

void TrainLevel::createEnemy(AnAnimatedEntity* reference, Kore::vec3 position, Kore::Quaternion rotation)
{
	NonPlayerCharacter* enemy = new NonPlayerCharacter(reference, position, rotation);
	enemy->ai = new CyborgAI(enemy->entity, animator);
	enemies.emplace_back(enemy);
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

void TrainLevel::groundInit(Kore::Graphics4::VertexStructure environmentSructure) {

	ALevelObject* floor = createNewObject("floor/floor.ogex", "floor/", environmentSructure, 1, Kore::vec3(-26, -3, 0.0), Kore::Quaternion(-1, 0, 0, 0));

	ALevelObject* hfloor = createNewObject("floor/hfloor.ogex", "floor/", environmentSructure, 1, Kore::vec3(-27.8, -2, 9.4), Kore::Quaternion(3, 0, 1, 0));
	ALevelObject* object = createObjectCopy(hfloor, Kore::vec3(hfloor->render->position.x(), hfloor->render->position.y(), -hfloor->render->position.z()), hfloor->render->rotation);

	ALevelObject* lfloor = createNewObject("floor/lfloor.ogex", "floor/", environmentSructure, 1, Kore::vec3(-27.8, -2, 5.4), Kore::Quaternion(3, 0, 1, 0));
	object = createObjectCopy(lfloor, Kore::vec3(lfloor->render->position.x(), lfloor->render->position.y(), -lfloor->render->position.z()), lfloor->render->rotation);

	

	float xoffset = 26.0f;
	float xoffset2 = 20;
	float zoffset = 0.0f;
	for (int i = 0; i < 7; i++) {
		if(i < 5)
		object = createObjectCopy(floor, Kore::vec3(floor->render->position.x()+xoffset, floor->render->position.y(), floor->render->position.z()+zoffset), floor->render->rotation);

		object = createObjectCopy(hfloor, Kore::vec3(hfloor->render->position.x() + xoffset2, hfloor->render->position.y(), hfloor->render->position.z()), hfloor->render->rotation);
		object = createObjectCopy(hfloor, Kore::vec3(hfloor->render->position.x() + xoffset2, hfloor->render->position.y(), -hfloor->render->position.z()), hfloor->render->rotation);

		object = createObjectCopy(lfloor, Kore::vec3(lfloor->render->position.x() + xoffset2, lfloor->render->position.y(), lfloor->render->position.z()), lfloor->render->rotation);
		object = createObjectCopy(lfloor, Kore::vec3(lfloor->render->position.x() + xoffset2, lfloor->render->position.y(), -lfloor->render->position.z()), lfloor->render->rotation);

		xoffset += 26.0f;
		xoffset2 += 20;
	}
}

void TrainLevel::houseInit(Kore::Graphics4::VertexStructure environmentSructure) {
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
}

void TrainLevel::carInit(Kore::Graphics4::VertexStructure environmentSructure) {

	ALevelObject* car = createNewObject("cars/car.ogex", "cars/", environmentSructure, 1, Kore::vec3(78, -2, 6), Kore::Quaternion(3, 0, 1, 0));
	car->render->tag = "car";
	ALevelObject* object = createObjectCopy(car, Kore::vec3(car->render->position.x(), car->render->position.y(), -car->render->position.z()), car->render->rotation);
	object->render->rotation.z = 3;
	object->render->tag = "car1";

}

void TrainLevel::tunnelInit(Kore::Graphics4::VertexStructure environmentSructure) {
	
	ALevelObject* tunnel = createNewObject("tunnel/tunnelNew.ogex", "tunnel/", environmentSructure, 1, Kore::vec3(0, 0, 0), Kore::Quaternion(3, 0, 0, 0));
	objects[0] = tunnel;
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
