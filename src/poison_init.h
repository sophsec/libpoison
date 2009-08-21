#ifndef _LIBPOISON_INIT_H_
#define _LIBPOISON_INIT_H_

#define SESSION_INITIALIZED 77

#include "poison_session.h"

int poison_init(poison_session_t *session, char *interface);
int poison_check_init(poison_session_t *session);
void poison_shutdown(poison_session_t *session);

#endif
