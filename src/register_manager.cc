#include "register_manager.hh"

#include "assert.hh"
#include "hash_map.hh"
#include "string_utils.hh"

namespace Kakoune
{

static inline String content_to_string(ConstArrayView<String> values)
{
    constexpr const char list_separator = ':';
    String res;

    bool first = true;
    for (auto it : values)
    {
        if (not first)
            res += list_separator;
        res += escape(it, list_separator, '\\');
        first = false;
    }
    return res;
}

class WatchedRegister: public Register
{
public:
    WatchedRegister(Codepoint name, std::unique_ptr<Register> reg):
        m_name(name),
        m_register(std::move(reg))
        {}
    void set(Context& ctx, ConstArrayView<String> values) override
    {
        auto old_values_view = m_register->get(ctx);
        Vector<String> old_values{old_values_view.begin(), old_values_view.end()};
        m_register->set(ctx, values);
        ctx.hooks().run_hook("RegisterChanged", format("{}={}", m_name, content_to_string(old_values)), ctx);
    }

    ConstArrayView<String> get(const Context& ctx) override
    {
        return m_register->get(ctx);
    }
private:
    const String m_name;
    std::unique_ptr<Register> m_register;
};

Register& RegisterManager::operator[](StringView reg) const
{
    if (reg.length() == 1)
        return (*this)[reg[0_byte]];

    static const HashMap<String, Codepoint> reg_names = {
        { "slash", '/' },
        { "dquote", '"' },
        { "pipe", '|' },
        { "caret", '^' },
        { "arobase", '@' },
        { "percent", '%' },
        { "dot", '.' },
        { "hash", '#' },
        { "underscore", '_' },
        { "colon", ':' }
    };
    auto it = reg_names.find(reg);
    if (it == reg_names.end())
        throw runtime_error(format("no such register: '{}'", reg));
    return (*this)[it->value];
}

Register& RegisterManager::operator[](Codepoint c) const
{
    c = to_lower(c);
    auto it = m_registers.find(c);
    if (it == m_registers.end())
        throw runtime_error(format("no such register: '{}'", c));

    return *(it->value);
}

void RegisterManager::add_register(Codepoint c, std::unique_ptr<Register> reg)
{
    auto& reg_ptr = m_registers[c];
    kak_assert(not reg_ptr);
    reg_ptr = std::make_unique<WatchedRegister>(c, std::move(reg));
}

}
