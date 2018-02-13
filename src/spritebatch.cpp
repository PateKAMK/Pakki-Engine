#include "spritebatch.h"
#include <stdio.h>
#ifdef P_ANDROID
#include <android/log.h>
#include <engine.h>
#include <spritebatch.h>

#endif

#ifdef P_WINDOWS
void init_batch(SpriteBatch* batch)//tee ebo
{

	glGenVertexArrays(1, &(batch->vao));

	glBindVertexArray(batch->vao);

	glGenBuffers(1, &batch->vbo);

	glBindBuffer(GL_ARRAY_BUFFER, batch->vbo);


	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);

	
	//this is the position attribute pointer
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
	//this is the color attribute pointer
	glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), (void*)offsetof(Vertex, color));
	//uv atrrib pointer
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv)); // pistä position kanssa ?
	//texid
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, textureid)); 
	glBindVertexArray(0);


	batch->sortingGlyphs.init_array(50);
	//batch->renderbatches.init_array(50);
	batch->glyphs.init_array(50);
	batch->VertexBuffer.init_array(50);
	batch->numVerts = 0;
	for (int i = 0; i < 30; i++) 
	{
		batch->bindableTextures[i] = i;
		batch->bindtextures[i] = -1;
	}
	batch->numTexturesToBind = 0;
}
#endif
#ifdef P_ANDROID
void init_batch(SpriteBatch* batch)//tee ebo
{
	glGenBuffers(1,&(batch->vbo));

    allocate_new_array<Glyph>(&(batch->dynArrGlyphs));
    allocate_new_array<RenderBatch>(&(batch->dynArrRenderBatches));
    allocate_new_array<Glyph*>(&(batch->dynArrSortingGlyphs));
    //glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);
}
#endif
void begin(SpriteBatch* batch)
{
	//glypsh clear
	batch->glyphs.clear_array();
	batch->sortingGlyphs.clear_array();
	batch->VertexBuffer.clear_array();
	batch->numVerts = 0;
	batch->numTexturesToBind = 0;
	for (int i = 0; i < 30; i++)
	{
		batch->bindtextures[i] = -1;
	}
	//batch->renderbatches.clear_array();
	//renderbatches clear
}
inline glm::vec2 rotatePoint(glm::vec2 point, float angle)
{
	glm::vec2 newV;
	newV.x = point.x * cos(angle) - point.y *sin(angle);
	newV.y = point.x * sin(angle) + point.y *cos(angle);
	return newV;
}

void push_to_batch(SpriteBatch* batch,const glm::vec4* destinationRectangle, const glm::vec4* uvRectangle, GLuint texture, const Color* color, float depth,float angle /*= 0*/)
{
	Glyph* temp = batch->glyphs.get_new_item();

	temp->texture = texture;
	temp->depth = depth;

	if(angle !=0)
	{
		glm::vec2 halfDims(destinationRectangle->z / 2, destinationRectangle->w / 2);
		glm::vec2 t1(-halfDims.x, halfDims.y);
		glm::vec2 t2(-halfDims.x, -halfDims.y);
		glm::vec2 t3(halfDims.x, -halfDims.y);
		glm::vec2 t4(halfDims.x, halfDims.y);

		t1 = rotatePoint(t1, angle) + halfDims;
		t2 = rotatePoint(t2, angle) + halfDims;
		t3 = rotatePoint(t3, angle) + halfDims;
		t4 = rotatePoint(t4, angle) + halfDims;

		temp->topLeft.color = *color;
		temp->topLeft.position = glm::vec2(destinationRectangle->x +t1.x, destinationRectangle->y + t1.y);
		temp->topLeft.uv = glm::vec2(uvRectangle->x, uvRectangle->y + uvRectangle->w);
		temp->topLeft.textureid = texture;

		temp->bottomLeft.color = *color;
		temp->bottomLeft.position = glm::vec2(destinationRectangle->x + t2.x, destinationRectangle->y + t2.y);
		temp->bottomLeft.uv = glm::vec2(uvRectangle->x, uvRectangle->y);
		temp->bottomLeft.textureid = texture;


		temp->bottomRight.color = *color;
		temp->bottomRight.position = glm::vec2(destinationRectangle->x + +t3.x, destinationRectangle->y + t3.y);
		temp->bottomRight.uv = glm::vec2(uvRectangle->x + uvRectangle->z, uvRectangle->y);
		temp->bottomRight.textureid = texture;


		temp->topRight.color = *color;
		temp->topRight.position = glm::vec2(destinationRectangle->x + +t4.x, destinationRectangle->y + t4.y);
		temp->topRight.uv = glm::vec2(uvRectangle->x + uvRectangle->z, uvRectangle->y + uvRectangle->w);
		temp->topRight.textureid = texture;
	}
	else 
	{
		temp->topLeft.color = *color;
		temp->topLeft.position = glm::vec2(destinationRectangle->x, destinationRectangle->y + destinationRectangle->w);
		temp->topLeft.uv = glm::vec2(uvRectangle->x, uvRectangle->y + uvRectangle->w);
		temp->topLeft.textureid = texture;


		temp->bottomLeft.color = *color;
		temp->bottomLeft.position = glm::vec2(destinationRectangle->x, destinationRectangle->y);
		temp->bottomLeft.uv = glm::vec2(uvRectangle->x, uvRectangle->y );
		temp->bottomLeft.textureid = texture;


		temp->bottomRight.color = *color;
		temp->bottomRight.position = glm::vec2(destinationRectangle->x + destinationRectangle->z, destinationRectangle->y);
		temp->bottomRight.uv = glm::vec2(uvRectangle->x + uvRectangle->z, uvRectangle->y);
		temp->bottomRight.textureid = texture;


		temp->topRight.color = *color;
		temp->topRight.position = glm::vec2(destinationRectangle->x + destinationRectangle->z, destinationRectangle->y + destinationRectangle->w);
		temp->topRight.uv = glm::vec2(uvRectangle->x + uvRectangle->z, uvRectangle->y + uvRectangle->w);
		temp->topRight.textureid = texture;

	}
	bool insert = true;
	for (int i = 0; i < 30; i++) 
	{
		if(batch->bindtextures[i] == texture)
		{
			insert = false;
			break;
		}
	}
	if(insert)
	{
		batch->bindtextures[batch->numTexturesToBind++] = texture;
	}
}
void create_render_batches(SpriteBatch* batch)
{
	if (batch->sortingGlyphs._size == 0) return;
	batch->VertexBuffer.clear_array();
	batch->VertexBuffer.resize_array(batch->sortingGlyphs._size * 6);
	int offset = 0;
	int cv = 0;

	//RenderBatch* temp = batch->renderbatches.get_new_item();

	//temp->numVertices = 6;
	//temp->offset = 0;
	//temp->texture = batch->sortingGlyphs.data[0]->texture;  
	batch->VertexBuffer.data[cv++] = batch->sortingGlyphs.data[0]->topLeft;
	batch->VertexBuffer.data[cv++] = batch->sortingGlyphs.data[0]->bottomLeft;
	batch->VertexBuffer.data[cv++] = batch->sortingGlyphs.data[0]->bottomRight;
	batch->VertexBuffer.data[cv++] = batch->sortingGlyphs.data[0]->bottomRight;
	batch->VertexBuffer.data[cv++] = batch->sortingGlyphs.data[0]->topRight;
	batch->VertexBuffer.data[cv++] = batch->sortingGlyphs.data[0]->topLeft;
	offset += 6;

	for (int cg = 1; cg <  batch->sortingGlyphs._size; cg++)
	{
		//if (batch->sortingGlyphs.data[cg]->texture != batch->sortingGlyphs.data[cg - 1]->texture) // if tecture is different we will place new render patch
		//{
		//	//_renderBatches.emplace_back(offset, 6, _sortingGlyphs[cg]->texture); // get 6 vertecies
		//	RenderBatch *temp = emblace_back<RenderBatch>(&batch->dynArrRenderBatches);
		//	temp->numVertices = 6;
		//	temp->offset = offset;
		//	temp->texture = batch->sortingGlyphs.data[cg]->texture;
		//}
		//else
		//{
		//	get_back<RenderBatch>(batch->dynArrRenderBatches)->numVertices += 6;// saattaa mennä riki kun väärä arvo palautetaan HOX
		//	//_renderBatches.back().numVertices += 6;
		//}

		batch->VertexBuffer.data[cv++] = batch->sortingGlyphs.data[cg]->topLeft;
		batch->VertexBuffer.data[cv++] = batch->sortingGlyphs.data[cg]->bottomLeft;
		batch->VertexBuffer.data[cv++] = batch->sortingGlyphs.data[cg]->bottomRight;
		batch->VertexBuffer.data[cv++] = batch->sortingGlyphs.data[cg]->bottomRight;
		batch->VertexBuffer.data[cv++] = batch->sortingGlyphs.data[cg]->topRight;
		batch->VertexBuffer.data[cv++] = batch->sortingGlyphs.data[cg]->topLeft;

		offset += 6;
	}
	//for(int i = 0; i < batch->VertexBuffer._size;i++)
	//{
	//	printf("%d    %f %f \n", batch->VertexBuffer.data[i].position.x, batch->VertexBuffer.data[i].position.y);
	//}
	glBindBuffer(GL_ARRAY_BUFFER, batch->vbo);
	//orphan the buffer
	glBufferData(GL_ARRAY_BUFFER, batch->VertexBuffer._size * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);
	//upload the data
	glBufferSubData(GL_ARRAY_BUFFER, 0, batch->VertexBuffer._size * sizeof(Vertex), batch->VertexBuffer.data);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	batch->numVerts = batch->VertexBuffer._size;
	batch->VertexBuffer.clear_array();
}
void post_batch_process(SpriteBatch* batch)
{
	//resize_array<Glyph*>(&(batch->dynArrSortingGlyphs), get_array_size(batch->dynArrGlyphs));
	batch->sortingGlyphs.clear_array();
	batch->sortingGlyphs.resize_array(batch->glyphs._size);
	for (int i = 0; i < batch->glyphs._size; i++)
	{
		batch->sortingGlyphs.data[i] = &batch->glyphs.data[i];
	}
	batch->sortingGlyphs.quick_sort(0, batch->sortingGlyphs._size, [](Glyph** lhv, Glyph** rhv) {return (*lhv)->depth < (*rhv)->depth; });
	//quick_sort_array<Glyph*>(batch->dynArrSortingGlyphs, 0, get_array_size(batch->dynArrSortingGlyphs), [](Glyph** lhv, Glyph** rhv) {return (*lhv)->texture < (*rhv)->texture; });



	create_render_batches(batch);
}
#ifdef P_WINDOWS
int render_batch(SpriteBatch *batch,Shader* shader,spriteCache* cache,glm::mat4x4* camMatrix)
{
	if (batch->numVerts == 0) return 0;


	use_shader(shader);
	//	glActiveTexture(GL_TEXTURE0);// tells u want to use texture unit 0	
	//glCheckError();
	set_matrix(shader, "P", camMatrix);
	GLint textureUniform = get_uniform_location(shader, "mySamples");

	for(int i = 0; i < batch->numTexturesToBind;i++)
	{
		glActiveTexture(GL_TEXTURE0 + batch->bindtextures[i]);
		glBindTexture(GL_TEXTURE_2D, cache->bindedTextures.textures[batch->bindtextures[i]].ID);
	}
	//int* tes = (int*)calloc(30, sizeof(int));
	glUniform1iv(textureUniform, 30, batch->bindableTextures);






	int ret = 0;
	glBindVertexArray(batch->vao);
	glDrawArrays(GL_TRIANGLES,0,batch->numVerts);
	//for (int i = 0; i < get_array_size(batch->dynArrRenderBatches); i++)
	//{
	//	ret++;
	//	glBindTexture(GL_TEXTURE_2D, batch->dynArrRenderBatches[i].texture);
	//	glDrawArrays(GL_TRIANGLES, batch->dynArrRenderBatches[i].offset, batch->dynArrRenderBatches[i].numVertices);
	//}
	//batch->numVerts = 0;

	glBindVertexArray(0);
	unuse_shader(shader);
	return ret;
}
#endif
#ifdef P_ANDROID
void render_batch(SpriteBatch *batch)
{
	//glBindVertexArray(batch->vao);
	glBindBuffer(GL_ARRAY_BUFFER, batch->vbo); // vertex_buffer is retrieved from glGenBuffers

	for (int i = 0; i < get_array_size(batch->dynArrRenderBatches); i++)
	{
		//positions
		glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,sizeof(Vertex),(void*)offsetof(Vertex, position));
		//this is the color attribute pointer
		glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), (void*)offsetof(Vertex, color));
		//uv atrrib pointer
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));


		glBindTexture(GL_TEXTURE_2D, batch->dynArrRenderBatches[i].texture);
		glDrawArrays(GL_TRIANGLES, batch->dynArrRenderBatches[i].offset, batch->dynArrRenderBatches[i].numVertices);
	}
	//glBindVertexArray(0);


	/* glCheckError();
    glBindTexture(GL_TEXTURE_2D,1);
	glBindBuffer(GL_ARRAY_BUFFER, VBO); // vertex_buffer is retrieved from glGenBuffers
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO); // index_buffer is retrieved from glGenBuffers
	//position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	//color
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	//texturecoord
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glCheckError();*/
}
#endif
#ifdef P_WINDOWS
void dispose_batch(SpriteBatch *batch)
{
	//free_dyn_array(batch->dynArrGlyphs);
	//free_dyn_array(batch->dynArrRenderBatches);
	//free_dyn_array(batch->dynArrSortingGlyphs);
	batch->glyphs.dispose_array();
	batch->sortingGlyphs.dispose_array();
	batch->VertexBuffer.dispose_array();
	if (batch->vao != 0) {
		glDeleteVertexArrays(1, &batch->vao);
		batch->vao = 0;
	}
	if (batch->vbo != 0) {
		glDeleteBuffers(1, &batch->vbo);
		batch->vbo = 0;
	}
}
#endif
#ifdef P_ANDROID
void dispose_batch(SpriteBatch *batch)
{
    free_dyn_array(batch->dynArrGlyphs);
    free_dyn_array(batch->dynArrRenderBatches);
    free_dyn_array(batch->dynArrSortingGlyphs);
    if (batch->vbo != 0) {
        glDeleteBuffers(1, &batch->vbo);
        batch->vbo = 0;
    }
}
//todo että tarviiko bindaa koko aikaa kaikkea
#endif