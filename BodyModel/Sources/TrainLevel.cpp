#include "TrainLevel.h"

void TrainLevel::Update(double deltaT)
{
	Level::update(deltaT);
}

void TrainLevel::Init() {
	renderer->loadEnvironmentShader("shader_basic_shading.vert", "shader_basic_shading.frag");
	renderer->loadEntityShader("shader.vert", "shader.frag");

	avatar = new Level::Avatar("avatar/avatar_male.ogex", "avatar/", renderer->entityGraphics.structure, 1.0f, true, true);
	
	Level::LevelObject* train = new Level::LevelObject("train/train9.ogex", "train/", renderer->environmentGraphics.structure, 1);
	Kore::Quaternion rotation = Kore::Quaternion(0, 0, 0, 1);
	rotation.rotate(Kore::Quaternion(vec3(1, 0, 0), -Kore::pi / 2.0));
	rotation.rotate(Kore::Quaternion(vec3(0, 0, 1), Kore::pi / 2.0));
	train->render->M = mat4::RotationY(-0.0075) * mat4::Translation(0, -3, 0) * rotation.matrix().Transpose();
	train->render->setLights(renderer->environmentGraphics.lightCount, renderer->environmentGraphics.lightPosLocation);
	
	Level::LevelObject* houseSmall = new Level::LevelObject("house/haus.ogex", "house/", renderer->environmentGraphics.structure, 1);
	houseSmall->render->M = mat4::Translation(17, -3.75, 0) * rotation.matrix().Transpose();

	/*
	train->render(tex_living_room, mLocation_living_room, mLocation_living_room_inverse, diffuse_living_room, specular_living_room, specular_power_living_room, false);

	for (int x = 0; x < 80; x++) {
		floor->M = mat4::Translation(5.5 + offsetX * x, -3.75, -819 + offsetZ * x) * livingRoomRot.matrix().Transpose();
		floor->render(tex_living_room, mLocation_living_room, mLocation_living_room_inverse, diffuse_living_room, specular_living_room, specular_power_living_room, false);
	}

	houseSmall->render(tex_living_room, mLocation_living_room, mLocation_living_room_inverse, diffuse_living_room, specular_living_room, specular_power_living_room, false);
	*/

	environment.emplace_back(train);
	environment.emplace_back(houseSmall);
	environment.emplace_back(new Level::LevelObject("floor/floor.ogex", "floor/", renderer->environmentGraphics.structure, 1));
	Level::init();
}