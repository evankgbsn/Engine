#ifndef CAMERAMANAGER_H
#define CAMERAMANAGER_H

#include "Camera.h"

#include <unordered_map>
#include <string>

class Window;

class CameraManager
{
public:

	static void Initialize();

	static void Terminate();

	static Camera& CreateCamera(const Camera::Type& type, const std::string& cameraName, Window* const window);

	static Camera& GetCamera(const std::string& cameraName);

	static void DestroyCamera(const std::string& cameraName);

	static void SetActiveCamera(const std::string& name);

	static Camera& GetActiveCamera();

private:

	CameraManager();

	~CameraManager();

	CameraManager(const CameraManager&) = delete;

	CameraManager& operator=(const CameraManager&) = delete;

	CameraManager(CameraManager&&) = delete;

	CameraManager& operator=(CameraManager&&) = delete;

	static CameraManager* instance;

	std::unordered_map<std::string, Camera* const> cameras;

	Camera* activeCamera = nullptr;

};


#endif // CAMERAMANAGER_H