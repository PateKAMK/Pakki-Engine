#pragma once
#include <glm/glm/common.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>


struct Camera
{
	glm::vec2	position;
	glm::mat4	cameraMatrix;
	glm::mat4	orthoMatrix;
	glm::vec2	scaledDimensions;
	float		scale;
	bool		update;
};
extern void init_camera(Camera* const camera,int screenWidht,int screenHeight);
extern void update_camera(Camera* const camera);
extern bool is_in_view(Camera* const camera, const glm::vec2 position, const glm::vec2 dimensions);
extern glm::vec2 point_to_world_position(const Camera* camera, const glm::vec2* point);
//todo screen widht height update