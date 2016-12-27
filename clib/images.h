
#include <SDL2/SDL_image.h>

extern SDL_Surface *grass;
extern char *raw_gradient_data;
extern int gradient_x, gradient_y;
void images_init_simulation_image(int screen_width, int screen_height);
void images_init();
SDL_Surface *image_load_converted(const char *path, int alpha);
SDL_Surface *images_duplicate(SDL_Surface *old);

extern SDL_Surface *images_simulation_image;
extern SDL_Texture *images_simulation_3d_image;

/// Clears the GPU accelerated render target
void images_simulation_3d_clear();

/// Downloads the GPU accelerated render target contents and replace the
/// 2D render image contents fully with the result
void images_simulation_3d_to_2d_download();

/// Upload the current 2D render image into the GPU accelerated render
/// target, replacing the previous contents there
void images_simulation_2d_to_3d_upload();

/// Download GPU accelerated render target, but don't replace the
/// current 2D render image but instead blit on top with alpha
void images_simulation_3d_to_2d_blit_ontop();

