
/* In foo.h */

#ifdef __cplusplus
extern "C" {
#endif

/*! doc...*/
void fate_foo_setup(void);

#ifdef FATE_HR_FOO
extern FATECALL void (*fate_foo_cleanup)(void);
#else
/*! doc... */
FATE_EXPORT FATECALL void fate_foo_cleanup(void);
#endif

#ifdef __cplusplus
}
#endif

/* In foo.c */

#ifdef FATE_HR_FOO
static void hot_reload(void) {
    /* The fate_hr module opens and manages the dl handle itself. 
     * It uses the fate_dl module. */
    const fate_hr_spec spec = {"fate", "foo"};
    fate_foo_cleanup = fate_hr_getsym(&spec, "cleanup");
}
#endif

FATECALL void fate_foo_setup(void) {
#ifdef FATE_HR_FOO
    hot_reload();
#endif
    /* Regular setup... */
}
#ifdef FATE_HR_FOO
FATECALL void (*fate_foo_cleanup)(void);
#else
FATECALL void fate_foo_cleanup(void) {
    /* code... */
}
#endif

