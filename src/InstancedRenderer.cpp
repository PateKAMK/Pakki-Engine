#include <InstancedRenderer.h>
#include <filesystem.h>


namespace InRender
{

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

		GLuint frag = compile_shader(GL_VERTEX_SHADER, frg);
		free(frg);
		fatalerror(frag);

		in->instaShader.program = glCreateProgram();

		glAttachShader(in->instaShader.program, vert);
		glAttachShader(in->instaShader.program, frag);

		add_attribute(&in->instaShader, "VertexPos");
		add_attribute(&in->instaShader, "VertexUv");
		fatalerror(link_shader(&in->instaShader, vert, frag));

		glGenVertexArrays(1, &(in->vao));
		glBindVertexArray(in->vao);

		glGenBuffers(1, &in->vbo);
		glBindBuffer(GL_ARRAY_BUFFER, in->vbo);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

		//this is the position attribute pointer
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(inVertex, pos));
		//this is the uv attribute pointer
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(inVertex, uv));
		
		glBindVertexArray(0);

		in->vertexbuffer.init_array(1000);
	}

	void push_to_renderer(instaRenderer* in,const float* x,const float* y,const float* z,const float* w,const float* uv1,const float* uv2,const float* uv3,const float* uv4)
	{
		inVertex* v1 = in->vertexbuffer.get_new_item();//topleft
		v1->pos.x = *x;
		v1->pos.y = *y + *w;
		v1->uv.x = *uv1;
		v1->uv.y = *uv2 + *uv4;

		inVertex* v2 = in->vertexbuffer.get_new_item();//bottom left
		v2->pos.x = *x;
		v2->pos.y = *y;
		v2->uv.x = *uv1;
		v2->uv.y = *uv2;


		inVertex* v3 = in->vertexbuffer.get_new_item();//bottom right
		v3->pos.x = *x + *z;
		v3->pos.y = *y ;
		v3->uv.x = *uv1 + *uv2;
		v3->uv.y = *uv2;


		inVertex* vcopy1 = in->vertexbuffer.get_new_item();//bottom left
		*vcopy1 = *v3;


		inVertex* v4 = in->vertexbuffer.get_new_item();//topright
		v4->pos.x = *x + *z;
		v4->pos.y = *y + *w;
		v4->uv.x = *uv1 + *uv2;
		v4->uv.y = *uv2 + *uv4;

		inVertex* vcopy2 = in->vertexbuffer.get_new_item();
		*vcopy2 = *v1;
	}
	void create_buffers(instaRenderer* in)
	{
		glBindBuffer(GL_ARRAY_BUFFER, in->vbo);
		glBufferData(GL_ARRAY_BUFFER, in->vertexbuffer._size * sizeof(inVertex),nullptr , GL_DYNAMIC_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, in->vertexbuffer._size * sizeof(inVertex), in->vertexbuffer.data);

		in->vertexbuffer.clear_array();
	}
	void render(instaRenderer* in,GLuint tex)
	{
		use_shader(&in->instaShader);
		
		glBindVertexArray(in->vao);


	}



}