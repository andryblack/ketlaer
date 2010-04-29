class HDMICallbacks {
 public:
  virtual void TvSystemChanged() = 0;
};

void InitHDMI(HDMICallbacks *callbacks);
void DeInitHDMI();

