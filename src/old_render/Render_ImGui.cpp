/*********************************************************************
* Copyright (C) Anton Kovalev (vertver), 2022-2023. All rights reserved.
* nfrage - engine code for NFRage project
**********************************************************************
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation; either
* version 2.1 of the License, or (at your option) any later version.
* 
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Lesser General Public License for more details.
* 
* You should have received a copy of the GNU Lesser General Public
* License along with this library; if not, write to the Free 
* Software Foundation, Inc., 51 Franklin Street, Fifth Floor, 
* Boston, MA 02110-1301 USA
*****************************************************************/
#include "RenderPch.h"

namespace nfr::render::imgui
{

    struct VERTEX_CONSTANT_BUFFER {
        float   mvp[4][4];
    };

    struct ImGui_ContextNRI_Data {
        nri::CommandBuffer*     ui_commandBuffer                = nullptr;
        nri::Texture*           ui_FontTexture                  = nullptr;
        nri::Memory*            ui_FontTextureMemory            = nullptr;
        nri::Descriptor*        ui_FontShaderResourceView       = nullptr;
        nri::DescriptorPool*    ui_DescriptorPool               = nullptr;
        nri::DescriptorSet*     ui_DescriptorSets               = nullptr;
        nri::Buffer*            ui_IndexAndVertexBuffer         = nullptr;
        nri::Memory*            ui_IndexAndVertexBufferMemory   = nullptr;
        size_t                  ui_IndexAndVertexBufferSize     = 0;
    };

    nri::PipelineLayout*    ui_pipelineLayout  = nullptr;
    nri::Pipeline*          ui_pipeline        = nullptr;

    bool inside_CreatePipelineLayout();
    void inside_DestroyPipelineLayout();
    bool inside_CreatePipeline();
    void inside_DestroyPipeline();
    bool inside_CreateFontTexture(ImGuiIO& io, ImGui_ContextNRI_Data* data);
    void inside_DestroyFontTexture(ImGui_ContextNRI_Data* data);
    bool inside_CreateBuffers(ImGui_ContextNRI_Data* data, size_t TargetMemorySize);
    void inside_DestroyBuffers(ImGui_ContextNRI_Data* data);
    

    bool inside_CreatePipelineLayout() {
        nri::StaticSamplerDesc staticSamplerDesc = {};
        staticSamplerDesc.samplerDesc.anisotropy = 1;
        staticSamplerDesc.samplerDesc.addressModes = {nri::AddressMode::REPEAT, nri::AddressMode::REPEAT, nri::AddressMode::REPEAT};
        staticSamplerDesc.samplerDesc.magnification = nri::Filter::LINEAR;
        staticSamplerDesc.samplerDesc.minification = nri::Filter::LINEAR;
        staticSamplerDesc.samplerDesc.mip = nri::Filter::LINEAR;
        staticSamplerDesc.registerIndex = 0;
        staticSamplerDesc.visibility = nri::ShaderStage::FRAGMENT;

        nri::DescriptorRangeDesc descriptorRange = {0, 1, nri::DescriptorType::TEXTURE, nri::ShaderStage::FRAGMENT};

        nri::DescriptorSetDesc descriptorSet = {&descriptorRange, 1, &staticSamplerDesc, 1};

        nri::PushConstantDesc pushConstant = {};
        pushConstant.registerIndex  = 0;
        pushConstant.size           = sizeof(VERTEX_CONSTANT_BUFFER);
        pushConstant.visibility     = nri::ShaderStage::VERTEX;

        nri::PipelineLayoutDesc pipelineLayoutDesc = {};
        pipelineLayoutDesc.descriptorSetNum = 1;
        pipelineLayoutDesc.descriptorSets = &descriptorSet;
        pipelineLayoutDesc.pushConstantNum = 1;
        pipelineLayoutDesc.pushConstants = &pushConstant;
        pipelineLayoutDesc.stageMask = nri::PipelineLayoutShaderStageBits::VERTEX | nri::PipelineLayoutShaderStageBits::FRAGMENT;

        if (NRI.CreatePipelineLayout(*device, pipelineLayoutDesc, ui_pipelineLayout) != nri::Result::SUCCESS) return false;

        return true;
    }

    void inside_DestroyPipelineLayout() {
        if (ui_pipelineLayout) {
            NRI.DestroyPipelineLayout(*ui_pipelineLayout);
            ui_pipelineLayout = nullptr;
        }
    }

    bool inside_CreatePipeline() {
        nri::GraphicsPipelineDesc graphicsPipelineDesc = {};

        //
        graphicsPipelineDesc.pipelineLayout = ui_pipelineLayout;

        //
        nri::ShaderDesc shaderStages[2];
        if (!nfr::render::shaders::FindAndLoadShaderBinary("imgui_vs", shaderStages[0])) return false;
        if (!nfr::render::shaders::FindAndLoadShaderBinary("imgui_ps", shaderStages[1])) return false;
        graphicsPipelineDesc.shaderStages   = shaderStages;
        graphicsPipelineDesc.shaderStageNum = std::size(shaderStages);

        //
        nri::VertexStreamDesc vertexStreamDesc = {};
        vertexStreamDesc.bindingSlot = 0;
        vertexStreamDesc.stride = sizeof(ImDrawVert);

        //
        nri::VertexAttributeDesc vertexAttributeDesc[3] = {};
        {
            vertexAttributeDesc[0].format = nri::Format::RG32_SFLOAT;
            vertexAttributeDesc[0].streamIndex = 0;
            vertexAttributeDesc[0].offset = (std::uint32_t)IM_OFFSETOF(ImDrawVert, pos);
            vertexAttributeDesc[0].d3d = {"POSITION", 0};
            vertexAttributeDesc[0].vk = {0};

            vertexAttributeDesc[1].format = nri::Format::RG32_SFLOAT;
            vertexAttributeDesc[1].streamIndex = 0;
            vertexAttributeDesc[1].offset = (std::uint32_t)IM_OFFSETOF(ImDrawVert, uv);
            vertexAttributeDesc[1].d3d = {"TEXCOORD", 0};
            vertexAttributeDesc[1].vk = {1};

            vertexAttributeDesc[2].format = nri::Format::RGBA8_UNORM;
            vertexAttributeDesc[2].streamIndex = 0;
            vertexAttributeDesc[2].offset = (std::uint32_t)IM_OFFSETOF(ImDrawVert, col);
            vertexAttributeDesc[2].d3d = {"COLOR", 0};
            vertexAttributeDesc[2].vk = {2};
        }

        //
        nri::InputAssemblyDesc inputAssemblyDesc = {};
        inputAssemblyDesc.topology          = nri::Topology::TRIANGLE_LIST;
        inputAssemblyDesc.attributes        = vertexAttributeDesc;
        inputAssemblyDesc.attributeNum      = (uint8_t)std::size(vertexAttributeDesc);
        inputAssemblyDesc.streams           = &vertexStreamDesc;
        inputAssemblyDesc.streamNum         = 1;

        graphicsPipelineDesc.inputAssembly  = &inputAssemblyDesc;

        //
        nri::RasterizationDesc rasterizationDesc = {};
        rasterizationDesc.viewportNum       = 1;
        rasterizationDesc.fillMode          = nri::FillMode::SOLID;
        rasterizationDesc.cullMode          = nri::CullMode::NONE;
        rasterizationDesc.sampleMask        = 0xFFFF;
        rasterizationDesc.sampleNum         = 1;
        rasterizationDesc.depthClamp       = true; // depthclipenable

        graphicsPipelineDesc.rasterization = &rasterizationDesc;

        //
        nri::ColorAttachmentDesc colorAttachmentDesc = {};
        colorAttachmentDesc.format          = swapchain_Format; // rt format
        colorAttachmentDesc.blendEnabled    = true;
        colorAttachmentDesc.colorBlend      = {nri::BlendFactor::SRC_ALPHA, nri::BlendFactor::ONE_MINUS_SRC_ALPHA, nri::BlendFunc::ADD};
        colorAttachmentDesc.alphaBlend      = {nri::BlendFactor::ONE_MINUS_SRC_ALPHA, nri::BlendFactor::ZERO, nri::BlendFunc::ADD};
        colorAttachmentDesc.colorWriteMask  = nri::ColorWriteBits::RGBA;

        nri::OutputMergerDesc outputMergerDesc = {};
        outputMergerDesc.colorNum           = 1;// rt count
        outputMergerDesc.color              = &colorAttachmentDesc;

        graphicsPipelineDesc.outputMerger = &outputMergerDesc;

        if (NRI.CreateGraphicsPipeline(*device, graphicsPipelineDesc, ui_pipeline) != nri::Result::SUCCESS)  return false;

        return true;
    }

    void inside_DestroyPipeline() {
        if (ui_pipeline) {
            NRI.DestroyPipeline(*ui_pipeline);
            ui_pipeline = nullptr;
        }
    }

    bool inside_CreateFontTexture(ImGuiIO& io, ImGui_ContextNRI_Data* data) {
        // get size
        int32_t fontTextureWidth = 0, fontTextureHeight = 0;
        uint8_t* fontPixels = nullptr;
        io.Fonts->GetTexDataAsRGBA32(&fontPixels, &fontTextureWidth, &fontTextureHeight);

        if (!fontPixels || !fontTextureWidth || !fontTextureHeight) return false;

        // resource
        nri::TextureDesc textureDesc = {};
        textureDesc.type        = nri::TextureType::TEXTURE_2D;
        textureDesc.usageMask   = nri::TextureUsageBits::SHADER_RESOURCE;
        textureDesc.format      = nri::Format::RGBA8_UNORM;
        textureDesc.size[0]     = (uint16_t)fontTextureWidth;
        textureDesc.size[1]     = (uint16_t)fontTextureHeight;
        textureDesc.size[2]     = 1;
        textureDesc.mipNum      = 1;
        textureDesc.arraySize   = 1;
        textureDesc.sampleNum   = 1;
        if (NRI.CreateTexture(*device, textureDesc, data->ui_FontTexture) != nri::Result::SUCCESS) 
            return false;
        
        // resource memory
        nri::ResourceGroupDesc resourceGroupDesc = {};
        resourceGroupDesc.memoryLocation    = nri::MemoryLocation::DEVICE;
        resourceGroupDesc.textureNum        = 1;
        resourceGroupDesc.textures          = &data->ui_FontTexture;
        if (NRI.AllocateAndBindMemory(*device, resourceGroupDesc, &data->ui_FontTextureMemory) != nri::Result::SUCCESS) 
            return false;
        
        // srv
        nri::Texture2DViewDesc texture2DViewDesc = {data->ui_FontTexture, nri::Texture2DViewType::SHADER_RESOURCE_2D, textureDesc.format};
        if (NRI.CreateTexture2DView(texture2DViewDesc, data->ui_FontShaderResourceView) != nri::Result::SUCCESS) 
            return false;
        
        // upload data
        nri::TextureSubresourceUploadDesc subresource = {};
        subresource.slices          = fontPixels;
        subresource.sliceNum        = 1;
        subresource.rowPitch        = (fontTextureWidth * 4 + 256 - 1u) & ~(256 - 1u); // align 256 bytes per line
        subresource.slicePitch      = subresource.rowPitch * fontTextureHeight;

        nri::TextureUploadDesc textureData = {};
        textureData.subresources    = &subresource;
        textureData.texture         = data->ui_FontTexture;
        textureData.nextLayout      = nri::TextureLayout::SHADER_RESOURCE;
        textureData.nextAccess      = nri::AccessBits::SHADER_RESOURCE;
        textureData.mipNum          = 1;
        textureData.arraySize       = 1;

        if ( NRI.UploadData(*commandQueue, &textureData, 1, nullptr, 0) != nri::Result::SUCCESS) 
            return false;

        // descriptor pool
        nri::DescriptorPoolDesc descriptorPoolDesc = {};
        descriptorPoolDesc.descriptorSetMaxNum  = 1;
        descriptorPoolDesc.textureMaxNum        = 1;
        descriptorPoolDesc.staticSamplerMaxNum  = 1;

        if (NRI.CreateDescriptorPool(*device, descriptorPoolDesc, data->ui_DescriptorPool) != nri::Result::SUCCESS)
            return false;
        
        // texture descriptor set
        if (NRI.AllocateDescriptorSets(*data->ui_DescriptorPool, *ui_pipelineLayout, 0, &data->ui_DescriptorSets, 1, nri::WHOLE_DEVICE_GROUP, 0) != nri::Result::SUCCESS)
            return false;

        nri::DescriptorRangeUpdateDesc descriptorRangeUpdateDesc = {};
        descriptorRangeUpdateDesc.descriptorNum = 1;
        descriptorRangeUpdateDesc.descriptors   = &data->ui_FontShaderResourceView;
        NRI.UpdateDescriptorRanges(*data->ui_DescriptorSets, nri::WHOLE_DEVICE_GROUP, 0, 1, &descriptorRangeUpdateDesc);
        
        //
        io.Fonts->SetTexID((ImTextureID)data->ui_FontShaderResourceView);

        return true;
    }

    void inside_DestroyFontTexture(ImGui_ContextNRI_Data* data) {
        data->ui_DescriptorSets = nullptr;
        
        if (data->ui_DescriptorPool) {
            NRI.DestroyDescriptorPool(*data->ui_DescriptorPool);
            data->ui_DescriptorPool = nullptr;
        }

        if (data->ui_FontShaderResourceView) {
            NRI.DestroyDescriptor(*data->ui_FontShaderResourceView);
            data->ui_FontShaderResourceView = nullptr;
        }

        if (data->ui_FontTexture) {
            NRI.DestroyTexture(*data->ui_FontTexture);
            data->ui_FontTexture = nullptr;
        }

        if (data->ui_FontTextureMemory) {
            NRI.FreeMemory(*data->ui_FontTextureMemory);
            data->ui_FontTextureMemory = nullptr;
        }
    }

    bool inside_CreateBuffers(ImGui_ContextNRI_Data* data, size_t TargetMemorySize) {
        // resource
        nri::BufferDesc bufferDesc = {};
        bufferDesc.size                     = TargetMemorySize;
        bufferDesc.usageMask                = nri::BufferUsageBits::VERTEX_BUFFER | nri::BufferUsageBits::INDEX_BUFFER;
        if (NRI.CreateBuffer(*device, bufferDesc, data->ui_IndexAndVertexBuffer) != nri::Result::SUCCESS)
            return false;

        // resource memory
        nri::ResourceGroupDesc resourceGroupDesc = {};
        resourceGroupDesc.memoryLocation    = nri::MemoryLocation::HOST_UPLOAD;
        resourceGroupDesc.bufferNum         = 1;
        resourceGroupDesc.buffers           = &data->ui_IndexAndVertexBuffer;

        if ( NRI.AllocateAndBindMemory(*device, resourceGroupDesc, &data->ui_IndexAndVertexBufferMemory) != nri::Result::SUCCESS)
            return false;

        //
        data->ui_IndexAndVertexBufferSize = TargetMemorySize;

        return true;
    }

    void inside_DestroyBuffers(ImGui_ContextNRI_Data* data) {
        if (data->ui_IndexAndVertexBuffer) {
            NRI.DestroyBuffer(*data->ui_IndexAndVertexBuffer);
            data->ui_IndexAndVertexBuffer = nullptr;
        }

        if (data->ui_IndexAndVertexBufferMemory) {
            NRI.FreeMemory(*data->ui_IndexAndVertexBufferMemory);
            data->ui_IndexAndVertexBufferMemory = nullptr;
        }

    }


    bool imgui_Init() {
        if (!inside_CreatePipelineLayout()) return false;
        if (!inside_CreatePipeline()) return false;
        return true;
    }

    void imgui_Free() {
        inside_DestroyPipeline();
        inside_DestroyPipelineLayout();
    }

    bool imgui_Render_Init(ImGuiContext* context) {
        if (!context) return false;

        ImGui_ContextNRI_Data* data = IM_NEW(ImGui_ContextNRI_Data)();
        ImGuiIO& io = context->IO;
        io.BackendRendererUserData  = (void*)data;
        io.BackendRendererName      = "imgui_impl_nri";
        io.BackendFlags             |= ImGuiBackendFlags_RendererHasVtxOffset;

        {
            auto result = NRI.CreateCommandBuffer(*frame_commandAllocator, data->ui_commandBuffer);
            if (result != nri::Result::SUCCESS) {
                nfr::dbg::Error("nri::CreateSwapChain CreateCommandBuffer failed");
                return false;
            }
        }

        return true;
    }

    void imgui_Render_Free(ImGuiContext* context) {
        if (!context) return;
        auto data = (ImGui_ContextNRI_Data*)context->IO.BackendRendererUserData;
        if (!data) return;


        if (data->ui_commandBuffer) {
            NRI.DestroyCommandBuffer(*data->ui_commandBuffer);
            data->ui_commandBuffer = nullptr;
        }

        inside_DestroyBuffers( data );
        inside_DestroyFontTexture( data );
        context->IO.Fonts->SetTexID(0);

        context->IO.BackendRendererName = nullptr;
        context->IO.BackendRendererUserData = nullptr;
        IM_DELETE(data);
    }

    void imgui_Render(ImGuiContext* context) {
        if (!context) 
            return;


        //
        auto data = (ImGui_ContextNRI_Data*)context->IO.BackendRendererUserData;
        if (!data) 
            return;

        if (!data->ui_commandBuffer || !commandQueue) 
            return;

        //
        nri::CommandBuffer* commandBuffer = data->ui_commandBuffer;

        // reload texture
        if (context->IO.Fonts->TexID == nullptr) {
            inside_DestroyFontTexture( data );
            if (!inside_CreateFontTexture(context->IO, data)) return ;
        }

        // get draw data
        ImGuiViewportP* drawviewport    = context->Viewports[0];
        const ImDrawData* drawData      = drawviewport->DrawDataP.Valid ? &drawviewport->DrawDataP : nullptr;

        if (!drawData) 
            return;

        // get total vertex and index count
        auto vertexDataSize         = (drawData->TotalVtxCount + 5000)* sizeof(ImDrawVert);
        auto indexDataSize          = (drawData->TotalIdxCount+ 10000) * sizeof(ImDrawIdx);
        auto vertexDataSizeAligned  = (vertexDataSize + 15) & ~15;
        std::uint64_t indexDataSizeAligned   = (indexDataSize + 15) & ~15;
        auto totalDataSizeAligned   = vertexDataSizeAligned + indexDataSizeAligned;
        if (!totalDataSizeAligned) 
            return;

        if (totalDataSizeAligned > data->ui_IndexAndVertexBufferSize) {
            inside_DestroyBuffers( data );
            inside_CreateBuffers( data, totalDataSizeAligned);
        }

        if (!data->ui_IndexAndVertexBuffer) 
            return;

        // update ib vb data
        uint8_t* indexData  = (uint8_t*)NRI.MapBuffer(*data->ui_IndexAndVertexBuffer, 0, totalDataSizeAligned);
        uint8_t* vertexData = indexData + indexDataSizeAligned;

        for (int32_t n = 0; n < drawData->CmdListsCount; n++) {
            const ImDrawList& drawList = *drawData->CmdLists[n];

            uint32_t size = drawList.VtxBuffer.Size * sizeof(ImDrawVert);
            memcpy(vertexData, drawList.VtxBuffer.Data, size);
            vertexData += size;

            size = drawList.IdxBuffer.Size * sizeof(ImDrawIdx);
            memcpy(indexData, drawList.IdxBuffer.Data, size);
            indexData += size;
        }

        NRI.UnmapBuffer(*data->ui_IndexAndVertexBuffer);

        // update matrix
        VERTEX_CONSTANT_BUFFER vertex_constant_buffer;
        {

            float L = drawData->DisplayPos.x;
            float R = drawData->DisplayPos.x + drawData->DisplaySize.x;
            float T = drawData->DisplayPos.y;
            float B = drawData->DisplayPos.y + drawData->DisplaySize.y;
            float mvp[4][4] =
            {
                { 2.0f/(R-L),   0.0f,           0.0f,       0.0f },
                { 0.0f,         2.0f/(T-B),     0.0f,       0.0f },
                { 0.0f,         0.0f,           0.5f,       0.0f },
                { (R+L)/(L-R),  (T+B)/(B-T),    0.5f,       1.0f },
            };

            memcpy(&vertex_constant_buffer.mvp, mvp, sizeof(mvp));
        }

        //
        NRI.BeginCommandBuffer(*commandBuffer, nullptr, 0);

        NRI.CmdBeginRenderPass(*commandBuffer, *backbuffer_current->framebuffer, nri::RenderPassBeginFlag::SKIP_FRAME_BUFFER_CLEAR);

        //
        NRI.CmdSetDescriptorPool(*commandBuffer, *data->ui_DescriptorPool);
        NRI.CmdSetPipelineLayout(*commandBuffer, *ui_pipelineLayout);
        NRI.CmdSetPipeline(*commandBuffer, *ui_pipeline);

        NRI.CmdSetConstants(*commandBuffer, 0, &vertex_constant_buffer, sizeof(vertex_constant_buffer));
        NRI.CmdSetIndexBuffer(*commandBuffer, *data->ui_IndexAndVertexBuffer, 0, sizeof(ImDrawIdx) == 2 ? nri::IndexType::UINT16 : nri::IndexType::UINT32);
        NRI.CmdSetVertexBuffers(*commandBuffer, 0, 1, &data->ui_IndexAndVertexBuffer, &indexDataSizeAligned);
        NRI.CmdSetDescriptorSets(*commandBuffer, 0, 1, &data->ui_DescriptorSets, nullptr);

        const nri::Viewport viewport = { 0.0f, 0.0f, context->IO.DisplaySize.x, context->IO.DisplaySize.y, 0.0f, 1.0f };
        NRI.CmdSetViewports(*commandBuffer, &viewport, 1);

        int32_t vertexOffset = 0;
        int32_t indexOffset = 0;
        for (int32_t n = 0; n < drawData->CmdListsCount; n++) {
            const ImDrawList& drawList = *drawData->CmdLists[n];
            for (int32_t i = 0; i < drawList.CmdBuffer.Size; i++) {
                const ImDrawCmd& drawCmd = drawList.CmdBuffer[i];
                if (drawCmd.UserCallback)
                    drawCmd.UserCallback(&drawList, &drawCmd);
                else  {
                    nri::Rect rect = {
                        (int32_t)drawCmd.ClipRect.x,
                        (int32_t)drawCmd.ClipRect.y,
                        (uint32_t)(drawCmd.ClipRect.z - drawCmd.ClipRect.x),
                        (uint32_t)(drawCmd.ClipRect.w - drawCmd.ClipRect.y)
                    };
                    NRI.CmdSetScissors(*commandBuffer, &rect, 1);

                    NRI.CmdDrawIndexed(*commandBuffer, drawCmd.ElemCount, 1, indexOffset, vertexOffset, 0);
                }
                indexOffset += drawCmd.ElemCount;
            }
            vertexOffset += drawList.VtxBuffer.Size;
        }

        //
        NRI.CmdEndRenderPass(*commandBuffer);
        NRI.EndCommandBuffer(*commandBuffer);

        //
        nri::WorkSubmissionDesc workSubmissionDesc = {};
        workSubmissionDesc.commandBufferNum     = 1;
        workSubmissionDesc.commandBuffers       = &commandBuffer;
        workSubmissionDesc.wait                 = &backbuffer_frame_semaphore;
        workSubmissionDesc.waitNum              = 1;
        workSubmissionDesc.signal               = &backbuffer_frame_semaphore;
        workSubmissionDesc.signalNum            = 1;

        NRI.SubmitQueueWork(*commandQueue, workSubmissionDesc, nullptr);
    }



}
