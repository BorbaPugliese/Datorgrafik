#include "interpolation.hpp"
#include <iostream>

glm::vec3
interpolation::evalLERP(glm::vec3 const& p0, glm::vec3 const& p1, float const x)
{
	//! \todo Implement this function

	//glm::vec2 linear = glm::vec2(glm::vec3(1.0f), glm::vec3(x)) * glm::mat2(glm::vec2(1, -1), glm::vec2(0, 1)) * glm::vec2(p0, p1);
	//return linear;

	return p0 * (1.f - x) + p1 * x;
	//return glm::vec3();
}

glm::vec3
interpolation::evalCatmullRom(glm::vec3 const& p0, glm::vec3 const& p1,
                              glm::vec3 const& p2, glm::vec3 const& p3,
                              float const t, float const x)
{
	//! \todo Implement this function

	glm::vec4 first_matrix = glm::vec4(1.0f,x, pow(x, 2), pow(x, 3));

	glm::mat4 second_matrix = glm::mat4(glm::vec4(0, -t, 2.f * t, -t), glm::vec4(1, 0, t - 3.f, 2.f - t), glm::vec4(0, t, 3.f - 2.f * t, t - 2.f), glm::vec4(0, 0, -t, t));

	//glm::mat4 second_matrix = glm::mat4(0, -t, 2.f * t, -t,      1, 0, t - 3.f, 2.f - t,      0, t, 3.f - 2.f * t, t - 2.f,     0, 0, -t, t);

	glm::vec4 Xthird_matrix = glm::vec4(p0.x, p1.x, p2.x, p3.x);
	glm::vec4 Ythird_matrix = glm::vec4(p0.y, p1.y, p2.y, p3.y);
	glm::vec4 Zthird_matrix = glm::vec4(p0.z, p1.z, p2.z, p3.z);

	float X = glm::dot(first_matrix * second_matrix, Xthird_matrix);
	float Y = glm::dot(first_matrix * second_matrix, Ythird_matrix);
	float Z = glm::dot(first_matrix * second_matrix, Zthird_matrix);

	return glm::vec3(X,Y,Z);

}
