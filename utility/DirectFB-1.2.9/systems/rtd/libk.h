#ifdef __cplusplus
extern "C" {
#endif

  void libk_init();
  void libk_uninit();
  void libk_getscreensize(int *w, int *h);
  void libk_copyrect_16(void *src, int pitch, int x, int y, int w, int h);
  void libk_copyrect_32(void *src, int pitch, int x, int y, int w, int h);

#ifdef __cplusplus
};
#endif
