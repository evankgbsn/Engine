#ifndef EDITOR_H
#define EDITOR_H

class UserInterfaceItem;

class Editor
{

public:

	static void Initialize();
	
	static void Terminate();

	Editor(const Editor&) = delete;

	Editor& operator=(const Editor&) = delete;

	Editor(Editor&&) = delete;

	Editor& operator=(Editor&&) = delete;

private:

	Editor();

	~Editor();

	static Editor* instance;

};


#endif // EDITOR_H