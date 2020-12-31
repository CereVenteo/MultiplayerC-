#pragma once

#define USE_TASK_MANAGER

struct Texture;

class ModuleResources : public Module
{
public:

	Texture *background = nullptr;
	Texture *sand = nullptr;
	Texture *shotter1 = nullptr;
	Texture *shotter2 = nullptr;
	Texture *shotter3 = nullptr;
	Texture *shotter4 = nullptr;
	Texture *bullet = nullptr;
	Texture *explosion = nullptr;

	AnimationClip *explosionClip = nullptr;

	AudioClip *audioClipShot = nullptr;
	AudioClip *audioClipExplosion = nullptr;

	bool finishedLoading = false;

	Texture* GetTextureFile(std::string fileName);

private:

	bool init() override;

#if defined(USE_TASK_MANAGER)
	
	class TaskLoadTexture : public Task
	{
	public:

		const char *filename = nullptr;
		Texture **texture = nullptr;

		void execute() override;
	};

	static const int MAX_RESOURCES = 16;
	TaskLoadTexture tasks[MAX_RESOURCES] = {};
	uint32 taskCount = 0;
	uint32 finishedTaskCount = 0;

	void onTaskFinished(Task *task) override;

	void loadTextureAsync(const char *filename, Texture **texturePtrAddress);

#endif

};

