#include "Networks.h"


#if defined(USE_TASK_MANAGER)

void ModuleResources::TaskLoadTexture::execute()
{
	*texture = App->modTextures->loadTexture(filename);
}

#endif


bool ModuleResources::init()
{
	background = App->modTextures->loadTexture("background.jpg");

#if !defined(USE_TASK_MANAGER)
	sand = App->modTextures->loadTexture("sand_background.jpg");
	shotter1 = App->modTextures->loadTexture("shotter1.png");
	shotter2 = App->modTextures->loadTexture("shotter2.png");
	shotter3 = App->modTextures->loadTexture("shotter3.png");
	shotter4 = App->modTextures->loadTexture("shotter4.png");
	bullet = App->modTextures->loadTexture("bullet.png");
	explosion = App->modTextures->loadTexture("explosion.png");
	loadingFinished = true;
	completionRatio = 1.0f;
#else
	loadTextureAsync("sand_background.jpg", &sand);
	loadTextureAsync("shotter1.png", &shotter1);
	loadTextureAsync("shotter2.png", &shotter2);
	loadTextureAsync("shotter3.png", &shotter3);
	loadTextureAsync("shotter4.png", &shotter4);
	loadTextureAsync("bullet.png", &bullet);
	loadTextureAsync("explosion.png", &explosion);
#endif

	audioClipShot = App->modSound->loadAudioClip("shot.wav");
	audioClipExplosion = App->modSound->loadAudioClip("explosion.wav");
	//App->modSound->playAudioClip(audioClipExplosion);

	return true;
}

Texture* ModuleResources::GetTextureFile(std::string fileName)
{
	if (fileName == "background.jpg") return background;
	else if (fileName == "sand_background.jpg") return sand;
	else if (fileName == "shotter1.png") return shotter1;
	else if (fileName == "shotter2.png") return shotter2;
	else if (fileName == "shotter3.png") return shotter3;
	else if (fileName == "shotter4.png") return shotter4;
	else if (fileName == "bullet.png") return bullet;
	else if (fileName == "explosion.png") return explosion;
}

#if defined(USE_TASK_MANAGER)

void ModuleResources::loadTextureAsync(const char * filename, Texture **texturePtrAddress)
{
	ASSERT(taskCount < MAX_RESOURCES);
	
	TaskLoadTexture *task = &tasks[taskCount++];
	task->owner = this;
	task->filename = filename;
	task->texture = texturePtrAddress;

	App->modTaskManager->scheduleTask(task, this);
}

void ModuleResources::onTaskFinished(Task * task)
{
	ASSERT(task != nullptr);

	TaskLoadTexture *taskLoadTexture = dynamic_cast<TaskLoadTexture*>(task);

	for (uint32 i = 0; i < taskCount; ++i)
	{
		if (task == &tasks[i])
		{
			finishedTaskCount++;
			task = nullptr;
			break;
		}
	}

	ASSERT(task == nullptr);

	if (finishedTaskCount == taskCount)
	{
		finishedLoading = true;

		// Create the explosion animation clip
		explosionClip = App->modRender->addAnimationClip();
		explosionClip->frameTime = 0.1f;
		explosionClip->loop = false;
		for (int i = 0; i < 16; ++i)
		{
			float x = (i % 4) / 4.0f;
			float y = (i / 4) / 4.0f;
			float w = 1.0f / 4.0f;
			float h = 1.0f / 4.0f;
			explosionClip->addFrameRect(vec4{ x, y, w, h });
		}
	}
}

#endif
