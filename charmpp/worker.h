#ifndef __WORKER_H__
#define __WORKER_H__

#include "astar.h"

class Worker : public CBase_Worker {

 public:
  Graph *G;
  /// Constructors ///
  Worker();
  Worker(CkMigrateMessage *msg);

  /// Entry Methods ///
  void sayHi(int from);

};


#endif //__WORKER_H__
