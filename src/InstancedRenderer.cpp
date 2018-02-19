#include <InstancedRenderer.h>
#include <filesystem.h>


namespace InRender
{
	static const GLfloat g_vertex_buffer_data[] = {
		-0.5f,  0.5f,
		0.5f, -0.5f,
		-0.5f, -0.5f,

		-0.5f,  0.5f,
		0.5f, -0.5f, 
		0.5f,  0.5f, 
	};

	void init_renderer(instaRenderer *in)
	{
		char* vs = NULL;
		char* frg = NULL;
		int size = 0;
		FileSystem::load_file_to_buffer("instavert.txt", &vs, &size);
		fatalerror(vs);

		GLuint vert = compile_shader(GL_VERTEX_SHADER, vs);
		free(vs);
		fatalerror(vert);

		FileSystem::load_file_to_buffer("instafrag.txt", &frg, &size);
		fatalerror(frg);

		GLuint frag = compile_shader(GL_FRAGMENT_SHADER, frg);
		free(frg);
		fatalerror(frag);

		in->instaShader.program = glCreateProgram();

		glAttachShader(in->instaShader.program, vert);
		glAttachShader(in->instaShader.program, frag);

		add_attribute(&in->instaShader, "uv");
		add_attribute(&in->instaShader, "vert");
		add_attribute(&in->instaShader, "pos");
		add_attribute(&in->instaShader, "scale");

		fatalerror(link_shader(&in->instaShader, vert, frag));


	


		glGenVertexArrays(1, &(in->vao));
		glBindVertexArray(in->vao);

		glGenBuffers(1, &in->position);
		glBindBuffer(GL_ARRAY_BUFFER, in->position);

		glBufferData(GL_ARRAY_BUFFER, 1000 * sizeof(inVertex), NULL, GL_DYNAMIC_DRAW);// bind later  positions + scale

		glGenBuffers(1, &in->uv);
		glBindBuffer(GL_ARRAY_BUFFER, in->uv);

		glBufferData(GL_ARRAY_BUFFER, 10000 * sizeof(vec2), NULL, GL_DYNAMIC_DRAW);// bind later  uv for each vertex


		glGenBuffers(1, &in->vertex);
		glBindBuffer(GL_ARRAY_BUFFER, in->vertex);
		glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);//bind now	vertex data




		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glEnableVertexAttribArray(3);

		//this is the uv attribute pointer
		glBindBuffer(GL_ARRAY_BUFFER, in->uv);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(vec2), (void*)0);
		//glVertexAttribDivisor(0, 1); dont set?
		//vertexposition
		glBindBuffer(GL_ARRAY_BUFFER, in->vertex);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, (void*)0);
		glVertexAttribDivisor(1, 0);//vert
		//position
		glBindBuffer(GL_ARRAY_BUFFER, in->position);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(inVertex), (void*)offsetof(inVertex,pos));
		glVertexAttribDivisor(2, 1);//pos
		//scale
		glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(inVertex), (void*)offsetof(inVertex, scale));
		glVertexAttribDivisor(3, 1);//scale




		//this is the uv attribute pointer
		//glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(inVertex, uv));
		
		glBindVertexArray(0);

		in->positionbuffer.init_array(1000);
		in->uvBuffer.init_array(10000);
	}
	static int num  = 0;
	void push_to_renderer(instaRenderer* in, const float& x, const float& y,const float& scale, const float& uv1, const float& uv2, const float& uv3, const float& uv4)
	{
		num++;
		inVertex* v1 = in->positionbuffer.get_new_item();//topleft
		v1->pos.x = x;
		v1->pos.y = y;
		v1->scale = scale;

		vec2* buf = in->uvBuffer.get_array(6);
		vec2* u1 = &buf[0];
		u1->x = uv1;
		u1->y = uv2 + uv4;
		vec2* u2 = &buf[1];//bot left
		u2->x = uv1 + uv3;
		u2->y = uv2;
		vec2* u3 = &buf[2];//bot right
		u3->x = uv1;
		u3->y = uv2;
		vec2* u4 = &buf[3];//bot right
		*u4 = *u1;
		vec2* u5 = &buf[4];//top right
		*u5 = *u2;
		vec2* u6 = &buf[5];//top left
		u6->x = uv1 + uv3;
		u6->y = uv2 + uv4;

/*		v1->pos.x = *x;
		v1->pos.y = *y + *w*/;
		/*v1->uv.x = *uv1;
		v1->uv.y = *uv2 + *uv4;*/

	//	inVertex* v2 = in->vertexbuffer.get_new_item();//bottom left
	//	/*v2->pos.x = *x;
	//	v2->pos.y = *y*/;
	//	/*v2->uv.x = *uv1;
	//	v2->uv.y = *uv2;*/


	//	inVertex* v3 = in->vertexbuffer.get_new_item();//bottom right
	///*	v3->pos.x = *x + *z;
	//	v3->pos.y = *y ;
	//	v3->uv.x = *uv1 + *uv2;
	//	v3->uv.y = *uv2;*/


	//	inVertex* vcopy1 = in->vertexbuffer.get_new_item();//bottom left
	//	/**vcopy1 = *v3;*/


	//	inVertex* v4 = in->vertexbuffer.get_new_item();//topright
	//	/*v4->pos.x = *x + *z;
	//	v4->pos.y = *y + *w;
	//	v4->uv.x = *uv1 + *uv2;
	//	v4->uv.y = *uv2 + *uv4;*/

	//	inVertex* vcopy2 = in->vertexbuffer.get_new_item();
	//	/**vcopy2 = *v1;*/
	}
	void create_buffers(instaRenderer* in)
	{
		/*glBindBuffer(GL_ARRAY_BUFFER, in->vbo);
		glBufferData(GL_ARRAY_BUFFER, in->vertexbuffer._size * sizeof(inVertex),nullptr , GL_DYNAMIC_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, in->vertexbuffer._size * sizeof(inVertex), in->vertexbuffer.data);*/

		glBindBuffer(GL_ARRAY_BUFFER, in->position);
		glBufferData(GL_ARRAY_BUFFER, in->positionbuffer._size * sizeof(inVertex), NULL, GL_DYNAMIC_DRAW); // Buffer orphaning, a common way to improve streaming perf. See above link for details.
		glBufferSubData(GL_ARRAY_BUFFER, 0, in->positionbuffer._size * sizeof(inVertex), in->positionbuffer.data);

		glBindBuffer(GL_ARRAY_BUFFER, in->uv);
		glBufferData(GL_ARRAY_BUFFER, in->uvBuffer._size * sizeof(vec2), NULL, GL_DYNAMIC_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER,0, in->uvBuffer._size*sizeof(vec2), in->uvBuffer.data);

		
		glBindBuffer(GL_ARRAY_BUFFER, in->vertex);
		glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);//bind now	vertex data

		glBindBuffer(GL_ARRAY_BUFFER, 0);

		in->num_items = in->positionbuffer._size;
		in->positionbuffer.clear_array();
		in->uvBuffer.clear_array();
	}
	void render(instaRenderer* in,GLuint tex,glm::mat4x4* camMatrix)
	{
		use_shader(&in->instaShader);
		
		glBindVertexArray(in->vao);

		set_matrix(&in->instaShader, "P", camMatrix);
		GLint textureUniform = get_uniform_location(&in->instaShader, "mySampler");
		glUniform1i(textureUniform, 0);
		glBindTexture(GL_TEXTURE_2D, tex);
		glBindVertexArray(in->vao);

		glDrawArraysInstanced(GL_TRIANGLES, 0, 6, in->num_items);

		glBindVertexArray(0);
		unuse_shader(&in->instaShader);
	}



}