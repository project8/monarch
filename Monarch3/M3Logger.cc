/*
 * M3Logger.cc
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

#include "M3Logger.hh"

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iomanip>
#include <iterator>
#ifndef _WIN32
#include <sys/time.h>
#endif
#include <time.h>

using namespace std;

namespace monarch3
{
    const string& EndColor() {static string* color = new string(COLOR_PREFIX COLOR_NORMAL COLOR_SUFFIX); return *color;}
    const string& FatalColor() {static string* color = new string(COLOR_PREFIX COLOR_BRIGHT COLOR_SEPARATOR COLOR_FOREGROUND_RED   COLOR_SUFFIX); return *color;}
    const string& ErrorColor() {static string* color = new string(COLOR_PREFIX COLOR_BRIGHT COLOR_SEPARATOR COLOR_FOREGROUND_RED   COLOR_SUFFIX); return *color;}
    const string& WarnColor() {static string* color = new string(COLOR_PREFIX COLOR_BRIGHT COLOR_SEPARATOR COLOR_FOREGROUND_YELLOW COLOR_SUFFIX); return *color;}
    const string& InfoColor() {static string* color = new string(COLOR_PREFIX COLOR_BRIGHT COLOR_SEPARATOR COLOR_FOREGROUND_GREEN  COLOR_SUFFIX); return *color;}
    const string& DebugColor() {static string* color = new string(COLOR_PREFIX COLOR_BRIGHT COLOR_SEPARATOR COLOR_FOREGROUND_CYAN  COLOR_SUFFIX); return *color;}
    const string& OtherColor() {static string* color = new string(COLOR_PREFIX COLOR_BRIGHT COLOR_SEPARATOR COLOR_FOREGROUND_WHITE COLOR_SUFFIX); return *color;}

    struct M3Logger::Private
    {
            static char sDateTimeFormat[16];
#ifdef _WIN32
			static time_t sRawTime;
			static tm sProcessedTime;
#else
			static time_t sRawTime;
			static tm* sProcessedTime;
#endif
			static char sTimeBuff[512];
            static size_t getTimeAbsoluteStr()
            {
                time(&M3Logger::Private::sRawTime);
#ifdef _WIN32
				gmtime_s(&sProcessedTime, &M3Logger::Private::sRawTime);
				return strftime(M3Logger::Private::sTimeBuff, 512,
					M3Logger::Private::sDateTimeFormat,
					&M3Logger::Private::sProcessedTime);

#else
				sProcessedTime = gmtime(&M3Logger::Private::sRawTime);
				return strftime(M3Logger::Private::sTimeBuff, 512,
					M3Logger::Private::sDateTimeFormat,
					M3Logger::Private::sProcessedTime);
#endif

            }

            const char* fLogger;
            bool fColored;
            ELevel fThreshold;

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
                    case eTrace : return DebugColor(); break;
                    case eDebug : return DebugColor(); break;
                    case eInfo  : return InfoColor(); break;
                    case eWarn  : return WarnColor(); break;
                    case eError : return ErrorColor(); break;
                    case eFatal : return FatalColor(); break;
                    default     : return OtherColor();
                }
            }


            void logCout(ELevel level, const string& message, const Location& loc)
            {
                getTimeAbsoluteStr();
                if (fColored)
                {
                    //cout << color << KTLogger::Private::sTimeBuff << " [" << setw(5) << level << "] " << setw(16) << left << loc.fFileName << "(" << loc.fLineNumber  << "): " << message << skKTEndColor << endl;
                    cout << Private::level2Color(level) << M3Logger::Private::sTimeBuff << " [" << setw(5) << Private::level2Str(level) << "] ";
                    copy(loc.fFileName.end() - std::min< int >(loc.fFileName.size(), 16), loc.fFileName.end(), ostream_iterator<char>(cout));
                    cout << "(" << loc.fLineNumber  << "): ";
                    cout << message << EndColor() << endl;
                }
                else
                {
                    //cout << KTLogger::Private::sTimeBuff << " [" << setw(5) << level << "] " << setw(16) << left << loc.fFileName << "(" << loc.fLineNumber  << "): " << message << endl;
                    cout << M3Logger::Private::sTimeBuff << " [" << setw(5) << level << "] ";
                    copy(loc.fFileName.end() - std::min< int >(loc.fFileName.size(), 16), loc.fFileName.end(), ostream_iterator<char>(cout));
                    cout << "(" << loc.fLineNumber  << "): ";
                    cout << message << endl;
                }
            }

            void logCerr(ELevel level, const string& message, const Location& loc)
            {
                getTimeAbsoluteStr();
                if (fColored)
                {
                    //cout << color << KTLogger::Private::sTimeBuff << " [" << setw(5) << level << "] " << setw(16) << left << loc.fFileName << "(" << loc.fLineNumber  << "): " << message << skKTEndColor << endl;
                    cout << Private::level2Color(level) << M3Logger::Private::sTimeBuff << " [" << setw(5) << Private::level2Str(level) << "] ";
                    copy(loc.fFileName.end() - std::min< int >(loc.fFileName.size(), 16), loc.fFileName.end(), ostream_iterator<char>(cout));
                    cout << "(" << loc.fLineNumber  << "): ";
                    cout << message << EndColor() << endl;
                }
                else
                {
                    //cout << KTLogger::Private::sTimeBuff << " [" << setw(5) << level << "] " << setw(16) << left << loc.fFileName << "(" << loc.fLineNumber  << "): " << message << endl;
                    cout << M3Logger::Private::sTimeBuff << " [" << setw(5) << Private::level2Str(level) << "] ";
                    copy(loc.fFileName.end() - std::min< int >(loc.fFileName.size(), 16), loc.fFileName.end(), ostream_iterator<char>(cout));
                    cout << "(" << loc.fLineNumber  << "): ";
                    cout << message << endl;
                }
            }
    };

    char M3Logger::Private::sDateTimeFormat[16];
    char M3Logger::Private::sTimeBuff[512];
#ifdef _WIN32
	time_t M3Logger::Private::sRawTime = time(NULL);
	tm M3Logger::Private::sProcessedTime;
#else
	time_t M3Logger::Private::sRawTime;
	tm* M3Logger::Private::sProcessedTime;
#endif
    M3Logger::M3Logger(const char* name) : fPrivate(new Private())
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
#ifdef _WIN32
		sprintf(M3Logger::Private::sDateTimeFormat, "%%H:%%M:%%SZ");
#else
		sprintf(M3Logger::Private::sDateTimeFormat, "%%T");
#endif
        SetLevel(eDebug);
    }

    M3Logger::M3Logger(const std::string& name) : fPrivate(new Private())
    {
        fPrivate->fLogger = name.c_str();
        fPrivate->fColored = true;
#ifdef _WIN32
		sprintf(M3Logger::Private::sDateTimeFormat, "%%H:%%M:%%SZ");
#else
		sprintf(M3Logger::Private::sDateTimeFormat, "%%T");
#endif
		
        SetLevel(eDebug);
    }

    M3Logger::~M3Logger()
    {
        delete fPrivate;
    }

    bool M3Logger::IsLevelEnabled(ELevel level) const
    {
        return level >= fPrivate->fThreshold;
    }

    void M3Logger::SetLevel(ELevel level) const
    {
#if defined(NDEBUG)
                fPrivate->fThreshold = level >= eInfo ? level : eInfo;
#else
                fPrivate->fThreshold = level;
#endif
    }

    void M3Logger::Log(ELevel level, const string& message, const Location& loc)
    {
        if (level >= eWarn)
        {
            fPrivate->logCerr(level, message, loc);
        }
        else
        {
            fPrivate->logCout(level, message, loc);
        }
    }
}
