
extern char *topology_map;
extern int topology_map_x;
extern int topology_map_y;
extern double *height_map;
void topology_init(int size_x, int size_y);
double topology_scan_type(int type, int x, int y, int size);
int get_topology(int x, int y);
void topology_calculate_drift(int x, int y, double *vx, double *vy);

#define TOPOLOGY_NONE 0
#define TOPOLOGY_GRASS 1

