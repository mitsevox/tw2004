typedef struct {
    float x, y, z;
} Vec3;

void Vec3Copy(const Vec3* src, Vec3* dst) {
    dst->x = src->x;
    dst->y = src->y;
    dst->z = src->z;
}
