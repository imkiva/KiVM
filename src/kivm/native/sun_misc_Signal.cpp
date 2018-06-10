//
// Created by kiva on 2018/5/27.
//

#include <kivm/kivm.h>
#include <kivm/native/classNames.h>
#include <sparsepp/spp.h>
#include <kivm/bytecode/execution.h>
#include <csignal>

using namespace kivm;

static void defaultSignalHandler(int signo) {
    if (signo == SIGINT) {
        D("VM: DefaultSignalHandler: received SIGINT, exiting...");
        exit(0);
    }
}

JAVA_NATIVE jint Java_sun_misc_Signal_findSignal(JNIEnv *env, jclass unused, jstring javaSignalName) {
#if defined(__APPLE__)
    static spp::sparse_hash_map<String, int> SIGNAL_TABLE{
        /* derived from /usr/include/bits/signum.h on RH7.2 */
        {"HUP",    SIGHUP},         /* Hangup (POSIX).  */
        {"INT",    SIGINT},         /* Interrupt (ANSI).  */
        {"QUIT",   SIGQUIT},        /* Quit (POSIX).  */
        {"ILL",    SIGILL},         /* Illegal instruction (ANSI).  */
        {"TRAP",   SIGTRAP},        /* Trace trap (POSIX).  */
        {"ABRT",   SIGABRT},        /* Abort (ANSI).  */
        {"EMT",    SIGEMT},         /* EMT trap  */
        {"FPE",    SIGFPE},         /* Floating-point exception (ANSI).  */
        {"KILL",   SIGKILL},        /* Kill, unblockable (POSIX).  */
        {"BUS",    SIGBUS},         /* BUS error (4.2 BSD).  */
        {"SEGV",   SIGSEGV},        /* Segmentation violation (ANSI).  */
        {"SYS",    SIGSYS},         /* Bad system call. Only on some Bsden! */
        {"PIPE",   SIGPIPE},        /* Broken pipe (POSIX).  */
        {"ALRM",   SIGALRM},        /* Alarm clock (POSIX).  */
        {"TERM",   SIGTERM},        /* Termination (ANSI).  */
        {"URG",    SIGURG},         /* Urgent condition on socket (4.2 BSD).  */
        {"STOP",   SIGSTOP},        /* Stop, unblockable (POSIX).  */
        {"TSTP",   SIGTSTP},        /* Keyboard stop (POSIX).  */
        {"CONT",   SIGCONT},        /* Continue (POSIX).  */
        {"CHLD",   SIGCHLD},        /* Child status has changed (POSIX).  */
        {"TTIN",   SIGTTIN},        /* Background read from tty (POSIX).  */
        {"TTOU",   SIGTTOU},        /* Background write to tty (POSIX).  */
        {"IO",     SIGIO},          /* I/O now possible (4.2 BSD).  */
        {"XCPU",   SIGXCPU},        /* CPU limit exceeded (4.2 BSD).  */
        {"XFSZ",   SIGXFSZ},        /* File size limit exceeded (4.2 BSD).  */
        {"VTALRM", SIGVTALRM},      /* Virtual alarm clock (4.2 BSD).  */
        {"PROF",   SIGPROF},        /* Profiling alarm clock (4.2 BSD).  */
        {"WINCH",  SIGWINCH},       /* Window size change (4.3 BSD, Sun).  */
        {"INFO",   SIGINFO},        /* Information request.  */
        {"USR1",   SIGUSR1},        /* User-defined signal 1 (POSIX).  */
        {"USR2",   SIGUSR2},        /* User-defined signal 2 (POSIX).  */
    };
#elif defined(__linux__)
    static spp::sparse_hash_map<String, int> SIGNAL_TABLE{
        {"HUP", SIGHUP},         /* Hangup (POSIX).  */
        {"INT", SIGINT},         /* Interrupt (ANSI).  */
        {"QUIT", SIGQUIT},        /* Quit (POSIX).  */
        {"ILL", SIGILL},         /* Illegal instruction (ANSI).  */
        {"TRAP", SIGTRAP},        /* Trace trap (POSIX).  */
        {"ABRT", SIGABRT},        /* Abort (ANSI).  */
        {"IOT", SIGIOT},         /* IOT trap (4.2 BSD).  */
        {"BUS", SIGBUS},         /* BUS error (4.2 BSD).  */
        {"FPE", SIGFPE},         /* Floating-point exception (ANSI).  */
        {"KILL", SIGKILL},        /* Kill, unblockable (POSIX).  */
        {"USR1", SIGUSR1},        /* User-defined signal 1 (POSIX).  */
        {"SEGV", SIGSEGV},        /* Segmentation violation (ANSI).  */
        {"USR2", SIGUSR2},        /* User-defined signal 2 (POSIX).  */
        {"PIPE", SIGPIPE},        /* Broken pipe (POSIX).  */
        {"ALRM", SIGALRM},        /* Alarm clock (POSIX).  */
        {"TERM", SIGTERM},        /* Termination (ANSI).  */
#ifdef SIGSTKFLT
        {"STKFLT",     SIGSTKFLT},      /* Stack fault.  */
#endif
#ifdef SIGCLD
        {"CLD", SIGCLD},         /* Same as SIGCHLD (System V).  */
#endif
        {"CHLD", SIGCHLD},        /* Child status has changed (POSIX).  */
        {"CONT", SIGCONT},        /* Continue (POSIX).  */
        {"STOP", SIGSTOP},        /* Stop, unblockable (POSIX).  */
        {"TSTP", SIGTSTP},        /* Keyboard stop (POSIX).  */
        {"TTIN", SIGTTIN},        /* Background read from tty (POSIX).  */
        {"TTOU", SIGTTOU},        /* Background write to tty (POSIX).  */
        {"URG", SIGURG},         /* Urgent condition on socket (4.2 BSD).  */
        {"XCPU", SIGXCPU},        /* CPU limit exceeded (4.2 BSD).  */
        {"XFSZ", SIGXFSZ},        /* File size limit exceeded (4.2 BSD).  */
        {"VTALRM", SIGVTALRM},      /* Virtual alarm clock (4.2 BSD).  */
        {"PROF", SIGPROF},        /* Profiling alarm clock (4.2 BSD).  */
        {"WINCH", SIGWINCH},       /* Window size change (4.3 BSD, Sun).  */
        {"POLL", SIGPOLL},        /* Pollable event occurred (System V).  */
        {"IO", SIGIO},          /* I/O now possible (4.2 BSD).  */
        {"PWR", SIGPWR},         /* Power failure restart (System V).  */
#ifdef SIGSYS
        {"SYS", SIGSYS},         /* Bad system call. Only on some Linuxen! */
#endif
    };
#endif

    auto signalNameOop = Resolver::instance(javaSignalName);
    if (signalNameOop == nullptr) {
        auto thread = Threads::currentThread();
        assert(thread != nullptr);
        thread->throwException(Global::java_lang_NullPointerException);
        return 0;
    }

    const auto &signalName = java::lang::String::toNativeString(signalNameOop);
    const auto &iter = SIGNAL_TABLE.find(signalName);
    if (iter == SIGNAL_TABLE.end()) {
        PANIC("sun/misc/Signal.findSignal(): unknown signal");
    }

    return iter->second;
}

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

#if defined(__APPLE__)
    act.__sigaction_u.__sa_handler = (void (*)(int)) realHandler;
    void *old_handler = (void *) oact.__sigaction_u.__sa_handler;

#elif defined(__linux__)
    act.sa_handler = (void (*)(int))realHandler;
    void *old_handler = (void *)oact.sa_handler;
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
