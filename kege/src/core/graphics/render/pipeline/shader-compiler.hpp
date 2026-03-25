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

#include "../core/graphics-core.hpp"
#include "pipeline-key.hpp"

namespace kege{

    class ShaderProgram;


    class ShaderCompiler
    {
    public:

        ShaderProgram* compileVariant(const kege::PipelineKey& key);
        ShaderProgram* getVariant(const kege::PipelineKey& key);

    private:

        struct VertexInfo
        {
            enum Semantic
            {
                POSITION,
                TEXCOORD,
                NORMAL,
                TANGENT,
                BITANGENT,
                COLOR,
                JOINTS,
                WEIGHTS,
            };

            Semantic semantic;
            int location;
        };

        std::string writeVertexShader(const kege::PipelineKey& key, const std::vector<std::string>& global_defines, std::vector<VertexInfo>& outputs);

        std::string writeFragmentShader(const kege::PipelineKey& key, const std::vector<VertexInfo>& inputs, const std::vector<std::string>& global_defines);

        ShaderProgram* compileShader(const kege::PipelineKey& key, const std::string& vs, const std::string& fs);

        std::string include(const std::string& fname );

        void defineVertexAttributeIO(const kege::PipelineKey& key, const std::vector<VertexInfo>& attributes, std::vector<std::string>& defines);

        void buildVertexLayout(const kege::PipelineKey& key, std::vector<VertexInfo>& attributes);

        void writeFragmentShaderInput(std::stringstream& source, const std::vector<VertexInfo>& inputs);

        void writeFragmentShaderIO(std::stringstream& source, const kege::PipelineKey& key);

        void writeVertexShaderIO(std::stringstream& source, const kege::PipelineKey& key);

        void writeCameraData(std::stringstream& source, const kege::PipelineKey& key);

        void writeObjectData(std::stringstream& source, const kege::PipelineKey& key);

        void writeDefines(std::stringstream& source, const std::vector<std::string>& defines);

        void writeVertexProcessingSelection(std::stringstream& source, const kege::PipelineKey& key);

        void writeLightingSelection(std::stringstream& source, const kege::PipelineKey& key);

        void writeVertexShaderMainFunction(std::stringstream& source, const kege::PipelineKey& key);

        void writeFragmentForwardOpaque(std::stringstream& source, const kege::PipelineKey& key);

        void writeFragmentShaderMainFunction(std::stringstream& source, const kege::PipelineKey& key);

        void writeVersionAndExtensions(std::stringstream& source);

        void writeFeatures(const kege::PipelineKey& key, std::vector<std::string>& defines);

    private:

        kege::Graphics* _graphics;
        int binding_index = 0;
        int set_index = 0;
    };
}
#endif /* kege_shader_compiler_hpp */
