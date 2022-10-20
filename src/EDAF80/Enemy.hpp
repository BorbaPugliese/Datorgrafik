#include <glm/vec3.hpp>
#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>

#include "config.hpp"
#include "core/Bonobo.h"
#include "core/helpers.hpp"
#include "core/node.hpp"
#include "core/ShaderProgramManager.hpp"

class Enemy
{
public:
	//default constructor for an enemy
	// add any parameters you need here
	Enemy(glm::vec3 starting_pos);

	// move enemy to passed vector
	void moveTo(glm::vec3 destination);

	// uses LERP to update the position of the enemy
	void moveTowardPlayer(glm::vec3 playerPos);

	// put whatever rendering stuff you need in here
	void render(glm::mat4 const& view_projection, GLuint const* const program, std::function<void(GLuint)> const& set_uniforms);


private:
	Node node;

	glm::vec3 position;
	glm::vec3 facing;
};
