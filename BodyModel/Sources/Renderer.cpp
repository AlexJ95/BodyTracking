
#include "Renderer.h"

Renderer::Renderer(std::vector<LevelObject> objects, std::vector<HumanoidEntity> entities) {
	levelObjects = objects;
	humanoidEntities = entities;
}

void Renderer::loadEnvironmentShader(String vertexShaderFile, String fragmentShaderFile)
{
	FileReader vs(vertexShaderFile);
	FileReader fs(fragmentShaderFile);
	environmentGraphics.vertexShader = new Shader(vs.readAll(), vs.size(), VertexShader);
	environmentGraphics.fragmentShader = new Shader(fs.readAll(), fs.size(), FragmentShader);

	environmentGraphics.structure.add("pos", Float3VertexData);
	environmentGraphics.structure.add("tex", Float2VertexData);
	environmentGraphics.structure.add("nor", Float3VertexData);

	environmentGraphics.pipeline = new PipelineState();
	environmentGraphics.pipeline->inputLayout[0] = &environmentGraphics.structure;
	environmentGraphics.pipeline->inputLayout[1] = nullptr;
	environmentGraphics.pipeline->vertexShader = environmentGraphics.vertexShader;
	environmentGraphics.pipeline->fragmentShader = environmentGraphics.fragmentShader;
	environmentGraphics.pipeline->depthMode = ZCompareLess;
	environmentGraphics.pipeline->depthWrite = true;
	environmentGraphics.pipeline->blendSource = Graphics4::SourceAlpha;
	environmentGraphics.pipeline->blendDestination = Graphics4::InverseSourceAlpha;
	environmentGraphics.pipeline->alphaBlendSource = Graphics4::SourceAlpha;
	environmentGraphics.pipeline->alphaBlendDestination = Graphics4::InverseSourceAlpha;
	environmentGraphics.pipeline->compile();

	environmentGraphics.tex = environmentGraphics.pipeline->getTextureUnit("tex");
	Graphics4::setTextureAddressing(environmentGraphics.tex, Graphics4::U, Repeat);
	Graphics4::setTextureAddressing(environmentGraphics.tex, Graphics4::V, Repeat);

	environmentGraphics.pLocation = environmentGraphics.pipeline->getConstantLocation("P");
	environmentGraphics.vLocation = environmentGraphics.pipeline->getConstantLocation("V");
	environmentGraphics.mLocation = environmentGraphics.pipeline->getConstantLocation("M");
	environmentGraphics.mLocationInverse = environmentGraphics.pipeline->getConstantLocation("MInverse");
	environmentGraphics.diffuse = environmentGraphics.pipeline->getConstantLocation("diffuseCol");
	environmentGraphics.specular = environmentGraphics.pipeline->getConstantLocation("specularCol");
	environmentGraphics.specularPower = environmentGraphics.pipeline->getConstantLocation("specularPow");
	environmentGraphics.lightPosLocation = environmentGraphics.pipeline->getConstantLocation("lightPos");
	environmentGraphics.lightCount = environmentGraphics.pipeline->getConstantLocation("numLights");
}

void Renderer::loadEntityShader(String vertexShaderFile, String fragmentShaderFile)
{
	FileReader vs(vertexShaderFile);
	FileReader fs(fragmentShaderFile);
	entityGraphics.vertexShader = new Shader(vs.readAll(), vs.size(), VertexShader);
	entityGraphics.fragmentShader = new Shader(fs.readAll(), fs.size(), FragmentShader);

	// This defines the structure of your Vertex Buffer
	entityGraphics.structure.add("pos", Float3VertexData);
	entityGraphics.structure.add("tex", Float2VertexData);
	entityGraphics.structure.add("nor", Float3VertexData);

	entityGraphics.pipeline = new PipelineState();
	entityGraphics.pipeline = new PipelineState();
	entityGraphics.pipeline->inputLayout[0] = &entityGraphics.structure;
	entityGraphics.pipeline->inputLayout[1] = nullptr;
	entityGraphics.pipeline->vertexShader = entityGraphics.vertexShader;
	entityGraphics.pipeline->fragmentShader = entityGraphics.fragmentShader;
	entityGraphics.pipeline->depthMode = ZCompareLess;
	entityGraphics.pipeline->depthWrite = true;
	entityGraphics.pipeline->blendSource = Graphics4::SourceAlpha;
	entityGraphics.pipeline->blendDestination = Graphics4::InverseSourceAlpha;
	entityGraphics.pipeline->alphaBlendSource = Graphics4::SourceAlpha;
	entityGraphics.pipeline->alphaBlendDestination = Graphics4::InverseSourceAlpha;
	entityGraphics.pipeline->compile();

	entityGraphics.tex = entityGraphics.pipeline->getTextureUnit("tex");
	Graphics4::setTextureAddressing(entityGraphics.tex, Graphics4::U, Repeat);
	Graphics4::setTextureAddressing(entityGraphics.tex, Graphics4::V, Repeat);

	entityGraphics.pLocation = entityGraphics.pipeline->getConstantLocation("P");
	entityGraphics.vLocation = entityGraphics.pipeline->getConstantLocation("V");
	entityGraphics.mLocation = entityGraphics.pipeline->getConstantLocation("M");
}

void Renderer::renderEnvironment() {
	for (LevelObject object : levelObjects) object.render(&environmentGraphics, false);
}

void Renderer::renderEntities() {
	for (HumanoidEntity entity : humanoidEntities) entity.animate(&entityGraphics);
}

void Renderer::update(float deltaT)
{
	Graphics4::begin();
	Graphics4::clear(Graphics4::ClearColorFlag | Graphics4::ClearDepthFlag, Graphics1::Color::Black, 1.0f, 0);
	Graphics4::setPipeline(entityGraphics.pipeline);

#ifdef KORE_STEAMVR
	VrInterface::begin();

	if (!controllerButtonsInitialized) initButtons();

	VrPoseState vrDevice;
	for (int i = 0; i < numOfEndEffectors; ++i) {
		if (endEffector[i]->getDeviceIndex() != -1) {

			if (i == head) {
				SensorState state = VrInterface::getSensorState(0);

				// Get HMD position and rotation
				endEffector[i]->setDesPosition(state.pose.vrPose.position);
				endEffector[i]->setDesRotation(state.pose.vrPose.orientation);
			}
			else {
				vrDevice = VrInterface::getController(endEffector[i]->getDeviceIndex());

				// Get VR device position and rotation
				endEffector[i]->setDesPosition(vrDevice.vrPose.position);
				endEffector[i]->setDesRotation(vrDevice.vrPose.orientation);
			}

			executeMovement(i);
		}
	}

	// Render for both eyes
	SensorState state;
	for (int j = 0; j < 2; ++j) {
		VrInterface::beginRender(j);

		Graphics4::clear(Graphics4::ClearColorFlag | Graphics4::ClearDepthFlag, Graphics1::Color::Black, 1.0f, 0);

		state = VrInterface::getSensorState(j);

		renderAvatar(state.pose.vrPose.eye, state.pose.vrPose.projection);

		if (renderRoom) renderLivingRoom(state.pose.vrPose.eye, state.pose.vrPose.projection);

		VrInterface::endRender(j);
	}

	VrInterface::warpSwap();

	Graphics4::restoreRenderTarget();
	Graphics4::clear(Graphics4::ClearColorFlag | Graphics4::ClearDepthFlag, Graphics1::Color::Black, 1.0f, 0);

	// Render on monitor
	mat4 P = getProjectionMatrix();
	mat4 V = getViewMatrix();

	if (!firstPersonMonitor) renderAvatar(V, P);
	else renderAvatar(state.pose.vrPose.eye, state.pose.vrPose.projection);

	if (renderRoom) {
		if (!firstPersonMonitor) renderLivingRoom(V, P);
		else renderLivingRoom(state.pose.vrPose.eye, state.pose.vrPose.projection);
	}
#endif
	renderEnvironment();
	renderEntities();

	Graphics4::end();
	Graphics4::swapBuffers();
}

Renderer::LevelObject::LevelObject(const char* meshFile, const char* textureFile, const Kore::Graphics4::VertexStructure& structure, float scale) : MeshObject(meshFile, textureFile, structure, scale) { math->getInstance(); }

void Renderer::LevelObject::render(EnvironmentGraphics* graphics, bool mirror)
{
	Graphics4::setPipeline(graphics->pipeline);

	Graphics4::setMatrix(graphics->vLocation, math->getViewMatrix());
	Graphics4::setMatrix(graphics->pLocation, math->getProjectionMatrix());
	for (int i = 0; i < meshesCount; ++i) {
		Geometry* geometry = geometries[i];
		mat4 modelMatrix = mat4::Identity();
		if (!mirror) modelMatrix = M * geometry->transform;
		else modelMatrix = Mmirror * geometry->transform;
		mat4 modelMatrixInverse = modelMatrix.Invert();

		Graphics4::setMatrix(graphics->mLocation, modelMatrix);
		Graphics4::setMatrix(graphics->mLocationInverse, modelMatrixInverse);

		unsigned int materialIndex = geometry->materialIndex;
		Material* material = findMaterialWithIndex(materialIndex);
		if (material != nullptr) {
			Graphics4::setFloat3(graphics->diffuse, material->diffuse);
			Graphics4::setFloat3(graphics->specular, material->specular);
			Graphics4::setFloat(graphics->specularPower, material->specular_power);
		}
		else {
			Graphics4::setFloat3(graphics->diffuse, vec3(1.0, 1.0, 1.0));
			Graphics4::setFloat3(graphics->specular, vec3(1.0, 1.0, 1.0));
			Graphics4::setFloat(graphics->specularPower, 1.0);
		}

		Texture* image = images[i];
		if (image != nullptr) Graphics4::setTexture(graphics->tex, image);

		Graphics4::setVertexBuffer(*vertexBuffers[i]);
		Graphics4::setIndexBuffer(*indexBuffers[i]);
		Graphics4::drawIndexedVertices();
	}
}

void Renderer::LevelObject::setLights(Kore::Graphics4::ConstantLocation lightCountLocation, Kore::Graphics4::ConstantLocation lightPosLocation)
{
	const int lightCount = (int)lights.size();
	for (int i = 0; i < lightCount; ++i) {
		Light* light = lights[i];
		lightPositions[i] = M * light->position;

		if (light->type == 0) {
			lightPositions[i].w() = 0;
		}
		else if (light->type == 1) {
			lightPositions[i].w() = 1;
		}
	}

	Graphics4::setInt(lightCountLocation, lightCount);
	Graphics4::setFloats(lightPosLocation, (float*)lightPositions, lightCount * 4);
}

Renderer::HumanoidEntity::HumanoidEntity(const char* meshFile, const char* textureFile, const Kore::Graphics4::VertexStructure& structure, float scale, Animator* anim) : MeshObject(meshFile, textureFile, structure, scale)
{
	math->getInstance();
	animator = anim;
}

void Renderer::HumanoidEntity::animate(EntityGraphics* graphics) {
	Graphics4::setPipeline(graphics->pipeline);

	Graphics4::setMatrix(graphics->vLocation, math->getViewMatrix());
	Graphics4::setMatrix(graphics->pLocation, math->getProjectionMatrix());
	Graphics4::setMatrix(graphics->mLocation, math->initTrans);

	// Update bones
	for (int i = 0; i < bones.size(); ++i) animator->invKin->initializeBone(bones[i]);

	for (int j = 0; j < meshesCount; ++j) {
		int currentBoneIndex = 0;	// Iterate over BoneCountArray

		Mesh* mesh = meshes[j];

		// Mesh Vertex Buffer
		float* vertices = vertexBuffers[j]->lock();
		for (int i = 0; i < mesh->numVertices; ++i) {
			vec4 startPos(0, 0, 0, 1);
			vec4 startNormal(0, 0, 0, 1);

			// For each vertex belonging to a mesh, the bone count array specifies the number of bones the influence the vertex
			int numOfBones = mesh->boneCountArray[i];

			float totalJointsWeight = 0;
			for (int b = 0; b < numOfBones; ++b) {
				vec4 posVec(mesh->vertices[i * 3 + 0], mesh->vertices[i * 3 + 1], mesh->vertices[i * 3 + 2], 1);
				vec4 norVec(mesh->normals[i * 3 + 0], mesh->normals[i * 3 + 1], mesh->normals[i * 3 + 2], 1);

				int index = mesh->boneIndices[currentBoneIndex] + 2;
				BoneNode* bone = animator->getBoneWithIndex(index);
				float boneWeight = mesh->boneWeight[currentBoneIndex];
				totalJointsWeight += boneWeight;

				startPos += (bone->finalTransform * posVec) * boneWeight;
				startNormal += (bone->finalTransform * norVec) * boneWeight;

				currentBoneIndex++;
			}

			// position
			vertices[i * 8 + 0] = startPos.x() * scale;
			vertices[i * 8 + 1] = startPos.y() * scale;
			vertices[i * 8 + 2] = startPos.z() * scale;
			// texCoord
			vertices[i * 8 + 3] = mesh->texcoord[i * 2 + 0];
			vertices[i * 8 + 4] = 1.0f - mesh->texcoord[i * 2 + 1];
			// normal
			vertices[i * 8 + 5] = startNormal.x();
			vertices[i * 8 + 6] = startNormal.y();
			vertices[i * 8 + 7] = startNormal.z();

			//log(Info, "%f %f %f %f %f %f %f %f", vertices[i * 8 + 0], vertices[i * 8 + 1], vertices[i * 8 + 2], vertices[i * 8 + 3], vertices[i * 8 + 4], vertices[i * 8 + 5], vertices[i * 8 + 6], vertices[i * 8 + 7]);
		}
		vertexBuffers[j]->unlock();

		Texture* image = images[j];

		Graphics4::setTexture(graphics->tex, image);
		Graphics4::setVertexBuffer(*vertexBuffers[j]);
		Graphics4::setIndexBuffer(*indexBuffers[j]);
		Graphics4::drawIndexedVertices();
	}
}

Renderer::Avatar::Avatar(const char* meshFile, const char* textureFile, const Kore::Graphics4::VertexStructure& structure, float scale, Animator* anim, bool renderTrackerAndController, bool renderAxisForEndEffector)
	: HumanoidEntity(meshFile, textureFile, structure, scale, anim) {
	renderTrackerAndControllers = renderTrackerAndController;
	renderAxisForEndEffectors = renderAxisForEndEffector;
	if (renderTrackerAndController) {
		viveObjects.emplace_back(new MeshObject("vivemodels/vivetracker.ogex", "vivemodels/", structure, 1));
		viveObjects.emplace_back(new MeshObject("vivemodels/vivecontroller.ogex", "vivemodels/", structure, 1));
	}

	if (renderTrackerAndController || renderAxisForEndEffector) {
		viveObjects.emplace_back(new MeshObject("vivemodels/axis.ogex", "vivemodels/", structure, 1));
	}
}

void Renderer::Avatar::animate(EntityGraphics* graphics)
{
	HumanoidEntity::animate(graphics);
	if (renderTrackerAndController) renderAllVRDevices(graphics);

	if (renderAxisForEndEffector) renderCSForEndEffector(graphics);
}

void Renderer::Avatar::renderAllVRDevices(EntityGraphics* graphics) {
	Graphics4::setPipeline(graphics->pipeline);

#ifdef KORE_STEAMVR
	VrPoseState controller;
	for (int i = 0; i < 16; ++i) {
		controller = VrInterface::getController(i);

		vec3 desPosition = controller.vrPose.position;
		Kore::Quaternion desRotation = controller.vrPose.orientation;

		if (controller.trackedDevice == TrackedDevice::ViveTracker) {
			renderControllerAndTracker(true, desPosition, desRotation);
		}
		else if (controller.trackedDevice == TrackedDevice::Controller) {
			renderControllerAndTracker(false, desPosition, desRotation);
		}

	}
#else
	for (int i = 0; i < numOfEndEffectors; ++i) {
		Kore::vec3 desPosition = endEffector[i]->getDesPosition();
		Kore::Quaternion desRotation = endEffector[i]->getDesRotation();

		if (i == hip || i == leftForeArm || i == rightForeArm || i == leftFoot || i == rightFoot) {
			renderControllerAndTracker(true, desPosition, desRotation, graphics);
		}
		else if (i == rightHand || i == leftHand) {
			renderControllerAndTracker(false, desPosition, desRotation, graphics);
		}
	}
#endif
}

void Renderer::Avatar::renderControllerAndTracker(int tracker, Kore::vec3 desPosition, Kore::Quaternion desRotation, EntityGraphics* graphics) {
	// World Transformation Matrix
	Kore::mat4 W = mat4::Translation(desPosition.x(), desPosition.y(), desPosition.z()) * desRotation.matrix().Transpose();

	// Mirror Transformation Matrix
	Kore::mat4 M = math->getMirrorMatrix() * W;

	if (tracker) {
		// Render a tracker for both feet and back
		renderVRDevice(0, W, graphics);
		//renderVRDevice(0, M);
	}
	else {
		// Render a controller for both hands
		renderVRDevice(1, W, graphics);
		//renderVRDevice(1, M);
	}

	// Render a local coordinate system only if the avatar is not calibrated
	if (!calibratedAvatar) {
		renderVRDevice(2, W, graphics);
		//renderVRDevice(2, M);
	}
}

void Renderer::Avatar::renderVRDevice(int index, Kore::mat4 M, EntityGraphics* graphics) {
	Graphics4::setMatrix(graphics->mLocation, M);
	viveObjects.at(index)->render(graphics->tex);
}

void Renderer::Avatar::renderCSForEndEffector(EntityGraphics* graphics) {
	Graphics4::setPipeline(graphics->pipeline);

	for (int i = 0; i < numOfEndEffectors; ++i) {
		BoneNode* bone = animator->getBoneWithIndex(endEffector[i]->getBoneIndex());

		vec3 endEffectorPos = bone->getPosition();
		endEffectorPos = math->initTrans * vec4(endEffectorPos.x(), endEffectorPos.y(), endEffectorPos.z(), 1);
		Kore::Quaternion endEffectorRot = math->initRot.rotated(bone->getOrientation());

		Kore::mat4 M = mat4::Translation(endEffectorPos.x(), endEffectorPos.y(), endEffectorPos.z()) * endEffectorRot.matrix().Transpose();
		Graphics4::setMatrix(graphics->mLocation, M);
		viveObjects[2]->render(graphics->tex);
	}
}