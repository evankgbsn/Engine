#ifndef FEATURESEARCHTOOL_H
#define FEATURESEARCHTOOL_H

#include <string>
#include <unordered_set>
#include <unordered_map>
#include <functional>

#include <glm/glm.hpp>

class UserInterfaceItem;
class Text;

class FeatureSearchTool
{

public:

	FeatureSearchTool();

	~FeatureSearchTool();

	FeatureSearchTool(const FeatureSearchTool&) = delete;

	FeatureSearchTool& operator=(const FeatureSearchTool&) = delete;

	FeatureSearchTool(FeatureSearchTool&&) = delete;

	FeatureSearchTool& operator=(FeatureSearchTool&&) = delete;

	void Enable();

	void Disable();

private:
	
	void InitializeFeatureNamesSet();

	void LoadFeatureSearchToolModels() const;

	void UpdateUserInterfaceItemsVisibility();

	void SetPositionToCursorPosition();

	void EnableInput();

	void DisableInput();

	glm::vec2 GetWindowOpenLocation() const;

	std::unordered_set<std::string> featureNamesSet;

	UserInterfaceItem* featureSearchBackground;

	Text* featureSearchBarText;

	std::function<void(int)>* characterInputFunction;

	std::function<void(int)>* characterInputFunctionShiftHeld;

	std::function<void(int)>* characterInputFunctionShiftRelease;

	std::function<void(int)>* characterInputFunctionBackspace;
};

#endif // FEATURESEARCHTOOL_H