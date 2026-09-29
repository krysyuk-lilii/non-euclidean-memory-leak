// main.cpp
#include "core/engine.h"
#include "core/scene.h"
#include "core/game_object.h"
#include "core/portal.h"
#include "gl/GL.h"
#include "gl/gl_check.h"
#include "gl/shader.h"
#include "gl/mesh.h"
#include "gl/obj_loader.h"
#include "render/camera.h"
#include "render/render_pass.h"
#include "render/portal_renderer.h"
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <cmath>


namespace
{
	void buildCube(std::vector<glm::vec3>& positions, std::vector<glm::vec3>& normals)
	{
		struct Face { glm::vec3 normal; glm::vec3 verts[6]; };
		const float s = 0.5f;
		const std::vector<Face> faces = {
			{{0,0,1},  {{-s,-s,s},{s,-s,s},{s,s,s},  {-s,-s,s},{s,s,s},{-s,s,s}}},
			{{0,0,-1}, {{s,-s,-s},{-s,-s,-s},{-s,s,-s},{s,-s,-s},{-s,s,-s},{s,s,-s}}},
			{{0,1,0},  {{-s,s,s},{s,s,s},{s,s,-s},  {-s,s,s},{s,s,-s},{-s,s,-s}}},
			{{0,-1,0}, {{-s,-s,-s},{s,-s,-s},{s,-s,s},{-s,-s,-s},{s,-s,s},{-s,-s,s}}},
			{{1,0,0},  {{s,-s,s},{s,-s,-s},{s,s,-s}, {s,-s,s},{s,s,-s},{s,s,s}}},
			{{-1,0,0}, {{-s,-s,-s},{-s,-s,s},{-s,s,s},{-s,-s,-s},{-s,s,s},{-s,s,-s}}},
		};
		for (auto& face : faces)
			for (auto& v : face.verts)
			{
				positions.push_back(v);
				normals.push_back(face.normal);
			}
	}

	void drawScene(const Camera& cam, Shader& meshShader, Scene& scene)
	{
		meshShader.use();
		glm::vec3 lightDir = glm::normalize(glm::vec3(0.4f, 1.0f, 0.6f));
		GL_CHECK(glUniform3fv(meshShader.uniform("u_lightDir"), 1, &lightDir[0]));
		glm::vec3 lightColor(1.0f);
		GL_CHECK(glUniform3fv(meshShader.uniform("u_lightColor"), 1, &lightColor[0]));
		glm::vec3 ambient(0.15f, 0.15f, 0.18f);
		GL_CHECK(glUniform3fv(meshShader.uniform("u_ambientColor"), 1, &ambient[0]));
		glm::vec3 viewPos = glm::vec3(glm::inverse(cam.view())[3]);
		GL_CHECK(glUniform3fv(meshShader.uniform("u_viewPos"), 1, &viewPos[0]));

		scene.render(cam);
	}

	void drawPortalCase(const Portal& p, const Camera& cam, Shader& meshShader, Mesh& caseMesh)
	{
		meshShader.use();
		const float caseScale = p.radius; // tune this
		glm::mat4 world = p.obj->localToWorld()
			* glm::scale(glm::mat4(1.0f), glm::vec3(caseScale));
		glm::mat4 mvp = cam.viewProjection() * world;
		glm::mat4 normalMatrix = glm::transpose(glm::inverse(world));
		GL_CHECK(glUniformMatrix4fv(meshShader.uniform("u_mvp"), 1, GL_FALSE, &mvp[0][0]));
		GL_CHECK(glUniformMatrix4fv(meshShader.uniform("u_world"), 1, GL_FALSE, &world[0][0]));
		GL_CHECK(glUniformMatrix4fv(meshShader.uniform("u_normalMatrix"), 1, GL_FALSE, &normalMatrix[0][0]));
		glm::vec4 caseColor(1.0f, 0.5f, 0.55f, 1.0f);
		GL_CHECK(glUniform4fv(meshShader.uniform("u_diffuseColor"), 1, &caseColor[0]));
		GL_CHECK(glUniform1f(meshShader.uniform("u_shininess"), 48.0f));
		caseMesh.draw(meshShader);
	}
}

int main(int, char**)
{
	Engine engine;
	if (!engine.init("Portal Engine", 1280, 720))
		return 1;
	SDL_SetWindowRelativeMouseMode(engine.window(), true);

	Shader meshShader = Shader::loadFromFiles("shaders/mesh");
	if (!meshShader.valid()) { engine.shutdown(); return 1; }

	PortalRenderer portalRenderer;
	if (!portalRenderer.init("shaders/portal")) { engine.shutdown(); return 1; }
	portalRenderer.setDebugClearColor({1, 0, 1, 1});

	std::vector<glm::vec3> cubePos, cubeNormal;
	buildCube(cubePos, cubeNormal);
	Mesh cube;
	cube.upload(cubePos, cubeNormal);

	std::vector<glm::vec3> casePos, caseNorm;
	if (!loadObj("assets/portalcase.obj", casePos, caseNorm))
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to load assets/portalcase.obj");
		engine.shutdown();
		return 1;
	}
	Mesh caseMesh;
	caseMesh.upload(casePos, caseNorm);

	Camera camera;
	camera.setup(1280.0f, 720.0f, 0.1f, 1000.0f, glm::radians(60.0f));

	Scene sceneA, sceneB;

	sceneA.backgroundColor = glm::vec4(0.05f, 0.05f, 0.2f, 1.0f);  // dim blue
	sceneB.backgroundColor = glm::vec4(0.25f, 0.1f, 0.02f, 1.0f);  // dim orange

	GameObject& cubeObject = sceneA.add(std::make_unique<GameObject>());
	cubeObject.onUpdate = [&](double fixedDt) { cubeObject.euler.y += static_cast<float>(fixedDt); };
	cubeObject.onRender = [&](const Camera& cam)
	{

		meshShader.use();
		
		glm::mat4 world = cubeObject.localToWorld() * glm::translate(glm::mat4(1.0f), glm::vec3({0.0f, 0.0f, -6.0f}));
		glm::mat4 mvp = cam.viewProjection() * world;
		glm::mat4 normalMatrix = glm::transpose(glm::inverse(world));
		GL_CHECK(glUniformMatrix4fv(meshShader.uniform("u_mvp"), 1, GL_FALSE, &mvp[0][0]));
		GL_CHECK(glUniformMatrix4fv(meshShader.uniform("u_world"), 1, GL_FALSE, &world[0][0]));
		GL_CHECK(glUniformMatrix4fv(meshShader.uniform("u_normalMatrix"), 1, GL_FALSE, &normalMatrix[0][0]));
		glm::vec4 diffuse(0.7f, 0.35f, 0.1f, 1.0f);
		GL_CHECK(glUniform4fv(meshShader.uniform("u_diffuseColor"), 1, &diffuse[0]));
		GL_CHECK(glUniform1f(meshShader.uniform("u_shininess"), 32.0f));
		cube.draw(meshShader);
	};

	std::vector<glm::vec3> suzannePos, suzanneNorm;
	if (!loadObj("assets/monkey.obj", suzannePos, suzanneNorm))
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to load assets/suzanne.obj");
		engine.shutdown();
		return 1;
	}
	Mesh suzanneMesh;
	suzanneMesh.upload(suzannePos, suzanneNorm);

	GameObject& suzanne = sceneB.add(std::make_unique<GameObject>());
	suzanne.pos = glm::vec3(9.0f, 1.25f, 0.0f); // in front of portalB (3,1.25,0), visible looking into sceneB
	suzanne.onUpdate = [&](double fixedDt) { suzanne.euler.y += static_cast<float>(fixedDt) * 0.5f; };
	suzanne.onRender = [&](const Camera& cam)
	{
		meshShader.use();
		glm::mat4 world = suzanne.localToWorld();
		glm::mat4 mvp = cam.viewProjection() * world;
		glm::mat4 normalMatrix = glm::transpose(glm::inverse(world));
		GL_CHECK(glUniformMatrix4fv(meshShader.uniform("u_mvp"), 1, GL_FALSE, &mvp[0][0]));
		GL_CHECK(glUniformMatrix4fv(meshShader.uniform("u_world"), 1, GL_FALSE, &world[0][0]));
		GL_CHECK(glUniformMatrix4fv(meshShader.uniform("u_normalMatrix"), 1, GL_FALSE, &normalMatrix[0][0]));
		glm::vec4 diffuse(0.6f, 0.5f, 0.45f, 1.0f);
		GL_CHECK(glUniform4fv(meshShader.uniform("u_diffuseColor"), 1, &diffuse[0]));
		GL_CHECK(glUniform1f(meshShader.uniform("u_shininess"), 16.0f));
		suzanneMesh.draw(meshShader);
	};

	Portal& portalA = createPortal(sceneA, {-3, 1.25f, 0}, glm::radians(90.0f), 1.5f);
	Portal& portalB = createPortal(sceneB, {3, 1.25f, 0}, glm::radians(-90.0f), 1.5f);
	linkPortals(portalA, sceneA, portalB, sceneB);
	portalA.obj->onRender = [&](const Camera& cam) {
		if (!portalRenderer.shouldSkipCase(portalA))
        	drawPortalCase(portalA, cam, meshShader, caseMesh);

		// Disc — drawn second, on top of the case.
		portalRenderer.draw(portalA, cam);
	};
	portalB.obj->onRender = [&](const Camera& cam) {
		if (!portalRenderer.shouldSkipCase(portalB))
        	drawPortalCase(portalB, cam, meshShader, caseMesh);
		portalRenderer.draw(portalB, cam);
	};

	PortalTraveler player;
	player.obj = &sceneA.add(std::make_unique<GameObject>());
	player.obj->pos = glm::vec3(0, 1.5f, 4.0f);
	player.currScene = &sceneA;
	float pitch = 0.0f;
	const float accel = 20.0f, maxSpeed = 4.0f, friction = 10.0f, lookSpeed = 0.0025f;

	bool paused = false;
	auto onEvent = [&](const SDL_Event& e)
	{
		if (e.type == SDL_EVENT_KEY_DOWN && e.key.scancode == SDL_SCANCODE_ESCAPE && !e.key.repeat)
		{
			paused = !paused;
			engine.setPaused(paused);
			SDL_SetWindowRelativeMouseMode(engine.window(), !paused);
			if (!paused) { float dx, dy; SDL_GetRelativeMouseState(&dx, &dy); }
		}
		if (e.type == SDL_EVENT_WINDOW_RESIZED)
		{
			camera.setup(static_cast<float>(e.window.data1), static_cast<float>(e.window.data2),
				camera.near, camera.far, camera.fovY);
		}
	};

	engine.run(
		[&](double fixedDt)
		{
			const bool* keys = SDL_GetKeyboardState(nullptr);
			float mx, my;
			SDL_GetRelativeMouseState(&mx, &my);
			player.obj->euler.y -= mx * lookSpeed;
			pitch = glm::clamp(pitch - my * lookSpeed, -1.5f, 1.5f);

			glm::vec3 fwd(-sinf(player.obj->euler.y), 0, -cosf(player.obj->euler.y));
			glm::vec3 right(-fwd.z, 0, fwd.x);
			glm::vec3 wishDir(0.0f);
			if (keys[SDL_SCANCODE_W]) wishDir += fwd;
			if (keys[SDL_SCANCODE_S]) wishDir -= fwd;
			if (keys[SDL_SCANCODE_D]) wishDir += right;
			if (keys[SDL_SCANCODE_A]) wishDir -= right;

			if (glm::length(wishDir) > 0.0f)
				player.velocity += glm::normalize(wishDir) * accel * static_cast<float>(fixedDt);
			else
				player.velocity -= player.velocity * friction * static_cast<float>(fixedDt);
			if (glm::length(player.velocity) > maxSpeed)
				player.velocity = glm::normalize(player.velocity) * maxSpeed;

			player.prevPos = player.obj->pos;
			player.obj->pos += player.velocity * static_cast<float>(fixedDt);

			for (auto& p : player.currScene->portals)
				if (tryCrossPortal(player, *p)) break;

			camera.setTransform(player.obj->pos, pitch, -player.obj->euler.y);

			sceneA.update(fixedDt);
			sceneB.update(fixedDt); // every scene ticks, not just the current one
		},
		[&](double /*alpha*/)
		{
			portalRenderer.renderViews(camera, *player.currScene,
				[&](const Camera& cam, Scene& s) { drawScene(cam, meshShader, s); });
			RenderPass::run(
				camera, nullptr,
				[&](const Camera& cam) { drawScene(cam, meshShader, *player.currScene); },
				player.currScene->backgroundColor);
		},
		onEvent
	);

	portalRenderer.shutdown();
	engine.shutdown();
	return 0;
}