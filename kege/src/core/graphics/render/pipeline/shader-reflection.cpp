//
//  shader-reflection.cpp
//  editor
//
//  Created by Kenneth Esdaile on 4/11/26.
//

#include <fstream>
#include <sstream>
#include <cctype>
#include <algorithm>
#include "shader-reflection.hpp"

namespace kege {

    // --- Main parser ---

    void ShaderReflection::add_define(const std::string& name, uint32_t value) {
        _define_map[name] = std::to_string(value);
    }

    void ShaderReflection::add_define(const std::string& name, const std::string& value) {
        _define_map[name] = value;
    }

    std::string ShaderReflection::resolve_value(const std::string& expr) const
    {
        std::string e = expr;
        e.erase(0, e.find_first_not_of(" \t\r\n"));
        e.erase(e.find_last_not_of(" \t\r\n") + 1);
        if (e.empty()) return "";
        return e;
    }

    uint32_t ShaderReflection::resolve_int(const std::string& expr) const
    {
        std::string e = expr;
        e.erase(0, e.find_first_not_of(" \t\r\n"));
        e.erase(e.find_last_not_of(" \t\r\n") + 1);
        if (e.empty()) return 0;
        if (std::all_of(e.begin(), e.end(), ::isdigit)) {
            return static_cast<uint32_t>(std::stoul(e));
        }
        auto it = _define_map.find(e);
        if (it!= _define_map.end()) return resolve_int(it->second);
        return 0;
    }

    void ShaderReflection::parse_preprocessor(TokenStream& ts) {
        ts.skip_ws();
        if (ts.match("#define")) {
            ts.skip_ws();
            std::string name = ts.read_identifier();
            if (name.empty()) return;

            ts.skip_ws();
            // Rest of line is the value, trim it
            std::string value = ts.read_line();
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t\r\n") + 1);

            ShaderDefine def{name, value};
            _defines_found.push_back(def);
            // Also add to map so we can resolve set/binding/array sizes immediately
            _define_map[name] = value;
        }
    }

    std::string ShaderReflection::strip_comments(const std::string& src) const
    {
        std::string out;
        out.reserve(src.size());
        bool in_line = false, in_block = false;
        for (size_t i = 0; i < src.size(); ++i) {
            if (!in_line &&!in_block && src[i] == '/' && i + 1 < src.size()) {
                if (src[i+1] == '/') { in_line = true; i++; continue; }
                if (src[i+1] == '*') { in_block = true; i++; continue; }
            }
            if (in_line && src[i] == '\n') { in_line = false; out += '\n'; continue; }
            if (in_block && src[i] == '*' && i + 1 < src.size() && src[i+1] == '/') { in_block = false; i++; continue; }
            if (!in_line &&!in_block) out += src[i];
        }
        return out;
    }

    bool ShaderReflection::parse_file(const std::string& filepath)
    {
        std::ifstream file(filepath);
        if (!file) return false;
        std::stringstream buffer;
        buffer << file.rdbuf();
        return parse_source(buffer.str());
    }

    bool ShaderReflection::parse_source(const std::string& glsl_source)
    {
        TokenStream ts{strip_comments(glsl_source)};

        // Second pass: uniforms/samplers that may depend on defines
        ts.pos = 0;
        while (!ts.eof())
        {
            if (ts.match("struct"))
            {
                parse_structs(ts);
            }
            else if (ts.match("layout"))
            {
                std::string set, binding, location;
                bool is_push = false;
                parse_layout_qualifier(ts, set, binding, location, is_push);

                ts.skip_ws();
                if (ts.match("in"))
                {
                    ShaderIO io = parse_io(ts, location);
                    _inputs.push_back(std::move(io));
                }
                else if (ts.match("out"))
                {
                    ShaderIO io = parse_io(ts, location);
                    _outputs.push_back(std::move(io));
                }
                else if (ts.match("uniform"))
                {
                    ts.skip_ws();
                    size_t save = ts.pos;
                    std::string next = ts.read_identifier();
                    ts.pos = save;

                    if (next == "sampler2D" || next.rfind("sampler", 0) == 0 ||
                        next.rfind("texture", 0) == 0 || next.rfind("image", 0) == 0)
                    {
                        parse_sampler(ts, set, binding);
                    }
                    else
                    {
                        parse_uniform_block(ts, set, binding);
                    }
                }
            }
            else
            {
                ts.pos++;
            }
        }
        return true;
    }

    void ShaderReflection::parse_structs(TokenStream& ts)
    {
        ShaderStruct s;
        s.name = ts.read_identifier();
        ts.skip_ws();
        if (ts.match_char('{'))
        {
            ts.pos--; // unread {
            std::string body = ts.read_balanced('{', '}');
            TokenStream body_ts{body};
            s.members = parse_member_list(body_ts);
            ts.match_char(';');
            _structs[s.name] = std::move(s);
        }
    }

    void ShaderReflection::parse_layout_qualifier(TokenStream& ts, std::string& set, std::string& binding, std::string& location, bool& is_push)
    {
        ts.skip_ws();
        if (!ts.match_char('(')) return;
        ts.pos--;  // unread (
        std::string inside = ts.read_balanced('(', ')');

        // parse key=value pairs
        size_t p = 0;
        while (p < inside.size())
        {
            while (p < inside.size() && std::isspace(inside[p])) p++;
            size_t key_start = p;
            while (p < inside.size() && (std::isalnum(inside[p]) || inside[p] == '_')) p++;
            std::string key = inside.substr(key_start, p - key_start);

            while (p < inside.size() && std::isspace(inside[p])) p++;
            if (p < inside.size() && inside[p] == '=')
            {
                p++;
                while (p < inside.size() && std::isspace(inside[p])) p++;
                size_t val_start = p;
                while (p < inside.size() && inside[p]!= ',' && inside[p]!= ')') p++;
                std::string val = inside.substr(val_start, p - val_start);

                if (key == "set") set = resolve_value(val);
                else if (key == "binding") binding = resolve_value(val);
                else if (key == "location") location = resolve_value(val);
            }
            else if (key == "push_constant")
            {
                is_push = true;
            }
            else if (key == "std140" || key == "std430")
            {
                // ignore for now
            }
            if (p < inside.size() && inside[p] == ',') p++;
        }
    }

    std::vector<ShaderMember> ShaderReflection::parse_member_list(TokenStream& ts)
    {
        std::vector<ShaderMember> members;
        while (!ts.eof()) {
            ts.skip_ws();
            std::string type = ts.read_identifier();
            if (type.empty()) break;
            std::string name = ts.read_identifier();
            if (name.empty()) break;

            ShaderMember m;
            m.type = type;
            m.name = name;

            ts.skip_ws();
            if (ts.match_char('[')) {
                std::string arr = ts.read_balanced('[', ']');
                m.array_expr = arr;
                m.array_size = resolve_int(arr);
            }
            ts.match_char(';');
            members.push_back(std::move(m));
        }
        return members;
    }

    void ShaderReflection::parse_uniform_block(TokenStream& ts, std::string set, std::string binding)
    {
        ShaderResource res;
        res.type = ShaderResourceType::UniformBuffer;
        res.set = set;
        res.binding = binding;
        res.block_name = ts.read_identifier();
        res.struct_type = res.block_name;

        ts.skip_ws();
        if (ts.match_char('{')) {
            ts.pos--;
            std::string body = ts.read_balanced('{', '}');
            TokenStream body_ts{body};
            res.members = parse_member_list(body_ts);
            res.name = ts.read_identifier(); // instance name
            ts.match_char(';');
            _resources.push_back(std::move(res));
        }
    }

    void ShaderReflection::parse_sampler(TokenStream& ts, std::string set, std::string binding)
    {
        ShaderResource res;
        res.type = ShaderResourceType::Sampler;
        res.set = set;
        res.binding = binding;
        res.struct_type = ts.read_identifier(); // sampler2D
        res.name = ts.read_identifier(); // u_albedo
        ts.match_char(';');
        _resources.push_back(std::move(res));
    }

    ShaderIO ShaderReflection::parse_io(TokenStream& ts, std::string& location)
    {
        ts.skip_ws();
        std::string type = ts.read_identifier();
        if (type.empty()) return {};
        std::string name = ts.read_identifier();

        ShaderIO io;
        io.location = location;
        io.type = type;
        io.name = name;
        ts.match_char(';');
        return io;
    }

    void ShaderReflection::clear() {
        _structs.clear();
        _resources.clear();
        _defines_found.clear();
        // keep _define_map because external defines should persist
    }

    // --- TokenStream helpers ---

    std::string ShaderReflection::TokenStream::read_line() {
        size_t start = pos;
        while (pos < src.size() && src[pos]!= '\n') pos++;
        std::string line = src.substr(start, pos - start);
        if (pos < src.size()) pos++; // skip \n
        return line;
    }

    void ShaderReflection::TokenStream::skip_ws()
    {
        while (pos < src.size() && std::isspace(src[pos])) pos++;
    }

    bool ShaderReflection::TokenStream::match(const char* str)
    {
        skip_ws();
        size_t len = std::strlen(str);
        if (pos + len > src.size()) return false;
        if (src.compare(pos, len, str) == 0) {
            pos += len;
            return true;
        }
        return false;
    }

    bool ShaderReflection::TokenStream::match_char(char c)
    {
        skip_ws();
        if (pos < src.size() && src[pos] == c)
        {
            pos++;
            return true;
        }
        return false;
    }

    std::string ShaderReflection::TokenStream::read_identifier()
    {
        skip_ws();
        size_t start = pos;
        if (pos < src.size() && (std::isalpha(src[pos]) || src[pos] == '_'))
        {
            pos++;
            while (pos < src.size() && (std::isalnum(src[pos]) || src[pos] == '_'))
            {
                pos++;
            }
            return src.substr(start, pos - start);
        }
        return "";
    }

    std::string ShaderReflection::TokenStream::read_until(char end)
    {
        size_t start = pos;
        while (pos < src.size() && src[pos]!= end) pos++;
        return src.substr(start, pos - start);
    }

    std::string ShaderReflection::TokenStream::read_balanced(char open, char close)
    {
        skip_ws();
        if (pos >= src.size() || src[pos]!= open) return "";
        pos++; // skip open
        int depth = 1;
        size_t start = pos;
        while (pos < src.size() && depth > 0)
        {
            if (src[pos] == open) depth++;
            else if (src[pos] == close) depth--;
            pos++;
        }
        return src.substr(start, pos - start - 1); // exclude final close
    }

} // namespace kege
