#pragma once
#include <glm/glm.hpp>

namespace PBD
{
	class PositionBasedDynamics
	{
	public:

		// -------------- standard PBD -----------------------------------------------------

		/** Determine the position corrections for a distance constraint between two particles:\n\n
		* \f$C(\mathbf{p}_0, \mathbf{p}_1) = \| \mathbf{p}_0 - \mathbf{p}_1\| - l_0 = 0\f$\n\n
		* More information can be found in the following papers: \cite Mueller07, \cite BMOT2013, \cite BMOTM2014, \cite BMM2015,
		*
		* @param p0 position of first particle
		* @param invMass0 inverse mass of first particle
		* @param p1 position of second particle
		* @param invMass1 inverse mass of second particle
		* @param restLength rest length of distance constraint
		* @param stiffness stiffness coefficient
		* @param corr0 position correction of first particle
		* @param corr1 position correction of second particle
		*/
		static bool solve_DistanceConstraint(
			const glm::vec3& p0, float invMass0,
			const glm::vec3& p1, float invMass1,
			const float restLength,
			const float stiffness,
			glm::vec3& corr0, glm::vec3& corr1);



		/** Determine the position corrections for a dihedral bending constraint.
		 * For a pair of adjacent triangles
		 * \f$(\mathbf{p}_1, \mathbf{p}_3, \mathbf{p}_2)\f$ and
		 * \f$(\mathbf{p}_1, \mathbf{p}_2, \mathbf{p}_4)\f$
		 * with the common edge \f$(\mathbf{p}_3, \mathbf{p}_4)\f$ a bilateral bending
		 * constraint is added by the constraint function
		 * \f{equation*}{
		 * C_{bend}(\mathbf{p}_1, \mathbf{p}_2,\mathbf{p}_3, \mathbf{p}_4) =
		 * \text{acos}\left( \frac{\mathbf{p}_{2,1} \times
		 * \mathbf{p}_{3,1}}{|\mathbf{p}_{2,1} \times
		 * \mathbf{p}_{3,1}|} \cdot
		 * \frac{\mathbf{p}_{2,1} \times
		 * \mathbf{p}_{4,1}}{|\mathbf{p}_{2,1} \times
		 *	\mathbf{p}_{4,1}|}\right)-\varphi_0
		 * \f}
		 * and stiffness \f$k_{bend}\f$. The scalar \f$\varphi_0\f$
		 * is the initial dihedral angle between the two triangles and
		 * \f$k_{bend}\f$ is a global user parameter defining the bending stiffness.\n\n
		 * More information can be found in the following papers: \cite Mueller07, \cite BMOT2013, \cite BMOTM2014, \cite BMM2015,
		 *
		 * @param p0 position of first particle
		 * @param invMass0 inverse mass of first particle
		 * @param p1 position of second particle
		 * @param invMass1 inverse mass of second particle
		 * @param p2 position of third particle
		 * @param invMass2 inverse mass of third particle
		 * @param p3 position of fourth particle
		 * @param invMass3 inverse mass of fourth particle
		 * @param restAngle rest angle \f$\varphi_0\f$
		 * @param stiffness stiffness coefficient
		 * @param corr0 position correction of first particle
		 * @param corr1 position correction of second particle
		 * @param corr2 position correction of third particle
		 * @param corr3 position correction of fourth particle
		 */
		static bool solve_DihedralConstraint(
			const glm::vec3& p0, float invMass0,		// angle on (p2, p3) between triangles (p0, p2, p3) and (p1, p3, p2)
			const glm::vec3& p1, float invMass1,
			const glm::vec3& p2, float invMass2,
			const glm::vec3& p3, float invMass3,
			const float restAngle,
			const float stiffness,
			glm::vec3& corr0, glm::vec3& corr1, glm::vec3& corr2, glm::vec3& corr3);



		/** Determine the position corrections for a constraint that conserves the volume
		* of single tetrahedron. Such a constraint has the form
		* \f{equation*}{
		* C(\mathbf{p}_1, \mathbf{p}_2, \mathbf{p}_3, \mathbf{p}_4) = \frac{1}{6}
		* \left(\mathbf{p}_{2,1} \times \mathbf{p}_{3,1}\right) \cdot \mathbf{p}_{4,1} - V_0,
		* \f}
		* where \f$\mathbf{p}_1\f$, \f$\mathbf{p}_2\f$, \f$\mathbf{p}_3\f$ and \f$\mathbf{p}_4\f$
		* are the four corners of the tetrahedron and \f$V_0\f$ is its rest volume.\n\n
		* More information can be found in the following papers: \cite Mueller07, \cite BMOT2013, \cite BMOTM2014, \cite BMM2015,
		*
		* @param p0 position of first particle
		* @param invMass0 inverse mass of first particle
		* @param p1 position of second particle
		* @param invMass1 inverse mass of second particle
		* @param p2 position of third particle
		* @param invMass2 inverse mass of third particle
		* @param p3 position of fourth particle
		* @param invMass3 inverse mass of fourth particle
		* @param restVolume rest angle \f$V_0\f$
		* @param stiffness stiffness coefficient
		* @param corr0 position correction of first particle
		* @param corr1 position correction of second particle
		* @param corr2 position correction of third particle
		* @param corr3 position correction of fourth particle
		*/
		static bool solve_VolumeConstraint(
			const glm::vec3& p0, float invMass0,
			const glm::vec3& p1, float invMass1,
			const glm::vec3& p2, float invMass2,
			const glm::vec3& p3, float invMass3,
			const float restVolume,
			const float stiffness,
			glm::vec3& corr0, glm::vec3& corr1, glm::vec3& corr2, glm::vec3& corr3);


		/** Determine the position corrections for a constraint that preserves a
		 * rest distance between a point and an edge.
		 *
		 * @param  p position of point particle
		 * @param  invMass inverse mass of point particle
		 * @param  p0 position of first edge particle
		 * @param  invMass0 inverse mass of first edge particle
		 * @param  p1 position of second edge particle
		 * @param  invMass1 inverse mass of second edge particle
		 * @param  restDist rest distance of point and edge
		 * @param  compressionStiffness stiffness coefficient for compression
		 * @param  stretchStiffness stiffness coefficient for stretching
		 * @param  corr position correction of point particle
		 * @param  corr0 position correction of first edge particle
		 * @param  corr1 position correction of second edge particle
		 */
		static bool solve_EdgePointDistanceConstraint(
			const glm::vec3& p, float invMass,
			const glm::vec3& p0, float invMass0,
			const glm::vec3& p1, float invMass1,
			const float restDist,
			const float compressionStiffness,
			const float stretchStiffness,
			glm::vec3& corr, glm::vec3& corr0, glm::vec3& corr1);


		/** Determine the position corrections for a constraint that preserves a
		* rest distance between a point and a triangle.
		*
		* @param  p position of point particle
		* @param  invMass inverse mass of point particle
		* @param  p0 position of first triangle particle
		* @param  invMass0 inverse mass of first triangle particle
		* @param  p1 position of second triangle particle
		* @param  invMass1 inverse mass of second triangle particle
		* @param  p2 position of third triangle particle
		* @param  invMass2 inverse mass of third triangle particle
		* @param  restDist rest distance of point and triangle
		* @param  compressionStiffness stiffness coefficient for compression
		* @param  stretchStiffness stiffness coefficient for stretching
		* @param  corr position correction of point particle
		* @param  corr0 position correction of first triangle particle
		* @param  corr1 position correction of second triangle particle
		* @param  corr2 position correction of third triangle particle
		*/
		static bool solve_TrianglePointDistanceConstraint(
			const glm::vec3& p, float invMass,
			const glm::vec3& p0, float invMass0,
			const glm::vec3& p1, float invMass1,
			const glm::vec3& p2, float invMass2,
			const float restDist,
			const float compressionStiffness,
			const float stretchStiffness,
			glm::vec3& corr, glm::vec3& corr0, glm::vec3& corr1, glm::vec3& corr2);


		/** Determine the position corrections for a constraint that preserves a
		* rest distance between two edges.
		*
		* @param  p0 position of first particle of edge 0
		* @param  invMass0 inverse mass of first particle of edge 0
		* @param  p1 position of second particle of edge 0
		* @param  invMass1 inverse mass of second particle of edge 0
		* @param  p2 position of first particle of edge 1
		* @param  invMass2 inverse mass of first particle of edge 1
		* @param  p3 position of second particle of edge 1
		* @param  invMass3 inverse mass of second particle of edge 1
		* @param  restDist rest distance between both edges
		* @param  compressionStiffness stiffness coefficient for compression
		* @param  stretchStiffness stiffness coefficient for stretching
		* @param  corr0 position correction of first particle of edge 0
		* @param  corr1 position correction of second particle of edge 0
		* @param  corr2 position correction of first particle of edge 1
		* @param  corr3 position correction of second particle of edge 1
		*/
		static bool solve_EdgeEdgeDistanceConstraint(
			const glm::vec3& p0, float invMass0,
			const glm::vec3& p1, float invMass1,
			const glm::vec3& p2, float invMass2,
			const glm::vec3& p3, float invMass3,
			const float restDist,
			const float compressionStiffness,
			const float stretchStiffness,
			glm::vec3& corr0, glm::vec3& corr1, glm::vec3& corr2, glm::vec3& corr3);

		// -------------- Isometric bending -----------------------------------------------------

		/** Initialize the local stiffness matrix Q. The matrix is
		 * required by the solver step. It must only be recomputed
		 * if the rest shape changes. \n\n
		 * Bending is simulated for the angle on (p2, p3) between
		 * the triangles (p0, p2, p3) and (p1, p3, p2).
		 *
		 * @param  p0 point 0 of stencil
		 * @param  p1 point 1 of stencil
		 * @param  p2 point 2 of stencil
		 * @param  p3 point 3 of stencil
		 * @param  Q returns the local stiffness matrix which is required by the solver
		 */
		static bool init_IsometricBendingConstraint(
			const glm::vec3& p0,
			const glm::vec3& p1,
			const glm::vec3& p2,
			const glm::vec3& p3,
			glm::mat4& Q
		);


		/** Determine the position corrections for the isometric bending constraint.
		 * This constraint can be used for almost inextensible surface models.\n\n
		 * More information can be found in: \cite BMM2015, \cite Bender2014
		 *
		 * @param p0 position of first particle
		 * @param invMass0 inverse mass of first particle
		 * @param p1 position of second particle
		 * @param invMass1 inverse mass of second particle
		 * @param p2 position of third particle
		 * @param invMass2 inverse mass of third particle
		 * @param p3 position of fourth particle
		 * @param invMass3 inverse mass of fourth particle
		 * @param  Q local stiffness matrix which must be initialized by calling init_IsometricBendingConstraint()
		 * @param  stiffness stiffness coefficient for bending
		 * @param corr0 position correction of first particle
		 * @param corr1 position correction of second particle
		 * @param corr2 position correction of third particle
		 * @param corr3 position correction of fourth particle
		 */
		static bool solve_IsometricBendingConstraint(
			const glm::vec3& p0, float invMass0,		// angle on (p2, p3) between triangles (p0, p2, p3) and (p1, p3, p2)
			const glm::vec3& p1, float invMass1,
			const glm::vec3& p2, float invMass2,
			const glm::vec3& p3, float invMass3,
			const glm::vec3& Q,
			const float stiffness,
			glm::vec3& corr0, glm::vec3& corr1, glm::vec3& corr2, glm::vec3& corr3);

		// -------------- Shape Matching  -----------------------------------------------------

		/** Initialize rest configuration infos for one shape matching cluster
		 * which are required by the solver step. It must only be reinitialized
		 * if the rest shape changes.
		 *
		 * @param  x0 rest configuration of all particles in the cluster
		 * @param  invMasses inverse masses of all particles in the cluster
		 * @param  numPoints number of particles in the cluster
		 * @param  restCm returns the center of mass of the rest configuration
		 */
		static bool init_ShapeMatchingConstraint(
			const glm::vec3 x0[], const float invMasses[], const int numPoints,
			glm::vec3& restCm);


		/** Determine the position corrections for a shape matching constraint.\n\n
		 * More information can be found in: \cite BMM2015, \cite BMOT2013, \cite BMOTM2014,
		 * \cite Muller2005, \cite Bender2013, \cite Diziol2011
		 *
		 * @param  x0			rest configuration of all particles in the cluster
		 * @param  x			current configuration of all particles in the cluster
		 * @param  invMasses	invMasses inverse masses of all particles in the cluster
		 * @param  numPoints	number of particles in the cluster
		 * @param  restCm		center of mass of the rest configuration
		 * @param  stiffness	stiffness coefficient
		 * @param  allowStretch allow stretching
		 * @param  corr			position corrections for all particles in the cluster
		 * @param  rot			returns determined rotation matrix
		 */
		static bool solve_ShapeMatchingConstraint(
			const glm::vec3 x0[], const glm::vec3 x[], const float invMasses[], const int numPoints,
			const glm::vec3& restCm,
			const float stiffness,
			const bool allowStretch,		// default false
			glm::vec3 corr[], glm::mat3* rot = NULL);


		// -------------- Strain Based Dynamics  -----------------------------------------------------

		/** Initialize rest configuration infos which are required by the solver step.
		 * Recomputation is only necessary when rest shape changes.\n\n
		 * The triangle is defined in the xy plane.
		 *
		 * @param  p0 point 0 of triangle
		 * @param  p1 point 1 of triangle
		 * @param  p2 point 2 of triangle
		 * @param  invRestMat returns a matrix required by the solver
		 */
		static bool init_StrainTriangleConstraint(
			const glm::vec3& p0,
			const glm::vec3& p1,
			const glm::vec3& p2,
			glm::mat2& invRestMat
		);

		/** Solve triangle constraint with strain based dynamics and return position corrections.\n\n
		 * More information can be found in: \cite BMM2015, \cite Mueller2014
		 *
		 * @param p0 position of first particle
		 * @param invMass0 inverse mass of first particle
		 * @param p1 position of second particle
		 * @param invMass1 inverse mass of second particle
		 * @param p2 position of third particle
		 * @param invMass2 inverse mass of third particle
		 * @param  invRestMat precomputed matrix determined by init_StrainTriangleConstraint()
		 * @param  xxStiffness stiffness coefficient for xx stretching
		 * @param  yyStiffness stiffness coefficient for yy stretching
		 * @param  xyStiffness stiffness coefficient for xy shearing
		 * @param  normalizeStretch	should stretching be normalized
		 * @param  normalizeShear should shearing be normalized
		 * @param  corr0 position correction for point 0
		 * @param  corr1 position correction for point 1
		 * @param  corr2 position correction for point 2
		 */
		static bool solve_StrainTriangleConstraint(
			const glm::vec3& p0, float invMass0,
			const glm::vec3& p1, float invMass1,
			const glm::vec3& p2, float invMass2,
			const glm::mat4& invRestMat,
			const float xxStiffness,
			const float yyStiffness,
			const float xyStiffness,
			const bool normalizeStretch,	// use false as default
			const bool normalizeShear,		// use false as default
			glm::vec3& corr0, glm::vec3& corr1, glm::vec3& corr2);


		/** Initialize rest configuration infos which are required by the solver step.
		 * Recomputation is only necessary when rest shape changes.
		 *
		 * @param  p0 point 0 of tet
		 * @param  p1 point 1 of tet
		 * @param  p2 point 2 of tet
		 * @param  p3 point 3 of tet
		 * @param  invRestMat returns a matrix required by the solver
		 */
		static bool init_StrainTetraConstraint(
			const glm::vec3& p0,
			const glm::vec3& p1,
			const glm::vec3& p2,
			const glm::vec3& p3,
			glm::mat3& invRestMat
		);

		// has no inversion handling. Possible simple solution: if the volume is negative, 
		// scale corrections down and use the volume constraint to fix the volume sign
		static bool solve_StrainTetraConstraint(
			const glm::vec3& p0, float invMass0,
			const glm::vec3& p1, float invMass1,
			const glm::vec3& p2, float invMass2,
			const glm::vec3& p3, float invMass3,
			const glm::mat3& invRestMat,
			const glm::vec3& stretchStiffness,			// xx, yy, zz
			const glm::vec3& shearStiffness,			// xy, xz, yz
			const bool normalizeStretch,		// use false as default
			const bool normalizeShear,		// use false as default
			glm::vec3& corr0, glm::vec3& corr1, glm::vec3& corr2, glm::vec3& corr3);

		/** Initialize contact between a particle and a tetrahedron and return
		* info which is required by the solver step.
		*
		* @param invMass0 inverse mass of particle which collides with tet
		* @param x0 particle position
		* @param v0 particle velocity
		* @param invMass inverse masses of tet particles
		* @param x positions of tet particles
		* @param v velocities of tet particles
		* @param bary barycentric coordinates of contact point in tet
		* @param normal contact normal in body 1
		* @param constraintInfo Stores the local and global position of the contact points and
		* the contact normal. \n
		* The joint info contains the following columns:\n
		* 0:	contact normal in body 1 (global)\n
		* 1:	contact tangent (global)\n
		* 0,2:   1.0 / normal^T * K * normal\n
		* 1,2:  maximal impulse in tangent direction\n
		*/
		static bool init_ParticleTetContactConstraint(
			const float invMass0,							// inverse mass is zero if particle is static
			const glm::vec3& x0,								// particle which collides with tet
			const glm::vec3& v0,								// velocity of particle
			const float invMass[],							// inverse masses of tet particles
			const glm::vec3 x[],								// positions of tet particles
			const glm::vec3 v[],								// velocities of tet particles
			const glm::vec3& bary,							// barycentric coordinates of contact point in tet
			const glm::vec3& normal,							// contact normal in body 1
			glm::mat3& constraintInfo);


		/** Perform a solver step for a contact constraint between a particle and a tetrahedron.
			* A contact constraint handles collisions and resting contacts between the bodies.
			* The contact info must be generated in each time step.
			*
			* @param invMass0 inverse mass of particle which collides with tet
			* @param x0 particle position
			* @param invMass inverse masses of tet particles
			* @param x positions of tet particles
			* @param bary barycentric coordinates of contact point in tet
			* @param constraintInfo information which is required by the solver. This
			* information must be generated in the beginning by calling init_RigidBodyContactConstraint().
			* @param corr0 position correction of particle
			* @param corr position corrections of tet particles
			*/
		static bool solve_ParticleTetContactConstraint(
			const float invMass0,							// inverse mass is zero if particle is static
			const glm::vec3& x0,								// particle which collides with tet
			const float invMass[],							// inverse masses of tet particles
			const glm::vec3 x[],								// positions of tet particles
			const glm::vec3& bary,							// barycentric coordinates of contact point in tet
			glm::mat3& constraintInfo,		// precomputed contact info
			float& lambda,
			glm::vec3& corr0,
			glm::vec3 corr[]);

		/** Perform a solver step for a contact constraint between a particle and a tetrahedron.
			* A contact constraint handles collisions and resting contacts between the bodies.
			* The contact info must be generated in each time step.
			*
			* @param invMass0 inverse mass of particle which collides with tet
			* @param x0 particle position
			* @param v0 particle velocity
			* @param invMass inverse masses of tet particles
			* @param x positions of tet particles
			* @param v velocities of tet particles
			* @param bary barycentric coordinates of contact point in tet
			* @param frictionCoeff friction coefficient
			* @param constraintInfo information which is required by the solver. This
			* information must be generated in the beginning by calling init_RigidBodyContactConstraint().
			* @param corr_v0 velocity correction of particle
			* @param corr_v velocity corrections of tet particles
			*/
		static bool velocitySolve_ParticleTetContactConstraint(
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
			glm::vec3 corr_v[]);

	};

	

}