#include "TrainLevel.h"

void TrainLevel::Update(double deltaT)
{
	Level::update(deltaT);
}

void TrainLevel::Init() {
	renderer->loadEnvironmentShader("shader_basic_shading.vert", "shader_basic_shading.frag");
	renderer->loadEntityShader("shader.vert", "shader.frag");
	const Kore::Graphics4::VertexStructure& entitySructure = renderer->entityGraphics->structure;
	const Kore::Graphics4::VertexStructure& environmentSructure = renderer->environmentGraphics->structure;
	Kore::Graphics4::ConstantLocation lightCount = renderer->environmentGraphics->lightCount;
	Kore::Graphics4::ConstantLocation lightPosLocation = renderer->environmentGraphics->lightPosLocation;

	TheAvatar* avatar = new TheAvatar("avatar/avatar_male.ogex", "avatar/", entitySructure, 1.0f, true, true);
	
	ALevelObject* train = new ALevelObject("train/train9.ogex", "train/", environmentSructure, 1);
	Kore::Quaternion rotation = Kore::Quaternion(0, 0, 0, 1);
	rotation.rotate(Kore::Quaternion(Kore::vec3(1, 0, 0), -Kore::pi / 2.0));
	rotation.rotate(Kore::Quaternion(Kore::vec3(0, 0, 1), Kore::pi / 2.0));
	train->render->M = Kore::mat4::RotationY(-0.0075) * Kore::mat4::Translation(0, -3, 0) * rotation.matrix().Transpose();
	renderer->setLights(*(train->render), lightCount, lightPosLocation);
	
	ALevelObject* houseSmall = new ALevelObject("house/haus.ogex", "house/", environmentSructure, 1);
	houseSmall->render->M = Kore::mat4::Translation(17, -3.75, 0) * rotation.matrix().Transpose();

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
	environment.emplace_back(new ALevelObject("floor/floor.ogex", "floor/", environmentSructure, 1));
	Level::init();
}