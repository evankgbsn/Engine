#include "Camera.h"

#include "../Windows/Window.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp >

Camera::~Camera()
{
}

void Camera::Rotate(const glm::vec3& axis, const float& angle)
{	
	target = position + glm::rotate(GetForwardVector(), angle, axis);
	UpdateView();
}

void Camera::Translate(const glm::vec3& translation)
{
	target += translation;
	position += translation;
	UpdateView();
}

glm::vec3 Camera::GetForwardVector() const
{
	return glm::normalize(target - position);
}

glm::vec3 Camera::GetRightVector() const
{
	return -glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), GetForwardVector()));
}

glm::vec3 Camera::GetUpVector() const
{
	return glm::normalize(glm::cross(GetForwardVector(), -GetRightVector()));
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
	return position;
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

void Camera::SetPosition(const glm::vec3& p)
{
	position = p;
	UpdateView();
}

void Camera::SetTarget(const glm::vec3& newTarget)
{
	target = newTarget;
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

	if (window != nullptr)
	{
		right = static_cast<float>(window->GetWidth());
		top = static_cast<float>(window->GetHeight());
		aspect = right / top;
		UpdateProjection();
	}
}

void Camera::UpdateProjection()
{
	switch (type)
	{
	case Camera::Type::PERSPECTIVE:
		projection = glm::perspective(fov, aspect, near, far);
		break;
	case Camera::Type::ORTHOGRAPHIC:
		projection = glm::orthoRH_ZO(left, right, top, bottom, near, far);
		break;
	default:
		break;
	}
}

void Camera::UpdateView()
{
	view = glm::lookAt(position, target, GetUpVector());
}

Camera::Camera(const Camera::Type& t, Window* const w) :
	type(t),
	window(w),
	view(glm::mat4(1.0f)),
	projection(glm::mat4(1.0f)),
	near(0.01f),
	far(100000.0f),
	fov(45.0f),
	left(0.0f),
	right(0.0f),
	top(0.0f),
	bottom(0.0f),
	position(glm::vec3(0.0f,0.0f,0.0f)),
	target(glm::vec3(0.0f, 0.0f, 1.0f))
{
	SetWindow(w);
	UpdateProjection();
	UpdateView();
}
