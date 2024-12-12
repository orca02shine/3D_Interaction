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

bool PositionBasedDynamics::solve_ParticleTetContactConstraint(
	const float invMass0,							// inverse mass is zero if particle is static
	const glm::vec3& x0,								// particle which collides with tet
	const float invMass[],							// inverse masses of tet particles
	const glm::vec3 x[],								// positions of tet particles
	const glm::vec3& bary,							// barycentric coordinates of contact point in tet
	glm::mat3& constraintInfo,		// precomputed contact info
	float& lambda,
	glm::vec3& corr0,
	glm::vec3 corr[])
{
	// constraintInfo contains
	// 0:	contact normal in body 1 (global)
	// 1:	contact tangent (global)
	// 0,2:  1.0 / normal^T * K * normal
	// 1,2: maximal impulse in tangent direction

	if ((invMass0 == 0.0) && (invMass[0] == 0.0) && (invMass[1] == 0.0) && (invMass[2] == 0.0))
		return false;

	const float bary0 = static_cast<float>(1.0) - bary[0] - bary[1] - bary[2];

	// compute world space contact point in body 2	
	const glm::vec3 cp1 = bary0 * x[0] + bary[0] * x[1] + bary[1] * x[2] + bary[2] * x[3];

	const glm::vec3& normal = glm::vec3{constraintInfo[0][0],constraintInfo[1][0],constraintInfo[2][0]};

	// 1.0 / normal^T * K * normal
	const float nKn_inv = constraintInfo[0][2];

	// penetration depth 
	const float C = glm::dot(normal,(x0-cp1));

	lambda = -nKn_inv * C;


	glm::vec3 p = lambda * normal;
	if (invMass0 != 0.0)
	{
		corr0 = invMass0 * p;
	}

	if (invMass[0] != 0.0)
		corr[0] = -invMass[0] * bary0 * p;
	if (invMass[1] != 0.0)
		corr[1] = -invMass[1] * bary[0] * p;
	if (invMass[2] != 0.0)
		corr[2] = -invMass[2] * bary[1] * p;
	if (invMass[3] != 0.0)
		corr[3] = -invMass[3] * bary[2] * p;

	return true;
}
bool PositionBasedDynamics::velocitySolve_ParticleTetContactConstraint(
	const float invMass0,							// inverse mass is zero if particle is static
	const glm::vec3& x0,								// particle which collides with tet
	const glm::vec3& v0,								// velocity of particle
	const float invMass[],							// inverse masses of tet particles
	const glm::vec3 x[],								// positions of tet particles
	const glm::vec3 v[],								// velocities of tet particles
	const glm::vec3& bary,							// barycentric coordinates of contact point in tet
	const float lambda,
	const float frictionCoeff,						// friction coefficient
	glm::mat3& constraintInfo,		// precomputed contact info
	glm::vec3& corr_v0,
	glm::vec3 corr_v[])
{
	// constraintInfo contains
	// 0:	contact normal in body 1 (global)
	// 1:	contact tangent (global)
	// 0,2:  1.0 / normal^T * K * normal
	// 1,2: maximal impulse in tangent direction

	if ((invMass0 == 0.0) && (invMass[0] == 0.0) && (invMass[1] == 0.0) && (invMass[2] == 0.0))
		return false;

	const float bary0 = static_cast<float>(1.0) - bary[0] - bary[1] - bary[2];

	// Friction
	// maximal impulse in tangent direction
	const float pMax = constraintInfo[1][2];
	const glm::vec3& tangent = glm::vec3{ constraintInfo[0][1],constraintInfo[1][1],constraintInfo[2][1] };;
	glm::vec3 pv = { 0,0,0 };
	if (frictionCoeff * lambda > pMax)
		pv = -pMax * tangent;
	else if (frictionCoeff * lambda < -pMax)
		pv = pMax * tangent;
	else
		pv = -frictionCoeff * lambda * tangent;

	if (invMass0 != 0.0)
	{
		corr_v0 = invMass0 * pv;
	}

	if (invMass[0] != 0.0)
		corr_v[0] = -invMass[0] * bary0 * pv;
	if (invMass[1] != 0.0)
		corr_v[1] = -invMass[1] * bary[0] * pv;
	if (invMass[2] != 0.0)
		corr_v[2] = -invMass[2] * bary[1] * pv;
	if (invMass[3] != 0.0)
		corr_v[3] = -invMass[3] * bary[2] * pv;


	return true;
}