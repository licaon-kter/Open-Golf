#include <stdbool.h>
#include <stdio.h>

#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include "3rd_party/cimgui/cimgui.h"
#include "3rd_party/cimguizmo/cimguizmo.h"
#include "3rd_party/sokol/sokol_app.h"
#include "3rd_party/sokol/sokol_audio.h"
#include "3rd_party/sokol/sokol_gfx.h"
#include "3rd_party/sokol/sokol_glue.h"
#include "3rd_party/sokol/sokol_imgui.h"
#include "3rd_party/sokol/sokol_time.h"
#include "editor_exe/editor.h"
#include "golf/config.h"
#include "golf/data.h"
#include "golf/debug_console.h"
#include "golf/game.h"
#include "golf/inputs.h"
#include "golf/log.h"
#include "golf/renderer.h"
#include "golf/ui.h"

static void init(void) {
    stm_setup();
    sg_setup(&(sg_desc){ 
            .buffer_pool_size = 2048, 
            .image_pool_size = 2048,
            .context = sapp_sgcontext(),
            });
    simgui_setup(&(simgui_desc_t) {
            .dpi_scale = sapp_dpi_scale() 
            });
    saudio_setup(&(saudio_desc){
            .sample_rate = 44100,
            .buffer_frames = 1024,
            .packet_frames = 64,
            .num_packets = 32, 
            });
}

static void cleanup(void) {
    sg_shutdown();
}

static void frame(void) {
    static bool inited = false;
    static uint64_t last_time = 0;
    static float time_since_import = 0.0f;

    float dt = (float) stm_sec(stm_laptime(&last_time));
    if (!inited) {
        golf_log_init();
        golf_data_init();
        golf_data_run_import(false);

        golf_inputs_init();
        golf_game_init();
        golf_ui_init();
        golf_renderer_init();

        editor_init();
        inited = true;
    }

    {
        sg_pass_action action = {
            .colors[0] = {
                .action = SG_ACTION_CLEAR,
                .value = { 0.0f, 0.0f, 0.0f, 1.0f },
            },
        };
        sg_begin_default_pass(&action, sapp_width(), sapp_height());
        sg_end_pass();

        simgui_new_frame(sapp_width(), sapp_height(), dt);
        ImGuizmo_BeginFrame();
    }

    {
        time_since_import += dt;
        if (time_since_import > 1.0f) {
            time_since_import = 0.0f;
            golf_data_run_import(false);
            golf_data_update(dt);
        }
    }

    {
        editor_update(dt);
        golf_renderer_draw_editor();
    }

    {
        sg_pass_action imgui_pass_action = {
            .colors[0] = {
                .action = SG_ACTION_DONTCARE,
            },
            .depth = {
                .action = SG_ACTION_CLEAR,
                .value = 1.0f,

            },
        };
        sg_begin_default_pass(&imgui_pass_action, sapp_width(), sapp_height());
        simgui_render();
        sg_end_pass();
        sg_commit();
    }

    {
        golf_inputs_update();
    }

    fflush(stdout);
}

static void event(const sapp_event *event) {
    simgui_handle_event(event);
    golf_inputs_handle_event(event);
}

sapp_desc sokol_main(int argc, char *argv[]) {
    return (sapp_desc){
        .init_cb = init,
            .frame_cb = frame,
            .cleanup_cb = cleanup,
            .event_cb = event,
            .width = 1280,
            .height = 720,
            .window_title = "Minigolf",
            .enable_clipboard = true,
            .clipboard_size = 1024,
            .fullscreen = false,
            .high_dpi = false,
            .html5_canvas_resize = false,
            .win32_console_utf8 = true,
            .win32_console_create = true,
            .swap_interval = 1,
    };
}

#define SOKOL_WIN32_FORCE_MAIN
#define SOKOL_IMPL
#include "3rd_party/sokol/sokol_app.h"
