
#include <pakki.h>
#ifndef P_ANDROID
#include <dybamic_array.h>
#include <cstring>
#include <vadefs.h>


static unsigned int TXTID = 0; // vie pois ku voit TODO
static unsigned int FONTID = 0; // vie pois ku voit TODO
#define DEFAULT_XOFFSET 25
#define DEFAULT_YOFFSET 30//25
#define HEADER_DEFULTHEIGHT 15
namespace Pakki
{
	int fet_id()
	{
		return TXTID;
	}

	inline bool mouse_collision(const glm::vec2* Mid, const glm::vec2* dims, const glm::vec2* point);

	void init_pakki(PakkiContext* context, unsigned int id, unsigned int textureFontID)
	{
		memset(context, 0, sizeof *context);
		allocate_new_array<drawdata>(&context->dyArrDrawBuffer,200);
		allocate_new_array<Vertex>(&context->vertexDrawBuffer);
		allocate_new_array<Renderbatch>(&context->batchDrawBuffer);
		allocate_new_array<drawdata>(&context->textBuffer);
		TXTID = id;
		FONTID = textureFontID;
	}
	void disposepakki(PakkiContext* context)
	{
		free_dyn_array<drawdata>(context->dyArrDrawBuffer);
		free_dyn_array<Vertex>(context->vertexDrawBuffer);
		free_dyn_array<Renderbatch>(context->batchDrawBuffer);
		free_dyn_array<drawdata>(context->textBuffer);
		context->dyArrDrawBuffer = NULL;
		context->vertexDrawBuffer = NULL;
		context->batchDrawBuffer = NULL;
		context->textBuffer = NULL;
		context->batchDrawBuffer = 0;
		memset(context, 0 ,sizeof *context);
	}
	void start_frame(PakkiContext* context, glm::vec2 mousepos, MouseState mousestate)
	{
		context->lastTouchPosX = context->inputs.mousePosition.x;
		context->lastTouchPosY = context->inputs.mousePosition.y;
		context->inputs.mousePosition = mousepos;
		clear_array<Vertex>(context->vertexDrawBuffer);
		clear_array<Renderbatch>(context->batchDrawBuffer);


		context->inputs.mousetate = context->inputs.mousetate & CurrentlyActive ? LastUpdateActive : InActive; // if last frame was active
		if (mousestate & CurrentlyActive) context->inputs.mousetate ^= CurrentlyActive;
		if (context->inputs.mousetate == InActive) context->Windows->state = context->Windows->state &~Hot;
		if(context->inputs.mousetate == CurrentlyActive) //find new hot
		{
			bool hot = true;
			for (Window* iterator = context->Windows; iterator != &context->Windows[context->numWindows]; iterator++) //reset states and create new ones
			{

				if(!mouse_collision(&iterator->position, &iterator->dimensions, &mousepos))
				{
					iterator->state = Inactive;
					continue;
				}
				if(iterator->state & Updated && hot)
				{
					context->Windows->state = context->Windows->state &~ Hot; // dehot old
					iterator->state |= (Hot | Active);						// hotten new
					iterator->state = iterator->state &~ Updated;
					hot = false;

					int current_pos = iterator - context->Windows;

					for (auto i = current_pos; i > 0; i--)			// set hot item to the top so rendering order keeps relevant
					{
						std::swap<Window>(context->Windows[i], context->Windows[i - 1]);
					}
					continue;
				}
				else if(iterator->state & Updated)
				{
					iterator->state |= Active;
				}
				iterator->state = iterator->state &~ Updated;	//window is not updatet yet so turn it off
			}
		}
		else 
		{
			for (Window* iterator = context->Windows; iterator != &context->Windows[context->numWindows]; iterator++)
			{
				if (iterator->state & Updated && mouse_collision(&iterator->position, &iterator->dimensions, &mousepos))
				{
					iterator->state |= Active;
					//printf("setting active window\n");
				}
				else if (iterator->state & Updated)
				{
					iterator->state = iterator->state &~ Active;
					//printf("turning active window off\n");
				}
				iterator->state = iterator->state &~ Updated;
			}
		}
		memset(context->UpdateQue, 0, sizeof(Window*)*MAX_WINDOWS); // after finding hot item reset update gue
		context->activeWindows = 0;
		clear_array<drawdata>(context->dyArrDrawBuffer);
		//context->numDrawElements = 0;
	}
	inline Window* find_window(Window* windows, WINID id, unsigned int size)
	{
		for (Window* iterator = windows; iterator != &windows[size]; iterator++)
		{
			if (iterator->id == id) return iterator;
		}
		return nullptr;
	}

	inline Window* create_window(PakkiContext* context, WINID id, glm::vec2 pos, glm::vec2 dims, WindowEnums type = Window_Default)
	{
		assert(context->numWindows < 100);
		Window* newWin = &context->Windows[context->numWindows++];
		memset(newWin, 0, sizeof(Window));	//debug purposes
		newWin->position = pos;
		newWin->dimensions = dims;
		newWin->id = id;
		newWin->type = type;
		auto i = context->numWindows - 1;
		for (; i > 1; i--)		//push last window to 2nd position so order is correct
		{
			std::swap<Window>(context->Windows[i], context->Windows[i - 1]);
		}
		return &context->Windows[i];
	}
	void window(PakkiContext* context, WINID ID, int xpos, int ypos, int dimx, int dimy, WindowEnums WindowFlag/* = Window_Default*/)
	{
		assert(!context->currentWindow);
		Window* currentWin = find_window(context->Windows, ID, context->numWindows);
		if (!currentWin)
		{
			currentWin = create_window(context, ID, glm::vec2(xpos, ypos), glm::vec2(dimx, dimy),WindowFlag);
		}
		assert(!(currentWin->state & Updated)); // if already updatet dont do again
		context->currentWindow = currentWin;
		context->UpdateQue[context->activeWindows++] = currentWin;
		currentWin->myBufferDataSize = 0;


		int i = get_array_size<drawdata>(context->dyArrDrawBuffer);
		currentWin->myBufferDataStart = emblace_back<drawdata>(&context->dyArrDrawBuffer);
		currentWin->myBufferDataSize++;

		currentWin->myBufferDataStart->position = currentWin->position;
		currentWin->myBufferDataStart->dimensions = glm::vec2(dimx, dimy);
		currentWin->myBufferDataStart->txtID = TXTID;
		currentWin->myBufferDataStart->uv = glm::vec4(0.5, 0.5, 0.25, 0.25); 
		currentWin->myBufferDataStart->color = { 50,50,50,200 };
		currentWin->state |= Updated;


		if (currentWin->state & Hot)currentWin->state ^= NoWidgetPressed; // off


		currentWin->xOffset = DEFAULT_XOFFSET;
		currentWin->yOffset = DEFAULT_YOFFSET;
		
		if(!(currentWin->type & Window_No_Header))
		{
			//context->numDrawElements++;
			currentWin->myBufferDataSize++;
			drawdata* head = emblace_back<drawdata>(&context->dyArrDrawBuffer);
			head->position = glm::vec2(currentWin->position.x, currentWin->position.y + currentWin->dimensions.y - HEADER_DEFULTHEIGHT);
			head->dimensions = glm::vec2(currentWin->dimensions.x, HEADER_DEFULTHEIGHT);
			head->txtID = TXTID;
			head->uv = glm::vec4(0.75, 0.2195, 0.25, 0.061); // kun huomaat ett� olet vammainen ota pois   0.7, 0.7, 0.25, 0.25 / 0.5, 0.5, 0.25, 0.25
			head->color = { 220,220,240,255 };
			currentWin->yOffset += HEADER_DEFULTHEIGHT * 2;
		}
	}
	void end_window(PakkiContext* context)
	{
		//if window is still hot, aka no widgets touched we will move it
		// todo window flag wich prevents this
		Window* win = context->currentWindow;
		if(win->state & Hot)
		{
			if(!(win->state & Active))
			{
				//printf("window is not active when it should \n");
			}
			if (win->state & NoWidgetPressed) // if anything else before wasnt pressed
			{
				//printf("no button pressed is pressed %d\n", win->state);

				if (context->inputs.mousetate == CurrentlyActive)
				{
					win->state |= Move; // when window becomes active we can move it
				}
				if (context->inputs.mousetate == LastUpdateActive)
				{
					win->state = win->state &~Move;  //when mouse is released we dont want to move it anymore
				}
			}

			if(!(win->type == Window_No_Move) && win->state & Move)
			{
				float lvecx = context->lastTouchPosX - win->position.x;
				float lvecy = context->lastTouchPosY - win->position.y;

				float nvecx = context->inputs.mousePosition.x - win->position.x;
				float nvecy = context->inputs.mousePosition.y - win->position.y;


				win->position -= glm::vec2(lvecx - nvecx,lvecy - nvecy);
			}
			win->state = win->state &~ NoWidgetPressed;
		}


		context->currentWindow = nullptr;
		// todo sort widgets because here we know what window has in it

		unsigned int textbuffersize = get_array_size<drawdata>(context->textBuffer);
		//if(textbuffersize > 0)
		//{
		//	drawdata* b = get_back<drawdata>(context->textBuffer);
		//	for(drawdata* i = context->textBuffer; i!= b;i++)
		//	{
		//		win->myBufferDataSize++;
		//		push_back_dyn_array<drawdata>(&context->dyArrDrawBuffer, i);
		//	}
		//int ssss = get_array_size<drawdata>(context->dyArrDrawBuffer);
		//printf("current size =  %d\n",ssss);
		//for (int i = 0; i < textbuffersize; i++)
		//{
		//	printf("%d\n", context->textBuffer[i].txtID);

		//}
		//printf("\n");
			for(int i = 0; i < textbuffersize;i++)
			{
				drawdata* d = emblace_back<drawdata>(&context->dyArrDrawBuffer);
				win->myBufferDataSize++;
				*d = context->textBuffer[i];
				//printf("inserting to %d value %d\n", d - context->dyArrDrawBuffer, context->textBuffer[i].txtID);
				//int kkk = 0;
			}
			clear_array<drawdata>(context->textBuffer);
			//printf("\n");

			//for(int i = 0; i < win->myBufferDataSize;i++)
			//{
			//	printf("%d\n", win->myBufferDataStart[i].txtID);
			//}
		//	printf(" my size %d", win->myBufferDataSize);
		//}
		//quick_sort_array<drawdata>(win->myBufferDataStart, 0, win->myBufferDataSize, [](drawdata* lhv, drawdata* rhv) {return lhv->txtID == TXTID || ; });
	}

	void inline format(Window* window, FormattingFlag flag,float dimy,float dimx)
	{
		if(flag & NewLine)
		{
			window->xOffset = DEFAULT_XOFFSET;
			window->yOffset += DEFAULT_YOFFSET + dimy;
		}
		if(flag & SameLine)
		{
			window->xOffset += DEFAULT_XOFFSET + dimx;
			window->yOffset  -= dimy;
		}
	}
	void sliderf(PakkiContext* context, float* userxdata,const char* txt){
		Window* win = context->currentWindow;
		assert(win);
		win->myBufferDataSize++;
		drawdata* sliderdata = emblace_back<drawdata>(&context->dyArrDrawBuffer);
		sliderdata->color = Color{ 255,255,255,255 };
		sliderdata->dimensions = glm::vec2(80,10);
		sliderdata->position = glm::vec2((win->position.x - win->dimensions.x) + win->xOffset + 70, (win->position.y + win->dimensions.y) - win->yOffset);
		sliderdata->txtID = TXTID;
		sliderdata->uv = glm::vec4(0.865, 0.656, 0.085, 0.033);

		float range = 80 * 2;
		float currentpos =  (range - (range * (*userxdata))) + ((win->position.x - win->dimensions.x) + win->xOffset + 70 - 80);
		glm::vec2 currentboxPos(currentpos, (win->position.y + win->dimensions.y) - win->yOffset);
		if(win->state & Hot)
		{
			bool collides = mouse_collision(&currentboxPos, &glm::vec2(10, 15), &context->inputs.mousePosition);
			if(collides && !(win->state & Move))
			{
				win->state = win->state &~NoWidgetPressed;
				currentboxPos.x = context->inputs.mousePosition.x;
				*userxdata = (((win->position.x - win->dimensions.x) + win->xOffset + 70 + 80) - currentboxPos.x) / (80 * 2);
				if (*userxdata > 1.f)*userxdata = 1.f;
				if (*userxdata < 0.f)*userxdata = 0.f;

			}
		}

		drawdata* butt = emblace_back<drawdata>(&context->dyArrDrawBuffer);
		butt->color = Color{ 200,200,200,255 };
		butt->dimensions = glm::vec2(10, 15);
		butt->position = currentboxPos;
		butt->txtID = TXTID;
		butt->uv = glm::vec4(0.935, 0.938, 0.065, 0.065);

		win->yOffset -= 10;
		format(win, NewLine, 10, 10);
		text(context, "%s %f",txt,*userxdata);
	}



	float hardtext(PakkiContext* context, char* buf, int* w);


	bool button(PakkiContext* context, const char* fmt, ...)
	{
		va_list args;
		__crt_va_start(args, fmt);
		char buf[100];
		int w = vsnprintf(buf, 100, fmt, args);
		if (w == -1 || w >= 100) w = 99;
		buf[w] = 0;
		__crt_va_end(args);
		Window* win = context->currentWindow;
		assert(win);
		win->yOffset += 10;
		//win->xOffset += 10;

		float xdim = hardtext(context, buf, &w);


		FormattingFlag flag = NewLine;
		//do new line
		/*win->xOffset = DEFAULT_XOFFSET;
		win->yOffset += DEFAULT_YOFFSET;*/
		bool ret = false;
		bool collision = false;
		//add new sprite

		//context->numDrawElements++;
		win->myBufferDataSize++;
		drawdata* buttondata = emblace_back<drawdata>(&context->dyArrDrawBuffer);
		buttondata->position = glm::vec2(xdim + win->position.x - win->dimensions.x, (win->position.y + win->dimensions.y) - win->yOffset);//currentWin->position;
		buttondata->dimensions = glm::vec2(w + 10, 20);
		//printf("button %f  %f \n", (win->position.x - win->dimensions.x) + win->xOffset, (win->position.y + win->dimensions.y) - win->yOffset);
		//printf("mouse %f  %f \n", context->inputs.mousePosition.x, context->inputs.mousePosition.y);
		buttondata->txtID = TXTID;
		buttondata->uv = glm::vec4(0.935, 0.938, 0.065, 0.065);

		if(win->state & Active) // tee aikasemmin jos haluut vaihtaa v�ri�
		{
			collision = mouse_collision(&glm::vec2(xdim + win->position.x - win->dimensions.x, (win->position.y + win->dimensions.y) - win->yOffset), &glm::vec2(w + 10, 20), &context->inputs.mousePosition);

			if(collision && win->state & Hot && !(win->state & Move))
			{
				win->state = win->state &~ NoWidgetPressed; // widget was pressed so dont move window
				//printf("BUTTON IS PRESSED %d\n",win->state);
				ret = context->inputs.mousetate == LastUpdateActive;
			}
		}

		if(collision && context->inputs.mousetate & CurrentlyActive)
		{
			buttondata->color = { 200,200,230,200 };
		}
		else if (collision)
		{
			buttondata->color = { 180,180,210,200 };
		}
		else
		{
			buttondata->color = { 170,170,190,200 };
		}

		//win->xOffset += 10;
		//win->yOffset +=  10;

		format(win, flag,10,10);
		return ret;
	}

	float hardtext(PakkiContext* context,char* buf,int* w)
	{


		unsigned int currensize = get_array_size<drawdata>(context->textBuffer);
		//resize_array<drawdata>(&context->textBuffer, w + currensize);
		Font* font = &context->font;

		Window* win = context->currentWindow;
		assert(win);
		float x = win->position.x - win->dimensions.x + win->xOffset;
		float y = win->position.y + win->dimensions.y - win->yOffset;
		float scale = 1;
		float curry = 0;
		for (char* p = buf; *p; p++)
		{
			if (*p < 32 || *p >= 128)continue;
			/* Calculate the vertex and texture coordinates */
			float x2 = x + font->character[*p].bl * scale;
			float y2 = -y - font->character[*p].bt * scale;


			float wi = font->character[*p].bw * scale;
			float he = font->character[*p].bh * scale;
			/* Advance the cursor to the start of the next character */
			x += font->character[*p].ax * scale;
			y += font->character[*p].ay * scale; // aina 0

			if (y == 0)
			{
				y = 0;
			}
			/* Skip quintessences(t: aleksi) that have no pixels */
			if (!wi || !he)
			{
				//font->negation +=6;
				continue;
			};
			drawdata* data = emblace_back<drawdata>(&context->textBuffer);


			data->color = Color{ 255,255,255,255 };
			data->txtID = font->txtid;
			assert(font->txtid == 5);
			data->position = glm::vec2(x2 + (wi / 2), -y2 - (he / 2));
			*w += wi / 2;
			data->dimensions = glm::vec2(wi / 2, he / 2);
			data->uv = glm::vec4(font->character[*p].tx, 1 - (font->character[*p].ty + (font->character[*p].bh / font->h)), font->character[*p].bw / font->w, (font->character[*p].bh / font->h));
			curry = x2;
		}

		//format(win, NewLine, 0, 0);
		return	win->xOffset + *w;
	}


	//void checkbox(PakkiContext* context, bool* userxdata, FormattingFlag flag)
	//{
	//	Window* win = context->currentWindow;
	//	assert(win);
	//	//do new line

	//	win->yOffset += 7.5;
	//	win->xOffset += 7.5;
	//	//add new sprite
	//	win->myBufferDataSize++;
	//	drawdata* buttondata = emblace_back<drawdata>(&context->dyArrDrawBuffer);
	//	buttondata->position = glm::vec2((win->position.x - win->dimensions.x) + win->xOffset, (win->position.y + win->dimensions.y) - win->yOffset);//currentWin->position;
	//	buttondata->dimensions = glm::vec2(15, 15);
	//	buttondata->txtID = TXTID;
	//	buttondata->uv = glm::vec4(0.935, 0.938, 0.065, 0.065);

	//	bool collision = false;

	//	if (win->state & Active) // tee aikasemmin jos haluut vaihtaa v�ri�
	//	{
	//		collision = mouse_collision(&glm::vec2((win->position.x - win->dimensions.x) + win->xOffset, (win->position.y + win->dimensions.y) - win->yOffset), &glm::vec2(20, 20), &context->inputs.mousePosition);
	//		if (collision)
	//		{
	//			win->state = win->state &~NoWidgetPressed;
	//		}
	//		if (collision && win->state & Hot && !(win->state & Move) && context->inputs.mousetate == LastUpdateActive)
	//		{
	//			*userxdata ^= 1;
	//		}
	//	}

	//	if (collision)
	//	{
	//		buttondata->color = { 150,150,170,200 };
	//	}
	//	else
	//	{
	//		buttondata->color = { 130,130,150,200 };
	//	}
	//	if (*userxdata)
	//	{
	//		win->myBufferDataSize++;
	//		buttondata = emblace_back<drawdata>(&context->dyArrDrawBuffer);
	//		buttondata->position = glm::vec2((win->position.x - win->dimensions.x) + win->xOffset, (win->position.y + win->dimensions.y) - win->yOffset);//currentWin->position;
	//		buttondata->dimensions = glm::vec2(15, 15);
	//		buttondata->txtID = TXTID;
	//		buttondata->uv = glm::vec4(0.874, 0.754, 0.03, 0.03);
	//		buttondata->color = { 255,255,255,255 };
	//	}


	//	//win->xOffset += 10;
	//	//win->yOffset += 10;
	//	format(win, flag, 7.5, 7.5);

	//}


	void text(PakkiContext* context,const char* fmt,...)
	{
		va_list args;
		__crt_va_start(args, fmt);
		char buf[100];
		int w = vsnprintf(buf, 100, fmt, args);
		if (w == -1 || w >= 100) w =99;
		buf[w] = 0;
		__crt_va_end(args);

		unsigned int currensize = get_array_size<drawdata>(context->textBuffer);
		//resize_array<drawdata>(&context->textBuffer, w + currensize);
		Font* font = &context->font;

		Window* win = context->currentWindow;
		assert(win);
		float x =  win->position.x - win->dimensions.x +win->xOffset;
		float y = win->position.y + win->dimensions.y  -win->yOffset;
		float scale = 1;
		for(char* p = buf;*p; p++)
		{
			if (*p < 32 || *p >= 128)continue;
			/* Calculate the vertex and texture coordinates */
			float x2 = x + font->character[*p].bl * scale ;
			float y2 = -y - font->character[*p].bt * scale;


			float wi = font->character[*p].bw * scale ;
			float he = font->character[*p].bh * scale ;
			/* Advance the cursor to the start of the next character */
			x += font->character[*p].ax * scale ;
			y += font->character[*p].ay * scale; // aina 0

			if(y == 0)
			{
				y = 0;
			}
			/* Skip quintessences(t: aleksi) that have no pixels */
			if (!wi || !he)
			{
				//font->negation +=6;
				continue;
			};
			drawdata* data = emblace_back<drawdata>(&context->textBuffer);


			data->color = Color{ 255,255,255,255 };
			data->txtID = font->txtid;
			assert(font->txtid == 5);
			data->position = glm::vec2(x2 + (wi / 2), -y2  - (he / 2) );
			data->dimensions = glm::vec2(wi / 2,he / 2 );
			data->uv = glm::vec4(font->character[*p].tx, 1 - (font->character[*p].ty + (font->character[*p].bh / font->h)) , font->character[*p].bw / font->w, (font->character[*p].bh / font->h));
		
		}

		format(win, NewLine, 0, 0);
	}

	void checkbox(PakkiContext* context, bool* userxdata, FormattingFlag flag)
	{
		Window* win = context->currentWindow;
		assert(win);
		//do new line
		
		win->yOffset += 7.5;
		win->xOffset += 7.5;
		//add new sprite
		win->myBufferDataSize++;
		drawdata* buttondata = emblace_back<drawdata>(&context->dyArrDrawBuffer);
		buttondata->position = glm::vec2((win->position.x - win->dimensions.x) + win->xOffset, (win->position.y + win->dimensions.y) - win->yOffset);//currentWin->position;
		buttondata->dimensions = glm::vec2(15, 15);
		buttondata->txtID = TXTID;
		buttondata->uv = glm::vec4(0.935, 0.938, 0.065, 0.065);

		bool collision = false;

		if (win->state & Active) // tee aikasemmin jos haluut vaihtaa v�ri�
		{
			collision = mouse_collision(&glm::vec2((win->position.x - win->dimensions.x) + win->xOffset, (win->position.y + win->dimensions.y) - win->yOffset), &glm::vec2(20, 20), &context->inputs.mousePosition);
			if(collision)
			{
				win->state = win->state &~NoWidgetPressed;
			}
			if (collision && win->state & Hot && !(win->state & Move) && context->inputs.mousetate == LastUpdateActive)
			{
				*userxdata ^= 1;
			}
		}

		if(collision)
		{
			buttondata->color = { 150,150,170,200 };
		}
		else
		{
			buttondata->color = { 130,130,150,200 };
		}
		if(*userxdata)
		{
			win->myBufferDataSize++;
			buttondata = emblace_back<drawdata>(&context->dyArrDrawBuffer);
			buttondata->position = glm::vec2((win->position.x - win->dimensions.x) + win->xOffset, (win->position.y + win->dimensions.y) - win->yOffset);//currentWin->position;
			buttondata->dimensions = glm::vec2(15, 15);
			buttondata->txtID = TXTID;
			buttondata->uv = glm::vec4(0.874, 0.754, 0.03, 0.03);
			buttondata->color = { 255,255,255,255 };
		}


		//win->xOffset += 10;
		//win->yOffset += 10;
		format(win, flag,-2, 7.5);

	}


	void render(PakkiContext* context)
	{
		int numBatches = 0;
		int vertCount = 0;



		if (context->numWindows == 0)return;

		unsigned int teeers = 0;
		for (int i = 0; i < context->activeWindows; i++)	//get drawable objects
		{
			teeers += context->UpdateQue[i]->myBufferDataSize;
		}
		//sort updateque
		quick_sort_array<Window*>(context->UpdateQue, 0, context->activeWindows, [context](Window** lhv, Window** rhv) {return  ((*lhv) - (*context->UpdateQue)) >= ((*rhv) - (*context->UpdateQue)); }); // if position in que is larger true
																																																		  //push to batch
																																																		  //for each window do batch
		unsigned int numDrawObjects = 0;
		for (int i = 0; i < context->activeWindows; i++)	//get drawable objects
		{
			numDrawObjects += context->UpdateQue[i]->myBufferDataSize;
		}

		Renderbatch* batchBuffer = context->batchDrawBuffer;
		//create vertex data
		resize_array<Vertex>(&context->vertexDrawBuffer, numDrawObjects * 6);
		//assert(numDrawObjects == 29);
		Vertex* DrawBuffer = context->vertexDrawBuffer;//(Vertex*)malloc(sizeof(Vertex) * numDrawObjects * 6);// 6 vertex per object, shader takes only position and uv as varying, matrix will be uniform and color todo later

		
		int offset = 0;

		int lastTextureId = -1;

		for (int win = 0; win < context->activeWindows; win++)//per win
		{
			for (auto wid = 0; wid < context->UpdateQue[win]->myBufferDataSize; wid++)//per widgets
			{
				//check here last texture and make sure that it is same as last, if it isnt make new batch
				if (lastTextureId == -1 || lastTextureId != context->UpdateQue[win]->myBufferDataStart[wid].txtID)
				{
					Renderbatch *temp = emblace_back<Renderbatch>(&batchBuffer);
					temp->numVertices = 6;
					temp->offset = offset;	//here batch starts
					temp->texture = context->UpdateQue[win]->myBufferDataStart[wid].txtID;
					lastTextureId = context->UpdateQue[win]->myBufferDataStart[wid].txtID;
				}
				else
				{
					get_back<Renderbatch>(batchBuffer)->numVertices += 6;
				}

				drawdata* data = &context->UpdateQue[win]->myBufferDataStart[wid]; // current image to render

				glm::vec4 destinationRectangle;


				destinationRectangle.x = data->position.x - data->dimensions.x;
				destinationRectangle.y = data->position.y - data->dimensions.y;
				destinationRectangle.z = data->dimensions.x * 2;
				destinationRectangle.w = data->dimensions.y * 2;


				Vertex topleft;
				topleft.position = glm::vec2(destinationRectangle.x, destinationRectangle.y + destinationRectangle.w);
				topleft.uv = glm::vec2(data->uv.x, data->uv.y + data->uv.w);
				topleft.color = data->color;

				Vertex topright;
				topright.position = glm::vec2(destinationRectangle.x + destinationRectangle.z, destinationRectangle.y + destinationRectangle.w);
				topright.uv = glm::vec2(data->uv.x + data->uv.z, data->uv.y + data->uv.w);
				topright.color = data->color;


				Vertex bottomleft;
				bottomleft.position = glm::vec2(destinationRectangle.x, destinationRectangle.y);
				bottomleft.uv = glm::vec2(data->uv.x, data->uv.y);
				bottomleft.color = data->color;


				Vertex bottomright;
				bottomright.position = glm::vec2(destinationRectangle.x + destinationRectangle.z, destinationRectangle.y);
				bottomright.uv = glm::vec2(data->uv.x + data->uv.z, data->uv.y);
				bottomright.color = data->color;



				DrawBuffer[vertCount++] = topleft;	//todo index buffering later
				DrawBuffer[vertCount++] = bottomleft;
				DrawBuffer[vertCount++] = bottomright;
				DrawBuffer[vertCount++] = bottomright;
				DrawBuffer[vertCount++] = topright;
				DrawBuffer[vertCount++] = topleft;

				offset += 6;
			}
			//store last texture id
			//set buffer size 0 per window and data position to nullptr
			//tldr reset window
			context->UpdateQue[win]->myBufferDataStart = NULL;
			context->UpdateQue[win]->myBufferDataSize = 0;
		}
		// give something to people to render HOX
		numBatches = get_array_size<Renderbatch>(batchBuffer);

		context->rendercallback(context->vertexDrawBuffer, context->batchDrawBuffer, numBatches, vertCount);
	}














	//void end_and_get_batches_frame(PakkiContext* context,int* numBatches, int *vertCount)
	//{
	//	if (context->numWindows == 0)return;
	//	//sort updateque
	//	quick_sort_array<Window*>(context->UpdateQue, 0, context->activeWindows, [context](Window** lhv, Window** rhv) {return  ((*lhv) - (*context->UpdateQue)) >= ((*rhv) - (*context->UpdateQue)); }); // if position in que is larger true
	//	//push to batch
	//		//for each window do batch
	//	unsigned int numDrawObjects = 0;
	//	for (int i = 0; i < context->activeWindows; i++)	//get drawable objects
	//	{
	//		numDrawObjects += context->UpdateQue[i]->myBufferDataSize;
	//	}

	//	Renderbatch* batchBuffer = context->batchDrawBuffer;
	//	//create vertex data
	//	resize_array<Vertex>(&context->vertexDrawBuffer, numDrawObjects * 6);
	//	Vertex* DrawBuffer = context->vertexDrawBuffer;//(Vertex*)malloc(sizeof(Vertex) * numDrawObjects * 6);// 6 vertex per object, shader takes only position and uv as varying, matrix will be uniform and color todo later

	//	*vertCount = 0;
	//	int offset = 0;

	//	int lastTextureId = -1;

	//	for (int win = 0; win < context->activeWindows; win++)//per win
	//	{
	//		for (auto wid = 0; wid < context->UpdateQue[win]->myBufferDataSize; wid++)//per widgets
	//		{
	//			//check here last texture and make sure that it is same as last, if it isnt make new batch
	//			if(lastTextureId == -1 || lastTextureId != context->UpdateQue[win]->myBufferDataStart[wid].txtID)
	//			{
	//				Renderbatch *temp = emblace_back<Renderbatch>(&batchBuffer);
	//				temp->numVertices = 6;
	//				temp->offset = offset;	//here batch starts
	//				temp->texture = context->UpdateQue[win]->myBufferDataStart[wid].txtID;
	//				lastTextureId = context->UpdateQue[win]->myBufferDataStart[wid].txtID;
	//			}
	//			else
	//			{
	//				get_back<Renderbatch>(batchBuffer)->numVertices += 6;
	//			}

	//			drawdata* data = &context->UpdateQue[win]->myBufferDataStart[wid]; // current image to render

	//			glm::vec4 destinationRectangle;


	//			destinationRectangle.x = data->position.x - data->dimensions.x;
	//			destinationRectangle.y = data->position.y - data->dimensions.y;
	//			destinationRectangle.z = data->dimensions.x * 2;
	//			destinationRectangle.w = data->dimensions.y * 2;


	//			Vertex topleft;
	//			topleft.position = glm::vec2(destinationRectangle.x, destinationRectangle.y + destinationRectangle.w);
	//			topleft.uv = glm::vec2(data->uv.x, data->uv.y + data->uv.w);
	//			topleft.color = data->color;

	//			Vertex topright;
	//			topright.position = glm::vec2(destinationRectangle.x + destinationRectangle.z, destinationRectangle.y + destinationRectangle.w);
	//			topright.uv = glm::vec2(data->uv.x + data->uv.z, data->uv.y + data->uv.w);
	//			topright.color = data->color;


	//			Vertex bottomleft;
	//			bottomleft.position = glm::vec2(destinationRectangle.x, destinationRectangle.y);
	//			bottomleft.uv = glm::vec2(data->uv.x, data->uv.y);
	//			bottomleft.color = data->color;


	//			Vertex bottomright;
	//			bottomright.position = glm::vec2(destinationRectangle.x + destinationRectangle.z, destinationRectangle.y);
	//			bottomright.uv = glm::vec2(data->uv.x + data->uv.z, data->uv.y);
	//			bottomright.color = data->color;



	//			DrawBuffer[(*vertCount)++] = topleft;	//todo index buffering later
	//			DrawBuffer[(*vertCount)++] = bottomleft;
	//			DrawBuffer[(*vertCount)++] = bottomright;
	//			DrawBuffer[(*vertCount)++] = bottomright;
	//			DrawBuffer[(*vertCount)++] = topright;
	//			DrawBuffer[(*vertCount)++] = topleft;

	//			offset += 6;
	//		}
	//		//store last texture id
	//		//set buffer size 0 per window and data position to nullptr
	//		//tldr reset window
	//		context->UpdateQue[win]->myBufferDataStart = NULL;
	//		context->UpdateQue[win]->myBufferDataSize = 0;
	//	}
	//	// give something to people to render HOX
	//	*numBatches = get_array_size<Renderbatch>(batchBuffer);
	//}



	void window(PakkiContext* context, uint32_t ID, WindowEnums WindowFlag/* = Window_Default*/)
	{
		window(context, ID, 200, 200, 100, 100, WindowFlag);
	}
	void window(PakkiContext* context, uint32_t ID, glm::vec2 pos, WindowEnums WindowFlag /*= Window_Default*/)
	{
		window(context, ID, pos.x, pos.y, 100, 100, WindowFlag);
	}
	inline bool mouse_collision(const glm::vec2* Mid, const glm::vec2* dims, const glm::vec2* point)
	{
		int xRange = abs(Mid->x - point->x);
		int yRange = abs(Mid->y - point->y);
		return xRange < dims->x && yRange < dims->y;
	}
}


//todo drawbuffer optimointi + oikein chekkaus
//sort widgets with sorted textures
//todo unsigned int pois kaikkialta!
// mouse position matriisiin n�hden
//todo oikea uv
#endif