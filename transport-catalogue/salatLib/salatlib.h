/* [≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡▲≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡] */
// ReSharper disable CppInconsistentNaming
#pragma once
#include <chrono>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>
/*[BEGIN:==================================================SALAT™====================================================]*/
/*[=======коммент для ревьюера: я сама писала эту библиотеку если что, а то б*лин оформила официально ка*пец :3======]*/
namespace salatlib {
    /*[BEGIN:=============================================VARIABLES==================================================]*/
#define IDENTITY arg_file << " => " << arg_line << " in " <<  arg_function
    constexpr const char* K_SALAT = "SALAT™: ";
    constexpr const char* K_SALT  = "SALT™ : ";
    constexpr const char* K_FICO  = "FICO™ : ";
    constexpr const char* K_RIN   = "RIN™  : ";
    constexpr const char* K_MSG   = "MSG: ";
    constexpr const char* K_SRC   = "SRC: ";
    constexpr const char* K_BEG   = "BEG: ";
    constexpr const char* K_END   = "END: ";
    constexpr const char* K_DUR   = "DUR: ";
    constexpr const char* K_REQ   = "REQ: ";
    constexpr const char* K_LOG   = "LOG: ";
    constexpr const char* K_WAR   = "WAR: ";
    constexpr const char* K_TRW   = "TRW: ";
    class SALAT {
        enum class eOwner { NONE , SALT , SALAT };
        struct sStandaloneTimer {
            double                                             timertime{};
            unsigned long long                                 timercount{};
            eOwner                                             owner{eOwner::NONE};
            std::chrono::time_point<std::chrono::steady_clock> time{};
        };
        static inline std::unordered_map<unsigned long long , sStandaloneTimer> standalone_timers_;
        static inline std::ostream* out_{&std::cerr}; // NOLINT(*-interfaces-global-init)
        public:
            enum class eIgnore { NONE , TRAILING , ALL };
            enum class eMessageLevel { LOG , WARNING };
            /*[END:============================================VARIABLES=============================================]*/

            /*[BEGIN:============================================EXTRA===============================================]*/
            static constexpr uint64_t HashStr(const char* str , const uint64_t hash = 14695981039346656037ull) {
                return (*str) ? HashStr(str + 1, (hash ^ static_cast<uint64_t>(*str)) * 1099511628211ull) : hash;
            }

            static bool IsWhitespace(const char arg_char) {
                switch(arg_char) {
                    case ' ' :
                    case '\t' :
                    case '\n' :
                    case '\r' :
                    case '\v' :
                    case '\f' : return true;
                    default : return false;
                }
            }

            static void TrimString(std::string& s) {
                const auto first = std::find_if_not(s.begin(), s.end(), IsWhitespace);
                if(first == s.end()) {
                    s.clear();
                    return;
                }
                const auto last = std::find_if_not(s.rbegin(), s.rend(), IsWhitespace).base();
                s.erase(last, s.end());
                s.erase(s.begin(), first);
            }

            static void SetLogOutput(std::ostream& arg_out) { out_ = &arg_out; }
            /*[END:=============================================EXTRA================================================]*/

            /*[BEGIN:===========================================RIN™=================================================]*/
            template<typename tRet>
            static tRet InvokeLog(const eMessageLevel arg_message_level
                                , const int           arg_line
                                , const std::string&  arg_function
                                , const tRet&         arg_message
                                , const std::string&  arg_file) {
                switch(arg_message_level) {
                    case eMessageLevel::LOG : if(out_ == &std::cerr) {
                            InvokeLog<std::string>(eMessageLevel::WARNING
                                                 , arg_line
                                                 , arg_function
                                                 , "YOU NEED TO SALAT_SETOUTPUT(std::ostream&) "
                                                 , arg_file);
                            break;
                        }
                        *out_ << K_RIN << K_LOG << K_SRC << IDENTITY << "\n" << K_RIN << K_MSG << arg_message <<
                                std::endl;
                        break;
                    case eMessageLevel::WARNING : *out_ << K_RIN << K_WAR << IDENTITY << "\n" << K_RIN << K_MSG <<
                                arg_message << std::endl;
                        break;
                }
                return arg_message;
            }

            template<typename tException = std::runtime_error>
            [[noreturn]] static void InvokeError(const int          arg_line
                                               , const char*        arg_function
                                               , const std::string& arg_message
                                               , const std::string& arg_file) {
                if(out_ == &std::cerr) {
                    InvokeLog<std::string>(eMessageLevel::WARNING
                                         , arg_line
                                         , arg_function
                                         , "YOU NEED TO SALAT_SETOUTPUT(std::ostream&) "
                                         , arg_file);
                    std::cerr << K_RIN << K_TRW << IDENTITY << "\n" << K_RIN << K_MSG << std::endl;
                    throw tException(arg_message);
                }
                *out_ << K_RIN << K_TRW << IDENTITY << "\n" << K_RIN << K_MSG << arg_message << std::endl;
                throw tException(arg_message);
            }

            /*[END:=============================================RIN™=================================================]*/

            /*[BEGIN:===========================================SALT™================================================]*/
            static void StandaloneTimerBegin(const int                arg_line
                                           , const unsigned long long arg_hash
                                           , const bool               arg_log
                                           , const std::string&       arg_function
                                           , const std::string&       arg_file) {
                if(out_ == &std::cerr) {
                    InvokeLog<std::string>(eMessageLevel::WARNING
                                         , arg_line
                                         , arg_function
                                         , "YOU NEED TO SALAT_SETOUTPUT(std::ostream&) "
                                         , arg_file);
                    return;
                }
                if(arg_log && standalone_timers_[arg_hash].owner != eOwner::NONE) {
                    InvokeLog<std::string>(eMessageLevel::WARNING
                                         , arg_line
                                         , arg_function
                                         , "YOU CANNOT USE SALAT_SALTBEGIN() AND SALAT_BEGIN() WITHIN"
                                           " THE SAME FUNCTION, CONSIDER SALAT_SALTCTMFBEGIN()"
                                         , arg_file);
                    return;
                }
                standalone_timers_[arg_hash].time = std::chrono::steady_clock::now();
                if(arg_log) {
                    standalone_timers_[arg_hash].owner = eOwner::SALT;
                    *out_ << K_SALT << K_BEG << IDENTITY << std::endl;
                }
            }

            static double StandaloneTimerEnd(const int                arg_line
                                           , const unsigned long long arg_hash
                                           , const bool               arg_log
                                           , const std::string&       arg_function
                                           , const std::string&       arg_file) {
                if(out_ == &std::cerr) {
                    InvokeLog<std::string>(eMessageLevel::WARNING
                                         , arg_line
                                         , arg_function
                                         , "YOU NEED TO SALAT_SETOUTPUT(std::ostream&) "
                                         , arg_file);
                    return -1;
                }
                if(arg_log && standalone_timers_[arg_hash].owner != eOwner::SALT) {
                    InvokeLog<std::string>(eMessageLevel::WARNING
                                         , arg_line
                                         , arg_function
                                         , "CANNOT STOP NOT STARTED SALT"
                                         , arg_file);
                    return 0;
                }
                if(arg_log && standalone_timers_[arg_hash].owner == eOwner::SALAT) {
                    InvokeLog<std::string>(eMessageLevel::WARNING
                                         , arg_line
                                         , arg_function
                                         , "YOU CANNOT USE SALAT_SALTBEGIN() AND SALAT_BEGIN()"
                                           " WITHIN THE SAME FUNCTION, CONSIDER SALAT_SALTCTMFBEGIN()"
                                         , arg_file);
                    return 0;
                }
                const std::chrono::duration<double , std::milli> ms = std::chrono::steady_clock::now() -
                        standalone_timers_[arg_hash].time;
                if(arg_log) {
                    standalone_timers_[arg_hash].owner = eOwner::NONE;
                    *out_ << K_SALT << K_END << IDENTITY << '\n';
                    *out_ << K_SALT << K_DUR << ms.count() << "ms" << std::endl;
                }
                return ms.count();
            }
            /*[END:=============================================SALT™================================================]*/

            /*[BEGIN:==========================================SALAT™================================================]*/
            static void StandaloneAverageBegin(const int          arg_line
                                             , const char*        arg_function
                                             , const std::string& arg_file) {
                if(out_ == &std::cerr) {
                    InvokeLog<std::string>(eMessageLevel::WARNING
                                         , arg_line
                                         , arg_function
                                         , "YOU NEED TO SALAT_SETOUTPUT(std::ostream&) "
                                         , arg_file);
                    return;
                }
                const long long unsigned int hash_key = HashStr(arg_function);
                if(standalone_timers_[hash_key].owner != eOwner::NONE) {
                    InvokeLog<std::string>(eMessageLevel::WARNING
                                         , arg_line
                                         , arg_function
                                         , "YOU CANNOT USE SALAT_SALTBEGIN() AND SALAT_BEGIN()"
                                           " WITHIN THE SAME FUNCTION, CONSIDER SALAT_SALTCTMFBEGIN()"
                                         , arg_file);
                    return;
                }
                standalone_timers_[hash_key].owner      = eOwner::SALAT;
                standalone_timers_[hash_key].timercount = 0;
                standalone_timers_[hash_key].timertime  = 0;
                StandaloneTimerBegin(arg_line, hash_key, false, arg_function, arg_file);
                *out_ << K_SALAT << K_BEG << IDENTITY << std::endl;
            }

            static void StandaloneAverageBreak(const int          arg_line
                                             , const char*        arg_function
                                             , const std::string& arg_file) {
                if(out_ == &std::cerr) {
                    InvokeLog<std::string>(eMessageLevel::WARNING
                                         , arg_line
                                         , arg_function
                                         , "YOU NEED TO SALAT_SETOUTPUT(std::ostream&) "
                                         , arg_file);
                    return;
                }
                const long long unsigned int hash_key = HashStr(arg_function);
                if(standalone_timers_[hash_key].owner != eOwner::SALAT) {
                    InvokeLog<std::string>(eMessageLevel::WARNING
                                         , arg_line
                                         , arg_function
                                         , "YOU CANNOT SALAT_BREAK() BEFORE SALAT_BEGIN()"
                                         , arg_file);
                    return;
                }
                standalone_timers_[hash_key].timertime += StandaloneTimerEnd(arg_line
                                                                           , hash_key
                                                                           , false
                                                                           , arg_function
                                                                           , arg_file);
                ++standalone_timers_[hash_key].timercount;
                StandaloneTimerBegin(arg_line, hash_key, false, arg_function, arg_file);
            }

            static double StandaloneAverageEnd(const int          arg_line
                                             , const char*        arg_function
                                             , const std::string& arg_file) {
                if(out_ == &std::cerr) {
                    InvokeLog<std::string>(eMessageLevel::WARNING
                                         , arg_line
                                         , arg_function
                                         , "YOU NEED TO SALAT_SETOUTPUT(std::ostream&) "
                                         , arg_file);
                    return -1;
                }
                const long long unsigned int hash_key = HashStr(arg_function);
                if(standalone_timers_[hash_key].owner != eOwner::SALAT) {
                    InvokeLog<std::string>(eMessageLevel::WARNING
                                         , arg_line
                                         , arg_function
                                         , "YOU CANNOT SALAT_END() BEFORE SALAT_BEGIN()"
                                         , arg_file);
                    return 0;
                }
                standalone_timers_[hash_key].timertime += StandaloneTimerEnd(arg_line
                                                                           , hash_key
                                                                           , false
                                                                           , arg_function
                                                                           , arg_file);
                ++standalone_timers_[hash_key].timercount;
                if(standalone_timers_[hash_key].timercount == 0) { return 0; }
                const double retnum = (standalone_timers_[hash_key].timertime) / static_cast<double>(standalone_timers_[
                    hash_key].timercount);
                *out_ << K_SALAT << K_END << IDENTITY << '\n';
                *out_ << K_SALAT << K_DUR << "AVREQ: " << retnum << "ms\n";
                *out_ << K_SALAT << K_DUR << "TOTAL: " << standalone_timers_[hash_key].timertime << "ms\n";
                *out_ << K_SALAT << K_REQ << standalone_timers_[hash_key].timercount << std::endl;
                standalone_timers_[hash_key].owner = eOwner::NONE;
                return retnum;
            }

            /*[END:============================================SALAT™================================================]*/

            /*[BEGIN:==========================================FICO™=================================================]*/
            template<eIgnore tIgnore>
            static int FileComparer(const std::string& file1path , const std::string& file2path) {
                bool          nodiff = true;
                std::ifstream file1(file1path);
                std::ifstream file2(file2path);
                if(!file1 || !file2) {
                    std::cerr << "Failed to open one of the files.\n";
                    return 1;
                }
                std::basic_string<char> line1 , line2;
                int                     line_number = 1;
                while(std::getline(file1, line1) && std::getline(file2, line2)) {
                    if constexpr(tIgnore == eIgnore::ALL) {
                        std::erase_if(line1, IsWhitespace);
                        std::erase_if(line2, IsWhitespace);
                    }
                    if constexpr(tIgnore == eIgnore::TRAILING) {
                        TrimString(line1);
                        TrimString(line2);
                    }
                    if(line1 != line2) {
                        nodiff = false;
                        *out_ << K_FICO << "NGL: " << line_number << "\n" << K_FICO << "FL1: " << line1 << "\n" <<
                                K_FICO << "LEN: " << line1.size() << "\n" << K_FICO << "FL2: " << line2 << "\n" <<
                                K_FICO << "LEN: " << line2.size() << "\n";
                    }
                    ++line_number;
                }
                *out_ << K_FICO << (nodiff ? "FILE: OK" : "FILE: NG") << std::endl;
                file1.close();
                file2.close();
                return nodiff ? 0 : -1;
            }

            static int FullFileComparer(const std::string& file1path , const std::string& file2path) {
                const int r1 = FileComparer<eIgnore::ALL>(file1path, file2path);
                const int r2 = FileComparer<eIgnore::TRAILING>(file1path, file2path);
                const int r3 = FileComparer<eIgnore::NONE>(file1path, file2path);
                return (r1 == 0 && r2 == 0 && r3 == 0) ? 0 : -1;
            }

            /*[END:=============================================FICO=================================================]*/
    };
#undef IDENTITY
}
/*[BEGIN:==============================================MACROS====================================================]*/
#ifndef SALAT_ENABLED
#define SALAT_ENABLED 1
#endif
#if SALAT_ENABLED
///SALAT™: RIN™: invoke report with log severety\n
///        msg : message
#define SALAT_RINLOG(msg) \
    salatlib::SALAT::InvokeLog(salatlib::SALAT::eMessageLevel::LOG, __LINE__, __func__, msg, __FILE_NAME__)

///SALAT™: RIN™: invoke report with log severety and custom function name\n
///        func: custom function title\n
///        msg : message
#define SALAT_RINCTMFLOG(func, msg) \
    salatlib::SALAT::InvokeLog(salatlib::SALAT::eMessageLevel::LOG, __LINE__, func, msg, __FILE_NAME__)

///SALAT™: RIN™: invoke report with warning severety\n
///        msg : message
#define SALAT_RINWAR(msg) \
    salatlib::SALAT::InvokeLog<decltype(msg)>(salatlib::SALAT::eMessageLevel::WARNING, __LINE__, __func__, msg, __FILE_NAME__)

///SALAT™: RIN™: invoke report with warning severety\n
///        func: custom function title\n
///        msg : message
#define SALAT_RINCTMFWAR(func, msg) \
    salatlib::SALAT::InvokeLog(salatlib::SALAT::eMessageLevel::WARNING, __LINE__, func, msg, __FILE_NAME__)

///SALAT™: SALT™: start StandALone Timer
#define SALAT_SALTBEGIN() \
    salatlib::SALAT::StandaloneTimerBegin(__LINE__, salatlib::SALAT::HashStr(__func__), true, __func__, __FILE_NAME__)

///SALAT™: SALT™: start StandALone Timer with a custom function title\n
///        func : custom function title\n
#define SALAT_SALTCTMFBEGIN(func) \
    salatlib::SALAT::StandaloneTimerBegin(__LINE__, salatlib::SALAT::HashStr(func), true, __func__, __FILE_NAME__)

///SALAT™: SALT™: end StandALone Timer
#define SALAT_SALTEND() \
    salatlib::SALAT::StandaloneTimerEnd(__LINE__, salatlib::SALAT::HashStr(__func__), true, __func__, __FILE_NAME__)

///SALAT™: SALT™: end StandALone Timer with a custom function title\n
///        func : custom function title
#define SALAT_SALTCTMFEND(func) \
    salatlib::SALAT::StandaloneTimerEnd(__LINE__ , salatlib::SALAT::HashStr(func) , true , __func__ , __FILE_NAME__)

///SALAT™: start StandALone Average Timer
#define SALAT_BEGIN() \
    salatlib::SALAT::StandaloneAverageBegin(__LINE__, __func__, __FILE_NAME__)

///SALAT™: start  StandALone Average Timer with a custom function title\n
///        func : custom function title
#define SALAT_CTMFBEGIN(func) \
    salatlib::SALAT::StandaloneAverageBegin(__LINE__, func, __FILE_NAME__)

///SALAT™: break StandALone Average Timer
#define SALAT_BREAK() \
    salatlib::SALAT::StandaloneAverageBreak(__LINE__, __func__, __FILE_NAME__)

///SALAT™: break  StandALone Average Timer with a custom function title\n
///        func : custom function title
#define SALAT_CTMFBREAK(func) \
    salatlib::SALAT::StandaloneAverageBreak(__LINE__, func, __FILE_NAME__)

///SALAT™: end StandALone Average Timer with
#define SALAT_END() \
    salatlib::SALAT::StandaloneAverageEnd(__LINE__, __func__, __FILE_NAME__)

///SALAT™: end    StandALone Average Timer with a custom function title\n
///        func : custom function title
#define SALAT_CTMFEND(func) \
    salatlib::SALAT::StandaloneAverageEnd(__LINE__, func, __FILE_NAME__)

///SALAT™: FICO™: run FIleCOmparer for file1 and file2, returns 0 or -1, ignores all whitespaces
#define SALAT_SOFTFICO(file1, file2)\
    salatlib::SALAT::FileComparer<salatlib::SALAT::eIgnore::ALL>(file1, file2)

///SALAT™: FICO™: run FIleCOmparer for file1 and file2, returns 0 or -1, ignores trailing whitespaces
#define SALAT_FICO(file1, file2)\
    salatlib::SALAT::FileComparer<salatlib::SALAT::eIgnore::TRAILING>(file1, file2)

///SALAT™: FICO™: run FIleCOmparer for file1 and file2, returns 0 or -1, doesnt ignore whitespaces
#define SALAT_STRICTFICO(file1, file2)\
    salatlib::SALAT::FileComparer<salatlib::SALAT::eIgnore::NONE>(file1, file2)

///SALAT™: FICO™: run 3 FIleCOmparers for file1 and file2, returns 0 or -1
#define SALAT_FULLFICO(file1, file2)\
    salatlib::SALAT::FullFileComparer(file1, file2)
#else
#define SALAT_RINLOG(msg) ((void)0)
#define SALAT_RINCTMFLOG(func, msg) ((void)0)
#define SALAT_RINWAR(msg) msg
#define SALAT_RINCTMFWAR(func, msg) msg
#define SALAT_SALTBEGIN() ((void)0)
#define SALAT_SALTCTMFBEGIN(func) ((void)0)
#define SALAT_SALTEND() ((void)0)
#define SALAT_SALTCTMFEND(func) ((void)0)
#define SALAT_BEGIN() ((void)0)
#define SALAT_CTMFBEGIN(func) ((void)0)
#define SALAT_BREAK() ((void)0)
#define SALAT_CTMFBREAK(func) ((void)0)
#define SALAT_END() ((void)0)
#define SALAT_CTMFEND(func) ((void)0)
#define SALAT_SOFTFICO(file1, file2) 0
#define SALAT_FICO(file1, file2) 0
#define SALAT_STRICTFICO(file1, file2) 0
#define SALAT_FULLFICO(file1, file2) 0
#endif

///SALAT™: SET DEFAULT OUTPUT FOR THE SALATLIB
#define SALAT_SETOUTPUT(out) \
    salatlib::SALAT::SetLogOutput(out)

///SALAT™: RIN™: invoke report with throw severety\n
///        msg : message
#define SALAT_RINTHR(msg) \
    salatlib::SALAT::InvokeError(__LINE__, __func__, msg, __FILE_NAME__)

///SALAT™: RIN™: invoke report with throw severety and custom exception type\n
///        type: custom exception type\n
///        msg : message
#define SALAT_RINCTMTTHR(type, msg) \
    salatlib::SALAT::InvokeError<type>(__LINE__, __func__, msg, __FILE_NAME__)

///SALAT™: RIN™: invoke report with throw severety and custom function title\n
///        func: custom function title\n
///        msg : message
#define SALAT_RINCTMFTHR(func, msg) \
    salatlib::SALAT::InvokeError(__LINE__, func, msg, __FILE_NAME__)

///SALAT™: RIN™: invoke report with throw severety custom exception type\n
///        func: custom function title\n
///        type: custom exception type\n
///        msg : message
#define SALAT_RINCTMTFTHR(type, func, msg) \
    salatlib::SALAT::InvokeError<type>(__LINE__, func, msg, __FILE_NAME__)

///SALAT™: XTR: check if char is whitespace
#define SALAT_XTRISWS(ch) \
    salatlib::SALAT::IsWhitespace(ch);

///SALAT™: XTR: trim string
#define SALAT_XTRTRIM(str) \
    salatlib::SALAT::TrimString(str);

///SALAT™: XTR: hash string
#define SALAT_XTRHASH(str) \
    salatlib::SALAT::HashStr(str);
/*[END:==============================================MACROS====================================================]*/
/*[END:====================================================SALAT™====================================================]*/
/* [≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡▲≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡] */
