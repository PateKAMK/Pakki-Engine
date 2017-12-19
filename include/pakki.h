#pragma once
#include <projectDefinitions.h>

#ifndef P_ANDROID

typedef unsigned int WINID;
#define ERROR_ID 1683
#define MAX_WINDOWS 20
#include <glm/glm/common.hpp>

#define GEN_ID __LINE__ + FILEID
namespace Pakki 
{

	 
	inline unsigned constexpr const_hash(char const *input) {
		return *input ?
			static_cast<WINID>(*input) + 33 * const_hash(input + 1) :
			ERROR_ID;
	}

	typedef struct
	{
		unsigned char	r;
		unsigned char	g;
		unsigned char	b;
		unsigned char	a;
	} Color;
	typedef struct
	{
		glm::vec2	position;
		glm::vec2	dimensions;
		glm::vec4	uv;
		uint32_t	txtID;	// for sorting
		Color		color;
	} drawdata;
	enum WindowEnums : int
	{
		Window_Default = 0,
		Window_No_Resize = (1 << 1),
		Window_No_Move = (1 << 2),
		Window_No_Header = (1 << 3),
	};

	typedef char FormattingFlag;

	enum FormattingFlags : FormattingFlag
	{
		NewLine = (1 << 1),
		SameLine = (1 << 2)
	};
#define WINFLAG int
	enum WindowState : WINFLAG
	{
		Inactive = 0,
		Hot = (1 << 1),	//mouseover
		Active = (1 << 2), //cliked
		Updated = (1 << 3),
		NoWidgetPressed = (1 << 4),
		Move = (1 << 5)
	};
	enum MouseState : char
	{
		InActive = 0, //needs redefinition from Windowstate
		CurrentlyActive = 1,
		LastUpdateActive = 2
	};
	typedef struct
	{
		glm::vec2		position;
		glm::vec2		dimensions;
		WindowEnums		type;
		WINFLAG			state;
		WINID			id;
		drawdata*		myBufferDataStart;
		unsigned int	myBufferDataSize;
		float			xOffset;
		float			yOffset;
	} Window;
typedef unsigned int MouseFlag;
	typedef struct
	{
		glm::vec2	mousePosition;
		MouseFlag	mousetate;
	} IOHandle;

	typedef struct
	{
		glm::vec2 position;
		glm::vec2 uv;
		Color	  color;
	} Vertex;

	struct Renderbatch
	{
		unsigned int	offset;	
		unsigned int	numVertices;
		unsigned int	texture;
	};

	// load characters as RGBA so rendering can be done in one shader, this is also reason why we use low pixel font(which can be changed if needed)
	typedef struct
	{
		float ax; // advance.x
		float ay; // advance.y

		float bw; // bitmap.width;
		float bh; // bitmap.rows;

		float bl; // bitmap_left;
		float bt; // bitmap_top;

		float tx; // x offset of glyph in texture coordinates
		float ty;
	} CharacterInfo;

	typedef struct
	{
		CharacterInfo	character[128];
		unsigned int	w;
		unsigned int	h;
		unsigned int	txtid;
	} Font;

	typedef struct
	{
		Window			Windows[MAX_WINDOWS];
		unsigned int	numWindows;
		Window*			UpdateQue[MAX_WINDOWS];
		unsigned int	activeWindows;
		Font			font;
		Window*			currentWindow;
		IOHandle		inputs;
		//unsigned int	numDrawElements;
		float			lastTouchPosX;
		float			lastTouchPosY;
		drawdata*		dyArrDrawBuffer;
		Vertex*			vertexDrawBuffer;
		Renderbatch*	batchDrawBuffer;
		drawdata*		textBuffer;
		void(*rendercallback)(Vertex* vertbuffer, Renderbatch* batchBuffer, int BatchSize, int vertexSize);
	} PakkiContext;



	void init_pakki(PakkiContext* context,unsigned int id,unsigned int textureFontID);
	void disposepakki(PakkiContext* context);
	void start_frame(PakkiContext* context, glm::vec2 mousepos, MouseState mousestate);
	void end_window(PakkiContext* context);
	//void end_and_get_batches_frame(PakkiContext* context, int* numBatches,int *vertCount);
	void window(PakkiContext* context, uint32_t ID, glm::vec2 pos, WindowEnums WindowFlag = Window_No_Move);
	void window(PakkiContext* context, uint32_t ID, WindowEnums WindowFlag = Window_Default);
	void window(PakkiContext* context, WINID ID, int xpos, int ypos, int dimx, int dimy, WindowEnums WindowFlag = Window_Default);
	void checkbox(PakkiContext* context, bool* userxdata, FormattingFlag flag = NewLine);
	void sliderf(PakkiContext* context, float* userxdata, const char* txt);
	int fet_id();//pois heti
	void render(PakkiContext* context);
	void text(PakkiContext* context, const char* fmt, ...);
	bool button(PakkiContext* context, const char* fmt, ...);
}
#endif