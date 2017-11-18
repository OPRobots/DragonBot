#ifndef ATDelay_h
#define ATDelay_h
#define LIBRARY_VERSION 1
class ATDelay
{

public:

ATDelay();
  ATDelay(int delay);
  
  void setDelay(int delay);
  bool check();
  void reset();


private:
  long _lastMillis;
  int _delay;
};
#endif
