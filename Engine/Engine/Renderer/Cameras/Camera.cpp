#include "Camera.h"

#include "../Windows/Window.h"

#include <glm/gtc/matrix_transform.hpp>

Camera::~Camera()
{
}

const float& Camera::GetFOV() const
{
	return fov;
}

const float& Camera::GetNear() const
{
	return near;
}

const float& Camera::GetFar() const
{
	return far;
}

const glm::vec3& Camera::GetPosition() const
{
	return view[3];
}

const glm::mat4& Camera::GetView() const
{
	return view;
}

const glm::mat4& Camera::GetProjection() const
{
	return projection;
}

const Camera::Type& Camera::GetType() const
{
	return type;
}

void Camera::SetPosition(const glm::vec3& position)
{
	view[3] = glm::vec4(position, view[3].w);
	UpdateView();
}

void Camera::SetView(const glm::mat4& newView)
{
	view = newView;
}

void Camera::SetProjection(const glm::mat4& newProjection)
{
	projection = newProjection;
}

void Camera::SetFOV(const float& newFOV)
{
	fov = newFOV;
	UpdateProjection();
}

void Camera::SetAspectRatio(const float& newAspectRatio)
{
	aspect = newAspectRatio;
	UpdateProjection();
}

void Camera::SetNear(const float& newNear)
{
	near = newNear;
	UpdateProjection();
}

void Camera::SetFar(const float& newFar)
{
	far = newFar;
	UpdateProjection();
}

void Camera::SetTop(const float& newTop)
{
	top = newTop;
	UpdateProjection();
}

void Camera::SetBottom(const float& newBottom)
{
	bottom = newBottom;
	UpdateProjection();
}

void Camera::SetLeft(const float& newLeft)
{
	left = newLeft;
	UpdateProjection();
}

void Camera::SetRight(const float& newRight)
{
	right = newRight;
	UpdateProjection();
}

void Camera::SetType(const Camera::Type& newType)
{
	type = newType;
}

void Camera::SetWindow(Window* const newWindow)
{
	window = newWindow;

	float windowWidth = 0;
	float windowHeight = 0;

	if (window != nullptr)
	{
		windowWidth = static_cast<float>(window->GetWidth());
		windowHeight = static_cast<float>(window->GetHeight());
	}

	right = windowWidth;
	top = windowHeight;
	aspect = windowWidth / windowHeight;

	UpdateProjection();
}

void Camera::UpdateProjection()
{
	switch (type)
	{
	case Camera::Type::PERSPECTIVE:
		projection = glm::perspective(fov, aspect, near, far);
		break;
	case Camera::Type::ORTHOGRAPHIC:
		projection = projection = glm::ortho(left, right, bottom, top, near, far);
		break;
	default:
		break;
	}
}

void Camera::UpdateView()
{
	glm::vec3 position = view[3];
	glm::vec3 forward = view[2];
	glm::vec3 up = view[1];
	view = glm::lookAt(position, position + ((position + forward) - position), up);
}

Camera::Camera(const Camera::Type& t, Window* const w) :
	type(t),
	window(w),
	view(glm::mat4(1.0f)),
	projection(glm::mat4(1.0f)),
	near(0.01f),
	far(1000.0f),
	fov(45.0f),
	left(0.0f),
	right(0.0f),
	top(0.0f),
	bottom(0.0f)
{
	SetWindow(w);
	UpdateProjection();
	UpdateView();
}
