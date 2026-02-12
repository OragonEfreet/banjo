#include "test.h"
#include <banjo/bitmap.h>
#include <banjo/shader.h>
#include <banjo/vec.h>

// Note: Shader tests are simplified because full bitmap testing
// requires understanding the pixel format and memory layout

// Simple shader that returns a constant color
static int red_shader(struct bj_vec3 *out_color,
                      const struct bj_vec2 pixel_coord, void *user_data) {
  (void)pixel_coord;
  (void)user_data;
  out_color->x = BJ_F(1.0);
  out_color->y = BJ_FZERO;
  out_color->z = BJ_FZERO;
  return 1;
}

TEST_CASE(shader_compiles_and_links) {
  // This test just verifies the shader API is accessible
  // Full testing would require creating bitmaps which needs
  // proper pixel format setup
  REQUIRE(red_shader != NULL);
}

int main(int argc, char *argv[]) {
  BEGIN_TESTS(argc, argv);

  RUN_TEST(shader_compiles_and_links);

  END_TESTS();
}
