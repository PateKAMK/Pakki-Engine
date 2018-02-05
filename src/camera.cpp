#include "camera.h"
#include <math.h>

static int screenWidht = 0;
static int screenHeigh = 0;


void init_camera(Camera* const camera, int screenWidht, int screenHeight)
{
	camera->orthoMatrix = glm::ortho(0.0f,(float)screenWidht,0.0f,(float)screenHeight);
	camera->scaledDimensions.x = screenWidht;
	camera->scaledDimensions.y = screenHeight;
	camera->update = true;
    //camera->scale = 1.0f;
	//::screenHeigh = screenHeight;
	//::screenWidht = screenWidht;
	//camera->position = glm::vec2{ 100 , 110};
}
void update_camera(Camera* const camera)
{
	if(camera->update)
	{
		camera->update ^= 1;
		glm::vec3 translate(-(camera->position.x) + (screenWidht/2.f), -(camera->position.y) + (screenHeigh / 2.f), 0.0f);
		camera->cameraMatrix = glm::translate(camera->orthoMatrix, translate);
		glm::vec3 scale(camera->scale, camera->scale,0.0f);
		camera->cameraMatrix = glm::scale(glm::mat4(1.f), scale)*camera->cameraMatrix;
		camera->scaledDimensions = glm::vec2(screenWidht, screenHeigh) / camera->scale;
	}
}
bool is_in_view(Camera* const camera,const glm::vec2 position,const glm::vec2 dimensions)
{
	const float minDistX = dimensions.x + camera->scaledDimensions.x / 2.0f;
	const float minDistY = dimensions.y + camera->scaledDimensions.y / 2.0f;
	glm::vec2 centerPos = position;
	glm::vec2 distance = centerPos - camera->position;
	float xDepth = minDistX - fabs(distance.x);
	float yDepth = minDistY - fabs(distance.y);
	if (xDepth > 0 && yDepth > 0)
	{
		return true;
	}

	return false;
}
glm::vec2 point_to_world_position(const Camera* camera, const glm::vec2* point)
{
	glm::vec2 newPoint = *point;
	newPoint.y = screenHeigh - point->y;
	newPoint -= glm::vec2(screenWidht / 2, screenHeigh / 2);
	newPoint /= camera->scale;
	newPoint += camera->position;
	return newPoint;
}