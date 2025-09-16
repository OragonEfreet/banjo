#!/usr/bin/env python3
import argparse, os, re, sys

# Old name -> New name (use None to keep unchanged)
RENAME = {
    # physics
    "bj_apply_angular_torque_2d": None,
    "bj_apply_drag_2d": None,
    "bj_apply_gravity_2d": None,
    "bj_apply_point_gravity_2d": None,
    "bj_apply_point_gravity_softened_2d": None,

    # audio control
    "bj_audio_device_is_playing": "bj_audio_playing",
    "bj_audio_device_pause": "bj_pause_audio_device",
    "bj_audio_device_play": "bj_play_audio_device",
    "bj_audio_device_reset": "bj_reset_audio_device",
    "bj_audio_device_stop": "bj_stop_audio_device",
    "bj_audio_play_note": "bj_play_audio_note",

    # lifecycle
    "bj_begin": "bj_initialize",
    "bj_begin_audio": None,
    "bj_begin_event": None,
    "bj_begin_time": None,
    "bj_begin_video": None,

    # distributions
    "bj_bernoulli_distribution": None,

    # bitmap
    "bj_bitmap_alloc": "bj_allocate_bitmap",
    "bj_bitmap_apply_shader": "bj_shader_bitmap",
    "bj_bitmap_blit": "bj_blit",
    "bj_bitmap_blit_mask": "bj_blit_mask",
    "bj_bitmap_blit_mask_stretched": "bj_blit_mask_stretched",
    "bj_bitmap_blit_stretched": "bj_blit_stretched",
    "bj_bitmap_blit_text": "bj_blit_text",
    "bj_bitmap_clear": "bj_clear_bitmap",
    "bj_bitmap_convert": "bj_convert_bitmap",
    "bj_bitmap_copy": "bj_copy_bitmap",
    "bj_bitmap_del": "bj_destroy_bitmap",
    "bj_bitmap_draw_circle": "bj_draw_circle",
    "bj_bitmap_draw_filled_circle": "bj_draw_filled_circle",
    "bj_bitmap_draw_filled_rectangle": "bj_draw_filled_rectangle",
    "bj_bitmap_draw_line": "bj_draw_line",
    "bj_bitmap_draw_rectangle": "bj_draw_rectangle",
    "bj_bitmap_draw_triangle": "bj_draw_triangle",
    "bj_bitmap_get": "bj_bitmap_pixel",  # ambiguous; leaving unchanged
    "bj_bitmap_height": "bj_bitmap_height",
    "bj_bitmap_init": "bj_init_bitmap",
    "bj_bitmap_mode": "bj_bitmap_mode",
    "bj_bitmap_new": "bj_create_bitmap",
    "bj_bitmap_new_from_file": "bj_create_bitmap_from_file",
    "bj_bitmap_new_from_pixels": "bj_create_bitmap_from_pixels",
    "bj_bitmap_pixels": None,
    "bj_bitmap_pixel_value": "bj_make_bitmap_pixel",
    "bj_bitmap_print": "bj_draw_text",
    "bj_bitmap_printf": "bj_draw_textf",
    "bj_bitmap_put_pixel": "bj_put_pixel",
    "bj_bitmap_reset": "bj_reset_bitmap",
    "bj_bitmap_rgb": "bj_make_bitmap_rgb",
    "bj_bitmap_set_clear_color": "bj_set_bitmap_clear_color",
    "bj_bitmap_set_colorkey": "bj_set_bitmap_colorkey",
    "bj_bitmap_stride": "bj_bitmap_stride",
    "bj_bitmap_vprintf": "bj_draw_vtextf",
    "bj_bitmap_width": "bj_bitmap_width",

    # misc/system
    "bj_call_main": None,
    "bj_call_main_callbacks": None,
    "bj_calloc": None,
    "bj_clear_error": None,

    # audio device open/close
    "bj_close_audio_device": None,
    "bj_open_audio_device": None,

    "bj_close_on_escape": None,
    "bj_compute_bitmap_stride": None,
    "bj_compute_pixel_mode": None,

    # events
    "bj_dispatch_event": None,
    "bj_dispatch_events": None,

    # lifecycle end
    "bj_end": "bj_shutdown",
    "bj_end_audio": None,
    "bj_end_event": None,
    "bj_end_time": None,
    "bj_end_video": None,

    # errors/logging
    "bj_error_check": "bj_check_error",
    "bj_forward_error": None,
    "bj_free": None,
    "bj_get_build_info": "bj_build_information",
    "bj_get_key_name": "bj_key_name",
    "bj_get_run_time": "bj_run_time",
    "bj_get_symbol": "bj_library_symbol",
    "bj_get_time_counter": "bj_time_counter",
    "bj_get_time_frequency": "bj_time_frequency",

    # kinematics
    "bj_kinematics_2d": "bj_compute_kinematics_2d",
    "bj_kinematics_velocity_2d": "bj_compute_kinematics_velocity_2d",

    # dynamic loading
    "bj_load_library": None,
    "bj_unload_library": None,

    # logging
    "bj_log_get_level": "bj_get_log_level",
    "bj_log_get_level_string": "bj_get_log_level_string",
    "bj_log_set_level": "bj_set_log_level",

    # libc/memory
    "bj_malloc": None,
    "bj_memcmp": None,
    "bj_memcpy": None,
    "bj_memmove": None,
    "bj_memory_set_defaults": "bj_set_memory_defaults",
    "bj_memory_unset_defaults": "bj_unset_memory_defaults",
    "bj_memset": None,
    "bj_memzero": None,
    "bj_message": "bj_log_message",

    # normals
    "bj_normal_double_distribution": None,
    "bj_normal_float_distribution": None,
    "bj_normal_long_double_distribution": None,

    # particles/rigid body
    "bj_particle_apply_force_2d": "bj_apply_particle_force_2d",
    "bj_particle_drag_coefficient_2d": "bj_compute_particle_drag_coefficient_2d",
    "bj_particle_drag_force_2d": "bj_compute_particle_drag_force_2d",
    "bj_particle_set_mass_2d": "bj_set_particle_mass_2d",

    # pcg
    "bj_pcg32_discard": "bj_discard_pcg32",
    "bj_pcg32_max": "bj_max_pcg32",
    "bj_pcg32_min": "bj_min_pcg32",
    "bj_pcg32_next": "bj_next_pcg32",
    "bj_pcg32_seed": "bj_seed_pcg32",

    # pixels
    "bj_pixel_rgb": "bj_make_pixel_rgb",
    "bj_pixel_value": "bj_get_pixel_value",

    # event queue
    "bj_poll_events": None,
    "bj_push_button_event": None,
    "bj_push_cursor_event": None,
    "bj_push_enter_event": None,
    "bj_push_event": None,
    "bj_push_key_event": None,

    # rand
    "bj_rand": None,
    "bj_realloc": None,

    # geometry
    "bj_rect_intersect": "bj_rect_intersection",

    # rigid body
    "bj_rigid_body_apply_force_2d": "bj_apply_rigidbody_force_2d",

    # callbacks
    "bj_set_button_callback": None,
    "bj_set_cursor_callback": None,
    "bj_set_enter_callback": None,
    "bj_set_error": None,
    "bj_set_key_callback": None,

    # sleep/time
    "bj_sleep": None,
    "bj_srand": None,

    # stepping
    "bj_step_angular_2d": None,
    "bj_step_particle_2d": None,
    "bj_step_rigid_body_2d": None,

    # stopwatch
    "bj_stopwatch_delay": None,
    "bj_stopwatch_elapsed": "bj_stopwatch_elapsed",
    "bj_stopwatch_reset": "bj_reset_stopwatch",
    "bj_stopwatch_step": "bj_step_stopwatch",
    "bj_stopwatch_step_delay": "bj_step_delay_stopwatch",

    # streams
    "bj_stream_alloc": "bj_allocate_stream",
    "bj_stream_del": "bj_close_stream",
    "bj_stream_len": "bj_get_stream_length",
    "bj_stream_new_read": "bj_open_stream_read",
    "bj_stream_new_read_from_file": "bj_open_stream_file",
    "bj_stream_read": "bj_read_stream",
    "bj_stream_seek": "bj_seek_stream",
    "bj_stream_tell": "bj_tell_stream",

    # time
    "bj_time": "bj_get_time",

    # uniforms
    "bj_uniform_double_distribution": None,
    "bj_uniform_float_distribution": None,
    "bj_uniform_int32_distribution": None,
    "bj_uniform_long_double_distribution": None,

    # window
    "bj_window_del": "bj_unbind_window",
    "bj_window_get_flags": "bj_get_window_flags",
    "bj_window_get_framebuffer": "bj_get_window_framebuffer",
    "bj_window_get_key": "bj_get_key",
    "bj_window_get_size": "bj_get_window_size",
    "bj_window_new": "bj_bind_window",
    "bj_window_set_should_close": "bj_set_window_should_close",
    "bj_window_should_close": "bj_should_close_window",
    "bj_window_update_framebuffer": "bj_update_window_framebuffer",
}

def build_pattern(keys):
    # longest first to help the regex engine
    parts = sorted((re.escape(k) for k in keys), key=len, reverse=True)
    return re.compile(r"\b(?:%s)\b" % "|".join(parts))

def replace_text(text, rx):
    def repl(m):
        k = m.group(0)
        v = RENAME.get(k)
        return v if v is not None else k
    return rx.subn(repl, text)

def process_file(path, rx, dry_run=False):
    if not os.path.isfile(path):
        print(f"skip (not a file): {path}", file=sys.stderr)
        return 0
    try:
        data = open(path, "r", encoding="utf-8", errors="ignore").read()
    except Exception as e:
        print(f"skip (read error): {path} [{e}]", file=sys.stderr)
        return 0
    new, n = replace_text(data, rx)
    if n and not dry_run:
        try:
            open(path, "w", encoding="utf-8", errors="ignore").write(new)
        except Exception as e:
            print(f"error (write): {path} [{e}]", file=sys.stderr)
            return 0
    if n:
        print(("{path} : {n} replacements" if not dry_run else
               "[dry-run] {path} : {n} matches").format(path=path, n=n))
    return n

def main():
    ap = argparse.ArgumentParser(description="Whole-word API renamer over explicit file list.")
    ap.add_argument("files", nargs="+", help="Files to edit. Pass any list; no directories.")
    ap.add_argument("--dry-run", action="store_true", help="Report matches only.")
    args = ap.parse_args()

    if not RENAME:
        print("RENAME map is empty.", file=sys.stderr)
        return 1

    rx = build_pattern(RENAME.keys())
    total = 0
    for f in args.files:
        total += process_file(f, rx, dry_run=args.dry_run)
    print(f"Total replacements: {total}")
    return 0

if __name__ == "__main__":
    sys.exit(main())
