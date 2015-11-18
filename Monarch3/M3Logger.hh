/*
 * M3Logger.hh
 * Based on KLogger.h, from KATRIN's Kasper
 *
 *  Created on: Jan 23, 2014
 *      Author: nsoblath
 */

#ifndef M3LOGGER_HH_
#define M3LOGGER_HH_


/**
 * @file
 * @brief Contains the logger class and macros, based on Kasper's KLogger class.
 * @date Created on: 18.11.2011
 * @author Marco Haag <marco.haag@kit.edu>
 *
 */

// UTILITY MACROS

#ifndef LOGGER_UTILITY_MACROS_
#define LOGGER_UTILITY_MACROS_

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

#define __FILE_LINE__      __FILE__ "(" TOSTRING(__LINE__) ")"
#define __FILENAME_LINE__  (strrchr(__FILE__, '/') ? strrchr(__FILE_LINE__, '/') + 1 : __FILE_LINE__)

#if defined(_MSC_VER)
#if _MSC_VER >= 1300
#define __FUNC__ __FUNCSIG__
#endif
#else
#if defined(__GNUC__)
#define __FUNC__ __PRETTY_FUNCTION__
#endif
#endif
#if !defined(__FUNC__)
#define __FUNC__ ""
#endif

#define va_num_args(...) va_num_args_impl(__VA_ARGS__, 5,4,3,2,1)
#define va_num_args_impl(_1,_2,_3,_4,_5,N,...) N

#define macro_dispatcher(func, ...) macro_dispatcher_(func, va_num_args(__VA_ARGS__))
#define macro_dispatcher_(func, nargs) macro_dispatcher__(func, nargs)
#define macro_dispatcher__(func, nargs) func ## nargs

#endif  /* LOGGER_UTILITY_MACROS_ */

#ifndef _WIN32

// COLOR DEFINITIONS
#define COLOR_NORMAL "0"
#define COLOR_BRIGHT "1"
#define COLOR_FOREGROUND_RED "31"
#define COLOR_FOREGROUND_GREEN "32"
#define COLOR_FOREGROUND_YELLOW "33"
#define COLOR_FOREGROUND_CYAN "36"
#define COLOR_FOREGROUND_WHITE "37"
#define COLOR_PREFIX "\033["
#define COLOR_SUFFIX "m"
#define COLOR_SEPARATOR ";"

// INCLUDES

#include <string>
#include <iostream>
#include <sstream>

// CLASS DEFINITIONS

namespace monarch3
{

    /**
     * The Monarch logger.
     *
     * The usage and syntax is inspired by log4j. logger itself uses the log4cxx library if it
     * was available on the system during compiling, otherwise it falls back to std::stream.
     *
     * The logger output can be configured in a file specified with the environment variable
     * @a LOGGER_CONFIGURATION (by default log4cxx.properties in the config directory).
     *
     * In most cases the following macro can be used
     * to instantiate a Logger in your code:
     * <pre>LOGGER(myLogger, "loggerName");</pre>
     *
     * This is equivalent to:
     * <pre>static mantis::logger myLogger("loggerName");</pre>
     *
     * For logging the following macros can be used. The source code location will then automatically
     * included in the output:
     *
     * <pre>
     * M3LOG(myLogger, level, "message");
     * M3TRACE(myLogger, "message");
     * M3DEBUG(myLogger, "message");
     * M3INFO(myLogger, "message");
     * M3WARN(myLogger, "message");
     * M3ERROR(myLogger, "message");
     * M3FATAL(myLogger, "message");
     *
     * M3ASSERT(myLogger, assertion, "message");
     *
     * M3LOG_ONCE(myLogger, level, "message");
     * M3TRACE_ONCE(myLogger, "message");
     * M3DEBUG_ONCE(myLogger, "message");
     * M3INFO_ONCE(myLogger, "message");
     * M3WARN_ONCE(myLogger, "message");
     * M3ERROR_ONCE(myLogger, "message");
     * M3FATAL_ONCE(myLogger, "message");
     * </pre>
     *
     */
    class M3Logger
    {
        public:
            enum ELevel {
                eTrace, eDebug, eInfo, eWarn, eError, eFatal
            };

        public:
            /**
             * A simple struct used by the Logger macros to pass information about the filename and line number.
             * Not to be used directly by the user!
             */
            struct Location {
                Location(const char* const fileName = "", const char* const functionName = "", int lineNumber = -1) :
                    fLineNumber(lineNumber), fFileName(fileName), fFunctionName(functionName)
                { }
                int fLineNumber;
                std::string fFileName;
                std::string fFunctionName;
            };

        public:
            static M3Logger& GetRootLogger() {
                static M3Logger rootLogger;
                return rootLogger;
            }

        public:
            /**
             * Standard constructor assigning a name to the logger instance.
             * @param name The logger name.
             */
            M3Logger(const char* name = 0);
            /// @overload
            M3Logger(const std::string& name);

            virtual ~M3Logger();

            /**
             * Check whether a certain log-level is enabled.
             * @param level The log level as string representation.
             * @return
             */
            bool IsLevelEnabled(ELevel level) const;

            /**
             * Set a loggers minimum logging level
             * @param level string identifying the log level
             */
            void SetLevel(ELevel level) const;

            /**
             * Log a message with the specified level.
             * Use the macro LOG(logger, level, message).
             * @param level The log level.
             * @param message The message.
             * @param loc Source code location (set automatically by the corresponding macro).
             */
            void Log(ELevel level, const std::string& message, const Location& loc = Location());

            /**
             * Log a message at TRACE level.
             * Use the macro TRACE(logger, message).
             * @param message The message.
             * @param loc Source code location (set automatically by the corresponding macro).
             */
            void LogTrace(const std::string& message, const Location& loc = Location())
            {
                Log(eTrace, message, loc);
            }
            /**
             * Log a message at DEBUG level.
             * Use the macro DEBUG(logger, message).
             * @param message The message.
             * @param loc Source code location (set automatically by the corresponding macro).
             */
            void LogDebug(const std::string& message, const Location& loc = Location())
            {
                Log(eDebug, message, loc);
            }
            /**
             * Log a message at DEBUG level.
             * Use the macro DEBUG(logger, message).
             * @param message The message.
             * @param loc Source code location (set automatically by the corresponding macro).
             */
            void LogInfo(const std::string& message, const Location& loc = Location())
            {
                Log(eInfo, message, loc);
            }
            /**
             * Log a message at INFO level.
             * Use the macro INFO(logger, message).
             * @param message The message.
             * @param loc Source code location (set automatically by the corresponding macro).
             */
            void LogWarn(const std::string& message, const Location& loc = Location())
            {
                Log(eWarn, message, loc);
            }
            /**
             * Log a message at ERROR level.
             * Use the macro ERROR(logger, message).
             * @param message The message.
             * @param loc Source code location (set automatically by the corresponding macro).
             */
            void LogError(const std::string& message, const Location& loc = Location())
            {
                Log(eError, message, loc);
            }
            /**
             * Log a message at FATAL level.
             * Use the macro FATAL(logger, message).
             * @param message The message.
             * @param loc Source code location (set automatically by the corresponding macro).
             */
            void LogFatal(const std::string& message, const Location& loc = Location())
            {
                Log(eFatal, message, loc);
            }

        private:
            struct Private;
            Private* fPrivate;
    };

}

// PRIVATE MACROS

#define __M3DEFAULT_LOGGER        monarch3::M3Logger::GetRootLogger()

#define __M3LOG_LOCATION         monarch3::M3Logger::Location(__FILE__, __FUNC__, __LINE__)

#define __M3LOG_LOG_4(I,L,M,O) \
        { \
    if (I.IsLevelEnabled(monarch3::M3Logger::e##L)) { \
        static bool _sLoggerMarker = false; \
        if (!O || !_sLoggerMarker) { \
            _sLoggerMarker = true; \
            std::ostringstream stream; stream << M; \
            I.Log(monarch3::M3Logger::e##L, stream.str(), __M3LOG_LOCATION); \
        } \
    } \
        }

#define __M3LOG_LOG_3(I,L,M)     __M3LOG_LOG_4(I,L,M,false)
#define __M3LOG_LOG_2(L,M)       __M3LOG_LOG_4(__M3DEFAULT_LOGGER,L,M,false)
#define __M3LOG_LOG_1(M)         __M3LOG_LOG_4(__M3DEFAULT_LOGGER,Debug,M,false)

#define __M3LOG_TRACE_2(I,M)     __M3LOG_LOG_4(I,Trace,M,false)
#define __M3LOG_TRACE_1(M)       __M3LOG_LOG_4(__M3DEFAULT_LOGGER,Trace,M,false)

#define __M3LOG_DEBUG_2(I,M)     __M3LOG_LOG_4(I,Debug,M,false)
#define __M3LOG_DEBUG_1(M)       __M3LOG_LOG_4(__M3DEFAULT_LOGGER,Debug,M,false)

#define __M3LOG_INFO_2(I,M)      __M3LOG_LOG_4(I,Info,M,false)
#define __M3LOG_INFO_1(M)        __M3LOG_LOG_4(__M3DEFAULT_LOGGER,Info,M,false)

#define __M3LOG_WARN_2(I,M)      __M3LOG_LOG_4(I,Warn,M,false)
#define __M3LOG_WARN_1(M)        __M3LOG_LOG_4(__M3DEFAULT_LOGGER,Warn,M,false)

#define __M3LOG_ERROR_2(I,M)     __M3LOG_LOG_4(I,Error,M,false)
#define __M3LOG_ERROR_1(M)       __M3LOG_LOG_4(__M3DEFAULT_LOGGER,Error,M,false)

#define __M3LOG_FATAL_2(I,M)     __M3LOG_LOG_4(I,Fatal,M,false)
#define __M3LOG_FATAL_1(M)       __M3LOG_LOG_4(__M3DEFAULT_LOGGER,Fatal,M,false)

#define __M3LOG_ASSERT_3(I,C,M)  if (!(C)) { __M3LOG_ERROR_2(I,M) }
#define __M3LOG_ASSERT_2(C,M)    __M3LOG_ASSERT_3(__M3DEFAULT_LOGGER,C,M)


#define __M3LOG_LOG_ONCE_3(I,L,M)     __M3LOG_LOG_4(I,L,M,true)
#define __M3LOG_LOG_ONCE_2(L,M)       __M3LOG_LOG_4(__M3DEFAULT_LOGGER,L,M,true)
#define __M3LOG_LOG_ONCE_1(M)         __M3LOG_LOG_4(__M3DEFAULT_LOGGER,Debug,M,true)

#define __M3LOG_TRACE_ONCE_2(I,M)     __M3LOG_LOG_4(I,Trace,M,true)
#define __M3LOG_TRACE_ONCE_1(M)       __M3LOG_LOG_4(__M3DEFAULT_LOGGER,Trace,M,true)

#define __M3LOG_DEBUG_ONCE_2(I,M)     __M3LOG_LOG_4(I,Debug,M,true)
#define __M3LOG_DEBUG_ONCE_1(M)       __M3LOG_LOG_4(__M3DEFAULT_LOGGER,Debug,M,true)

#define __M3LOG_INFO_ONCE_2(I,M)      __M3LOG_LOG_4(I,Info,M,true)
#define __M3LOG_INFO_ONCE_1(M)        __M3LOG_LOG_4(__M3DEFAULT_LOGGER,Info,M,true)

#define __M3LOG_WARN_ONCE_2(I,M)      __M3LOG_LOG_4(I,Warn,M,true)
#define __M3LOG_WARN_ONCE_1(M)        __M3LOG_LOG_4(__M3DEFAULT_LOGGER,Warn,M,true)

#define __M3LOG_ERROR_ONCE_2(I,M)     __M3LOG_LOG_4(I,Error,M,true)
#define __M3LOG_ERROR_ONCE_1(M)       __M3LOG_LOG_4(__M3DEFAULT_LOGGER,Error,M,true)

#define __M3LOG_FATAL_ONCE_2(I,M)     __M3LOG_LOG_4(I,Fatal,M,true)
#define __M3LOG_FATAL_ONCE_1(M)       __M3LOG_LOG_4(__M3DEFAULT_LOGGER,Fatal,M,true)


// PUBLIC MACROS

#define M3LOGGER(I,K)      static monarch3::M3Logger I(K);

#define M3LOG(...)         macro_dispatcher(__M3LOG_LOG_, __VA_ARGS__)(__VA_ARGS__)
#define M3TRACE(...)       macro_dispatcher(__M3LOG_TRACE_, __VA_ARGS__)(__VA_ARGS__)
#define M3DEBUG(...)       macro_dispatcher(__M3LOG_DEBUG_, __VA_ARGS__)(__VA_ARGS__)
#define M3INFO(...)        macro_dispatcher(__M3LOG_INFO_, __VA_ARGS__)(__VA_ARGS__)
#define M3WARN(...)        macro_dispatcher(__M3LOG_WARN_, __VA_ARGS__)(__VA_ARGS__)
#define M3ERROR(...)       macro_dispatcher(__M3LOG_ERROR_, __VA_ARGS__)(__VA_ARGS__)
#define M3FATAL(...)       macro_dispatcher(__M3LOG_FATAL_, __VA_ARGS__)(__VA_ARGS__)
#define M3ASSERT(...)      macro_dispatcher(__M3LOG_ASSERT_, __VA_ARGS__)(__VA_ARGS__)

#define M3LOG_ONCE(...)    macro_dispatcher(__M3LOG_LOG_ONCE_, __VA_ARGS__)(__VA_ARGS__)
#define M3TRACE_ONCE(...)  macro_dispatcher(__M3LOG_TRACE_ONCE_, __VA_ARGS__)(__VA_ARGS__)
#define M3DEBUG_ONCE(...)  macro_dispatcher(__M3LOG_DEBUG_ONCE_, __VA_ARGS__)(__VA_ARGS__)
#define M3INFO_ONCE(...)   macro_dispatcher(__M3LOG_INFO_ONCE_, __VA_ARGS__)(__VA_ARGS__)
#define M3WARN_ONCE(...)   macro_dispatcher(__M3LOG_WARN_ONCE_, __VA_ARGS__)(__VA_ARGS__)
#define M3ERROR_ONCE(...)  macro_dispatcher(__M3LOG_ERROR_ONCE_, __VA_ARGS__)(__VA_ARGS__)
#define M3FATAL_ONCE(...)  macro_dispatcher(__M3LOG_FATAL_ONCE_, __VA_ARGS__)(__VA_ARGS__)

#else /*_WIN32*/

#include <iostream>
#define M3LOGGER(I,K)      

#define M3LOG(I,K)         std::cout << "LOG: " << K << std::endl;
#ifndef NDEBUG
#define M3TRACE(I,K)       std::cout << "TRACE: " << K << std::endl;
#define M3DEBUG(I,K)       std::cout << "DEBUG: " << K << std::endl;
#else
#define M3TRACE(I,K)
#define M3DEBUG(I,K)
#endif
#define M3INFO(I,K)        std::cout << "INFO: " << K << std::endl;
#define M3WARN(I,K)        std::cout << "WARN: " << K << std::endl;
#define M3ERROR(I,K)       std::cout << "ERROR: " << K << std::endl;
#define M3FATAL(I,K)       std::cout << "FATAL: " << K << std::endl;
#define M3ASSERT(I,K)      std::cout << "ASSERT: " << K << std::endl;

#define M3LOG_ONCE(I,K)    std::cout << "LOG: " << K << std::endl;
#ifndef NDEBUG
#define M3TRACE_ONCE(I,K)  std::cout << "TRACE: " << K << std::endl;
#define M3DEBUG_ONCE(I,K)  std::cout << "DEBUG: " << K << std::endl;
#else
#define M3TRACE_ONCE(I,K)
#define M3DEBUG_ONCE(I,K)
#endif
#define M3INFO_ONCE(I,K)   std::cout << "INFO: " << K << std::endl;
#define M3WARN_ONCE(I,K)   std::cout << "WARN: " << K << std::endl;
#define M3ERROR_ONCE(I,K)  std::cout << "ERROR: " << K << std::endl;
#define M3FATAL_ONCE(I,K)  std::cout << "FATAL: " << K << std::endl;

#endif


#endif /* M3LOGGER_HH_ */
