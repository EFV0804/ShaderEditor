#include "GraphicsPipeline.h"
#include "Renderer.h"
#include "Mesh.h"

GraphicsPipeline::GraphicsPipeline(std::vector<vk::PipelineShaderStageCreateInfo> &stages, std::vector<vk::PushConstantRange>& pushConstants) :
pipelineLayout(),
graphicsPipeline()
{
    createGraphicsPipeline(stages, pushConstants);
}


void GraphicsPipeline::createGraphicsPipeline(std::vector<vk::PipelineShaderStageCreateInfo> &stages,
                                              std::vector<vk::PushConstantRange> pushConstants)
{

    // Get Vertex State Input Info
    VertexInputDescription descriptions = Vertex::getVertexInputDescription();

    vk::PipelineVertexInputStateCreateInfo vertexInputInfo;
    vertexInputInfo.pVertexAttributeDescriptions = descriptions.attributesDescriptions.data();
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(descriptions.attributesDescriptions.size());
    vertexInputInfo.pVertexBindingDescriptions = descriptions.bindingsDescriptions.data();
    vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(descriptions.bindingsDescriptions.size());

    // Get Input Assembly State Info
    vk::PipelineInputAssemblyStateCreateInfo inputAssemblyInfo = {};
    inputAssemblyInfo.sType = vk::StructureType::ePipelineInputAssemblyStateCreateInfo;
    inputAssemblyInfo.pNext = nullptr;
    inputAssemblyInfo.topology = vk::PrimitiveTopology::eTriangleList;
    inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

    // Get Rasterization
    vk::PolygonMode polygonMode = vk::PolygonMode::eFill;

    vk::PipelineRasterizationStateCreateInfo rasterization = {};
    rasterization.sType = vk::StructureType::ePipelineRasterizationStateCreateInfo;
    rasterization.pNext = nullptr;
    rasterization.depthClampEnable = VK_FALSE;
    rasterization.rasterizerDiscardEnable = VK_FALSE;
    rasterization.polygonMode = polygonMode;
    rasterization.lineWidth = 1.0f;
    rasterization.cullMode = vk::CullModeFlagBits::eNone;
    rasterization.frontFace = vk::FrontFace::eClockwise;
    rasterization.depthBiasEnable = VK_FALSE;
    rasterization.depthBiasConstantFactor = 0.0f;
    rasterization.depthBiasClamp = 0.0f;
    rasterization.depthBiasSlopeFactor = 0.0f;

    // Get Multisampling info
    vk::PipelineMultisampleStateCreateInfo multisamplingInfo = {};
    multisamplingInfo.sType = vk::StructureType::ePipelineMultisampleStateCreateInfo;
    multisamplingInfo.pNext = nullptr;
    multisamplingInfo.sampleShadingEnable = VK_FALSE;
    multisamplingInfo.rasterizationSamples = vk::SampleCountFlagBits::e1;
    multisamplingInfo.minSampleShading = 1.0f;
    multisamplingInfo.pSampleMask = nullptr;
    multisamplingInfo.alphaToCoverageEnable = VK_FALSE;
    multisamplingInfo.alphaToOneEnable = VK_FALSE;

    // Get ColorBlend Attachments
    vk::PipelineColorBlendAttachmentState colorBlendAttachment = {};
    colorBlendAttachment.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
            vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
    colorBlendAttachment.blendEnable = VK_FALSE;

    // Get viewport state
    vk::Viewport viewport;
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)Renderer::Get().getSwapchainExtent().width;
    viewport.height = (float)Renderer::Get().getSwapchainExtent().height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    vk::Rect2D scissor;
    scissor.offset = 0;
    scissor.extent = Renderer::Get().getSwapchainExtent();

    vk::PipelineViewportStateCreateInfo viewportInfo = {};
    viewportInfo.sType = vk::StructureType::ePipelineViewportStateCreateInfo;
    viewportInfo.pNext = nullptr;
    viewportInfo.viewportCount = 1;
    viewportInfo.pViewports = &viewport;
    viewportInfo.scissorCount = 1;
    viewportInfo.pScissors = &scissor;

    // Get color blend state
    vk::PipelineColorBlendStateCreateInfo colorBlending = {};
    colorBlending.sType = vk::StructureType::ePipelineColorBlendStateCreateInfo;
    colorBlending.pNext = nullptr;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = vk::LogicOp::eCopy;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;

    // Get Pipeline Layout
    vk::PipelineLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = vk::StructureType::ePipelineLayoutCreateInfo;
    layoutInfo.pNext = nullptr;
    layoutInfo.setLayoutCount = 1;
    layoutInfo.pSetLayouts = &Renderer::Get().getCameraDescriptorLayout();
    layoutInfo.pushConstantRangeCount = pushConstants.size();
    layoutInfo.pPushConstantRanges = pushConstants.data();

    pipelineLayout = Renderer::Get().device.createPipelineLayout(layoutInfo);

    vk::GraphicsPipelineCreateInfo pipelineInfo = {};
    pipelineInfo.sType = vk::StructureType::eGraphicsPipelineCreateInfo;
    pipelineInfo.pNext = nullptr;
    pipelineInfo.stageCount = stages.size();
    pipelineInfo.pStages = stages.data();
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssemblyInfo;
    pipelineInfo.pViewportState = &viewportInfo;
    pipelineInfo.pRasterizationState = &rasterization;
    pipelineInfo.pMultisampleState = &multisamplingInfo;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.layout = pipelineLayout;
    pipelineInfo.renderPass = Renderer::Get().getRenderPass();
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

    vk::Result result = Renderer::Get().device.createGraphicsPipelines(VK_NULL_HANDLE,
                                                                 1,
                                                                 &pipelineInfo,
                                                                 nullptr,
                                                                 &graphicsPipeline);

    SD_INTERNAL_ASSERT_WITH_MSG(_RENDERER_,result == vk::Result::eSuccess, "Pipeline initialisation failed." );


}

void GraphicsPipeline::cleanUp() const {
    Renderer::Get().device.destroyDescriptorSetLayout(descriptorSetLayout);
    Renderer::Get().device.destroyPipelineLayout(pipelineLayout);
    Renderer::Get().device.destroyPipeline(graphicsPipeline);
}