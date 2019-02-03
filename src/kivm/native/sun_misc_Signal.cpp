//
// Created by kiva on 2018/5/27.
//

#include <kivm/kivm.h>
#include <kivm/native/classNames.h>
#include <sparsepp/spp.h>
#include <kivm/bytecode/execution.h>
#include <csignal>

using namespace kivm;

#if defined(KIVM_PLATFORM_WINDOWS)
#define CDECL __cdecl
#else
#define CDECL
#endif

static void CDECL defaultSignalHandler(int signo) {
    if (signo == SIGINT) {
        D("VM: DefaultSignalHandler: received SIGINT, exiting...");
        exit(0);
    }
}

JAVA_NATIVE jint Java_sun_misc_Signal_findSignal(JNIEnv *env, jclass unused, jstring javaSignalName) {
#if defined(KIVM_PLATFORM_APPLE)
    static HashMap<String, int> SIGNAL_TABLE{
        /* derived from /usr/include/bits/signum.h on RH7.2 */
        {L"HUP",    SIGHUP},         /* Hangup (POSIX).  */
        {L"INT",    SIGINT},         /* Interrupt (ANSI).  */
        {L"QUIT",   SIGQUIT},        /* Quit (POSIX).  */
        {L"ILL",    SIGILL},         /* Illegal instruction (ANSI).  */
        {L"TRAP",   SIGTRAP},        /* Trace trap (POSIX).  */
        {L"ABRT",   SIGABRT},        /* Abort (ANSI).  */
        {L"EMT",    SIGEMT},         /* EMT trap  */
        {L"FPE",    SIGFPE},         /* Floating-point exception (ANSI).  */
        {L"KILL",   SIGKILL},        /* Kill, unblockable (POSIX).  */
        {L"BUS",    SIGBUS},         /* BUS error (4.2 BSD).  */
        {L"SEGV",   SIGSEGV},        /* Segmentation violation (ANSI).  */
        {L"SYS",    SIGSYS},         /* Bad system call. Only on some Bsden! */
        {L"PIPE",   SIGPIPE},        /* Broken pipe (POSIX).  */
        {L"ALRM",   SIGALRM},        /* Alarm clock (POSIX).  */
        {L"TERM",   SIGTERM},        /* Termination (ANSI).  */
        {L"URG",    SIGURG},         /* Urgent condition on socket (4.2 BSD).  */
        {L"STOP",   SIGSTOP},        /* Stop, unblockable (POSIX).  */
        {L"TSTP",   SIGTSTP},        /* Keyboard stop (POSIX).  */
        {L"CONT",   SIGCONT},        /* Continue (POSIX).  */
        {L"CHLD",   SIGCHLD},        /* Child status has changed (POSIX).  */
        {L"TTIN",   SIGTTIN},        /* Background read from tty (POSIX).  */
        {L"TTOU",   SIGTTOU},        /* Background write to tty (POSIX).  */
        {L"IO",     SIGIO},          /* I/O now possible (4.2 BSD).  */
        {L"XCPU",   SIGXCPU},        /* CPU limit exceeded (4.2 BSD).  */
        {L"XFSZ",   SIGXFSZ},        /* File size limit exceeded (4.2 BSD).  */
        {L"VTALRM", SIGVTALRM},      /* Virtual alarm clock (4.2 BSD).  */
        {L"PROF",   SIGPROF},        /* Profiling alarm clock (4.2 BSD).  */
        {L"WINCH",  SIGWINCH},       /* Window size change (4.3 BSD, Sun).  */
        {L"INFO",   SIGINFO},        /* Information request.  */
        {L"USR1",   SIGUSR1},        /* User-defined signal 1 (POSIX).  */
        {L"USR2",   SIGUSR2},        /* User-defined signal 2 (POSIX).  */
    };
#elif defined(KIVM_PLATFORM_UNIX)
    static HashMap<String, int> SIGNAL_TABLE{
        {L"HUP", SIGHUP},         /* Hangup (POSIX).  */
        {L"INT", SIGINT},         /* Interrupt (ANSI).  */
        {L"QUIT", SIGQUIT},        /* Quit (POSIX).  */
        {L"ILL", SIGILL},         /* Illegal instruction (ANSI).  */
        {L"TRAP", SIGTRAP},        /* Trace trap (POSIX).  */
        {L"ABRT", SIGABRT},        /* Abort (ANSI).  */
        {L"IOT", SIGIOT},         /* IOT trap (4.2 BSD).  */
        {L"BUS", SIGBUS},         /* BUS error (4.2 BSD).  */
        {L"FPE", SIGFPE},         /* Floating-point exception (ANSI).  */
        {L"KILL", SIGKILL},        /* Kill, unblockable (POSIX).  */
        {L"USR1", SIGUSR1},        /* User-defined signal 1 (POSIX).  */
        {L"SEGV", SIGSEGV},        /* Segmentation violation (ANSI).  */
        {L"USR2", SIGUSR2},        /* User-defined signal 2 (POSIX).  */
        {L"PIPE", SIGPIPE},        /* Broken pipe (POSIX).  */
        {L"ALRM", SIGALRM},        /* Alarm clock (POSIX).  */
        {L"TERM", SIGTERM},        /* Termination (ANSI).  */
#ifdef SIGSTKFLT
        {L"STKFLT",     SIGSTKFLT},      /* Stack fault.  */
#endif
#ifdef SIGCLD
        {L"CLD", SIGCLD},         /* Same as SIGCHLD (System V).  */
#endif
        {L"CHLD", SIGCHLD},        /* Child status has changed (POSIX).  */
        {L"CONT", SIGCONT},        /* Continue (POSIX).  */
        {L"STOP", SIGSTOP},        /* Stop, unblockable (POSIX).  */
        {L"TSTP", SIGTSTP},        /* Keyboard stop (POSIX).  */
        {L"TTIN", SIGTTIN},        /* Background read from tty (POSIX).  */
        {L"TTOU", SIGTTOU},        /* Background write to tty (POSIX).  */
        {L"URG", SIGURG},         /* Urgent condition on socket (4.2 BSD).  */
        {L"XCPU", SIGXCPU},        /* CPU limit exceeded (4.2 BSD).  */
        {L"XFSZ", SIGXFSZ},        /* File size limit exceeded (4.2 BSD).  */
        {L"VTALRM", SIGVTALRM},      /* Virtual alarm clock (4.2 BSD).  */
        {L"PROF", SIGPROF},        /* Profiling alarm clock (4.2 BSD).  */
        {L"WINCH", SIGWINCH},       /* Window size change (4.3 BSD, Sun).  */
        {L"POLL", SIGPOLL},        /* Pollable event occurred (System V).  */
        {L"IO", SIGIO},          /* I/O now possible (4.2 BSD).  */
        {L"PWR", SIGPWR},         /* Power failure restart (System V).  */
#ifdef SIGSYS
        {L"SYS", SIGSYS},         /* Bad system call. Only on some Linuxen! */
#endif
    };
#elif defined(KIVM_PLATFORM_WINDOWS)
    static HashMap<String, int> SIGNAL_TABLE{
        /* derived from signal.h */
        {L"INT",   SIGINT},         /* Interrupt (ANSI).  */
        {L"ILL",   SIGILL},         /* Illegal instruction (ANSI).  */
        {L"ABRT",  SIGABRT},        /* Abort (ANSI).  */
        {L"ABRT2", SIGABRT2},        /* Abort (ANSI).  */
        {L"FPE",   SIGFPE},         /* Floating-point exception (ANSI).  */
        {L"SEGV",  SIGSEGV},        /* Segmentation violation (ANSI).  */
        {L"TERM",  SIGTERM},        /* Termination (ANSI).  */
        {L"BREAK", SIGBREAK},        /* Termination (ANSI).  */
    };
#endif

    auto signalNameOop = Resolver::instance(javaSignalName);
    if (signalNameOop == nullptr) {
        auto thread = Threads::currentThread();
        assert(thread != nullptr);
        thread->throwException(Global::_NullPointerException, false);
        return 0;
    }

    const auto &signalName = java::lang::String::toNativeString(signalNameOop);
    const auto &iter = SIGNAL_TABLE.find(signalName);
    if (iter == SIGNAL_TABLE.end()) {
        auto thread = Threads::currentThread();
        thread->throwException((InstanceKlass *) BootstrapClassLoader::get()
                ->loadClass(L"java/lang/IllegalArgumentException"),
            L"unknown signal");
        return -1;
    }

    return iter->second;
}

#if defined(KIVM_PLATFORM_WINDOWS)

JAVA_NATIVE jlong Java_sun_misc_Signal_handle0(JNIEnv *env, jclass unused, jint signo, jlong handlerAddr) {
    void (*realHandler)(int) = (handlerAddr == 2)
                               ? (void (*)(int)) defaultSignalHandler
                               : (void (*)(int)) handlerAddr;
    signal(signo, realHandler);
    return 2;
}

#else

JAVA_NATIVE jlong Java_sun_misc_Signal_handle0(JNIEnv *env, jclass unused, jint signo, jlong handlerAddr) {
    void *realHandler = (handlerAddr == 2)
                        ? (void *) defaultSignalHandler
                        : (void *) handlerAddr;

    // copied from jvm_bsd.cpp
    switch (signo) {
        case SIGUSR1:
        case SIGFPE:
        case SIGKILL:
        case SIGSEGV:
        case SIGQUIT:
            // these signals are unmodifiable
            return -1;

        case SIGINT:
            // already used defaultSignalHandler(int)
            return 2;

        case SIGHUP:
        case SIGTERM: {
            // detect whether the sig is `ignored`. if not, set the handler. if yes, return 1.
            struct sigaction oldAction{};
            if (sigaction(signo, nullptr, &oldAction) == -1) {
                PANIC("sigaction() returned -1");
            }

#if defined(__APPLE__)
            void *oldHandler = (void *) oldAction.__sigaction_u.__sa_handler;        // this is the `current handler`.
#elif defined(__linux__)
            void *oldHandler = (void *)oldAction.sa_handler;
#endif
            if (oldHandler == (void *) SIG_IGN) {
                return 1;
            }
            break;
        }

        default:
            SHOULD_NOT_REACH_HERE();
    }

    // register new handler
    struct sigaction act{}, oact{};
    sigfillset(&act.sa_mask);
    act.sa_flags = SA_RESTART;

#if defined(KIVM_PLATFORM_APPLE)
    act.__sigaction_u.__sa_handler = (void (*)(int)) realHandler;
    void *old_handler = (void *) oact.__sigaction_u.__sa_handler;

#elif defined(KIVM_PLATFORM_UNIX)
    act.sa_handler = (void (*)(int)) realHandler;
    void *old_handler = (void *) oact.sa_handler;

#elif defined(KIVM_PLATFORM_WINDOWS)
    act.__sigaction_u.__sa_handler = (void (*)(int)) realHandler;
    void *old_handler = (void *) oact.__sigaction_u.__sa_handler;
#endif

    if (sigaction(signo, &act, &oact) != -1) {
        if (old_handler == (void *) defaultSignalHandler) {
            return 2;
        } else {
            return (jlong) old_handler;
        }
    } else {
        // error
        return -1;
    }
}

#endif
