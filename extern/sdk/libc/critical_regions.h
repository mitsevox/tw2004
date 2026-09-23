#ifndef _DOLPHIN_CRITICAL_REGIONS_H
#define _DOLPHIN_CRITICAL_REGIONS_H

#ifdef __cplusplus
extern "C" {
#endif // ifdef __cplusplus

// This MSL's lock numbers: abort_exit.c takes 0, alloc.c 1, and every stdio function
// (fwrite, ftell, fseek, printf) the same lock 2.
enum critical_regions {
  atexit_funcs_access,
  malloc_pool_access,
  files_access
};

void __begin_critical_region(int region);
void __end_critical_region(int region);

#ifdef __cplusplus
};
#endif // ifdef __cplusplus

#endif
