#ifndef MNLXT_INTERNAL_H_
#define MNLXT_INTERNAL_H_

#define MNLXT_FLAG(bit) (0x1 << bit)
#define MNLXT_SET_PROP_FLAG(p, bit) p->prop_flags |= MNLXT_FLAG(bit)
#define MNLXT_UNSET_PROP_FLAG(p, bit) p->prop_flags &= ~MNLXT_FLAG(bit)
#define MNLXT_GET_PROP_FLAG(p, bit) (p->prop_flags & MNLXT_FLAG(bit))

typedef void (*mnlxt_data_free_cb_t)(void *);
typedef int (*mnlxt_data_put_cb_t)(struct nlmsghdr *, void *, uint16_t);

typedef struct mnlxt_data_cb_s {
	const char *name;
	mnl_cb_t parse;
	mnlxt_data_put_cb_t put;
	mnlxt_data_free_cb_t free;
	uint16_t flags;
} mnlxt_data_cb_t;

#endif /* MNLXT_INTERNAL_H_ */
