CC         = cc
LD         = $(CC)
CPPFLAGS   = -D_POSIX_C_SOURCE=200809L
#CPPFLAGS   = -D_XOPEN_SOURCE=700

# debug
YACC       = yacc -d -t -v
CFLAGS     = -c -O0 -g -std=c99 -pedantic -Wall -Wextra $(CPPFLAGS)
LDFLAGS    =

# production
#YACC       = yacc -d
#CFLAGS     = -c -Os -g0 -std=c99 -pedantic -Wall -Wextra $(CPPFLAGS)
#LDFLAGS    = -s -static
