#include "PositionBasedDynamics.h"
#include <cfloat>

using namespace PBD;

const double eps = static_cast<double>(1e-6);

bool PositionBasedDynamics::solve_DistanceConstraint(
	const glm::vec3& p0, float invMass0,
	const glm::vec3& p1, float invMass1,
	const float restLength,
	const float stiffness,
	glm::vec3& corr0, glm::vec3& corr1)
{
	float wSum = invMass0 + invMass1;
	if (wSum == 0.0)
		return false;

	glm::vec3 n = p1 - p0;
	float d = glm::length(n);
	n=glm::normalize(n);

	glm::vec3 corr;
	corr = stiffness * n * (d - restLength) / wSum;

	corr0 = invMass0 * corr;
	corr1 = -invMass1 * corr;
	return true;
}

bool PositionBasedDynamics::solve_DihedralConstraint(
	const glm::vec3& p0, float invMass0,		// angle on (p2, p3) between triangles (p0, p2, p3) and (p1, p3, p2)
	const glm::vec3& p1, float invMass1,
	const glm::vec3& p2, float invMass2,
	const glm::vec3& p3, float invMass3,
	const float restAngle,
	const float stiffness,
	glm::vec3& corr0, glm::vec3& corr1, glm::vec3& corr2, glm::vec3& corr3)
{
	// derivatives from Bridson, Simulation of Clothing with Folds and Wrinkles
	// his modes correspond to the derivatives of the bending angle arccos(n1 dot n2) with correct scaling

	if (invMass0 == 0.0 && invMass1 == 0.0)
		return false;

	glm::vec3 e = p3 - p2;
	float  elen = glm::length(e);
	if (elen < eps)
		return false;

	float invElen = static_cast<float>(1.0) / elen;

	glm::vec3 n1 = glm::cross((p2 - p0), (p3 - p0)); n1 = n1 / (glm::length(n1) * glm::length(n1));
	glm::vec3 n2 = glm::cross((p3 - p1), (p2 - p1)); n2 = n2 / (glm::length(n2) * glm::length(n2));

	glm::vec3 d0 = elen * n1;
	glm::vec3 d1 = elen * n2;
	glm::vec3 d2 = glm::dot((p0 - p3), e) * invElen * n1 + glm::dot((p1 - p3), e) * invElen * n2;
	glm::vec3 d3 = glm::dot((p2 - p0), e) * invElen * n1 + glm::dot((p2 - p1), e) * invElen * n2;

	n1 = glm::normalize(n1);
	n2 = glm::normalize(n2);
	float dot = glm::dot(n1, n2);

	if (dot < -1.0) dot = -1.0;
	if (dot > 1.0) dot = 1.0;
	float phi = acos(dot);

	// Real phi = (-0.6981317 * dot * dot - 0.8726646) * dot + 1.570796;	// fast approximation

	float lambda =
		invMass0 * glm::length(d0)*glm::length(d0) +
		invMass1 * glm::length(d1) * glm::length(d1) +
		invMass2 * glm::length(d2) * glm::length(d2) +
		invMass3 * glm::length(d3) * glm::length(d3);

	if (lambda == 0.0)
		return false;

	// stability
	// 1.5 is the largest magic number I found to be stable in all cases :-)
	//if (stiffness > 0.5 && fabs(phi - b.restAngle) > 1.5)		
	//	stiffness = 0.5;

	lambda = (phi - restAngle) / lambda * stiffness;

	if (glm::dot(glm::cross(n1,n2),e) > 0.0)
		lambda = -lambda;

	corr0 = -invMass0 * lambda * d0;
	corr1 = -invMass1 * lambda * d1;
	corr2 = -invMass2 * lambda * d2;
	corr3 = -invMass3 * lambda * d3;

	return true;
}

bool PositionBasedDynamics::solve_VolumeConstraint(
	const glm::vec3& p0, float invMass0,
	const glm::vec3& p1, float invMass1,
	const glm::vec3& p2, float invMass2,
	const glm::vec3& p3, float invMass3,
	const float restVolume,
	const float stiffness,
	glm::vec3& corr0, glm::vec3& corr1, glm::vec3& corr2, glm::vec3& corr3)

{
	float volume = static_cast<float>(1.0 / 6.0) * glm::dot(glm::cross(p1 - p0, p2 - p0), p3 - p0);

	corr0 = { 0,0,0 }; corr1 = { 0,0,0 }; corr2 = { 0,0,0 }; corr3 = { 0,0,0 };

	if (stiffness == 0.0)
		return false;

	glm::vec3 grad0 = glm::cross(p1 - p2, p3 - p2);
	glm::vec3 grad1 = glm::cross(p2 - p0, p3 - p0);
	glm::vec3 grad2 = glm::cross(p0 - p1, p3 - p1);
	glm::vec3 grad3 = glm::cross(p1 - p0, p2 - p0);

	float lambda =
		invMass0 * glm::length(grad0) * glm::length(grad0) +
		invMass1 * glm::length(grad1) * glm::length(grad1) +
		invMass2 * glm::length(grad2) * glm::length(grad2) +
		invMass3 * glm::length(grad3) * glm::length(grad3);

	if (fabs(lambda) < eps)
		return false;

	lambda = stiffness * (volume - restVolume) / lambda;

	corr0 = -lambda * invMass0 * grad0;
	corr1 = -lambda * invMass1 * grad1;
	corr2 = -lambda * invMass2 * grad2;
	corr3 = -lambda * invMass3 * grad3;

	return true;
}

// ----------------------------------------------------------------------------------------------