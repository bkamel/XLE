// Distributed under the MIT License (See
// accompanying file "LICENSE" or the website
// http://www.opensource.org/licenses/mit-license.php)

#pragma once

#include "../IDevice.h"
#include "../Metal/Forward.h"
#include "../../Utility/VariantUtils.h"
#include "../../Utility/IteratorUtils.h"
#include <vector>
#include <memory>
#include <string>

namespace Utility { class ParameterBox; }
namespace RenderCore { class IThreadContext; class MiniInputElementDesc; class UniformsStreamInterface; }

namespace RenderCore { namespace Techniques
{
	class DrawablesPacket;
	class ParsingContext;
	class IUniformBufferDelegate;
	class IShaderResourceDelegate;
	class IMaterialDelegate;
	class ITechniqueDelegate;

	class SequencerTechnique
	{
	public:
		std::vector<std::pair<uint64_t, std::shared_ptr<IUniformBufferDelegate>>> _sequencerUniforms;
		std::vector<std::shared_ptr<IShaderResourceDelegate>> _sequencerResources;

		std::shared_ptr<IMaterialDelegate> _materialDelegate;
		std::shared_ptr<ITechniqueDelegate> _techniqueDelegate;
	};

	class IMaterialContext
	{
	public:
		virtual ~IMaterialContext();
	};

	class DrawableGeo
    {
    public:
        class VertexStream
        {
        public:
            IResourcePtr	_resource;
            std::vector<MiniInputElementDesc> _vertexElements;
            unsigned		_vertexStride = 0u;
            uint64_t		_vertexElementsHash = 0ull;
            unsigned		_vbOffset = 0u;
            unsigned		_dynVBEnd = 0u;
            unsigned		_instanceStepDataRate = 0u;
        };
        VertexStream        _vertexStreams[4];
        unsigned            _vertexStreamCount = 0;

        IResourcePtr		_ib;
        Format				_ibFormat = Format(0);
        unsigned			_dynIBBegin = ~0u;
        unsigned			_dynIBEnd = 0u;

        struct Flags
        {
            enum Enum { Temporary       = 1 << 0 };
            using BitField = unsigned;
        };
        Flags::BitField     _flags = 0u;
    };

	class Drawable
	{
	public:
		std::string							_techniqueConfig;
        std::shared_ptr<IMaterialContext>	_material;
        std::shared_ptr<DrawableGeo>        _geo;

        typedef void (ExecuteDrawFn)(
            Metal::DeviceContext&,
			ParsingContext& parserContext,
            const Drawable&, const Metal::BoundUniforms&,
            const Metal::ShaderProgram&);
        ExecuteDrawFn*						_drawFn;

        std::shared_ptr<UniformsStreamInterface>  _uniformsInterface;
	};

	void Draw(
		IThreadContext& context,
        Techniques::ParsingContext& parserContext,
		unsigned techniqueIndex,
		const SequencerTechnique& sequencerTechnique,
		const ParameterBox* seqShaderSelectors,
		const Drawable& drawable);

}}