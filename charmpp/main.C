#include "main.decl.h"

#include "main.h"
#include "worker.decl.h"


/* readonly */ CProxy_Main mainProxy;


// Entry point of Charm++ application
Main::Main(CkArgMsg* msg) {

  // Initialize the local member variables
  doneCount = 0;    // Set doneCount to 0
  checkinCount = 0; // Set checkinCount to 0
  curStep = 1;
  numElements = 5;  // Default numElements to 5


  // There should be 0 or 1 command line arguements.
  //   If there is one, it is the number of "Worker"
  //   chares that should be created.
  if (msg->argc > 1)
    numElements = atoi(msg->argv[1]);

  CkPrintf("numElements = %d.\n", numElements);

  // We are done with msg so delete it.
  delete msg;

  // Display some info about this execution
  //   for the user.
  CkPrintf("Running \"Hello World\" with %d elements "
           "using %d processors.\n",
           numElements, CkNumPes());

  // Set the mainProxy readonly to point to a
  //   proxy for the Main chare object (this
  //   chare object).
  mainProxy = thisProxy;

  // Create the array of Hello chare objects.
  workerArray = CProxy_Worker::ckNew(numElements);

  startStep();
}


// Constructor needed for chare object migration (ignore for now)
// NOTE: This constructor does not need to appear in the ".ci" file
Main::Main(CkMigrateMessage* msg) { }


void Main::startStep() {
  CkPrintf("Main::startStep() called\n");
  // Invoke the "sayHi()" entry method on all of the
  //   elements in the helloArray array of chare objects.

  checkinCount = 0; // repeatitive
  workerArray.sayHi(curStep);
}

void Main::stepCheckin() {
  checkinCount++;
  // CkPrintf("Main::stepCheckin() called, count = %d\n", checkinCount);
  if (checkinCount >= numElements) {
    curStep++;
    checkinCount = 0;
    workerArray.sayHi(curStep);
  }
}

// When called, the "done()" entry method will cause the program
//   to exit.
void Main::done() {

  // Increment the doneCount.  If all of the Hello chare
  //   objects have indicated that they are done, then exit.
  //   Otherwise, continue waiting for the Hello chare objects.
  doneCount++;
  if (doneCount >= numElements)
    CkExit();
}


#include "main.def.h"
