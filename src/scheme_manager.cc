#include "scheme_manager.hh"

#include "string.hh"

#include <chibi/eval.h>

namespace Kakoune
{

SchemeManager::SchemeManager()
{
  m_ctx = sexp_make_eval_context(NULL, NULL, NULL, 0, 0);
  sexp_load_standard_env(m_ctx, NULL, SEXP_SEVEN);
//  sexp_load_standard_ports(m_ctx, NULL, stdin, stdout, stderr, 0);
}

SchemeManager::~SchemeManager()
{
  sexp_destroy_context(m_ctx);
}

String SchemeManager::eval(StringView scheme_code, const Context& context)
{
  sexp_gc_var2(computation_result, result_as_string);
  sexp_gc_preserve2(m_ctx, computation_result, result_as_string);

  // eval the Scheme code and convert result to a string
  computation_result = sexp_eval_string(m_ctx, scheme_code.zstr(), -1, NULL);
  result_as_string = sexp_write_to_string(m_ctx, computation_result);
  sexp_uint_t length = sexp_string_size(result_as_string);
  char* data = sexp_string_data(result_as_string);
  String result{data, (ByteCount) length};

  sexp_gc_release2(m_ctx);

  return result;
}

}
