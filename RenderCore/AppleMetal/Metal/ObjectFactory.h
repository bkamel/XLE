//
//  ObjectFactory.h
//  RenderCore_AppleMetal
//
//  Created by Ken Domke on 2/13/18.
//

#pragma once

#include "../../../Externals/Misc/OCPtr.h"

@protocol MTLDevice;
@protocol MTLTexture;
@protocol MTLBuffer;
@class MTLTextureDescriptor;

/* KenD -- could switch all of these typedefs from NSObject with a protocol to simply `id`
 * However, cannot have `OCPtr<id<MTLTexture>>` because OCPtr will not work; ObjType will be `id<MTLTexture> *`, which is
 * off because `id` is already a pointer.  `OCPtr<NSObject<MTLTexture>>` is fine because ObjType will be `NSObject<MTLTexture> *`, which is fine.
 * `OCPtr<id>` is also fine.
 */
typedef NSObject<MTLTexture> AplMtlTexture;
typedef NSObject<MTLBuffer> AplMtlBuffer;
typedef NSObject<MTLDevice> AplMtlDevice;

namespace RenderCore { class IDevice; }

namespace RenderCore { namespace Metal_AppleMetal
{
    /* KenD -- void* instead for RawMTLHandle? */
    using RawMTLHandle = uint64_t;
    static const RawMTLHandle RawMTLHandle_Invalid = 0;

    class ObjectFactory
    {
    public:
        TBC::OCPtr<AplMtlTexture> CreateTexture(MTLTextureDescriptor* textureDesc); // <MTLTexture>
        TBC::OCPtr<AplMtlBuffer> CreateBuffer(const void* bytes, unsigned length); // <MTLBuffer>

        ObjectFactory(id<MTLDevice> mtlDevice);
        ObjectFactory() = delete;
        ~ObjectFactory();

        ObjectFactory& operator=(const ObjectFactory&) = delete;
        ObjectFactory(const ObjectFactory&) = delete;
    private:
        TBC::OCPtr<AplMtlDevice> _mtlDevice; // <MTLDevice>
    };

    class DeviceContext;

    ObjectFactory& GetObjectFactory(IDevice& device);
    ObjectFactory& GetObjectFactory(DeviceContext&);
    ObjectFactory& GetObjectFactory();
}}