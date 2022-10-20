#include "assignment5.hpp"
#include "parametric_shapes.hpp"

#include "config.hpp"
#include "core/Bonobo.h"
#include "core/FPSCamera.h"
#include "core/helpers.hpp"
#include "core/node.hpp"
#include "core/ShaderProgramManager.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>
#include <tinyfiledialogs.h>

#include <clocale>
#include <stdexcept>

edaf80::Assignment5::Assignment5(WindowManager& windowManager) :
	mCamera(0.5f * glm::half_pi<float>(),
	        static_cast<float>(config::resolution_x) / static_cast<float>(config::resolution_y),
	        0.01f, 1000.0f),
	inputHandler(), mWindowManager(windowManager), window(nullptr)
{
	WindowManager::WindowDatum window_datum{ inputHandler, mCamera, config::resolution_x, config::resolution_y, 0, 0, 0, 0};

	window = mWindowManager.CreateGLFWWindow("EDAF80: Assignment 5", window_datum, config::msaa_rate);
	if (window == nullptr) {
		throw std::runtime_error("Failed to get a window: aborting!");
	}

	bonobo::init();
}

edaf80::Assignment5::~Assignment5()
{
	bonobo::deinit();
}

void
edaf80::Assignment5::run()
{
	// Set up the camera
	mCamera.mWorld.SetTranslate(glm::vec3(0.0f, 0.0f, 6.0f));
	mCamera.mWorld.LookAt(glm::vec3(0.0f));
	mCamera.mMouseSensitivity = glm::vec2(0.003f);
	mCamera.mMovementSpeed = glm::vec3(10.0f); // 3 m/s => 10.8 km/h
	auto camera_position = mCamera.mWorld.GetTranslation();

	// Create the shader programs
	ShaderProgramManager program_manager;
	GLuint fallback_shader = 0u;
	program_manager.CreateAndRegisterProgram("Fallback",
	                                         { { ShaderType::vertex, "common/fallback.vert" },
	                                           { ShaderType::fragment, "common/fallback.frag" } },
	                                         fallback_shader);
	if (fallback_shader == 0u) {
		LogError("Failed to load fallback shader");
		return;
	}

	GLuint skybox_shader = 0u;
	program_manager.CreateAndRegisterProgram("Skybox",
		{ { ShaderType::vertex, "EDAF80/cube_doors.vert" },
		  { ShaderType::fragment, "EDAF80/cube_doors.frag" } },
		skybox_shader);
	if (skybox_shader == 0u)
		LogError("Failed to load skybox shader");

	GLuint enemy_shader = 0u;
	program_manager.CreateAndRegisterProgram("Enemy",
		{ { ShaderType::vertex, "EDAF80/enemy.vert" },
		  { ShaderType::fragment, "EDAF80/enemy.frag" } },
		enemy_shader);
	if (enemy_shader == 0u)
		LogError("Failed to load enemy shader");

	auto const light_position = glm::vec3(-2.0f, 4.0f, 2.0f);
	bool use_normal_mapping = false;
	float elapsed_time_s = 0.0f;
	auto const set_uniforms = [&use_normal_mapping, &light_position, &camera_position, &elapsed_time_s](GLuint program) {
		glUniform1i(glGetUniformLocation(program, "use_normal_mapping"), use_normal_mapping ? 1 : 0);
		glUniform3fv(glGetUniformLocation(program, "light_position"), 1, glm::value_ptr(light_position));
		glUniform3fv(glGetUniformLocation(program, "camera_position"), 1, glm::value_ptr(camera_position));
		glUniform1f(glGetUniformLocation(program, "t"), elapsed_time_s);
	};

	auto skybox_shape = parametric_shapes::createSphere(200.0f, 1000u, 1000u);
	if (skybox_shape.vao == 0u) {
		LogError("Failed to retrieve the mesh for the skybox");
		return;
	}

	Node skybox;

	GLuint cubemap = bonobo::loadTextureCubeMap(
		config::resources_path("cubemaps/Door/Spawn_door.jpg"),
		config::resources_path("cubemaps/Door/Spawn_door.jpg"),
		config::resources_path("cubemaps/Door/Spawn_door.jpg"),
		config::resources_path("cubemaps/Door/Spawn_door.jpg"),
		config::resources_path("cubemaps/Door/Spawn_door.jpg"),
		config::resources_path("cubemaps/Door/Spawn_door.jpg"));

	skybox.set_geometry(skybox_shape);
	skybox.add_texture("cubemap", cubemap, GL_TEXTURE_CUBE_MAP);

	auto enemy_shape = bonobo::loadObjects(config::resources_path("scenes/Enemy_Alien.obj"));
	Node Enemy;
	Enemy.set_geometry(enemy_shape[0]);
	GLuint enemy_texture = bonobo::loadTexture2D(config::resources_path("textures/T_SEEKER_ORM_JPG.jpg"));
	Enemy.add_texture("enemy_tex", enemy_texture, GL_TEXTURE_2D);



	//
	// Todo: Insert the creation of other shader programs.
	//       (Check how it was done in assignment 3.)
	

	//
	// Todo: Load your geometry
	//

	glClearDepthf(1.0f);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glEnable(GL_DEPTH_TEST);


	auto lastTime = std::chrono::high_resolution_clock::now();

	bool show_logs = true;
	bool show_gui = true;
	bool shader_reload_failed = false;
	bool show_basis = false;
	float basis_thickness_scale = 1.0f;
	float basis_length_scale = 1.0f;

	int collision(glm::vec3 u, glm::vec3 v) {
		return = u - v * (dot(u, v));
	}

	int points = 0;
	int r = 20; //Need to calculate 

	//CTRL+K+U uncomment
	//enum State {
	//	NEW_GAME, PLAY_GAME, END_GAME,
	//};
	//State current_state = NEW_GAME;

	//switch (current_state) {
	//case NEW_GAME:
	//	// Do first time setup of variables here
	//	// Prepare for a new round
	//		current_state = PLAY_GAME;
	//	break
	//		;
	//case PLAY_GAME:
	//	// Game logic here
	//	// Control input, physics update, render
	//		if (player_dead) {
	//			current_state = END_GAME;
	//		}
	//	break
	//		;
	//case END_GAME:
	//	// Deal with showing high - scores
	//		// Ask if the player wants to restart
	//		if (restart) {
	//			current_state = NEW_GAME;
	//		}

	while (!glfwWindowShouldClose(window)) {
		auto const nowTime = std::chrono::high_resolution_clock::now();
		auto const deltaTimeUs = std::chrono::duration_cast<std::chrono::microseconds>(nowTime - lastTime);
		lastTime = nowTime;
		elapsed_time_s += std::chrono::duration<float>(deltaTimeUs).count();
		auto& io = ImGui::GetIO();
		inputHandler.SetUICapture(io.WantCaptureMouse, io.WantCaptureKeyboard);

		glfwPollEvents();
		inputHandler.Advance();
		mCamera.Update(deltaTimeUs, inputHandler);
		camera_position = mCamera.mWorld.GetTranslation();

		if (inputHandler.GetKeycodeState(GLFW_KEY_R) & JUST_PRESSED) {
			shader_reload_failed = !program_manager.ReloadAllPrograms();
			if (shader_reload_failed)
				tinyfd_notifyPopup("Shader Program Reload Error",
				                   "An error occurred while reloading shader programs; see the logs for details.\n"
				                   "Rendering is suspended until the issue is solved. Once fixed, just reload the shaders again.",
				                   "error");
		}
		if (inputHandler.GetKeycodeState(GLFW_KEY_F3) & JUST_RELEASED)
			show_logs = !show_logs;
		if (inputHandler.GetKeycodeState(GLFW_KEY_F2) & JUST_RELEASED)
			show_gui = !show_gui;
		if (inputHandler.GetKeycodeState(GLFW_KEY_F11) & JUST_RELEASED)
			mWindowManager.ToggleFullscreenStatusForWindow(window);


		// Retrieve the actual framebuffer size: for HiDPI monitors,
		// you might end up with a framebuffer larger than what you
		// actually asked for. For example, if you ask for a 1920x1080
		// framebuffer, you might get a 3840x2160 one instead.
		// Also it might change as the user drags the window between
		// monitors with different DPIs, or if the fullscreen status is
		// being toggled.
		int framebuffer_width, framebuffer_height;
		glfwGetFramebufferSize(window, &framebuffer_width, &framebuffer_height);
		glViewport(0, 0, framebuffer_width, framebuffer_height);


		//
		// Todo: If you need to handle inputs, you can do it here
		//


		mWindowManager.NewImGuiFrame();

		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

		if (!shader_reload_failed) {
			//
			// Todo: Render all your geometry here.
			//
			Enemy.render(mCamera.GetWorldToClipMatrix());
			Enemy.set_program(&enemy_shader, set_uniforms);

			skybox.render(mCamera.GetWorldToClipMatrix());
			skybox.set_program(&skybox_shader, set_uniforms);
			
			Enemy.get_transform().SetTranslate(glm::vec3(200,0,0));

			//Enemy.render(mCamera.GetWorldToClipMatrix());
			//Enemy.set_program(&enemy_shader, set_uniforms);

		}

		//To initialize the array of enemies
		auto array_of_enemies = std::vector<glm::vec3>();

		if (click_shooting){

		auto pv = mCamera.mWorld.GetTranslation();
		auto v = mCamera.mWorld.GetFront();

		for (int i = 0; i < sizeof(array_of_enemies); i++) {

			glm::vec3 ps = array_of_enemies[i]; //get the position of the enemy

			//If hits the enemy
			if (abs(collision(ps - pv, v) < r)) {
				//Remove the enemy and gain points
				int enemy = 1; // Here we call the function to remove the enemy
				points = points + 1; //Idn why is not initialized 
			}

		}

		}
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		//
		// Todo: If you want a custom ImGUI window, you can set it up
		//       here
		//
		bool const opened = ImGui::Begin("Scene Controls", nullptr, ImGuiWindowFlags_None);
		if (opened) {
			ImGui::Checkbox("Show basis", &show_basis);
			ImGui::SliderFloat("Basis thickness scale", &basis_thickness_scale, 0.0f, 100.0f);
			ImGui::SliderFloat("Basis length scale", &basis_length_scale, 0.0f, 100.0f);
		}
		ImGui::End();

		if (show_basis)
			bonobo::renderBasis(basis_thickness_scale, basis_length_scale, mCamera.GetWorldToClipMatrix());
		if (show_logs)
			Log::View::Render();
		mWindowManager.RenderImGuiFrame(show_gui);

		glfwSwapBuffers(window);
	}
}

int main()
{
	std::setlocale(LC_ALL, "");

	Bonobo framework;

	try {
		edaf80::Assignment5 assignment5(framework.GetWindowManager());
		assignment5.run();
	} catch (std::runtime_error const& e) {
		LogError(e.what());
	}
}
