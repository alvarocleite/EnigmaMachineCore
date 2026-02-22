
#ifndef ENIGMACORE_EXPORT_H
#define ENIGMACORE_EXPORT_H

#ifdef ENIGMACORE_STATIC_DEFINE
#  define ENIGMACORE_EXPORT
#  define ENIGMACORE_NO_EXPORT
#else
#  ifndef ENIGMACORE_EXPORT
#    ifdef EnigmaCore_EXPORTS
        /* We are building this library */
#      define ENIGMACORE_EXPORT 
#    else
        /* We are using this library */
#      define ENIGMACORE_EXPORT 
#    endif
#  endif

#  ifndef ENIGMACORE_NO_EXPORT
#    define ENIGMACORE_NO_EXPORT 
#  endif
#endif

#ifndef ENIGMACORE_DEPRECATED
#  define ENIGMACORE_DEPRECATED __attribute__ ((__deprecated__))
#endif

#ifndef ENIGMACORE_DEPRECATED_EXPORT
#  define ENIGMACORE_DEPRECATED_EXPORT ENIGMACORE_EXPORT ENIGMACORE_DEPRECATED
#endif

#ifndef ENIGMACORE_DEPRECATED_NO_EXPORT
#  define ENIGMACORE_DEPRECATED_NO_EXPORT ENIGMACORE_NO_EXPORT ENIGMACORE_DEPRECATED
#endif

/* NOLINTNEXTLINE(readability-avoid-unconditional-preprocessor-if) */
#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef ENIGMACORE_NO_DEPRECATED
#    define ENIGMACORE_NO_DEPRECATED
#  endif
#endif

#endif /* ENIGMACORE_EXPORT_H */
