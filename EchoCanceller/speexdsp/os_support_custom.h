#define OVERRIDE_SPEEX_FATAL
#define OVERRIDE_SPEEX_WARNING
#define OVERRIDE_SPEEX_WARNING_INT
#define OVERRIDE_SPEEX_NOTIFY

static inline void _speex_fatal(const char *str, const char *file, int line)
{
   printf ("Fatal (internal) error in %s, line %d: %s\n", file, line, str);
   exit(1);
}

static inline void speex_warning(const char *str)
{
   printf ( "warning: %s\n", str);
}

static inline void speex_warning_int(const char *str, int val)
{
   printf ("warning: %s %d\n", str, val);
}

static inline void speex_notify(const char *str)
{
   printf ("notification: %s\n", str);

}
