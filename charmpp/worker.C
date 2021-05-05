#include "worker.decl.h"

#include "worker.h"
#include "main.decl.h"


extern /* readonly */ CProxy_Main mainProxy;


Worker::Worker() {
  // Nothing to do when the Worker chare object is created.
  //   This is where member variables would be initialized
  //   just like in a C++ class constructor.
}


// Constructor needed for chare object migration (ignore for now)
// NOTE: This constructor does not need to appear in the ".ci" file
Worker::Worker(CkMigrateMessage *msg) { }


void Worker::sayHi(int step) {

  // Have this chare object say hello to the user.
  CkPrintf("\"Hello\" from Worker chare # %d on "
           "processor %d , step %d).\n",
           thisIndex, CkMyPe(), step);

  // Report to the Main chare object that this chare object
  //   has completed its task.
  if (step >= 3) 
    mainProxy.done();
  else
    mainProxy.stepCheckin();
}

#include "worker.def.h"
