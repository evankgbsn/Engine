#ifndef USERINTERFACEMANAGER_H
#define USERINTERFACEMANAGER_H

class UserInterfaceManager
{
public:

	static void Initialize();

	static void Terminate();

private:
	
	UserInterfaceManager();

	~UserInterfaceManager();

	UserInterfaceManager(const UserInterfaceManager&) = delete;

	UserInterfaceManager& operator=(const UserInterfaceManager&) = delete;

	UserInterfaceManager(UserInterfaceManager&&) = delete;

	UserInterfaceManager& operator=(UserInterfaceManager&&) = delete;

	static UserInterfaceManager* instance;

};

#endif // USERINTERFACEMANAGER_H