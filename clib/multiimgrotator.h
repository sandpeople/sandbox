

int multiimgrotator_AddImage(size_t w, size_t h);

int multiimgrotator_ScaleImage(int id, double scale_x, double scale_y);

void multiimgrotator_TranslateImage(int id,
        double center_x, double center_y, double center_z,
        double offset_x, double offset_y, double offset_z,
        double rotation_euler_x,
        double rotation_euler_y,
        double rotation_euler_z)

void multimgrotator_RemoveImage(int id);


