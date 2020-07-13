
#include <Kore/Graphics4/PipelineState.h>
#include <Kore/Graphics1/Color.h>
#include <Kore/System.h>
#include <Kore/Log.h>
#include <Kore/IO/FileReader.h>

#include "Settings.h"
#include "MeshObject.h"
#include "CustomMath.h"
#include "InverseKinematics.h"
#include "RotationUtility.h"
#include "Animator.h"

using namespace Kore;
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

	
	struct LevelObject : public MeshObject {
	private:
		CustomMath* math;

		static const int maxLightCount = 10;
		Kore::vec4 lightPositions[maxLightCount];
		float scale;

	public:
		//EnvironmentGraphics* graphics;
		LevelObject(const char* meshFile, const char* textureFile, const Kore::Graphics4::VertexStructure& structure, float scale = 1.0f);
		void LevelObject::render(EnvironmentGraphics* graphics, bool mirror);
		void LevelObject::setLights(Kore::Graphics4::ConstantLocation lightCountLocation, Kore::Graphics4::ConstantLocation);
	};
	

	struct HumanoidEntity : public MeshObject {
	protected:
		CustomMath* math;
		Animator* animator;
	public:
		EndEffector** endEffector;
		HumanoidEntity(const char* meshFile, const char* textureFile, const Kore::Graphics4::VertexStructure& structure, float scale, Animator* anim);

		void animate(EntityGraphics* graphics);
	};

	struct Avatar : public HumanoidEntity {
		// Null terminated array of MeshObject pointers (Vive Controller and Tracker)
		std::vector<MeshObject*> viveObjects;
		bool renderTrackerAndControllers;
		bool renderAxisForEndEffectors;
		bool* calibratedAvatar;

		Avatar(const char* meshFile, const char* textureFile, const Kore::Graphics4::VertexStructure& structure, float scale, Animator* anim, bool renderTrackerAndController, bool renderAxisForEndEffector);
		void animate(EntityGraphics* graphics);

		void renderAllVRDevices(EntityGraphics* graphics); //renders all VR devices
		void renderControllerAndTracker(int tracker, Kore::vec3 desPosition, Kore::Quaternion desRotation, EntityGraphics* graphics); //renders VR Stuff, calls method below, is a subroutine
		void renderVRDevice(int index, Kore::mat4 M, EntityGraphics* graphics); //renders VR stuff, is a subroutine

		void renderCSForEndEffector(EntityGraphics* graphics); //renders stuff, not sure what though
	};



	EnvironmentGraphics environmentGraphics;

	EntityGraphics entityGraphics;

	std::vector<LevelObject> levelObjects;

	std::vector<HumanoidEntity> humanoidEntities;

	Renderer(std::vector<LevelObject> objects, std::vector<HumanoidEntity> entities);

	void loadEnvironmentShader(String vertexShaderFile, String fragmentShaderFile);

	void loadEntityShader(String vertexShaderFile, String fragmentShaderFile);

	void renderEnvironment(); //as the name implies

	void renderEntities(); //as the name implies

	void update(float deltaT);
};
