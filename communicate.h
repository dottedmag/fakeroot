#ifndef FAKEROOT_COMMUNICATE_H
#define FAKEROOT_COMMUNICATE_H

#include "config.h"
#include "fakerootconfig.h"

#define LCHOWN_SUPPORT

/* I've got a chicken-and-egg problem here. I want to have
   stat64 support, only running on glibc2.1 or later. To
   find out what glibc we've got installed, I need to 
   #include <features.h>.
   But, before including that file, I have to define _LARGEFILE64_SOURCE
   etc, cause otherwise features.h will not define it's internal defines.
   As I assume that pre-2.1 libc's will just ignore those _LARGEFILE64_SOURCE
   defines, I hope I can get away with this approach:
*/
  
/*First, unconditionally define these, so that glibc 2.1 features.h defines
  the needed 64 bits defines*/
#ifndef _LARGEFILE64_SOURCE
# define _LARGEFILE64_SOURCE 
#endif
#ifndef _LARGEFILE_SOURCE
# define _LARGEFILE_SOURCE 
#endif

/* Then include features.h, to find out what glibc we run */
#ifdef HAVE_FEATURES_H
#include <features.h>
#endif

#ifdef HAVE_SYS_FEATURE_TESTS_H
#include <sys/feature_tests.h>
#endif

/* Then decide whether we do or do not use the stat64 support */
#if defined(sun) || __GLIBC__ > 2 || (__GLIBC__ == 2 && __GLIBC_MINOR__ >= 1)
#define STAT64_SUPPORT
#else
#warning Not using stat64 support
/* if glibc is 2.0 or older, undefine these again */
#undef STAT64_SUPPORT
#undef _LARGEFILE64_SOURCE 
#undef _LARGEFILE_SOURCE 
#endif

/* Sparc glibc 2.0.100 is broken, dlsym segfaults on --fxstat64.. 
#define STAT64_SUPPORT */

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/utsname.h>
#ifdef HAVE_STDINT_H
#include <stdint.h>
#endif
#ifdef HAVE_INTTYPES_H
#include <inttypes.h>
#endif

#ifndef FAKEROOT_FAKENET
#define FAKEROOTKEY_ENV "FAKEROOTKEY"
#endif /* ! FAKEROOT_FAKENET */

#define FAKEROOTUID_ENV           "FAKEROOTUID"
#define FAKEROOTGID_ENV           "FAKEROOTGID"
#define FAKEROOTEUID_ENV          "FAKEROOTEUID"
#define FAKEROOTEGID_ENV          "FAKEROOTEGID"
#define FAKEROOTDONTTRYCHOWN_ENV  "FAKEROOTDONTTRYCHOWN"

#define FAKELIBDIR                "/usr/lib/fakeroot"
#define FAKELIBNAME               "libfakeroot.so.0"
#ifdef FAKEROOT_FAKENET
#define FD_BASE_ENV               "FAKEROOT_FD_BASE"
#endif /* FAKEROOT_FAKENET */

#ifdef __GNUC__
#  define UNUSED __attribute__((unused))
#else 
#  define UNUSED 
#endif

#ifndef S_ISTXT
#  define S_ISTXT S_ISVTX
#endif

#ifndef ALLPERMS
#  define ALLPERMS (S_ISUID|S_ISGID|S_ISVTX|S_IRWXU|S_IRWXG|S_IRWXO)/* 07777 */
#endif

/* Define big enough _constant size_ types for the various types of the
   stat struct. I cannot (or rather, shouldn't) use struct stat itself
   in the communication between the fake-daemon and the client (libfake),
   as the sizes elements of struct stat may depend on the compiler or
   compile time options of the C compiler, or the C library used. Thus,
   the fake-daemon may have to communicate with two clients that have
   different views of struct stat (this is the case for libc5 and
   libc6 (glibc2) compiled programmes on Linux). This currently isn't
   enabled any more, but used to be in libtricks.
*/

enum {chown_func,
        /*2*/  chmod_func,
        /*3*/  mknod_func, 
               stat_func,
        /*5*/  unlink_func,
               debugdata_func,
               reqoptions_func,
               last_func};

#include "message.h"

#ifndef FAKEROOT_FAKENET
  const char *env_var_set(const char *env);
  extern int init_get_msg();
#else /* FAKEROOT_FAKENET */
  extern const char *env_var_set(const char *env);
#endif /* FAKEROOT_FAKENET */
  extern void send_stat(const struct stat *st, func_id_t f);
  extern void send_fakem(const struct fake_msg *buf);
  extern void send_get_stat(struct stat *buf);
#ifndef FAKEROOT_FAKENET
  extern void cpyfakemstat(struct fake_msg *b1, const struct stat     *st);
  /*extern void cpyfakemfake(struct fake_msg *b1, const struct fakestat *b2);
    extern void cpyfakefakem(struct fakestat *b1, const struct fake_msg *b2);
  */
  extern key_t get_ipc_key();
#endif /* ! FAKEROOT_FAKENET */
  extern void cpyfakefake (struct fakestat *b1, const struct fakestat *b2);
  extern void cpystatfakem(struct    stat  *st,  const struct fake_msg *buf);

#ifdef STAT64_SUPPORT  
  extern void send_stat64(const struct stat64 *st, func_id_t f);
  extern void send_get_stat64(struct stat64 *buf);
  extern void stat64from32(struct stat64 *s64, const struct stat *s32);
  extern void stat32from64(struct stat *s32, const struct stat64 *s64);
#endif

#ifdef FAKEROOT_FAKENET
  extern void close_comm_sd(void);

#endif /* FAKEROOT_FAKENET */

#ifndef FAKEROOT_FAKENET
extern int msg_snd;
extern int msg_get;
extern int sem_id;

#endif /* ! FAKEROOT_FAKENET */
#endif
