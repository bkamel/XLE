// Distributed under the MIT License (See
// accompanying file "LICENSE" or the website
// http://www.opensource.org/licenses/mit-license.php)

#include "ShaderInstantiation.h"
#include "GraphSyntax.h"
#include "../Utility/Streams/PathUtils.h"
#include "../Utility/StringUtils.h"
#include "../Utility/StringFormat.h"
#include <set>
#include <stack>
#include <regex>
#include <sstream>

namespace ShaderPatcher
{
	static std::string MakeGraphName(const std::string& baseName, uint64_t instantiationHash = 0)
    {
        if (!instantiationHash) return baseName;
        return baseName + "_" + std::to_string(instantiationHash);
    }

	std::vector<std::string> InstantiateShader(
		StringSection<> entryFile,
		StringSection<> entryFn,
		const ShaderPatcher::InstantiationParameters& instantiationParameters)
	{
		return InstantiateShader(
			ShaderPatcher::LoadGraph(entryFile, entryFn),
			instantiationParameters);
	}

	std::vector<std::string> InstantiateShader(
		const INodeGraphProvider::NodeGraph& initialGraph,
		const ShaderPatcher::InstantiationParameters& instantiationParameters)
	{
		std::set<std::string> includes;

		struct PendingInstantiation
		{
			ShaderPatcher::INodeGraphProvider::NodeGraph _graph;
			ShaderPatcher::InstantiationParameters _instantiationParams;
		};

        std::vector<std::string> fragments;
		std::stack<PendingInstantiation> instantiations;
		instantiations.emplace(PendingInstantiation{initialGraph, instantiationParameters});

		bool rootInstantiation = true;
        while (!instantiations.empty()) {
            auto inst = std::move(instantiations.top());
            instantiations.pop();

			// Slightly different rules for function name generation with inst._scope is not null. inst._scope is
			// only null for the original instantiation request -- in that case, we want the outer most function
			// to have the same name as the original request
			auto scaffoldName = MakeGraphName(inst._graph._name, inst._instantiationParams.CalculateHash());
			if (rootInstantiation) {
				scaffoldName = inst._graph._name;
				rootInstantiation = false;
			}
			auto implementationName = scaffoldName + "_impl";
			auto instFn = ShaderPatcher::GenerateFunction(
				inst._graph._graph, implementationName, 
				inst._instantiationParams, *inst._graph._subProvider);

			fragments.push_back(ShaderPatcher::GenerateScaffoldFunction(inst._graph._signature, instFn._signature, scaffoldName, implementationName));
			fragments.push_back(instFn._text);
                
			for (const auto&dep:instFn._dependencies._dependencies) {

				std::string filename = dep._archiveName;

				static std::regex archiveNameRegex(R"--(([\w\.\\/]+)::(\w+))--");
				std::smatch archiveNameMatch;
				if (std::regex_match(dep._archiveName, archiveNameMatch, archiveNameRegex) && archiveNameMatch.size() >= 3) {
					filename = archiveNameMatch[1].str();
				}

				// if it's a graph file, then we must create a specific instantiation
				if (XlEqString(MakeFileNameSplitter(filename).Extension(), "graph")) {
					instantiations.emplace(
						PendingInstantiation{inst._graph._subProvider->FindGraph(dep._archiveName).value(), dep._parameters});
				} else {
					// This is just an include of a normal shader header
					if (!inst._instantiationParams._parameterBindings.empty()) {
						includes.insert(std::string(StringMeld<MaxPath>() << filename + "_" << inst._instantiationParams.CalculateHash()));
					} else {
						auto sig = inst._graph._subProvider->FindSignature(dep._archiveName);
						includes.insert(sig.value()._sourceFile);
					}
				}
			}
        }

		{
			std::stringstream str;
			for (const auto&i:includes)
				str << "#include <" << i << ">" << std::endl;
			fragments.push_back(str.str());
		}

		return fragments;
	}
}