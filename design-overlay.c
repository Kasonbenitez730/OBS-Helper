#include <obs-module.h>
#include <graphics/graphics.h>
#include <util/bmem.h>
#include <math.h>

OBS_DECLARE_MODULE()
OBS_MODULE_USE_DEFAULT_LOCALE("design-overlay", "en-US")

#define PLUGIN_VERSION "1.0.0"
#define PLUGIN_NAME "Design Overlay"

// Professional color constants
#define COLOR_GRID_BLUE       0xFF0096FF  // Material Design Blue
#define COLOR_BOOTSTRAP_PINK  0xFFFF0096  // Bootstrap Pink
#define COLOR_SAFE_ORANGE     0xFFFF6B35  // Safe Zone Orange
#define COLOR_SAFE_GREEN      0xFF4CAF50  // Desktop Safe Green
#define COLOR_CROSSHAIR_YELLOW 0xFFFFFF00 // Bright Yellow
#define COLOR_BRAND_BLUE      0xFF00D4FF  // Brand Blue

struct design_overlay_data {
    obs_source_t *source;
    
    // Enable/disable flags
    bool enabled;
    bool show_material_grid;
    bool show_bootstrap_grid;
    bool show_safe_zones;
    bool show_crosshair;
    bool show_rule_of_thirds;
    bool show_center_guides;
    bool show_branding;
    
    // Appearance settings
    float grid_opacity;
    float safe_zone_opacity;
    float crosshair_opacity;
    uint32_t grid_color;
    uint32_t safe_zone_color;
    uint32_t crosshair_color;
    
    // Configuration
    uint32_t canvas_width;
    uint32_t canvas_height;
    int material_grid_size;
    int bootstrap_columns;
    float bootstrap_gutter;
    int safe_zone_type;
    float custom_safe_zone_percent;
    
    // Runtime state
    bool needs_redraw;
    uint64_t last_render_time;
};

// Forward declarations
static const char *design_overlay_get_name(void *unused);
static void *design_overlay_create(obs_data_t *settings, obs_source_t *source);
static void design_overlay_destroy(void *data);
static void design_overlay_update(void *data, obs_data_t *settings);
static void design_overlay_get_defaults(obs_data_t *settings);
static obs_properties_t *design_overlay_get_properties(void *data);
static uint32_t design_overlay_get_width(void *data);
static uint32_t design_overlay_get_height(void *data);
static void design_overlay_video_render(void *data, gs_effect_t *effect);
static void design_overlay_video_tick(void *data, float seconds);

// ============================================================================
// Utility functions
// ============================================================================

static void set_shader_color(gs_eparam_t *color_param, uint32_t color, float opacity)
{
    if (!color_param) return;
    
    float a = ((color >> 24) & 0xFF) / 255.0f;
    float r = ((color >> 16) & 0xFF) / 255.0f;
    float g = ((color >> 8) & 0xFF) / 255.0f;
    float b = (color & 0xFF) / 255.0f;
    
    a *= opacity;
    a = fmaxf(0.0f, fminf(1.0f, a));
    
    struct vec4 color_vec = {r, g, b, a};
    gs_effect_set_vec4(color_param, &color_vec);
}

static void draw_simple_line(float x1, float y1, float x2, float y2,
                            uint32_t color, float opacity,
                            gs_effect_t *effect, gs_eparam_t *color_param)
{
    if (!effect || !color_param) return;
    
    set_shader_color(color_param, color, opacity);
    
    gs_render_start(true);
    gs_vertex2f(x1, y1);
    gs_vertex2f(x2, y2);
    gs_render_stop(GS_LINES);
}

// ============================================================================
// Rendering functions
// ============================================================================

static void render_material_grid(struct design_overlay_data *ctx, gs_effect_t *effect, gs_eparam_t *color_param)
{
    const int size = ctx->material_grid_size;
    if (size <= 0) return;
    
    const uint32_t color = ctx->grid_color;
    const float opacity = ctx->grid_opacity;
    
    // Vertical lines
    for (int x = 0; x <= (int)ctx->canvas_width; x += size) {
        draw_simple_line((float)x, 0.0f, (float)x, (float)ctx->canvas_height,
                        color, opacity, effect, color_param);
    }
    
    // Horizontal lines
    for (int y = 0; y <= (int)ctx->canvas_height; y += size) {
        draw_simple_line(0.0f, (float)y, (float)ctx->canvas_width, (float)y,
                        color, opacity, effect, color_param);
    }
}

static void render_bootstrap_grid(struct design_overlay_data *ctx, gs_effect_t *effect, gs_eparam_t *color_param)
{
    const int cols = ctx->bootstrap_columns;
    const float gutter = ctx->bootstrap_gutter;
    
    if (cols <= 0) return;
    
    // Bootstrap container calculation (exact 90% width, centered)
    const float container_w = floorf((float)ctx->canvas_width * 0.9f);
    const float col_w = floorf((container_w - ((cols - 1) * gutter)) / cols);
    const float start_x = floorf(((float)ctx->canvas_width - container_w) / 2.0f);
    
    const uint32_t color = COLOR_BOOTSTRAP_PINK;
    const float opacity = ctx->grid_opacity;
    
    // Container boundaries (outer lines)
    draw_simple_line(start_x, 0.0f, start_x, (float)ctx->canvas_height,
                    color, opacity * 0.5f, effect, color_param);
    draw_simple_line(start_x + container_w, 0.0f, start_x + container_w, (float)ctx->canvas_height,
                    color, opacity * 0.5f, effect, color_param);
    
    // Column dividers
    for (int i = 1; i < cols; i++) {
        float x = start_x + (i * (col_w + gutter));
        draw_simple_line(x, 0.0f, x, (float)ctx->canvas_height,
                        color, opacity, effect, color_param);
    }
}

static void render_safe_zones(struct design_overlay_data *ctx, gs_effect_t *effect, gs_eparam_t *color_param)
{
    float percent;
    uint32_t color;
    
    switch (ctx->safe_zone_type) {
        case 0: // Mobile
            percent = 0.8f;
            color = COLOR_SAFE_ORANGE;
            break;
        case 1: // Desktop
            percent = 0.9f;
            color = COLOR_SAFE_GREEN;
            break;
        case 2: // Broadcast SMPTE
            percent = 0.93f;
            color = COLOR_SAFE_ORANGE;
            break;
        case 3: // Custom
            percent = ctx->custom_safe_zone_percent;
            color = ctx->safe_zone_color;
            break;
        default:
            percent = 0.9f;
            color = COLOR_SAFE_GREEN;
    }
    
    percent = fmaxf(0.1f, fminf(0.99f, percent));
    
    const float safe_w = floorf((float)ctx->canvas_width * percent);
    const float safe_h = floorf((float)ctx->canvas_height * percent);
    const float margin_x = floorf(((float)ctx->canvas_width - safe_w) / 2.0f);
    const float margin_y = floorf(((float)ctx->canvas_height - safe_h) / 2.0f);
    const float opacity = ctx->safe_zone_opacity;
    
    // Main safe zone rectangle
    // Top
    draw_simple_line(margin_x, margin_y, margin_x + safe_w, margin_y,
                    color, opacity, effect, color_param);
    // Right
    draw_simple_line(margin_x + safe_w, margin_y, margin_x + safe_w, margin_y + safe_h,
                    color, opacity, effect, color_param);
    // Bottom
    draw_simple_line(margin_x + safe_w, margin_y + safe_h, margin_x, margin_y + safe_h,
                    color, opacity, effect, color_param);
    // Left
    draw_simple_line(margin_x, margin_y + safe_h, margin_x, margin_y,
                    color, opacity, effect, color_param);
    
    // Corner markers for professional look
    const float marker_size = 20.0f;
    
    // Top-left
    draw_simple_line(margin_x - marker_size, margin_y, margin_x + marker_size, margin_y,
                    color, opacity, effect, color_param);
    draw_simple_line(margin_x, margin_y - marker_size, margin_x, margin_y + marker_size,
                    color, opacity, effect, color_param);
    
    // Top-right
    draw_simple_line(margin_x + safe_w - marker_size, margin_y, margin_x + safe_w + marker_size, margin_y,
                    color, opacity, effect, color_param);
    draw_simple_line(margin_x + safe_w, margin_y - marker_size, margin_x + safe_w, margin_y + marker_size,
                    color, opacity, effect, color_param);
    
    // Bottom-right
    draw_simple_line(margin_x + safe_w - marker_size, margin_y + safe_h, margin_x + safe_w + marker_size, margin_y + safe_h,
                    color, opacity, effect, color_param);
    draw_simple_line(margin_x + safe_w, margin_y + safe_h - marker_size, margin_x + safe_w, margin_y + safe_h + marker_size,
                    color, opacity, effect, color_param);
    
    // Bottom-left
    draw_simple_line(margin_x - marker_size, margin_y + safe_h, margin_x + marker_size, margin_y + safe_h,
                    color, opacity, effect, color_param);
    draw_simple_line(margin_x, margin_y + safe_h - marker_size, margin_x, margin_y + safe_h + marker_size,
                    color, opacity, effect, color_param);
}

static void render_rule_of_thirds(struct design_overlay_data *ctx, gs_effect_t *effect, gs_eparam_t *color_param)
{
    const uint32_t color = COLOR_CROSSHAIR_YELLOW;
    const float opacity = ctx->crosshair_opacity * 0.6f;
    
    // Precise mathematical positioning
    const float third_x1 = floorf((float)ctx->canvas_width / 3.0f);
    const float third_x2 = floorf((float)ctx->canvas_width * 2.0f / 3.0f);
    const float third_y1 = floorf((float)ctx->canvas_height / 3.0f);
    const float third_y2 = floorf((float)ctx->canvas_height * 2.0f / 3.0f);
    
    // Vertical lines
    draw_simple_line(third_x1, 0.0f, third_x1, (float)ctx->canvas_height,
                    color, opacity, effect, color_param);
    draw_simple_line(third_x2, 0.0f, third_x2, (float)ctx->canvas_height,
                    color, opacity, effect, color_param);
    
    // Horizontal lines
    draw_simple_line(0.0f, third_y1, (float)ctx->canvas_width, third_y1,
                    color, opacity, effect, color_param);
    draw_simple_line(0.0f, third_y2, (float)ctx->canvas_width, third_y2,
                    color, opacity, effect, color_param);
    
    // Intersection dots for precision
    const float dot_size = 4.0f;
    
    // Four intersection points
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            float x = (i == 0) ? third_x1 : third_x2;
            float y = (j == 0) ? third_y1 : third_y2;
            
            // Small cross at intersection
            draw_simple_line(x - dot_size, y, x + dot_size, y,
                           color, opacity, effect, color_param);
            draw_simple_line(x, y - dot_size, x, y + dot_size,
                           color, opacity, effect, color_param);
        }
    }
}

static void render_crosshair(struct design_overlay_data *ctx, gs_effect_t *effect, gs_eparam_t *color_param)
{
    const float cx = floorf((float)ctx->canvas_width / 2.0f);
    const float cy = floorf((float)ctx->canvas_height / 2.0f);
    const float size = 40.0f;
    const uint32_t color = COLOR_CROSSHAIR_YELLOW;
    const float opacity = ctx->crosshair_opacity;
    
    // Main crosshair lines
    draw_simple_line(cx - size, cy, cx + size, cy,
                    color, opacity, effect, color_param);
    draw_simple_line(cx, cy - size, cx, cy + size,
                    color, opacity, effect, color_param);
    
    // Center dot
    const float dot_size = 3.0f;
    draw_simple_line(cx - dot_size, cy, cx + dot_size, cy,
                    color, opacity, effect, color_param);
    draw_simple_line(cx, cy - dot_size, cx, cy + dot_size,
                    color, opacity, effect, color_param);
    
    // Tick marks for precision
    const float tick_size = 8.0f;
    const float tick_offset = size + 5.0f;
    
    // Horizontal ticks
    draw_simple_line(cx - tick_offset, cy - tick_size, cx - tick_offset, cy + tick_size,
                    color, opacity * 0.7f, effect, color_param);
    draw_simple_line(cx + tick_offset, cy - tick_size, cx + tick_offset, cy + tick_size,
                    color, opacity * 0.7f, effect, color_param);
    
    // Vertical ticks
    draw_simple_line(cx - tick_size, cy - tick_offset, cx + tick_size, cy - tick_offset,
                    color, opacity * 0.7f, effect, color_param);
    draw_simple_line(cx - tick_size, cy + tick_offset, cx + tick_size, cy + tick_offset,
                    color, opacity * 0.7f, effect, color_param);
}

static void render_center_guides(struct design_overlay_data *ctx, gs_effect_t *effect, gs_eparam_t *color_param)
{
    const float cx = floorf((float)ctx->canvas_width / 2.0f);
    const float cy = floorf((float)ctx->canvas_height / 2.0f);
    const uint32_t color = 0xFF888888; // Gray
    const float opacity = ctx->crosshair_opacity * 0.4f;
    
    // Full-screen center lines
    draw_simple_line(cx, 0.0f, cx, (float)ctx->canvas_height,
                    color, opacity, effect, color_param);
    draw_simple_line(0.0f, cy, (float)ctx->canvas_width, cy,
                    color, opacity, effect, color_param);
}

static void render_branding(struct design_overlay_data *ctx, gs_effect_t *effect, gs_eparam_t *color_param)
{
    if (!ctx->show_branding) return;
    
    // Simple "design.rip" text representation in bottom right
    const float margin = 20.0f;
    const float x = (float)ctx->canvas_width - margin - 60.0f;
    const float y = (float)ctx->canvas_height - margin - 10.0f;
    const uint32_t color = COLOR_BRAND_BLUE;
    const float opacity = 0.7f;
    
    // Simple text-like lines to represent "design.rip"
    const float char_width = 8.0f;
    const float char_height = 12.0f;
    
    // "d" 
    draw_simple_line(x, y, x, y + char_height, color, opacity, effect, color_param);
    draw_simple_line(x, y, x + char_width, y, color, opacity, effect, color_param);
    draw_simple_line(x + char_width, y, x + char_width, y + char_height, color, opacity, effect, color_param);
    draw_simple_line(x, y + char_height, x + char_width, y + char_height, color, opacity, effect, color_param);
    
    // dot
    draw_simple_line(x + char_width * 1.5f, y + char_height * 0.8f, x + char_width * 1.5f + 2, y + char_height * 0.8f, 
                    color, opacity, effect, color_param);
}

// ============================================================================
// Source callbacks
// ============================================================================

static const char *design_overlay_get_name(void *unused)
{
    UNUSED_PARAMETER(unused);
    return PLUGIN_NAME;
}

static void *design_overlay_create(obs_data_t *settings, obs_source_t *source)
{
    struct design_overlay_data *ctx = bzalloc(sizeof(struct design_overlay_data));
    if (!ctx) {
        return NULL;
    }
    
    ctx->source = source;
    ctx->needs_redraw = true;
    ctx->last_render_time = 0;
    
    design_overlay_update(ctx, settings);
    
    blog(LOG_INFO, "[Design Overlay] Clean overlay created (version %s)", PLUGIN_VERSION);
    return ctx;
}

static void design_overlay_destroy(void *data)
{
    struct design_overlay_data *ctx = data;
    if (!ctx) return;
    
    blog(LOG_INFO, "[Design Overlay] Clean overlay destroyed");
    bfree(ctx);
}

static void design_overlay_update(void *data, obs_data_t *settings)
{
    struct design_overlay_data *ctx = data;
    if (!ctx) return;
    
    // Basic flags
    ctx->enabled = obs_data_get_bool(settings, "enabled");
    ctx->show_material_grid = obs_data_get_bool(settings, "show_material_grid");
    ctx->show_bootstrap_grid = obs_data_get_bool(settings, "show_bootstrap_grid");
    ctx->show_safe_zones = obs_data_get_bool(settings, "show_safe_zones");
    ctx->show_crosshair = obs_data_get_bool(settings, "show_crosshair");
    ctx->show_rule_of_thirds = obs_data_get_bool(settings, "show_rule_of_thirds");
    ctx->show_center_guides = obs_data_get_bool(settings, "show_center_guides");
    ctx->show_branding = obs_data_get_bool(settings, "show_branding");
    
    // Opacity settings
    ctx->grid_opacity = (float)obs_data_get_double(settings, "grid_opacity") / 100.0f;
    ctx->safe_zone_opacity = (float)obs_data_get_double(settings, "safe_zone_opacity") / 100.0f;
    ctx->crosshair_opacity = (float)obs_data_get_double(settings, "crosshair_opacity") / 100.0f;
    
    // Colors
    ctx->grid_color = (uint32_t)obs_data_get_int(settings, "grid_color");
    ctx->safe_zone_color = (uint32_t)obs_data_get_int(settings, "safe_zone_color");
    ctx->crosshair_color = (uint32_t)obs_data_get_int(settings, "crosshair_color");
    
    // Configuration
    ctx->canvas_width = (uint32_t)obs_data_get_int(settings, "canvas_width");
    ctx->canvas_height = (uint32_t)obs_data_get_int(settings, "canvas_height");
    ctx->material_grid_size = (int)obs_data_get_int(settings, "material_grid_size");
    ctx->bootstrap_columns = (int)obs_data_get_int(settings, "bootstrap_columns");
    ctx->bootstrap_gutter = (float)obs_data_get_double(settings, "bootstrap_gutter");
    ctx->safe_zone_type = (int)obs_data_get_int(settings, "safe_zone_type");
    ctx->custom_safe_zone_percent = (float)obs_data_get_double(settings, "custom_safe_zone_percent") / 100.0f;
    
    // Validation
    if (ctx->canvas_width < 100) ctx->canvas_width = 1920;
    if (ctx->canvas_height < 100) ctx->canvas_height = 1080;
    if (ctx->canvas_width > 7680) ctx->canvas_width = 7680;
    if (ctx->canvas_height > 4320) ctx->canvas_height = 4320;
    
    if (ctx->material_grid_size < 4) ctx->material_grid_size = 8;
    if (ctx->material_grid_size > 128) ctx->material_grid_size = 128;
    
    if (ctx->bootstrap_columns < 1) ctx->bootstrap_columns = 12;
    if (ctx->bootstrap_columns > 24) ctx->bootstrap_columns = 24;
    
    ctx->needs_redraw = true;
}

static void design_overlay_get_defaults(obs_data_t *settings)
{
    obs_data_set_default_bool(settings, "enabled", true);
    obs_data_set_default_bool(settings, "show_material_grid", true);
    obs_data_set_default_bool(settings, "show_bootstrap_grid", false);
    obs_data_set_default_bool(settings, "show_safe_zones", true);
    obs_data_set_default_bool(settings, "show_crosshair", true);
    obs_data_set_default_bool(settings, "show_rule_of_thirds", false);
    obs_data_set_default_bool(settings, "show_center_guides", false);
    obs_data_set_default_bool(settings, "show_branding", true);
    
    // Clean opacity defaults
    obs_data_set_default_double(settings, "grid_opacity", 30.0);
    obs_data_set_default_double(settings, "safe_zone_opacity", 70.0);
    obs_data_set_default_double(settings, "crosshair_opacity", 90.0);
    
    // Clean colors
    obs_data_set_default_int(settings, "grid_color", COLOR_GRID_BLUE);
    obs_data_set_default_int(settings, "safe_zone_color", COLOR_SAFE_ORANGE);
    obs_data_set_default_int(settings, "crosshair_color", COLOR_CROSSHAIR_YELLOW);
    
    // Configuration defaults
    obs_data_set_default_int(settings, "canvas_width", 1920);
    obs_data_set_default_int(settings, "canvas_height", 1080);
    obs_data_set_default_int(settings, "material_grid_size", 8);
    obs_data_set_default_int(settings, "bootstrap_columns", 12);
    obs_data_set_default_double(settings, "bootstrap_gutter", 30.0);
    obs_data_set_default_int(settings, "safe_zone_type", 1);
    obs_data_set_default_double(settings, "custom_safe_zone_percent", 85.0);
}

static obs_properties_t *design_overlay_get_properties(void *data)
{
    UNUSED_PARAMETER(data);
    
    obs_properties_t *props = obs_properties_create();
    
    // Main controls
    obs_properties_add_bool(props, "enabled", "Enable Design Overlay");
    
    // Grid systems
    obs_properties_add_text(props, "grid_header", "=== Grid Systems ===", OBS_TEXT_INFO);
    obs_properties_add_bool(props, "show_material_grid", "Material Design Grid (8px)");
    obs_properties_add_bool(props, "show_bootstrap_grid", "Bootstrap Grid (12 columns)");
    obs_properties_add_int_slider(props, "material_grid_size", "Material Grid Size (px)", 4, 64, 4);
    obs_properties_add_int_slider(props, "bootstrap_columns", "Bootstrap Columns", 6, 24, 1);
    obs_properties_add_float_slider(props, "bootstrap_gutter", "Bootstrap Gutter (px)", 10.0, 50.0, 5.0);
    obs_properties_add_float_slider(props, "grid_opacity", "Grid Opacity (%)", 10.0, 80.0, 5.0);
    obs_properties_add_color(props, "grid_color", "Grid Color");
    
    // Safe zones
    obs_properties_add_text(props, "safe_header", "=== Safe Zones ===", OBS_TEXT_INFO);
    obs_properties_add_bool(props, "show_safe_zones", "Show Safe Zones");
    
    obs_property_t *safe_zone_list = obs_properties_add_list(props, "safe_zone_type", "Safe Zone Type", 
                                                           OBS_COMBO_TYPE_LIST, OBS_COMBO_FORMAT_INT);
    obs_property_list_add_int(safe_zone_list, "Mobile (80%)", 0);
    obs_property_list_add_int(safe_zone_list, "Desktop (90%)", 1);
    obs_property_list_add_int(safe_zone_list, "Broadcast (93%)", 2);
    obs_property_list_add_int(safe_zone_list, "Custom", 3);
    
    obs_properties_add_float_slider(props, "custom_safe_zone_percent", "Custom Safe Zone (%)", 50.0, 95.0, 1.0);
    obs_properties_add_float_slider(props, "safe_zone_opacity", "Safe Zone Opacity (%)", 30.0, 100.0, 5.0);
    obs_properties_add_color(props, "safe_zone_color", "Safe Zone Color");
    
    // Measurement tools
    obs_properties_add_text(props, "tools_header", "=== Measurement Tools ===", OBS_TEXT_INFO);
    obs_properties_add_bool(props, "show_crosshair", "Show Crosshair");
    obs_properties_add_bool(props, "show_rule_of_thirds", "Show Rule of Thirds");
    obs_properties_add_bool(props, "show_center_guides", "Show Center Guides");
    obs_properties_add_float_slider(props, "crosshair_opacity", "Tools Opacity (%)", 30.0, 100.0, 5.0);
    obs_properties_add_color(props, "crosshair_color", "Tools Color");
    
    // Branding
    obs_properties_add_text(props, "brand_header", "=== Branding ===", OBS_TEXT_INFO);
    obs_properties_add_bool(props, "show_branding", "Show design.rip");
    
    // Canvas settings
    obs_properties_add_text(props, "canvas_header", "=== Canvas Settings ===", OBS_TEXT_INFO);
    obs_properties_add_int(props, "canvas_width", "Canvas Width (px)", 640, 7680, 1);
    obs_properties_add_int(props, "canvas_height", "Canvas Height (px)", 480, 4320, 1);
    
    return props;
}

static uint32_t design_overlay_get_width(void *data)
{
    struct design_overlay_data *ctx = data;
    return ctx ? ctx->canvas_width : 1920;
}

static uint32_t design_overlay_get_height(void *data)
{
    struct design_overlay_data *ctx = data;
    return ctx ? ctx->canvas_height : 1080;
}

static void design_overlay_video_tick(void *data, float seconds)
{
    struct design_overlay_data *ctx = data;
    if (!ctx) return;
    
    UNUSED_PARAMETER(seconds);
    ctx->last_render_time = obs_get_video_frame_time();
}

static void design_overlay_video_render(void *data, gs_effect_t *effect)
{
    struct design_overlay_data *ctx = data;
    if (!ctx || !ctx->enabled) return;
    
    UNUSED_PARAMETER(effect);
    
    // Get solid effect
    gs_effect_t *solid_effect = obs_get_base_effect(OBS_EFFECT_SOLID);
    if (!solid_effect) return;
    
    gs_eparam_t *color_param = gs_effect_get_param_by_name(solid_effect, "color");
    if (!color_param) return;
    
    gs_technique_t *tech = gs_effect_get_technique(solid_effect, "Solid");
    if (!tech) return;
    
    // Clean rendering setup
    gs_blend_state_push();
    gs_enable_blending(true);
    gs_blend_function(GS_BLEND_SRCALPHA, GS_BLEND_INVSRCALPHA);
    
    gs_technique_begin(tech);
    gs_technique_begin_pass(tech, 0);
    
    // Render in clean order
    if (ctx->show_center_guides) {
        render_center_guides(ctx, solid_effect, color_param);
    }
    
    if (ctx->show_rule_of_thirds) {
        render_rule_of_thirds(ctx, solid_effect, color_param);
    }
    
    if (ctx->show_material_grid) {
        render_material_grid(ctx, solid_effect, color_param);
    }
    
    if (ctx->show_bootstrap_grid) {
        render_bootstrap_grid(ctx, solid_effect, color_param);
    }
    
    if (ctx->show_safe_zones) {
        render_safe_zones(ctx, solid_effect, color_param);
    }
    
    if (ctx->show_crosshair) {
        render_crosshair(ctx, solid_effect, color_param);
    }
    
    if (ctx->show_branding) {
        render_branding(ctx, solid_effect, color_param);
    }
    
    gs_technique_end_pass(tech);
    gs_technique_end(tech);
    
    gs_blend_state_pop();
    
    ctx->needs_redraw = false;
}

// ============================================================================
// Source info structure
// ============================================================================

struct obs_source_info design_overlay_source_info = {
    .id = "design_overlay_source",
    .type = OBS_SOURCE_TYPE_INPUT,
    .output_flags = OBS_SOURCE_VIDEO | OBS_SOURCE_CUSTOM_DRAW,
    .get_name = design_overlay_get_name,
    .create = design_overlay_create,
    .destroy = design_overlay_destroy,
    .update = design_overlay_update,
    .get_defaults = design_overlay_get_defaults,
    .get_properties = design_overlay_get_properties,
    .get_width = design_overlay_get_width,
    .get_height = design_overlay_get_height,
    .video_render = design_overlay_video_render,
    .video_tick = design_overlay_video_tick,
    .icon_type = OBS_ICON_TYPE_DESKTOP_CAPTURE,
};

// ============================================================================
// Module entry points
// ============================================================================

bool obs_module_load(void)
{
    obs_register_source(&design_overlay_source_info);
    blog(LOG_INFO, "[Design Overlay] Clean plugin loaded (version %s)", PLUGIN_VERSION);
    return true;
}

void obs_module_unload(void)
{
    blog(LOG_INFO, "[Design Overlay] Clean plugin unloaded");
}

const char *obs_module_description(void)
{
    return "Clean Design Overlay Plugin v" PLUGIN_VERSION " - Precise Material Design grids, Bootstrap layouts, safe zones and measurement tools with clean rendering";
}

const char *obs_module_name(void)
{
    return PLUGIN_NAME;
}