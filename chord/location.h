#ifndef _LOCATION_H_
#define _LOCATION_H_
/*
 *
 * Copyright (C) 2000 Frans Kaashoek (kaashoek@lcs.mit.edu)
 * Copyright (C) 2001 Frans Kaashoek (kaashoek@lcs.mit.edu) and 
 *                    Frank Dabek (fdabek@lcs.mit.edu).
 *
 *  Permission is hereby granted, free of charge, to any person obtaining
 *  a copy of this software and associated documentation files (the
 *  "Software"), to deal in the Software without restriction, including
 *  without limitation the rights to use, copy, modify, merge, publish,
 *  distribute, sublicense, and/or sell copies of the Software, and to
 *  permit persons to whom the Software is furnished to do so, subject to
 *  the following conditions:
 *
 *  The above copyright notice and this permission notice shall be
 *  included in all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 *  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 *  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 *  LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 *  OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 *  WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

//#define FAKE_DELAY

class chord;

// the identifier for the ihash class
struct hashID {
  hashID () {}
  hash_t operator() (const chordID &ID) const {
    return ID.getui ();
  }
};

struct location;

struct RPC_delay_args {
  chordID ID;
  rpc_program prog;
  int procno;
  ptr<void> in;
  void *out;
  aclnt_cb cb;
  u_int64_t s;

  tailq_entry<RPC_delay_args> q_link;

  RPC_delay_args (chordID _ID, rpc_program _prog, int _procno,
		 ptr<void> _in, void *_out, aclnt_cb _cb, u_int64_t _s) :
    ID (_ID), prog (_prog), procno (_procno), 
		   in (_in), out (_out), cb (_cb), s (_s) {};
    
};

struct frpc_state {
  chord_RPC_res *res;
  void *out;
  int procno;
  aclnt_cb cb;
  chordID ID;
  u_int64_t s;
  int outgoing_len;
  timecb_t *tmo;
  long seqno;

  tailq_entry<frpc_state> q_link;

  frpc_state (chord_RPC_res *r, void *o, int pr, 
	      aclnt_cb c,
	      chordID id, u_int64_t S, int out_size, long s) 
    : res (r), out (o), procno (pr), cb (c), ID (id), s (S), outgoing_len (out_size), tmo (NULL), seqno (s) {};
};

struct delayed_reply {
  long xid;
  void *out;
  long outlen;
  
  tailq_entry <delayed_reply> q_link;

  delayed_reply (long x, void *o, long l) : 
    xid (x), outlen (l)
  { 
    out = (void *)New char[outlen];
    memcpy (out, o, l);
  };

  ~delayed_reply () {
    free (out);
  }
};

struct location {
  int refcnt;	// locs w. refcnt == 0 are in the cache; refcnt > 0 are fingers
  chordID n;
  net_address addr;
  sockaddr_in saddr;
  ihash_entry<location> fhlink;
  tailq_entry<location> cachelink;
  u_int64_t rpcdelay;
  u_int64_t nrpc;
  u_int64_t maxdelay;
  float a_lat;

  location (chordID &_n, net_address &_r);
  ~location ();
};

struct node {
  chordID n;
  bool alive;
};

class locationtable : public virtual refcount {
  static const int delayed_timer = 1;  // seconds

  ptr<chord> chordnode;
  ihash<chordID,location,&location::n,&location::fhlink,hashID> locs;
  tailq<location, &location::cachelink> cachedlocs;  // the cached location

  timecb_t *delayed_tmo;
  int size_cachedlocs;
  int max_cachedlocs;

  u_int64_t rpcdelay;
  u_int64_t nrpc;
  float a_lat;
  u_int64_t nrpcfailed;
  u_int64_t nsent;
  u_int64_t npending;

  u_long nnodessum;
  u_long nnodes;
  unsigned nvnodes;

  float cwind;
  int left;

  tailq<RPC_delay_args, &RPC_delay_args::q_link> Q;
  tailq<frpc_state, &frpc_state::q_link> sent_Q;

  timecb_t *idle_timer;

  ptr<axprt_dgram> dgram_xprt;
  //ptr<aclnt> dgram_clnt;

  qhash<long, svccb *> octbl;
  unsigned long last_xid;
  
  locationtable ();

  void connect_cb (location *l, callback<void, ptr<axprt_stream> >::ref cb, 
		   int fd);
  void doRPCcb (chordID ID, aclnt_cb cb, u_int64_t s, 
		ptr<aclnt> c, clnt_stat err);

  void dorpc_connect_cb(location *l, ptr<axprt_stream> x);
  void chord_connect(chordID ID, callback<void, ptr<axprt_stream> >::ref cb);
  void decrefcnt (location *l);
  void touch_cachedlocs (location *l);
  void add_cachedlocs (location *l);
  void delete_cachedlocs (void);
  void remove_cachedlocs (location *l);

  void update_latency (chordID ID, u_int64_t lat);
  void ratecb ();
  void update_cwind (int acked);
  void rexmit_handler (long seqno);
  void enqueue_rpc (RPC_delay_args *args);
  void rpc_done (frpc_state *C);
  void reset_idle_timer ();
  void idle ();
  void setup_rexmit_timer (chordID ID, frpc_state *C);
  void timeout_cb (frpc_state *C);
  void issue_RPC (long seqno, ptr<aclnt> c, chord_RPC_arg *farg, 
		  chord_RPC_res *res, aclnt_cb cb);
  void issue_RPC_delay (long seqno, ptr<aclnt> c, chord_RPC_arg *farg, 
			chord_RPC_res *res, aclnt_cb cb);

#ifdef FAKE_DELAY
  void doRPC_delayed (RPC_delay_args *args);
#endif / *FAKE_DELAY */

 public:
  locationtable (ptr<chord> _chordnode, int _max_connections);
  bool betterpred1 (chordID current, chordID target, chordID newpred);
  char betterpred2 (chordID myID, chordID current, chordID target, 
		    chordID newpred);
  bool betterpred3 (chordID myID, chordID current, chordID target, 
		    chordID newpred);
  bool betterpred_greedy (chordID myID, chordID current, chordID target, 
			  chordID newpred); 
  char betterpred_distest (chordID myID, chordID current, 
			   chordID target, 
			   chordID newpred);

  void incvnodes () { nvnodes++; };
  void replace_estimate (u_long o, u_long n);
  void insert (chordID &_n, sfs_hostname _s, int _p);
  location *getlocation (chordID &x);
  void deleteloc (chordID &n);
  void cacheloc (chordID &x, net_address &r);
  void updateloc (chordID &x, net_address &r);
  void increfcnt (chordID &n);
  bool lookup_anyloc (chordID &n, chordID *r);
  chordID closestsuccloc (chordID x);
  chordID closestpredloc (chordID x);
  net_address & getaddress (chordID &x);
  chordID query_location_table (chordID x);
  void changenode (node *n, chordID &n, net_address &r);
  void checkrefcnt (int i);
  void doRPC (chordID &from, chordID &n, rpc_program progno, 
	      int procno, ptr<void> in, 
	      void *out, aclnt_cb cb, u_int64_t s);

  void doRPC_udp (chordID &from, chordID &n, rpc_program progno, 
		  int procno, ptr<void> in, 
		  void *out, aclnt_cb cb, u_int64_t s);

  void doRPC_tcp (chordID &from, chordID &n, rpc_program progno, 
		  int procno, ptr<void> in, 
		  void *out, aclnt_cb cb, u_int64_t s);


  void doRPC_tcp_connect_cb (RPC_delay_args *args, int fd);

  void doRPC_issue (chordID &from, chordID &ID, 
		    rpc_program prog, int procno, 
		    ptr<void> in, void *out, aclnt_cb cb,
		    u_int64_t sp,
		    ptr<aclnt> c);



  void stats ();

  long new_xid (svccb *sbp);
  void reply (long xid, void *out, long outlen);
  bool doForeignRPC (ptr<aclnt> c, rpc_program prog,
		     unsigned long procno,
		     ptr<void> in,
		     void *out,
		     chordID ID,
		     aclnt_cb cb);

  void doForeignRPC_cb (frpc_state *C, rpc_program prog,
			ptr<aclnt> c,
			clnt_stat err);
  
};

#endif _LOCATION_H_

