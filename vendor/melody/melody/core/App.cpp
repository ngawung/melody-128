#include "melody.hpp"

using namespace melody;

App* App::_instance = 0;

App::App() {
	App* p_app = this;
	_instance = p_app;
}

App::~App() {
	delete _currentScene;
}

void App::start(Scene* scene) {
	_currentScene = scene;
	scene->initialize();
}

void App::update() {
	if (_currentScene != nullptr)
		_currentScene->update();
}


// GET & SET

void App::set_scene(Scene* scene) {
	if (_currentScene != nullptr) {
		delete _currentScene;
	}

	_currentScene = scene;
	_currentScene->initialize();
}

Scene* App::get_scene() {
	return _currentScene;
}

App* App::get_instance() {
	return _instance;
}