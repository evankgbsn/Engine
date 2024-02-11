#ifndef EDITOR_H
#define EDITOR_H

class UserInterfaceItem;
class FeatureSearchTool;

class Editor
{

public:

	static void Initialize();
	
	static void Terminate();

	static bool Operating();

	static void Open();

	static void Close();

	static bool IsOpen();

	Editor(const Editor&) = delete;

	Editor& operator=(const Editor&) = delete;

	Editor(Editor&&) = delete;

	Editor& operator=(Editor&&) = delete;

private:

	Editor();

	~Editor();

	static Editor* instance;

	FeatureSearchTool* const featureSearchTool;

	bool operating;

	bool open;
};


#endif // EDITOR_H