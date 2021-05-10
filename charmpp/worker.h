#ifndef __WORKER_H__
#define __WORKER_H__

#include "astar.h"

using OpenListMember = std::tuple<float, float, Node*, Node*>;
class Worker : public CBase_Worker {

public:
  Graph *G;
  int numElements;
  int src, dst;
  std::priority_queue<OpenListMember, std::vector<OpenListMember>, myComparator > open_list;
  std::vector< std::vector<msg> > message_set;
  bool dst_found;
  bool in_barrier_mode;


  /// Constructors ///
  Worker(int, int, int);
  ~Worker();
  Worker(CkMigrateMessage *msg);

  /// Entry Methods ///
  void hdastar();
  void receiveNode(float, float, int, int);
  void next_iter();
  void setDstFound();
  void reportOpenListSize();

private:
  int hash(Node &n);
  struct msg create_msg(float h_, float dist_, int node_, int parent_);
};


#endif //__WORKER_H__
