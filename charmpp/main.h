#ifndef __MAIN_H__
#define __MAIN_H__


class Main : public CBase_Main {

 private:
  /// Member Variables (Object State) ///
  int numElements;
  int doneCount;
  bool dst_found;

  CProxy_Worker workerArray;
  
 public:

  /// Constructors ///
  Main(CkArgMsg* msg);
  Main(CkMigrateMessage* msg);

  /// Entry Methods ///
  void startStep();
  void dstFound();
  void done();
  void reportIn(CkReductionMsg *msg);

};


#endif //__MAIN_H__
