#include "TrainLevel.h"
#include "MainForm.h"

void TrainLevel::update(double deltaT)
{
	//code level-specific runtime logic here
	Level::update(deltaT);

	//environment.at(1)->position.x() -= 0.1f;

	environment.at(1)->render->position.x() -= 0.1f;
	spawn(deltaT);
	checkEnemyCollision();
	
}

void TrainLevel::init() {
	controlsSetup();
	audioSetup();
	graphicsSetup();
	Level::init();
	form = new MainForm();
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
	Kore::Graphics4::ConstantLocation lightCount = renderer->environmentGraphics->lightCount;
	Kore::Graphics4::ConstantLocation lightPosLocation = renderer->environmentGraphics->lightPosLocation;

	//Load Avatar
	avatar = new TheAvatar("avatar/avatar_male.ogex", "avatar/", entitySructure, 1.0f, Kore::vec3(0, 0, 0), Kore::Quaternion(0, 0, 0, 0), true, true);

	//create Pool od entitys
	for (int i = 0; i < poolSize; i++)
	{
		//Load Enemy
		createEnemy(new AnAnimatedEntity("avatar/avatar_male.ogex", "avatar/", entitySructure, 1.0f, Kore::vec3(0, 0, -1000), Kore::Quaternion(0, 0, 0, 0)), Kore::vec3(0, 0, -1000), Kore::Quaternion(0, 0, 0, 0));
	}


	//Load Environment
	/*ALevelObject* train = new ALevelObject("train/train9.ogex", "train/", environmentSructure, 1);
	Kore::Quaternion rotation = Kore::Quaternion(0, 0, 0, 1);
	rotation.rotate(Kore::Quaternion(Kore::vec3(1, 0, 0), -Kore::pi / 2.0));
	rotation.rotate(Kore::Quaternion(Kore::vec3(0, 0, 1), Kore::pi / 2.0));
	train->render->M = Kore::mat4::RotationY(-0.0075) * Kore::mat4::Translation(0, -3, 0) * rotation.matrix().Transpose();
	renderer->setLights(*(train->render), lightCount, lightPosLocation);*/
	ALevelObject* floor = createNewObject("floor/floor.ogex", "floor/", environmentSructure, 1, Kore::vec3(0, -3, 0), Kore::Quaternion(-1, 0, 0, 0));
	objects[0] = floor;

	ALevelObject* floor2 = new ALevelObject(floor, Kore::vec3(25, -3, 0.2), Kore::Quaternion(-1, 0, 0, 0));
	environment.emplace_back(floor2);

	renderer->setLights(*floor->render, lightCount, lightPosLocation);
	ALevelObject* houseSmall = createNewObject("house/haus.ogex", "house/", environmentSructure, 1, Kore::vec3(0, 0, 0), Kore::Quaternion(0, 0, 0, 0));
	//houseSmall->render->M = Kore::mat4::Translation(17, -3.75, 0) * rotation.matrix().Transpose();

	/*
	train->render(tex_living_room, mLocation_living_room, mLocation_living_room_inverse, diffuse_living_room, specular_living_room, specular_power_living_room, false);

	for (int x = 0; x < 80; x++) {
		floor->M = mat4::Translation(5.5 + offsetX * x, -3.75, -819 + offsetZ * x) * livingRoomRot.matrix().Transpose();
		floor->render(tex_living_room, mLocation_living_room, mLocation_living_room_inverse, diffuse_living_room, specular_living_room, specular_power_living_room, false);
	}

	houseSmall->render(tex_living_room, mLocation_living_room, mLocation_living_room_inverse, diffuse_living_room, specular_living_room, specular_power_living_room, false);
	*/
	
	//environment.emplace_back(train);
	//environment.emplace_back(houseSmall);
	//environment.emplace_back(new ALevelObject("floor/floor.ogex", "floor/", environmentSructure, 1));
}

void TrainLevel::spawn(double deltaT)
{

	if (countDown > maxWaitintTime | (levelStarted & countDown > maxWaitintTime / 2.0))
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