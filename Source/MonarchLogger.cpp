/*
 * mt_logger.cc
 *
 *  Created on: Jan 21, 2014
 *      Author: nsoblath
 */

/*
 * mt_logger.cxx
 * based on KLogger.cxx from KATRIN's Kasper
 *
 *  Created on: 18.11.2011
 *      Author: Marco Haag <marco.haag@kit.edu>
 */

#include "MonarchLogger.hpp"

#include <cstdlib>
#include <cstring>

using namespace std;

static const string skMEndColor =   COLOR_PREFIX COLOR_NORMAL COLOR_SUFFIX;
static const string skMFatalColor = COLOR_PREFIX COLOR_BRIGHT COLOR_SEPARATOR COLOR_FOREGROUND_RED    COLOR_SUFFIX;
static const string skMErrorColor = COLOR_PREFIX COLOR_BRIGHT COLOR_SEPARATOR COLOR_FOREGROUND_RED    COLOR_SUFFIX;
static const string skMWarnColor =  COLOR_PREFIX COLOR_BRIGHT COLOR_SEPARATOR COLOR_FOREGROUND_YELLOW COLOR_SUFFIX;
static const string skMInfoColor =  COLOR_PREFIX COLOR_BRIGHT COLOR_SEPARATOR COLOR_FOREGROUND_GREEN  COLOR_SUFFIX;
static const string skMDebugColor = COLOR_PREFIX COLOR_BRIGHT COLOR_SEPARATOR COLOR_FOREGROUND_CYAN   COLOR_SUFFIX;
static const string skMOtherColor = COLOR_PREFIX COLOR_BRIGHT COLOR_SEPARATOR COLOR_FOREGROUND_WHITE  COLOR_SUFFIX;


#if defined(LOG4CXX_FOUND)

/*
 * Default implementation for systems with the 'log4cxx' library installed.
 */

#include <log4cxx/logstring.h>
#include <log4cxx/basicconfigurator.h>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/patternlayout.h>
#include <log4cxx/level.h>
#include <log4cxx/consoleappender.h>
#include <log4cxx/logmanager.h>
#include <log4cxx/logger.h>

using namespace log4cxx;

#ifndef M_LOG4CXX_COLORED_PATTERN_LAYOUT_H
#define M_LOG4CXX_COLORED_PATTERN_LAYOUT_H

namespace log4cxx
{

    class LOG4CXX_EXPORT MColoredPatternLayout : public PatternLayout
    {
        public:
            DECLARE_LOG4CXX_OBJECT(MColoredPatternLayout)
        BEGIN_LOG4CXX_CAST_MAP()
        LOG4CXX_CAST_ENTRY(MColoredPatternLayout)
        LOG4CXX_CAST_ENTRY_CHAIN(Layout)
        END_LOG4CXX_CAST_MAP()

        MColoredPatternLayout() : PatternLayout() {}
            MColoredPatternLayout(const LogString& pattern) : PatternLayout(pattern) {};
            virtual ~MColoredPatternLayout() {}

        protected:
            virtual void format(LogString& output, const spi::LoggingEventPtr& event, helpers::Pool& pool) const;
            virtual std::string getColor(const LevelPtr& level) const;

    };

    LOG4CXX_PTR_DEF(MColoredPatternLayout);

}

#endif /* M_LOG4CXX_COLORED_PATTERN_LAYOUT_H */

IMPLEMENT_LOG4CXX_OBJECT(MColoredPatternLayout)

void MColoredPatternLayout::format(LogString& output, const spi::LoggingEventPtr& event, helpers::Pool& pool) const
{
    PatternLayout::format(output, event, pool);
    output = getColor(event->getLevel()) + output + skMEndColor;
    return;
}

string MColoredPatternLayout::getColor(const LevelPtr& level) const
{
    switch(level->toInt())
    {
        case Level::FATAL_INT:
            return skMFatalColor;
            break;
        case Level::ERROR_INT:
            return skMErrorColor;
            break;
        case Level::WARN_INT:
            return skMWarnColor;
            break;
        case Level::INFO_INT:
            return skMInfoColor;
            break;
        case Level::DEBUG_INT:
            return skMDebugColor;
            break;
        case Level::TRACE_INT:
            return skMDebugColor;
            break;
        default:
            return skMOtherColor;
    }
}

namespace monarch
{

    struct MStaticInitializer
    {
            MStaticInitializer()
            {
                // the check of whether the logger repository is configured was removed to allow nested use of the log4cxx-based logger.
                // otherwise the logger from the parent package isn't able to override the settings of the child.
                //if (LogManager::getLoggerRepository()->isConfigured())
                //    return;
                //        AppenderList appenders = Logger::getRootLogger()->getAllAppenders();

                char* envLoggerConfig;
                envLoggerConfig = getenv("LOGGER_CONFIGURATION");
                if (envLoggerConfig != 0)
                {
                    PropertyConfigurator::configure(envLoggerConfig);
                }
                else
                {
#ifdef LOGGER_CONFIGURATION
                    PropertyConfigurator::configure(LOGGER_CONFIGURATION);
#else
                    LogManager::getLoggerRepository()->setConfigured(true);
                    LoggerPtr root = Logger::getRootLogger();
#ifdef NDEBUG
                    Logger::getRootLogger()->setLevel(Level::getInfo());
#endif
                    static const LogString TTCC_CONVERSION_PATTERN(LOG4CXX_STR("%r [%-5p] %16c: %m%n"));
                    //LayoutPtr layout(new PatternLayout(TTCC_CONVERSION_PATTERN));
                    LayoutPtr layout(new MColoredPatternLayout(TTCC_CONVERSION_PATTERN));
                    AppenderPtr appender(new ConsoleAppender(layout));
                    root->addAppender(appender);
#endif
                }

            }
    } static sMLoggerInitializer;

}

namespace monarch
{

    struct MonarchLogger::Private
    {
            void log(const LevelPtr& level, const string& message, const Location& loc)
            {
                fLogger->forcedLog(level, message, ::log4cxx::spi::LocationInfo(loc.fFileName, loc.fFunctionName, loc.fLineNumber));
            }

            static LevelPtr level2Ptr(ELevel level)
            {
                switch(level)
                {
                    case eTrace : return Level::getTrace();
                    case eDebug : return Level::getDebug();
                    case eInfo  : return Level::getInfo();
                    case eWarn  : return Level::getWarn();
                    case eError : return Level::getError();
                    case eFatal : return Level::getFatal();
                    default     : return Level::getOff();
                }
            }

            LoggerPtr fLogger;
    };

    MonarchLogger::MonarchLogger(const char* name) : fPrivate(new Private())
    {
        fPrivate->fLogger = (name == 0) ? Logger::getRootLogger() : Logger::getLogger(name);
    }

    MonarchLogger::MonarchLogger(const std::string& name) : fPrivate(new Private())
    {
        fPrivate->fLogger = Logger::getLogger(name);
    }

    MonarchLogger::~MonarchLogger()
    {
        delete fPrivate;
    }

    bool MonarchLogger::IsLevelEnabled(ELevel level) const
    {
        return fPrivate->fLogger->isEnabledFor( Private::level2Ptr(level) );
    }

    void MonarchLogger::SetLevel(ELevel level) const
    {
        fPrivate->fLogger->setLevel( Private::level2Ptr(level) );
    }

    void MonarchLogger::Log(ELevel level, const string& message, const Location& loc)
    {
        fPrivate->log(Private::level2Ptr(level), message, loc);
    }

}

#else

/**
 * Fallback solution for systems without log4cxx.
 */

#include <iomanip>

namespace monarch
{
    struct MonarchLogger::Private
    {
            const char* fLogger;
            bool fColored;

            static const char* level2Str(ELevel level)
            {
                switch(level)
                {
                    case eTrace : return "TRACE"; break;
                    case eDebug : return "DEBUG"; break;
                    case eInfo  : return "INFO"; break;
                    case eWarn  : return "WARN"; break;
                    case eError : return "ERROR"; break;
                    case eFatal : return "FATAL"; break;
                    default     : return "XXX";
                }
            }

            static string level2Color(ELevel level)
            {
                switch(level)
                {
                    case eTrace : return skMDebugColor; break;
                    case eDebug : return skMDebugColor; break;
                    case eInfo  : return skMInfoColor; break;
                    case eWarn  : return skMWarnColor; break;
                    case eError : return skMErrorColor; break;
                    case eFatal : return skMErrorColor; break;
                    default     : return skMOtherColor;
                }
            }


            void logCout(const char* level, const string& message, const Location& /*loc*/, const string& color = skMOtherColor)
            {
                if (fColored)
                    cout << color << __DATE__ " " __TIME__ " [" << setw(5) << level << "] " << setw(16) << fLogger << ": " << message << skMEndColor << endl;
                else
                    cout << __DATE__ " " __TIME__ " [" << setw(5) << level << "] " << setw(16) << fLogger << ": " << message << endl;
            }

            void logCerr(const char* level, const string& message, const Location& /*loc*/, const string& color = skMOtherColor)
            {
                if (fColored)
                    cerr << color << __DATE__ " " __TIME__ " [" << setw(5) << level << "] " << setw(16) << fLogger << ": " << message << skMEndColor << endl;
                else
                    cerr << __DATE__ " " __TIME__ " [" << setw(5) << level << "] " << setw(16) << fLogger << ": " << message << endl;
            }
    };

    MonarchLogger::MonarchLogger(const char* name) : fPrivate(new Private())
    {
        if (name == 0)
        {
            fPrivate->fLogger = "root";
        }
        else
        {
            const char* logName = strrchr(name, '/') ? strrchr(name, '/') + 1 : name;
            fPrivate->fLogger = logName;
        }
        fPrivate->fColored = true;
    }

    MonarchLogger::MonarchLogger(const std::string& name) : fPrivate(new Private())
    {
        fPrivate->fLogger = name.c_str();
        fPrivate->fColored = true;
    }

    MonarchLogger::~MonarchLogger()
    {
        delete fPrivate;
    }

    bool MonarchLogger::IsLevelEnabled(ELevel level) const
    {
#ifdef NDEBUG
        return level > eDebug;
#else
        (void) level;
        return true;
#endif
    }

    void MonarchLogger::SetLevel(ELevel /*level*/) const
    {
        // TODO: implement levels for fallback logger
    }

    void MonarchLogger::Log(ELevel level, const string& message, const Location& loc)
    {
        const char* levelStr = Private::level2Str(level);

        if (level >= eFatal)
        {
            fPrivate->logCerr(levelStr, message, loc, Private::level2Color(level));
        }
        else
        {
            fPrivate->logCout(levelStr, message, loc, Private::level2Color(level));
        }
    }
}

#endif
