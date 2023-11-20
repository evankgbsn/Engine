#include "CameraManager.h"

#include "../../Utils/Logger.h"
#include "../Windows/Window.h"

CameraManager* CameraManager::instance = nullptr;

// A default camera to return as reference during failure.
static Camera cam(Camera::Type::PERSPECTIVE, nullptr);

void CameraManager::Initialize()
{
	if (instance == nullptr)
	{
		instance = new CameraManager();
	}
	else
	{
		Logger::Log(std::string("Calling CameraManager::Initialize() before CameraManager::Terminate()."), Logger::Category::Warning);
	}
}

void CameraManager::Terminate()
{
	if (instance != nullptr)
	{
		delete instance;
	}
	else
	{
		Logger::Log(std::string("Calling CameraManager::Terminate() before CameraManager::Initialize()."), Logger::Category::Warning);
	}
}

Camera& CameraManager::CreateCamera(const Camera::Type& type, const std::string& cameraName, Window* const window)
{
	if (instance != nullptr)
	{
		const auto& camera = instance->cameras.find(cameraName);
		if (camera == instance->cameras.end())
		{
			Camera* const newCamera = new Camera(type, window);
			instance->cameras.insert(std::make_pair(cameraName, newCamera));
			return *newCamera;
		}
		else
		{
			Logger::Log(std::string("Trying to create a camera with name: ") + cameraName + std::string(". A camera with that name already exists."), Logger::Category::Warning);
			return *camera->second;
		}

	}
	else
	{
		Logger::Log(std::string("Calling CameraManager::CreateCamera() before CameraManager::Initialize()"), Logger::Category::Error);
		return cam;
	}
}

Camera& CameraManager::GetCamera(const std::string& cameraName)
{
	if (instance != nullptr)
	{
		const auto& camera = instance->cameras.find(cameraName);
		if (camera != instance->cameras.end())
		{
			return *camera->second;
		}
		else
		{
			Logger::Log(std::string("A camera with the name: ") + cameraName + std::string(" does not exist. CameraManager::GetCamera()"), Logger::Category::Warning);
			return cam;
		}
	}
	else
	{
		Logger::Log(std::string("Calling CameraManager::GetCamera() before CameraManager::Initialize()"), Logger::Category::Error);
		return cam;
	}
}

void CameraManager::DestroyCamera(const std::string& cameraName)
{
	if (instance != nullptr)
	{
		const auto& camera = instance->cameras.find(cameraName);
		if (camera != instance->cameras.end())
		{
			delete camera->second;
			instance->cameras.erase(camera);
		}
		else
		{
			Logger::Log(std::string("A camera with the name: ") + cameraName + std::string(" does not exist. CameraManager::DestroyCamera()"), Logger::Category::Warning);
		}
	}
	else
	{
		Logger::Log(std::string("Calling CameraManager::DestroyCamera() before CameraManager::Initialize()"), Logger::Category::Error);
	}
}

CameraManager::CameraManager() :
	cameras(std::unordered_map<std::string, Camera* const>())
{

}

CameraManager::~CameraManager()
{
	for (const auto& camera : cameras)
	{
		delete camera.second;
	}

	cameras.clear();

	instance = nullptr;
}
