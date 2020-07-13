
#include <Kore/Math/Quaternion.h>

#include "Settings.h"

using namespace Kore;

class CustomMath
{
	static CustomMath* instance; //This class is designed to be a singleton
public:
	mat4 initTrans;
	mat4 initTransInv;
	Kore::Quaternion initRot;
	Kore::Quaternion initRotInv;
	vec3 cameraPos;
	vec4 camForward;
	
	static CustomMath* getInstance(); // Get the singleton instance

	mat4 getMirrorMatrix(); // Esed to mirror avatar in demo

	Kore::mat4 getProjectionMatrix(); // Not sure what exactly this one does

	Kore::mat4 getViewMatrix(); // Get the matrix describing the players view

	void setSetViewMatrixCoefficients(vec3 cameraPosition, vec4);

	void initTransAndRot(); // Initializes the quaternion initRot and matrix initTrans and their inverses
};