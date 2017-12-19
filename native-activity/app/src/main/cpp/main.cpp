/*
 * Copyright (C) 2010 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

//BEGIN_INCLUDE(all)
#include <initializer_list>
#include <memory>
#include <cstdlib>
#include <jni.h>
#include <errno.h>
#include <cassert>

#include <EGL/egl.h>
#include <GLES2/gl2.h>

#include <android/sensor.h>
#include <android/log.h>
#include <android_native_app_glue.h>
#include <engine.h>
#include <time.h>

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "native-activity", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "native-activity", __VA_ARGS__))
/**
 * Our saved state data.
 */








struct saved_state {
    float angle;
    int32_t x;
    int32_t y;
};

/**
 * Shared state for our app.
 */

struct and_engine {
    struct android_app* app;
    engine* mainEngine;
    ASensorManager* sensorManager;
    const ASensor* accelerometerSensor;
    ASensorEventQueue* sensorEventQueue;

    //int animating;
    EGLDisplay display;
    EGLSurface surface;
    EGLContext context;
    int32_t width;
    int32_t height;
    struct saved_state state;
};

/**
 * Initialize an EGL context for the current display.
 */
static int engine_init_display(struct and_engine* engine) {

    const EGLint attribs[] = {
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
            EGL_BLUE_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_RED_SIZE, 8,
            EGL_ALPHA_SIZE, 8,
            EGL_DEPTH_SIZE, 16,
            EGL_NONE
    };
    EGLint w, h, format;
    EGLint numConfigs;
    EGLConfig config;
    engine->display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    eglInitialize(engine->display, 0, 0);

    // Here, the application chooses the configuration it desires.
    // find the best match if possible, otherwise use the very first one
    eglChooseConfig(engine->display, attribs, nullptr, 0, &numConfigs);
    EGLConfig* supportedConfigs = new EGLConfig[numConfigs];
    assert(supportedConfigs);
    eglChooseConfig(engine->display, attribs, supportedConfigs, numConfigs, &numConfigs);
    assert(numConfigs);
    int i = 0;
    for (; i < numConfigs; i++)
    {
        EGLConfig& cfg = supportedConfigs[i];
        EGLint a;
        //	LOGI("EGL_CONFIG[%d]:", i);
        eglGetConfigAttrib(engine->display, cfg, EGL_RED_SIZE, &a);
        //	LOGI("  EGL_RED_SIZE: %d", a);
        eglGetConfigAttrib(engine->display, cfg, EGL_GREEN_SIZE, &a);
        //	LOGI("  EGL_GREEN_SIZE: %d", a);
        eglGetConfigAttrib(engine->display, cfg, EGL_BLUE_SIZE, &a);
        //	LOGI("  EGL_BLUE_SIZE: %d", a);
        eglGetConfigAttrib(engine->display, cfg, EGL_ALPHA_SIZE, &a);
        //	LOGI("  EGL_ALPHA_SIZE: %d", a);
        eglGetConfigAttrib(engine->display, cfg, EGL_DEPTH_SIZE, &a);
        //	LOGI("  EGL_DEPTH_SIZE: %d", a);
        eglGetConfigAttrib(engine->display, cfg, EGL_STENCIL_SIZE, &a);
        //	LOGI("  EGL_STENCIL_SIZE: %d", a);
        eglGetConfigAttrib(engine->display, cfg, EGL_SAMPLE_BUFFERS, &a);
        //	LOGI("  EGL_SAMPLE_BUFFERS: %d", a);
    }

    config = supportedConfigs[0];
    for (; i < numConfigs; i++)
    {
        EGLint r, g, b, d;
        EGLConfig& cfg = supportedConfigs[i];
        if (eglGetConfigAttrib(engine->display, cfg, EGL_RED_SIZE, &r) &&
            eglGetConfigAttrib(engine->display, cfg, EGL_GREEN_SIZE, &g) &&
            eglGetConfigAttrib(engine->display, cfg, EGL_BLUE_SIZE, &b) &&
            eglGetConfigAttrib(engine->display, cfg, EGL_DEPTH_SIZE, &d) &&
            r == 8 && g == 8 && b == 8 && d == 0 ) {

            config = supportedConfigs[i];
            break;
        }
    }

    // EGL_NATIVE_VISUAL_ID is an attribute of the EGLConfig that is
    // guaranteed to be accepted by ANativeWindow_setBuffersGeometry().
    // As soon as we picked a EGLConfig, we can safely reconfigure the
    // ANativeWindow buffers to match, using EGL_NATIVE_VISUAL_ID.
    eglGetConfigAttrib(engine->display, config, EGL_NATIVE_VISUAL_ID, &format);
    engine->surface = eglCreateWindowSurface(engine->display, config, engine->app->window, NULL);
    EGLint contextAttribs[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE, EGL_NONE };
    engine->context = eglCreateContext(engine->display, config, NULL, contextAttribs);

    if (eglMakeCurrent(engine->display, engine->surface, engine->surface, engine->context) == EGL_FALSE) {
        assert(0);//LOGW("Unable to eglMakeCurrent");
    }

    // Get size of the surface
    eglQuerySurface(engine->display, engine->surface, EGL_WIDTH, &w);
    eglQuerySurface(engine->display, engine->surface, EGL_HEIGHT, &h);
    //window->setSize(w,h); mikon luokan juttu?
    engine->state.angle = 0;
    engine->height = h;
    engine->width = w;
    LOGI("OpenGL information:");
    LOGI("  GL_VENDOR: %s", glGetString(GL_VENDOR));
    LOGI("  GL_RENDERER: %s", glGetString(GL_RENDERER));
    LOGI("  GL_VERSION: %s", glGetString(GL_VERSION));
    LOGI("  GL_EXTENSIONS: %s", glGetString(GL_EXTENSIONS));
    //LOGI("Surface size: %dx%d", w, h);

    //m_active = true; tää on mikon luokan juttu?
    return 0;
}

/**
 * Just the current frame in the display.
 */
/********************************************************************************************************************************************************************/


/**
 * Tear down the EGL context currently associated with the display.
 */
static void engine_term_display(struct and_engine* engine) {
    if (engine->display != EGL_NO_DISPLAY) {
        eglMakeCurrent(engine->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (engine->context != EGL_NO_CONTEXT) {
            eglDestroyContext(engine->display, engine->context);
        }
        if (engine->surface != EGL_NO_SURFACE) {
            eglDestroySurface(engine->display, engine->surface);
        }
        eglTerminate(engine->display);
    }
    //engine->animating = 0;
    engine->display = EGL_NO_DISPLAY;
    engine->context = EGL_NO_CONTEXT;
    engine->surface = EGL_NO_SURFACE;
}

/**
 * Process the next input event.
 */
static int xinput = 0;
static int yinput = 0;
static int xinput2 = 0;
static int yinput2 = 0;
static int32_t engine_handle_input(struct android_app* app, AInputEvent* event) {
   // struct and_engine* engine = (struct and_engine*)app->userData;

    int action = AMotionEvent_getAction(event)& AMOTION_EVENT_ACTION_MASK;
    //if(action == AMOTION_EVENT_ACTION_UP)
    //{
       // xinput = 0;
       // yinput = 0;
        //xinput2 = 0;
        //yinput2 = 0;
   //     return  1;
    //}
    xinput = 0;
    yinput = 0;
    xinput2 = 0;
    yinput2 = 0;
    if (action == AINPUT_EVENT_TYPE_MOTION) {

        size_t count = AMotionEvent_getPointerCount(event);
        int ids[2] = {0, 0};

        if (count == 1 ) {

            xinput = AMotionEvent_getX(event, ids[0]);
            yinput = AMotionEvent_getY(event, ids[0]);

  /*          if(AMotionEvent_getAction(event) != AMOTION_EVENT_ACTION_UP)
            {
                ids[0] = AMotionEvent_getPointerId(event, 0);
                xinput = AMotionEvent_getX(event, ids[0]);
                yinput = AMotionEvent_getY(event, ids[0]);
            }
            if(AMotionEvent_getAction(event) == AMOTION_EVENT_ACTION_UP)
            {
                ids[0] = AMotionEvent_getPointerId(event, 0);
                xinput = 0;
                yinput = 0;
            }

            return 1;*/
        } else if (count > 1 ) {

            ids[0] = AMotionEvent_getPointerId(event, 0);
            xinput = AMotionEvent_getX(event, ids[0]);
            yinput = AMotionEvent_getY(event, ids[0]);
            ids[1] = AMotionEvent_getPointerId(event, 1);
            xinput2 = AMotionEvent_getX(event, ids[1]);
            yinput2 = AMotionEvent_getY(event, ids[1]);

        }


    }
return 0;
    /* if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION) {
         //engine->animating = 1;
         engine->state.x = AMotionEvent_getX(event, 0);
         engine->state.y = AMotionEvent_getY(event, 0);
         xinput = engine->state.x;
         yinput = engine->state.y;
         //LOGI("state: x=%f y=%f", engine->state.x ,engine->state.y);
        LOGI("accelerometer: x=%d y=%d",
              engine->state.x ,engine->state.y);
         // event.acceleration.z);
         return 1;
     }
     else
     {
         xinput = 0;
         yinput = 0;
     }*/

}

/**
 * Process the next main command.
 */



static void engine_handle_cmd(struct android_app* app, int32_t cmd) {
    struct and_engine* engine = (struct and_engine*)app->userData;
    switch (cmd) {
        case APP_CMD_SAVE_STATE:
            // The system has asked us to save our current state.  Do so.
            engine->app->savedState = malloc(sizeof(struct saved_state));
            *((struct saved_state*)engine->app->savedState) = engine->state;
            engine->app->savedStateSize = sizeof(struct saved_state);
            break;
        case APP_CMD_INIT_WINDOW:
            // The window is being shown, get it ready.
            if (engine->app->window != NULL) {
                engine_init_display(engine);

                engine->mainEngine->widht = engine->width;
                engine->mainEngine->height = engine->height;
                engine_init(engine->mainEngine, engine->mainEngine->camera,engine->mainEngine->shader);


                /*engine->mainEngine->camera->update = true;
                engine->mainEngine->camera->scale = 1.f;
                init_camera(engine->mainEngine->camera, engine->width,engine-> height);

                GLuint vert = compile_shader(GL_VERTEX_SHADER, VERT_SRC);
                        fatalerror(vert);
                GLuint frag = compile_shader(GL_FRAGMENT_SHADER, FRAG_SRC);
                        fatalerror(frag);
                engine->mainEngine->shader->program = glCreateProgram();
                glAttachShader( engine->mainEngine->shader->program, vert);
                glAttachShader( engine->mainEngine->shader->program, frag);

                        fatalerror(link_shader( engine->mainEngine->shader, vert, frag));
                add_attribute( engine->mainEngine->shader, "position");
                //init_verts(vVertices);
                //engine->shader = shader;
                //engine->camera = camera;*/
                LOGI("Display initialized");
                //engine_draw_frame(engine); HOXHOXHOX
            }
            break;
        case APP_CMD_TERM_WINDOW:
            // The window is being hidden or closed, clean it up.
            engine_term_display(engine);
            break;
        case APP_CMD_GAINED_FOCUS:
            // When our app gains focus, we start monitoring the accelerometer.
            if (engine->accelerometerSensor != NULL) {
                ASensorEventQueue_enableSensor(engine->sensorEventQueue,
                                               engine->accelerometerSensor);
                // We'd like to get 60 events per second (in us).
                ASensorEventQueue_setEventRate(engine->sensorEventQueue,
                                               engine->accelerometerSensor,
                                               (1000L/60)*1000);
            }
            break;
        case APP_CMD_LOST_FOCUS:
            // When our app loses focus, we stop monitoring the accelerometer.
            // This is to avoid consuming battery while not being used.
            if (engine->accelerometerSensor != NULL) {
                ASensorEventQueue_disableSensor(engine->sensorEventQueue,
                                                engine->accelerometerSensor);
            }
            // Also stop animating.
            //engine->animating = 0;
            //engine_draw_frame(engine); HOXHOXHOX
            break;
    }
}

/*
 * AcquireASensorManagerInstance(void)
 *    Workaround ASensorManager_getInstance() deprecation false alarm
 *    for Android-N and before, when compiling with NDK-r15
 */
#include <dlfcn.h>
ASensorManager* AcquireASensorManagerInstance(android_app* app) {

    if(!app)
        return nullptr;

    typedef ASensorManager *(*PF_GETINSTANCEFORPACKAGE)(const char *name);
    void* androidHandle = dlopen("libandroid.so", RTLD_NOW);
    PF_GETINSTANCEFORPACKAGE getInstanceForPackageFunc = (PF_GETINSTANCEFORPACKAGE)
            dlsym(androidHandle, "ASensorManager_getInstanceForPackage");
    if (getInstanceForPackageFunc) {
        JNIEnv* env = nullptr;
        app->activity->vm->AttachCurrentThread(&env, NULL);

        jclass android_content_Context = env->GetObjectClass(app->activity->clazz);
        jmethodID midGetPackageName = env->GetMethodID(android_content_Context,
                                                       "getPackageName",
                                                       "()Ljava/lang/String;");
        jstring packageName= (jstring)env->CallObjectMethod(app->activity->clazz,
                                                            midGetPackageName);

        const char *nativePackageName = env->GetStringUTFChars(packageName, 0);
        ASensorManager* mgr = getInstanceForPackageFunc(nativePackageName);
        env->ReleaseStringUTFChars(packageName, nativePackageName);
        app->activity->vm->DetachCurrentThread();
        if (mgr) {
            dlclose(androidHandle);
            return mgr;
        }
    }

    typedef ASensorManager *(*PF_GETINSTANCE)();
    PF_GETINSTANCE getInstanceFunc = (PF_GETINSTANCE)
            dlsym(androidHandle, "ASensorManager_getInstance");
    // by all means at this point, ASensorManager_getInstance should be available
    assert(getInstanceFunc);
    dlclose(androidHandle);

    return getInstanceFunc();
}

/**
 * This is the main entry point of a native application that is using
 * android_native_app_glue.  It runs in its own thread, with its own
 * event loop for receiving input events and doing other things.
 */

int do_touch(engine* mainEngine,glm::vec2 touch)
{
    int ret = 0;
    if(touch.x < 1900 && touch.x > 1700 && touch.y < 700 && touch.y > 500)
    {
        ret = 1;
    }
    //else if(touch.x < 600 && touch.x > 500 && touch.y < 600 && touch.y > 500)
  //  {
   //     ret = 2;
   // }
    else if(touch.x < 500 && touch.x > 300 && touch.y < 700 && touch.y > 500)
    {
        ret = 3;
    }

    return ret;
}
void android_main(struct android_app* state) {



    struct and_engine a_engine;
    engine mainEngine;
    memset(&mainEngine,0,sizeof(engine));
    Camera camera;
    camera.scale = 1.f;
    Shader shader{0,0};
    mainEngine.camera = &camera;
    mainEngine.shader = &shader;
    //mainEngine.height = 600;
    //mainEngine.widht = 800;
    SpriteBatch batch;
    DebugRenderer drenderer;
    mainEngine.drenderer = &drenderer;
    memset(&batch,0,sizeof batch);
    mainEngine.batch = &batch;
    mainEngine.platformState = (void*)(state->activity->assetManager);
    memset(&a_engine, 0, sizeof(and_engine));
    a_engine.mainEngine = &mainEngine;




    state->userData = &a_engine;
    state->onAppCmd = engine_handle_cmd;
    state->onInputEvent = engine_handle_input;
    a_engine.app = state; //
    // Prepare to monitor accelerometer
    a_engine.sensorManager = AcquireASensorManagerInstance(state);
    a_engine.accelerometerSensor = ASensorManager_getDefaultSensor(
            a_engine.sensorManager,
            ASENSOR_TYPE_ACCELEROMETER);
    a_engine.sensorEventQueue = ASensorManager_createEventQueue(
            a_engine.sensorManager,
            state->looper, LOOPER_ID_USER,
            NULL, NULL);

    if (state->savedState != NULL) { // ??
        // We are starting with a previous saved state; restore from it.
        a_engine.state = *(struct saved_state*)state->savedState;
    }

    // loop waiting for stuff to do.
    double t = 0.0;
    constexpr double dt = 1.0 / 60.0;
    double currentTime = (double)clock();
    double accumulator = 0.0;
    //hello_world();



    while (1) {
        // Read all pending events.
        double newTime = (double)clock();
        double frameTime = newTime - currentTime;
        currentTime = newTime;
        accumulator += frameTime;


        int ident;
        int events;
        struct android_poll_source* source;

        // If not animating, we will block forever waiting for events.
        // If animating, we loop until all events are read, then continue
        // to draw the next frame of animation.
        while ((ident=ALooper_pollAll(0, NULL, &events,
                                      (void**)&source)) >= 0) {

            // Process this event.
            if (source != NULL) {
                source->process(state, source);
            }

            // If a sensor has data, process it now.
            if (ident == LOOPER_ID_USER) {
                if (a_engine.accelerometerSensor != NULL) {
                    ASensorEvent event;
                    while (ASensorEventQueue_getEvents(a_engine.sensorEventQueue,
                                                       &event, 1) > 0) {
                        //LOGI("accelerometer: x=%f y=%f z=%f",
                        //   event.acceleration.x, event.acceleration.y,
                        // event.acceleration.z);
                    }
                }
            }

            // Check if we are exiting.
            if (state->destroyRequested != 0) {
                engine_term_display(&a_engine);
                LOGI("Display terminated");
                return;
            }
        }

        /*if (engine.animating) {
            // Done with events; draw next animation frame.
            engine.state.angle += .01f;
            if (engine.state.angle > 1) {
                engine.state.angle = 0;
            }

            // Drawing is throttled to the screen update rate, so there
            // is no need to do timing here.

        }*/

        if(a_engine.display != NULL) {
            //while (accumulator >= dt)//processloop
            //{
                accumulator -= dt;
                t += dt;
                Color c{255,255,255,255};
                glm::vec4 shoot(1700,500,200,200);
                draw_debug_box(mainEngine.drenderer,&shoot,&c,0);
                //shoot.x = 500;
                //draw_debug_box(mainEngine.drenderer,&shoot,&c,0);
                 shoot.x = 300;
                draw_debug_box(mainEngine.drenderer,&shoot,&c,0);
                int ind1 = 0;
                int ind2 = 0;

                glm::vec2 touch(xinput, yinput);
                touch = point_to_world_position(&camera, &touch);
                ind1 = do_touch(&mainEngine, touch);


                touch = glm::vec2(xinput2, yinput2);
                touch = point_to_world_position(&camera, &touch);
                ind2 = do_touch(&mainEngine, touch);

                mainEngine.key.arrowU = true;
                mainEngine.key.arrowR = false;
                mainEngine.key.arrowL = false;

            if(ind1)
                 {
                     switch (ind1)
                     {
                         case 1:
                             mainEngine.key.arrowR = true;
                             break;
                         case 2:
                             mainEngine.key.arrowR = true;
                             break;
                         case 3:
                             mainEngine.key.arrowL = true;
                             break;
                         default:
                             break;
                     }
                 }
                if(ind2)
                {
                    switch (ind2)
                    {
                        case 1:
                            mainEngine.key.arrowR = true;
                            break;
                        case 2:
                            mainEngine.key.arrowR = true;
                            break;
                        case 3:
                            mainEngine.key.arrowL = true;
                            break;
                        default:
                            break;
                    }
                }

                //LOGI("touch x %d and y %d\n",(int)touch.x,(int)touch.y);


                engine_events(&mainEngine, dt,1.f);
            //}


            engine_draw(&mainEngine);
            eglSwapBuffers(a_engine.display, a_engine.surface);
        }

    }
    engine_clearup(&mainEngine);
}
//END_INCLUDE(all)
