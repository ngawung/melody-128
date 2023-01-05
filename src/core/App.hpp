#include <memory>

#include "core/Scene.hpp"

class App {
public:

	static App& getInstance()
	{
		static App instance;
		return instance;
	}

	void update();
	void setScene(std::shared_ptr<Scene> scene);
	std::shared_ptr<Scene> getScene();

private:
	App();

	App(const App&) = delete;
	App& operator=(const App&) = delete;

	std::shared_ptr<Scene> currentScene;
};
