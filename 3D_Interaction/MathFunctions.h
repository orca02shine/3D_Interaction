#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include<glm/gtx/quaternion.hpp>

namespace PBD
{
	class MathFunctions
	{
	private:
		static void jacobiRotate(glm::mat3& A,
			glm::mat3& R,
			int p,
			int q);

	public:
		static float infNorm(const glm::mat3& A);
		static float oneNorm(const glm::mat3& A);

		static void Decomposition(const glm::mat3& A,
			glm::mat3& eigenVecs,
			glm::vec3& eigenVals);

		static void polarDecomposition(const glm::mat3& A,
			glm::mat3& R,
			glm::mat3& U,
			glm::mat3& D);

		static void polarDecompositionStable(const glm::mat3& M,
			const float tolerance,
			glm::mat3& R);

		static void svdWithInversionHandling(const glm::mat3& A,
			glm::vec3& sigma,
			glm::mat3& U,
			glm::mat3& VT);

		static float cotTheta(const glm::vec3& v, const glm::vec3& w);

		/** Computes the cross product matrix of a vector.
		 * @param  v		input vector
		 * @param  v_hat	resulting cross product matrix
		 */
		static void crossProductMatrix(const glm::vec3& v, glm::mat3& v_hat);

		/** Implementation of the paper: \n
		 * Matthias M?ller, Jan Bender, Nuttapong Chentanez and Miles Macklin,
		 * "A Robust Method to Extract the Rotational Part of Deformations",
		 * ACM SIGGRAPH Motion in Games, 2016
		 */
		static void extractRotation(const glm::mat3& A, glm::quat& q, const unsigned int maxIter);
	};
}
