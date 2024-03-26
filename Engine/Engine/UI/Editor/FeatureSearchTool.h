#ifndef FEATURESEARCHTOOL_H
#define FEATURESEARCHTOOL_H

#include <string>
#include <unordered_set>

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

	std::unordered_set<std::string> featureNamesSet;

	UserInterfaceItem* featureSearchBackground;

	Text* const featureSearchBarText;
};

#endif // FEATURESEARCHTOOL_H