
#include "Renderer.h"

void Renderer::init(std::vector<LevelObject*> objects, std::vector<AnimatedEntity*> entities, Animator* anim) {
	math = math->getInstance();
	levelObjects = objects;
	animatedEntities = entities;
	animator = anim;
}

void Renderer::loadEnvironmentShader(String vertexShaderFile, String fragmentShaderFile)
{
	Kore::FileReader vs(vertexShaderFile);
	Kore::FileReader fs(fragmentShaderFile);
	environmentGraphics->vertexShader = new Shader(vs.readAll(), vs.size(), VertexShader);
	environmentGraphics->fragmentShader = new Shader(fs.readAll(), fs.size(), FragmentShader);

	environmentGraphics->structure.add("pos", Float3VertexData);
	environmentGraphics->structure.add("tex", Float2VertexData);
	environmentGraphics->structure.add("nor", Float3VertexData);

	environmentGraphics->pipeline = new PipelineState();
	environmentGraphics->pipeline->inputLayout[0] = &environmentGraphics->structure;
	environmentGraphics->pipeline->inputLayout[1] = nullptr;
	environmentGraphics->pipeline->vertexShader = environmentGraphics->vertexShader;
	environmentGraphics->pipeline->fragmentShader = environmentGraphics->fragmentShader;
	environmentGraphics->pipeline->depthMode = ZCompareLess;
	environmentGraphics->pipeline->depthWrite = true;
	environmentGraphics->pipeline->blendSource = Kore::Graphics4::SourceAlpha;
	environmentGraphics->pipeline->blendDestination = Kore::Graphics4::InverseSourceAlpha;
	environmentGraphics->pipeline->alphaBlendSource = Kore::Graphics4::SourceAlpha;
	environmentGraphics->pipeline->alphaBlendDestination = Kore::Graphics4::InverseSourceAlpha;
	environmentGraphics->pipeline->compile();

	environmentGraphics->tex = environmentGraphics->pipeline->getTextureUnit("tex");
	Kore::Graphics4::setTextureAddressing(environmentGraphics->tex, Kore::Graphics4::U, Repeat);
	Kore::Graphics4::setTextureAddressing(environmentGraphics->tex, Kore::Graphics4::V, Repeat);

	environmentGraphics->pLocation = environmentGraphics->pipeline->getConstantLocation("P");
	environmentGraphics->vLocation = environmentGraphics->pipeline->getConstantLocation("V");
	environmentGraphics->mLocation = environmentGraphics->pipeline->getConstantLocation("M");
	environmentGraphics->mLocationInverse = environmentGraphics->pipeline->getConstantLocation("MInverse");
	environmentGraphics->diffuse = environmentGraphics->pipeline->getConstantLocation("diffuseCol");
	environmentGraphics->specular = environmentGraphics->pipeline->getConstantLocation("specularCol");
	environmentGraphics->specularPower = environmentGraphics->pipeline->getConstantLocation("specularPow");
	environmentGraphics->lightPosLocation = environmentGraphics->pipeline->getConstantLocation("lightPos");
	environmentGraphics->lightCount = environmentGraphics->pipeline->getConstantLocation("numLights");
}

void Renderer::loadEntityShader(String vertexShaderFile, String fragmentShaderFile)
{
	Kore::FileReader vs(vertexShaderFile);
	Kore::FileReader fs(fragmentShaderFile);
	entityGraphics->vertexShader = new Shader(vs.readAll(), vs.size(), VertexShader);
	entityGraphics->fragmentShader = new Shader(fs.readAll(), fs.size(), FragmentShader);

	// This defines the structure of your Vertex Buffer
	entityGraphics->structure.add("pos", Float3VertexData);
	entityGraphics->structure.add("tex", Float2VertexData);
	entityGraphics->structure.add("nor", Float3VertexData);

	entityGraphics->pipeline = new PipelineState();
	entityGraphics->pipeline = new PipelineState();
	entityGraphics->pipeline->inputLayout[0] = &entityGraphics->structure;
	entityGraphics->pipeline->inputLayout[1] = nullptr;
	entityGraphics->pipeline->vertexShader = entityGraphics->vertexShader;
	entityGraphics->pipeline->fragmentShader = entityGraphics->fragmentShader;
	entityGraphics->pipeline->depthMode = ZCompareLess;
	entityGraphics->pipeline->depthWrite = true;
	entityGraphics->pipeline->blendSource = Kore::Graphics4::SourceAlpha;
	entityGraphics->pipeline->blendDestination = Kore::Graphics4::InverseSourceAlpha;
	entityGraphics->pipeline->alphaBlendSource = Kore::Graphics4::SourceAlpha;
	entityGraphics->pipeline->alphaBlendDestination = Kore::Graphics4::InverseSourceAlpha;
	entityGraphics->pipeline->compile();

	entityGraphics->tex = entityGraphics->pipeline->getTextureUnit("tex");
	Kore::Graphics4::setTextureAddressing(entityGraphics->tex, Kore::Graphics4::U, Repeat);
	Kore::Graphics4::setTextureAddressing(entityGraphics->tex, Kore::Graphics4::V, Repeat);

	entityGraphics->pLocation = entityGraphics->pipeline->getConstantLocation("P");
	entityGraphics->vLocation = entityGraphics->pipeline->getConstantLocation("V");
	entityGraphics->mLocation = entityGraphics->pipeline->getConstantLocation("M");
}

void Renderer::renderEnvironment() {
	for (LevelObject* object : levelObjects) render(object, false);
}

void Renderer::renderEntities() {
	for (AnimatedEntity* entity : animatedEntities) animate(entity);
}

void Renderer::update(float deltaT)
{
	Kore::Graphics4::begin();
	Kore::Graphics4::clear(Kore::Graphics4::ClearColorFlag | Kore::Graphics4::ClearDepthFlag, Kore::Graphics1::Color::Black, 1.0f, 0);
	Kore::Graphics4::setPipeline(entityGraphics->pipeline);

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

	Kore::Graphics4::end();
	Kore::Graphics4::swapBuffers();
}


//Subroutines for LevelObjects
void Renderer::render(LevelObject* object, bool mirror)
{
	Kore::Graphics4::setPipeline(environmentGraphics->pipeline);

	Kore::Graphics4::setMatrix(environmentGraphics->vLocation, math->getViewMatrix());
	Kore::Graphics4::setMatrix(environmentGraphics->pLocation, math->getProjectionMatrix());
	for (int i = 0; i < object->meshesCount; ++i) {
		Geometry* geometry = object->geometries[i];
		Kore::mat4 modelMatrix = Kore::mat4::Identity();
		if (!mirror) modelMatrix = object->M * geometry->transform;
		else modelMatrix = object->Mmirror * geometry->transform;
		Kore::mat4 modelMatrixInverse = modelMatrix.Invert();

		Kore::Graphics4::setMatrix(environmentGraphics->mLocation, modelMatrix);
		Kore::Graphics4::setMatrix(environmentGraphics->mLocationInverse, modelMatrixInverse);

		unsigned int materialIndex = geometry->materialIndex;
		Material* material = object->findMaterialWithIndex(materialIndex);
		if (material != nullptr) {
			Kore::Graphics4::setFloat3(environmentGraphics->diffuse, material->diffuse);
			Kore::Graphics4::setFloat3(environmentGraphics->specular, material->specular);
			Kore::Graphics4::setFloat(environmentGraphics->specularPower, material->specular_power);
		}
		else {
			Kore::Graphics4::setFloat3(environmentGraphics->diffuse, Kore::vec3(1.0, 1.0, 1.0));
			Kore::Graphics4::setFloat3(environmentGraphics->specular, Kore::vec3(1.0, 1.0, 1.0));
			Kore::Graphics4::setFloat(environmentGraphics->specularPower, 1.0);
		}

		Texture* image = object->images[i];
		if (image != nullptr) Kore::Graphics4::setTexture(environmentGraphics->tex, image);

		Kore::Graphics4::setVertexBuffer(*object->vertexBuffers[i]);
		Kore::Graphics4::setIndexBuffer(*object->indexBuffers[i]);
		Kore::Graphics4::drawIndexedVertices();
	}
}

void Renderer::setLights(LevelObject object, Kore::Graphics4::ConstantLocation lightCountLocation, Kore::Graphics4::ConstantLocation lightPosLocation)
{
	const int lightCount = (int)object.lights.size();
	for (int i = 0; i < lightCount; ++i) {
		Light* light = object.lights[i];
		lightPositions[i] = object.M * light->position;

		if (light->type == 0) {
			lightPositions[i].w() = 0;
		}
		else if (light->type == 1) {
			lightPositions[i].w() = 1;
		}
	}

	Kore::Graphics4::setInt(lightCountLocation, lightCount);
	Kore::Graphics4::setFloats(lightPosLocation, (float*)lightPositions, lightCount * 4);
}

// Subroutine(s) for AnimatedEntities
void Renderer::animate(AnimatedEntity* entity) {
	Kore::Graphics4::setPipeline(entityGraphics->pipeline);

	Kore::Graphics4::setMatrix(entityGraphics->vLocation, math->getViewMatrix());
	Kore::Graphics4::setMatrix(entityGraphics->pLocation, math->getProjectionMatrix());
	Kore::Graphics4::setMatrix(entityGraphics->mLocation, math->initTrans);

	// Update bones
	for (int i = 0; i < entity->bones.size(); ++i) entity->invKin->initializeBone(entity->bones[i]);

	for (int j = 0; j < entity->meshesCount; ++j) {
		int currentBoneIndex = 0;	// Iterate over BoneCountArray

		Mesh* mesh = entity->meshes[j];

		// Mesh Vertex Buffer
		float* vertices = entity->vertexBuffers[j]->lock();
		for (int i = 0; i < mesh->numVertices; ++i) {
			Kore::vec4 startPos(0, 0, 0, 1);
			Kore::vec4 startNormal(0, 0, 0, 1);

			// For each vertex belonging to a mesh, the bone count array specifies the number of bones the influence the vertex
			int numOfBones = mesh->boneCountArray[i];

			float totalJointsWeight = 0;
			for (int b = 0; b < numOfBones; ++b) {
				Kore::vec4 posVec(mesh->vertices[i * 3 + 0], mesh->vertices[i * 3 + 1], mesh->vertices[i * 3 + 2], 1);
				Kore::vec4 norVec(mesh->normals[i * 3 + 0], mesh->normals[i * 3 + 1], mesh->normals[i * 3 + 2], 1);

				int index = mesh->boneIndices[currentBoneIndex] + 2;
				BoneNode* bone = animator->getBoneWithIndex(entity, index);
				float boneWeight = mesh->boneWeight[currentBoneIndex];
				totalJointsWeight += boneWeight;

				startPos += (bone->finalTransform * posVec) * boneWeight;
				startNormal += (bone->finalTransform * norVec) * boneWeight;

				currentBoneIndex++;
			}

			// position
			vertices[i * 8 + 0] = startPos.x() * entity->scale;
			vertices[i * 8 + 1] = startPos.y() * entity->scale;
			vertices[i * 8 + 2] = startPos.z() * entity->scale;
			// texCoord
			vertices[i * 8 + 3] = mesh->texcoord[i * 2 + 0];
			vertices[i * 8 + 4] = 1.0f - mesh->texcoord[i * 2 + 1];
			// normal
			vertices[i * 8 + 5] = startNormal.x();
			vertices[i * 8 + 6] = startNormal.y();
			vertices[i * 8 + 7] = startNormal.z();

			//log(Info, "%f %f %f %f %f %f %f %f", vertices[i * 8 + 0], vertices[i * 8 + 1], vertices[i * 8 + 2], vertices[i * 8 + 3], vertices[i * 8 + 4], vertices[i * 8 + 5], vertices[i * 8 + 6], vertices[i * 8 + 7]);
		}
		entity->vertexBuffers[j]->unlock();

		Texture* image = entity->images[j];

		Kore::Graphics4::setTexture(entityGraphics->tex, image);
		Kore::Graphics4::setVertexBuffer(*entity->vertexBuffers[j]);
		Kore::Graphics4::setIndexBuffer(*entity->indexBuffers[j]);
		Kore::Graphics4::drawIndexedVertices();
	}
}

//Subroutines specifically for the Avatar
void Renderer::animate(Avatar* avatar)
{
	animate((AnimatedEntity*) avatar);
	if (renderTrackerAndController) renderAllVRDevices(avatar);

	if (renderAxisForEndEffector) renderCSForEndEffector(avatar);
}

void Renderer::renderAllVRDevices(Avatar* avatar) {
	Kore::Graphics4::setPipeline(entityGraphics->pipeline);

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
		Kore::vec3 desPosition = avatar->endEffector[i]->getDesPosition();
		Kore::Quaternion desRotation = avatar->endEffector[i]->getDesRotation();

		if (i == hip || i == leftForeArm || i == rightForeArm || i == leftFoot || i == rightFoot) {
			renderControllerAndTracker(avatar, true, desPosition, desRotation);
		}
		else if (i == rightHand || i == leftHand) {
			renderControllerAndTracker(avatar, false, desPosition, desRotation);
		}
	}
#endif
}

void Renderer::renderControllerAndTracker(Avatar* avatar, int tracker, Kore::vec3 desPosition, Kore::Quaternion desRotation) {
	// World Transformation Matrix
	Kore::mat4 W = Kore::mat4::Translation(desPosition.x(), desPosition.y(), desPosition.z()) * desRotation.matrix().Transpose();

	// Mirror Transformation Matrix
	Kore::mat4 M = math->getMirrorMatrix() * W;

	if (tracker) {
		// Render a tracker for both feet and back
		renderVRDevice(avatar, 0, W);
		//renderVRDevice(0, M);
	}
	else {
		// Render a controller for both hands
		renderVRDevice(avatar, 1, W);
		//renderVRDevice(1, M);
	}

	// Render a local coordinate system only if the avatar is not calibrated
	if (!avatar->calibratedAvatar) {
		renderVRDevice(avatar, 2, W);
		//renderVRDevice(2, M);
	}
}

void Renderer::renderVRDevice(Avatar* avatar, int index, Kore::mat4 M) {
	Kore::Graphics4::setMatrix(entityGraphics->mLocation, M);
	avatar->viveObjects.at(index)->render(entityGraphics->tex);
}

void Renderer::renderCSForEndEffector(Avatar* avatar) {
	Kore::Graphics4::setPipeline(entityGraphics->pipeline);

	for (int i = 0; i < numOfEndEffectors; ++i) {
		BoneNode* bone = animator->getBoneWithIndex((AnimatedEntity*) avatar, avatar->endEffector[i]->getBoneIndex());

		Kore::vec3 endEffectorPos = bone->getPosition();
		endEffectorPos = math->initTrans * Kore::vec4(endEffectorPos.x(), endEffectorPos.y(), endEffectorPos.z(), 1);
		Kore::Quaternion endEffectorRot = math->initRot.rotated(bone->getOrientation());

		Kore::mat4 M = Kore::mat4::Translation(endEffectorPos.x(), endEffectorPos.y(), endEffectorPos.z()) * endEffectorRot.matrix().Transpose();
		Kore::Graphics4::setMatrix(entityGraphics->mLocation, M);
		avatar->viveObjects[2]->render(entityGraphics->tex);
	}
}