#ifndef ENGINE_H
#define ENGINE_H

#include <string>
#include <mutex>
#include <thread>
#include <unordered_map>
#include <functional>

class Engine
{
public:

	struct Version
	{
		Version(uint32_t Major, uint32_t Minor, uint32_t Patch) :
			major(Major),
			minor(Minor),
			patch(Patch),
			version(((((uint32_t)(0)) << 29) | (((uint32_t)(major)) << 22) | (((uint32_t)(minor)) << 12) | ((uint32_t)(patch))))
		{}

		const std::string ToString() const
		{
			return std::to_string(major) + "." + std::to_string(minor) + "." + std::to_string(patch);
		}

		uint32_t major, minor, patch, version;
	};

public:

	// Initialize the Engine and all the Engine subsystems.
	static void Initialize(const std::string& gameName, const Version& gameVersion);

	// Terminate the Engine and all the Engine subsystems.
	static void Terminate();

	// Update one frame in the engine.
	static void Start();

	static bool Operating();

	// Get the name of the game that is currently running.
	static const std::string& GetGameName();

	// Get the version of the game that is currently running.
	static const Version& GetGameVersion();

	static void SetGameThreadFunc(void(*newGameThreadFunc)());

	static void SetLoadAssetsFunc(void(*newLoadAssetsFunc)());

private:

	Engine() = delete;

	Engine(const std::string& gameName, const Version& gameVersion);

	~Engine();

	Engine(const Engine&) = delete;

	Engine& operator=(Engine&) = delete;

	Engine(const Engine&&) = delete;

	Engine& operator=(Engine&&) = delete;

	void SpawnAndDetachGameThread();

	void LoadAssets();

	void InitializeEditor();

	// The single instance of the Engine class.
	static Engine* instance;

	bool shouldUpdate = true;

	// The name of the game that the engine is currently running.
	const std::string& gameName;

	// The version of the game that the engine is currently running.
	const Version& gameVersion;

	// The version of the engine.
	const Version engineVersion;

	void(*userGameThreadFunc)() = nullptr;

	std::thread* userGameThread = nullptr;

	std::unordered_map<void(*)(), std::thread*> spawnedGameThreads;

	void(*loadAssetsFunc)() = nullptr;

	std::function<void(int keyCode)>* toggleEditorFunction = nullptr;
};

#endif // ENGINE_H
