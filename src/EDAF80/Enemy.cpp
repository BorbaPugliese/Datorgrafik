#include "Enemy.hpp"
#include "interpolation.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/trigonometric.hpp>

#include "core/helpers.hpp"
#include "core/Log.h"

Enemy::Enemy(glm::vec3 starting_pos) {
	auto enemy_shape = bonobo::loadObjects(config::resources_path("scenes/Enemy_Alien.obj"));
	
	node.set_geometry(enemy_shape[0]);
	GLuint enemy_texture = bonobo::loadTexture2D(config::resources_path("textures/T_SEEKER_ORM_JPG.jpeg"));
	node.add_texture("enemy_tex", enemy_texture, GL_TEXTURE_2D);
	moveTo(starting_pos);
}

//GLuint enemy_texture = bonobo::loadTexture2D(config::resources_path("textures/T_SEEKER_ORM_JPG.jpg"));
//Enemy.add_texture("enemy_tex", enemy_texture, GL_TEXTURE_2D);

void Enemy::moveTo(glm::vec3 destination) {
	position = destination;
	node.get_transform().SetTranslate(position);
}

void Enemy::moveTowardPlayer(glm::vec3 playerPos) {
	glm::vec3 enemyToPlayer = normalize(playerPos - position);

	float step_size = 0.4;
	glm::mat3 step_matrix = glm::mat3(step_size);
	glm::vec3 step = position + enemyToPlayer *step_size;
	moveTo(step);

	float d = dot(glm::vec2(1, 0), normalize(glm::vec2(enemyToPlayer.x, enemyToPlayer.z)));
	float facing_angle = std::acos(d);
	if (enemyToPlayer.z > 0) {
		facing_angle *= -1;
	}
	node.get_transform().SetRotateY(facing_angle);
}

void Enemy::render(glm::mat4 const& view_projection,
				   GLuint const* const program,
				   std::function<void(GLuint)> const& set_uniforms)
{
	node.set_program(program, set_uniforms);
	node.render(view_projection, glm::mat4(1));
}

void Enemy::shooting(int &points, glm::vec3 pv, glm::vec3 v) {

	glm::vec3 ps = position;
	glm::vec3 u = (ps - pv);
	glm::vec3 rejection = u - v * (dot(u, v));

	float magnitude = sqrt((rejection.x * rejection.x) + (rejection.y * rejection.y) + (rejection.z * rejection.z));
	int r = 20;

	auto spawn_locations = std::vector<glm::vec3>();
	spawn_locations.push_back(glm::vec3(250, 0, 0));
	spawn_locations.push_back(glm::vec3(-250, 0, 0));
	spawn_locations.push_back(glm::vec3(0, 0, 250));
	spawn_locations.push_back(glm::vec3(0, 0, -250));

	//If hits the enemy
	if (magnitude < r && (glm::length(ps)<200)) {
		//Remove the enemy and gain points
		int random_index = rand() % 4;
		float random_multiplier = rand() % 10/10 + 1; //to get a number between 1 and 2

		moveTo(spawn_locations[random_index] * random_multiplier);	 // Here we call the function to remove the enemy
		points = points + 1;
		std::cout << "points:" << points << std::endl;
		
	}
}
bool Enemy::playerAlive(glm::vec3 v) {

	glm::vec3 ps = position;

	glm::vec3 distance = ps - v;

	if (glm::length(distance) < 20) {
		return false;
	}
	else {
		return true;
	}
}

