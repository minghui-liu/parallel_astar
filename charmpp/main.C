#include "main.decl.h"

#include "main.h"
#include "worker.decl.h"


/* readonly */ CProxy_Main mainProxy;


// Entry point of Charm++ application
Main::Main(CkArgMsg* msg) {

  // Initialize the local member variables
  doneCount = 0;    // Set doneCount to 0
  numElements = 5;  // Default numElements to 5
  dst_found = false;

  // There should be 0 or 1 command line arguements.
  //   If there is one, it is the number of "Worker"
  //   chares that should be created.
  if (msg->argc > 1)
    numElements = atoi(msg->argv[1]);

  //numElements = CkNumPes() / 2;
  //if(numElements==0)numElements+=1;
  FILE *fp = fopen((char*)"../test_graphs-4/src_dst.out","r");
  int src, dst;
  while(fscanf(fp, "%d %d", &src, &dst) != EOF){};
  fclose(fp);

  CkPrintf("numElements = %d.\n", numElements);

  // We are done with msg so delete it.
  delete msg;

  // Display some info about this execution
  //   for the user.
  CkPrintf("Running \"HDA*\" with %d elements "
           "using %d processors.\n",
           numElements, CkNumPes());

  // Set the mainProxy readonly to point to a
  //   proxy for the Main chare object (this
  //   chare object).
  mainProxy = thisProxy;

  // Create the array of Hello chare objects.
  workerArray = CProxy_Worker::ckNew(src, dst, numElements, numElements);

  CkCallback *cb = new CkCallback(CkIndex_Main::reportIn(NULL),  mainProxy);
  workerArray.ckSetReductionClient(cb);

  start = CmiTimer();
  workerArray.hdastar();
}


// Constructor needed for chare object migration (ignore for now)
// NOTE: This constructor does not need to appear in the ".ci" file
Main::Main(CkMigrateMessage* msg) { }


void Main::done() {
  doneCount++;
  if (doneCount == numElements) {
    workerArray.reportOpenListSize();
    doneCount=0;
  }
}


void Main::reportIn(CkReductionMsg *msg) {
  int total = *(int *)msg->getData();
  CkPrintf("Total openlist size = %d\n", total);
  if (total == 0) {
    CkPrintf("Total Time spent is = %lf \n",1000*(CmiTimer()-start));
    CkExit();
  }
  workerArray.remove_barrier_and_resume();
}

void Main::dstFound(int distance) {
  workerArray.setDstFound(distance);
}


#include "main.def.h"
