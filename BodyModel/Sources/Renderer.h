#pragma once

#include "Settings.h"
#include "CustomMath.h"
#include "Animator.h"
#include "LevelObject.h"
#include "AnimatedEntity.h"

#include <Kore/IO/FileReader.h>
#include <Kore/Graphics4/PipelineState.h>
#include <Kore/Graphics1/Color.h>

using namespace Kore::Graphics4;

class Renderer
{	
public:
	struct EnvironmentGraphics {
		TextureUnit tex;
		ConstantLocation pLocation;
		ConstantLocation vLocation;
		ConstantLocation mLocation;
		ConstantLocation mLocationInverse;
		ConstantLocation diffuse;
		ConstantLocation specular;
		ConstantLocation specularPower;
		ConstantLocation lightPosLocation;
		ConstantLocation lightCount;

		VertexStructure structure;
		Shader* vertexShader;
		Shader* fragmentShader;
		PipelineState* pipeline;
	};

	struct EntityGraphics {
		TextureUnit tex;
		ConstantLocation pLocation;
		ConstantLocation vLocation;
		ConstantLocation mLocation;

		VertexStructure structure;
		Shader* vertexShader;
		Shader* fragmentShader;
		PipelineState* pipeline;
	};

	void render(LevelObject* object, bool mirror);
	void setLights(LevelObject object, Kore::Graphics4::ConstantLocation lightCountLocation, Kore::Graphics4::ConstantLocation);

	void animate(AnimatedEntity* entity);

	void animate(Avatar* avatar);

	void renderAllVRDevices(Avatar* avatar); //renders all VR devices
	void renderControllerAndTracker(Avatar* avatar, int tracker, Kore::vec3 desPosition, Kore::Quaternion desRotation); //renders VR Stuff, calls method below, is a subroutine
	void renderVRDevice(Avatar* avatar, int index, Kore::mat4 M); //renders VR stuff, is a subroutine

	void renderCSForEndEffector(Avatar* avatar); //renders stuff, not sure what though

	CustomMath* math;
	Animator* animator;

	//LevelObject variables
	static const int maxLightCount = 10;
	Kore::vec4 lightPositions[maxLightCount];

	EnvironmentGraphics *environmentGraphics;
	EntityGraphics *entityGraphics;

	std::vector<LevelObject*> levelObjects;
	std::vector<AnimatedEntity*> animatedEntities;
	Avatar* avatar;

	Renderer();

	void init(std::vector<LevelObject*> objects, std::vector<AnimatedEntity*> entities, Avatar* avatar, Animator* anim);

	void loadEnvironmentShader(String vertexShaderFile, String fragmentShaderFile);
	void loadEntityShader(String vertexShaderFile, String fragmentShaderFile);

	void renderEnvironment(); //as the name implies
	void renderEntities(); //as the name implies

	void update(float deltaT);
};
