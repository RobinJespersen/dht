%#include <chord_types.h>
%#include <dhash_types.h>

typedef int32_t dhash_hopcount;

enum store_status {
  DHASH_STORE = 0,
  DHASH_CACHE = 1,
  DHASH_FRAGMENT = 2,
  DHASH_REPLICA = 3
};

enum dhash_dbtype {
  DHASH_BLOCK = 0,
  DHASH_FRAG = 1
};

struct dhash_valueattr {
  unsigned size;
};


struct s_dhash_insertarg {
  chordID key;
  dhash_ctype ctype;
  dhash_dbtype dbtype;
  chord_node_wire from;
  dhash_value data;
  int offset;
  int32_t nonce; /* XXX remove */
  store_status type;
  dhash_valueattr attr;
  bool last; /* used by the merkle code only */
};

struct s_dhash_fetch_arg {
  chordID key;
  dhash_ctype ctype;
  dhash_dbtype dbtype;
  chord_node_wire from;
  int32_t start;
  int32_t len;
  int32_t cookie;
  int32_t nonce; /* XXX remove */
};

struct s_dhash_keystatus_arg {
  chordID key;
};


struct dhash_pred {
  chord_node_wire p;
};

struct dhash_getkeys_ok {
  chordID keys<>;
};

union dhash_getkeys_res switch (dhash_stat status) {
 case DHASH_OK:
   dhash_getkeys_ok resok;
default:
   void; 
};

struct s_dhash_getkeys_arg {
  chordID pred_id;
  chordID start;
};


struct dhash_storeresok {
  bool already_present;
  bool done;
  chordID source;
};

union dhash_storeres switch (dhash_stat status) {
 case DHASH_RETRY:
   dhash_pred pred;
 case DHASH_OK:
   dhash_storeresok resok;
 default:
   void;
};

struct dhash_fetchiter_complete_res {
  dhash_value res;
  int32_t offset;
  dhash_valueattr attr;
  chordID source;
  int32_t cookie;
};

union dhash_fetchiter_res switch (dhash_stat status) {
 case DHASH_COMPLETE:
   dhash_fetchiter_complete_res compl_res;
 case DHASH_CONTINUE:
   void;
 case DHASH_NOENT:
   void;
 default:
   void;
};


struct s_dhash_block_arg {
  int32_t nonce; /* XXX remove */
  dhash_value res;
  int32_t offset;
  dhash_valueattr attr;
  chordID source;
  int32_t cookie;
  /* if a the sender of this RPC doesn't have the block 
   * then he sends back a list of successors.
   */
  chord_node_wire nodelist<>; 
};


struct s_dhash_storecb_arg {
  int32_t nonce; /* XXX remove */
  dhash_stat status;
};


struct dhash_offer_arg {
  bigint keys<64>;
};

struct dhash_offer_resok {
   bool accepted<64>;
};

union dhash_offer_res switch (dhash_stat status) {
 case DHASH_OK:
   dhash_offer_resok resok;
default:
   void; 
};


program DHASH_PROGRAM {
  version DHASH_VERSION {

    dhash_storeres
    DHASHPROC_STORE (s_dhash_insertarg) = 1;

    dhash_getkeys_res
    DHASHPROC_GETKEYS (s_dhash_getkeys_arg) = 2;

    dhash_fetchiter_res
    DHASHPROC_FETCHITER (s_dhash_fetch_arg) = 3;

    dhash_offer_res
    DHASHPROC_OFFER (dhash_offer_arg) = 4;

  } = 1;
} = 344449;
