#pragma once
#include <projectDefinitions.h>

#ifndef P_ANDROID
#include <pakki.h>

#include <engine.h>

void init_pakki(Pakki::PakkiContext* context,engine* engine,int* numdraw);
void start_frame(Pakki::PakkiContext* context,glm::vec2 mpossss, bool mouseActive);
//void do_window();
//void end_pakki();
void dispose_pakki(Pakki::PakkiContext* context);
//void render_pakki();

#endif