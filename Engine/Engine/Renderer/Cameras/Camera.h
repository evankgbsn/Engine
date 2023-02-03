#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>

class Window;

class Camera
{
public:

	enum class Type
	{
		ORTHOGRAPHIC,
		PERSPECTIVE
	};

	Camera(const Camera::Type& type, Window* const window);

	~Camera();

	void Rotate(const glm::vec3& axis, const float& angle);

	void Translate(const glm::vec3& translation);

	glm::vec3 GetForwardVector() const;

	glm::vec3 GetRightVector() const;

	glm::vec3 GetUpVector() const;

	const float& GetFOV() const;

	const float& GetNear() const;

	const float& GetFar() const;

	const glm::vec3& GetPosition() const;

	const glm::mat4& GetView() const;

	const glm::mat4& GetProjection() const;

	const Camera::Type& GetType() const;

	void SetPosition(const glm::vec3& position);

	void SetTarget(const glm::vec3& newTarget);

	void SetView(const glm::mat4& newView);

	void SetProjection(const glm::mat4& newProjection);

	// PERSPECTIVE BEGIN: These should only be called for perspective cameras and will have no effect on orthographic cameras.
	void SetFOV(const float& newFOV);

	void SetAspectRatio(const float& newAspectRatio);
	// PERSPECTIVE END

	void SetNear(const float& newNear);

	void SetFar(const float& newFar);

	// ORTHOGRAPHIC BEGIN: THese functions will only have effect on orthographics cameras and will have no effect on perspective cameras.
	void SetTop(const float& newTop);

	void SetBottom(const float& newBottom);

	void SetLeft(const float& newLeft);

	void SetRight(const float& newRight);
	// ORTHOGRAPHIC END

	void SetType(const Camera::Type& newType);

	void SetWindow(Window* const newWindow);
	
private:

	void UpdateProjection();

	void UpdateView();

protected:

	Camera() = delete;

	Camera(const Camera&) = delete;

	Camera& operator=(const Camera&) = delete;

	Camera(Camera&&) = delete;

	Camera& operator=(Camera&&) = delete;

	Type type;

	Window* window;

	float near;

	float far;

	// PERSPECTIVE BEGIN: Field of view and aspect ratio is only relevant to perspective cameras.
	float fov;

	float aspect;
	// PERSPECTIVE END

	// ORTHOGRAPHIC BEGIN: These are only used with orthographic cameras.
	float top;

	float bottom;

	float left;

	float right;
	// ORTHOGRAPHIC END

	glm::mat4 view;

	glm::mat4 projection;

	glm::vec3 position;

	glm::vec3 target;

};

#endif // CAMERA_H