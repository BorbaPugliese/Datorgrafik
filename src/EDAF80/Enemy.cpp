#include "Enemy.hpp"
#include "interpolation.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/trigonometric.hpp>

#include "core/helpers.hpp"
#include "core/Log.h"

Enemy::Enemy(glm::vec3 starting_pos) {
	auto enemy_shape = bonobo::loadObjects(config::resources_path("scenes/Enemy_Alien.obj"));
	
	node.set_geometry(enemy_shape[0]);
	GLuint enemy_texture = bonobo::loadTexture2D(config::resources_path("textures/T_SEEKER_ORM_JPG.jpg"));
	node.add_texture("enemy_tex", enemy_texture, GL_TEXTURE_2D);

	moveTo(starting_pos);
}

void Enemy::moveTo(glm::vec3 destination) {
	position = destination;
	node.get_transform().SetTranslate(position);
}

void Enemy::moveTowardPlayer(glm::vec3 playerPos) {
	glm::vec3 enemyToPlayer = normalize(playerPos - position);

	float step_size = 0.5;
	glm::mat3 step_matrix = glm::mat3(step_size);
	glm::vec3 step = position + enemyToPlayer *step_size;
	moveTo(step);
}

void Enemy::render(glm::mat4 const& view_projection,
				   GLuint const* const program,
				   std::function<void(GLuint)> const& set_uniforms)
{
	node.set_program(program, set_uniforms);
	node.render(view_projection, glm::mat4(1));
}
