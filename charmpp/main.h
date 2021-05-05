#ifndef __MAIN_H__
#define __MAIN_H__


class Main : public CBase_Main {

 private:
  /// Member Variables (Object State) ///
  int numElements;

  int doneCount;
  int checkinCount;
  int curStep;

  CProxy_Worker workerArray;
  
 public:

  /// Constructors ///
  Main(CkArgMsg* msg);
  Main(CkMigrateMessage* msg);

  /// Entry Methods ///
  void startStep();
  void stepCheckin();
  void done();

};


#endif //__MAIN_H__
