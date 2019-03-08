#ifndef SRC_MNLXT_TOOLS_H_
#define SRC_MNLXT_TOOLS_H_

#define MNLXT_FLAG(bit) (0x1 << bit)
#define MNLXT_SET_PROP_FLAG(p, bit) p->prop_flags |= MNLXT_FLAG(bit)
#define MNLXT_UNSET_PROP_FLAG(p, bit) p->prop_flags &= ~MNLXT_FLAG(bit)
#define MNLXT_GET_PROP_FLAG(p, bit) (p->prop_flags & MNLXT_FLAG(bit))

#endif /* SRC_MNLXT_TOOLS_H_ */
