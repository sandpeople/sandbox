
#include <SDL2/SDL_image.h>

extern SDL_Surface *images_simulation_image;
extern SDL_Surface *grass;
extern char *raw_gradient_data;
extern int gradient_x, gradient_y;
void images_init_simulation_image(int screen_width, int screen_height);
void images_init();
SDL_Surface *image_load_converted(const char *path, int alpha);
SDL_Surface *images_duplicate(SDL_Surface *old);

