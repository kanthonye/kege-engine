//
//  shader-reflection.hpp
//  editor
//
//  Created by Kenneth Esdaile on 4/11/26.
//

#ifndef shader_reflection_hpp
#define shader_reflection_hpp

#include <stdio.h>
#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <cstdint>

namespace kege {

    enum class ShaderResourceType {
        UniformBuffer,
        StorageBuffer,
        Sampler,
        SampledImage,
        StorageImage,
        PushConstant
    };

    struct ShaderMember {
        std::string type;
        std::string name;
        std::string array_expr;
        
        uint32_t array_size;

        uint32_t offset = 0;
        uint32_t size = 0;
        uint32_t alignment = 0;
        uint32_t stride = 0;
    };

    struct ShaderIO {
        std::string type;
        std::string name;
        std::string location;
    };

    struct ShaderStruct {
        std::string name;
        std::vector<ShaderMember> members;
        uint32_t size = 0;
    };

    struct ShaderResource {
        ShaderResourceType type;
        std::string name;
        std::string block_name;
        std::string struct_type;
        std::string set;
        std::string binding;
        std::string count;
        std::vector<ShaderMember> members;
        uint32_t block_size = 0;
    };

    struct ShaderDefine {
        std::string name;
        std::string value; // raw string, could be "0" or "MAX_LIGHTS"
    };

    class ShaderReflection {
    public:
        // External defines you set before parsing, like MAX_LIGHTS = 8
        void add_define(const std::string& name, uint32_t value);
        void add_define(const std::string& name, const std::string& value);

        bool parse_file(const std::string& filepath);
        bool parse_source(const std::string& glsl_source);

        const std::unordered_map<std::string, ShaderStruct>& get_structs() const { return _structs; }
        const std::vector<ShaderResource>& get_resources() const { return _resources; }
        const std::unordered_map<std::string, std::string>& get_defines() const { return _define_map; } // defines parsed from file
        const std::vector<ShaderIO>& get_outputs() const { return _outputs; }
        const std::vector<ShaderIO>& get_inputs() const { return _inputs; }

        uint32_t resolve_int(const std::string& expr) const;
        void calculate_layouts();
        void clear();

    private:

        struct TokenStream {
            std::string src;
            size_t pos = 0;
            void skip_ws();
            bool match(const char* str);
            bool match_char(char c);
            std::string read_identifier();
            std::string read_until(char end);
            std::string read_balanced(char open, char close);
            std::string read_line();
            bool eof() const { return pos >= src.size(); }
        };

        void parse_preprocessor(TokenStream& ts);
        void parse_structs(TokenStream& ts);
        void parse_layout_qualifier(TokenStream& ts, std::string& set, std::string& binding, std::string& location, bool& is_push);
        void parse_uniform_block(TokenStream& ts, std::string set, std::string binding);
        void parse_sampler(TokenStream& ts, std::string set, std::string binding);
        ShaderIO parse_io(TokenStream& ts, std::string& location);
        std::vector<ShaderMember> parse_member_list(TokenStream& ts);

        // Add these to ShaderReflection class
        uint32_t align_to(uint32_t offset, uint32_t alignment) const;
        uint32_t get_base_alignment(const std::string& type) const;
        uint32_t get_type_size(const std::string& type) const;
        void layout_struct_members(std::vector<ShaderMember>& members, uint32_t& current_offset);

        std::string resolve_value(const std::string& expr) const;
        std::string strip_comments(const std::string& src) const;

//        uint32_t get_base_alignment(const std::string& type) const;
//        uint32_t get_type_size(const std::string& type) const;
//        uint32_t align_to(uint32_t offset, uint32_t alignment) const;
//        void layout_struct_members(std::vector<ShaderMember>& members, uint32_t& current_offset);

    private:

        std::unordered_map<std::string, ShaderStruct> _structs;
        std::vector<ShaderResource> _resources;
        std::vector<ShaderIO> _outputs;
        std::vector<ShaderIO> _inputs;
        std::unordered_map<std::string, std::string> _define_map; // name -> value, for resolution
        std::vector<ShaderDefine> _defines_found; // defines we parsed from the file
    };

} // namespace kege
#endif /* shader_reflection_hpp */
