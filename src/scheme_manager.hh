#ifndef scheme_manager_hh_INCLUDED
#define scheme_manager_hh_INCLUDED

#include "string.hh"
#include "utils.hh"

#include <chibi/eval.h>

namespace Kakoune
{

class Context;

class SchemeManager : public Singleton<SchemeManager>
{
public:
    SchemeManager();
    ~SchemeManager();

    String eval(StringView cmdline, const Context& context);
    //void register_env_var(StringView str, bool prefix, EnvVarRetriever retriever);

    //CandidateList complete_env_var(StringView prefix, ByteCount cursor_pos) const;

private:
    sexp m_ctx;
};

}

#endif // scheme_manager_hh_INCLUDED

