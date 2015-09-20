
extern char *topology_map;
void topology_init(int size_x, int size_y);
double topology_scan_type(int type, int x, int y, int size);
int get_topology(int x, int y);

#define TOPOLOGY_NONE 0
#define TOPOLOGY_GRASS 1

