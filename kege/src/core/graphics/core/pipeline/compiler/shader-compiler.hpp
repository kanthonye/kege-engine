//
//  shader-compiler.hpp
//  assets
//
//  Created by Kenneth Esdaile on 2/11/26.
//

#ifndef kege_shader_compiler_hpp
#define kege_shader_compiler_hpp

#include <string>
#include <vector>
#include <map>
#include <set>
#include <unordered_map>
#include <variant>
#include <memory>
#include <optional>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cctype>

#include "../../../../graphics/core/device/core/instance.hpp"

#include "shader.hpp"
#include "shader-common.hpp"
#include "shader-reflection.hpp"
#include "shader-pipeline.hpp"

namespace kege{

    class ShaderCompiler
    {
    private:

        struct VertexInfo
        {
            kege::VertexBit semantic;
            int location;
        };

        struct Define
        {
            std::string name;
            int value;
        };

        struct Defines
        {
            std::map< std::string, size_t > table;
            std::vector< Define > defs;
        };

        struct Stage
        {
            std::set<std::string> defined_sets;
            std::map< std::string, int > defines;
            std::vector< ShaderIO > output;
            std::vector< ShaderIO > input;
            ShaderStageFlag type;
        };

    public:

        void addDefine(Defines& defines, const std::string& name, int index = 1);
        ref::ShaderPipeline compileVariant(const kege::PipelineKey& key);
        void getVariant(const kege::PipelineKey& key);
        ShaderCompiler(kege::GraphicsDevice* graphics, const std::string& filepath);

    private:

        ref::ShaderPipeline compileShader(const kege::PipelineKey& key, const std::string& vs, const std::string& fs);


        void addAttribute(kege::VertexBit semantic, int location, kege::ShaderVar type, const char* name );

        void beginVertexShader(std::stringstream& source);
        void endVertexShader(std::stringstream& source);
        void beginFragmentShader(std::stringstream& source);
        void endFragmentShader(std::stringstream& source);

        void writeResource(std::stringstream& source, const std::string& fname);
        void writeOutputs(std::stringstream& source, const std::string& fname);
        void writeInputs(std::stringstream& source, const std::string& fname);

        void writeVersion(std::stringstream& source);
        void writeExtensions(std::stringstream& source);
        //void writeMacros(std::stringstream& source);
        void processOutput(std::stringstream& source);
        void processInput(std::stringstream& source);
        void processResources(std::stringstream& source);
        void writeFunctions(std::stringstream& source);
        void writeMainFn(std::stringstream& source);

        void writeCameraData(std::stringstream& source);
        void writeObjectData(std::stringstream& source);

        std::string writeVertexShader();
        std::string writeFragmentShader();

        std::string include(const std::string& fname );

        void define(std::stringstream& source, const std::string& name, int index = 1);


        int resolveDefnInt(Stage* stage, const std::string& str);
        kege::VertexBit resolveVertexBitType( const std::string& str );

        bool checkFeatures(uint64_t signature) { return (_key.features & signature); }
        bool hasInput(int signature);
        void writeFeatures();

    private:

        std::string _filepath;

        std::map< kege::ShaderStageFlag, Stage > _stages;
        Stage* _curr_stage;


        struct SetMeta
        {
            int set;
            std::map< std::string, uint32_t > bindings;
        };

        std::map< std::string, SetMeta > _set_meta_map;

        //std::map< std::string, Pair > _set_qualifiers;
        std::map<int, kege::ShaderSetDesc> _bind_set_descs;
        kege::PushBlockDescs _push_blocks;

        std::map< std::string, int > _io_defs;
        std::vector< std::string > _output_fnames;

        std::map< std::string, kege::VertexBit > _vertex_bit_string_map;

        Defines _global_defines;

        kege::PipelineKey _key;

        kege::GraphicsDevice* _graphics;

        int _output_location = 0;
        int _input_location = 0;
        int _set_count = 0;

        bool _has_lights;
        bool _has_material;
        bool _has_light_funct;
        bool _has_normal_mapping;
        int _input_signature;
    };
}
#endif /* kege_shader_compiler_hpp */
