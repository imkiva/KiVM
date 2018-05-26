//
// Created by kiva on 2018/5/27.
//

#include <kivm/kivm.h>
#include <kivm/native/classNames.h>
#include <unordered_map>
#include <kivm/bytecode/execution.h>
#include <csignal>

using namespace kivm;

JAVA_NATIVE jint Java_sun_misc_Signal_findSignal(JNIEnv *env, jclass unused, jstring javaSignalName) {
#ifdef __APPLE__
    static std::unordered_map<String, int> SIGNAL_TABLE{
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
#elif defined __linux__
    static std::unordered_map<String, int> SIGNAL_TABLE{
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
        {L"CLD", SIGCLD},         /* Same as SIGCHLD (System V).  */
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
#endif

    auto signalNameOop = Resolver::instance(javaSignalName);
    if (signalNameOop == nullptr) {
        PANIC("java.lang.NullPointerException");
    }

    const auto &signalName = java::lang::String::toNativeString(signalNameOop);
    const auto &iter = SIGNAL_TABLE.find(signalName);
    if (iter == SIGNAL_TABLE.end()) {
        PANIC("sun/misc/Signal.findSignal(): unknown signal");
    }

    return iter->second;
}
