#ifndef __PASTRY_H
#define __PASTRY_H

#include "protocol.h"
#include "node.h"
#include <vector>
#include <set>
using namespace std;

class Pastry : public Protocol {
public:
  const unsigned idlength;
  typedef long long NodeID;

  Pastry(Node*);
  ~Pastry();

  virtual void join(Args*);
  virtual void leave(Args*);
  virtual void crash(Args*);
  virtual void insert(Args*);
  virtual void lookup(Args*);

private:
  NodeID _id;
  const unsigned _b; // config. parameter b from paper, base of NodeID
  const unsigned _L; // config. parameter L from paper, 
  const unsigned _M; // config. parameter M from paper, size of neighborhood

  void route(NodeID*, void*);
  unsigned shared_prefix_len(NodeID, NodeID);
  unsigned get_digit(NodeID, unsigned);


  //
  // ROUTING TABLE
  //
  // routing table entry
  class RTEntry : public pair<NodeID, IPAddress>  { public:
    bool operator<= (const NodeID n) const { return first <= n; }
    bool operator< (const NodeID n) const { return first < n; }
    bool operator>= (const NodeID n) const { return first >= n; }
    bool operator> (const NodeID n) const { return first > n; }
  };

  // single row in a routing table
  typedef vector<RTEntry> RTRow;

  // whole routing table.  is array since
  vector<RTRow> _rtable;

  //
  // NEIGHBORHOOD SET
  //
  

  //
  // LEAF SET
  //
  typedef set<RTEntry> LS;
  LS _lleafset; // lower half of leaf set
  LS _hleafset; // higher half of leaf set

  // finds IP address such that (D - RTEntry) is minimal
  class RTEntrySmallestDiff { public:
    RTEntrySmallestDiff(NodeID D) : _D(D), diff(-1) {}
    public:
      void operator()(const Pastry::RTEntry &rt) {
        if((_D - rt.first) < diff) {
          diff = (_D - rt.first);
          ip = rt.second;
        }
      }
      IPAddress ip;
      NodeID diff;
    private:
      NodeID _D;
  };
};

#endif // __PASTRY_H