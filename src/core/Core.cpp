#include "core/App.hpp"

App::App() {}

void App::update()
{
	if (currentScene != nullptr)
		currentScene->update();
}

void App::setScene(std::shared_ptr<Scene> scene)
{
	if (currentScene.use_count() != 0)
		currentScene = nullptr;

	currentScene = scene;
}

std::shared_ptr<Scene> App::getScene()
{
	return currentScene;
}

Scene::Scene() {}
Scene::~Scene() {}
void Scene::update() {}
