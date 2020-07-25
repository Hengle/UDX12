#include <UDX12/Desc.h>

using namespace Ubpa;

D3D12_SHADER_RESOURCE_VIEW_DESC UDX12::Desc::SRV::Tex2D(DXGI_FORMAT format) {
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(D3D12_SHADER_RESOURCE_VIEW_DESC));
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Format = format;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.PlaneSlice = 0;
    srvDesc.Texture2D.MipLevels = 1;
    srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
    return srvDesc;
}

D3D12_SHADER_RESOURCE_VIEW_DESC UDX12::Desc::SRV::TexCube(DXGI_FORMAT format) {
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(D3D12_SHADER_RESOURCE_VIEW_DESC));
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
	srvDesc.TextureCube.MostDetailedMip = 0;
	srvDesc.TextureCube.MipLevels = 1;
	srvDesc.TextureCube.ResourceMinLODClamp = 0.0f;
	srvDesc.Format = format;
	return srvDesc;
}

D3D12_DEPTH_STENCIL_VIEW_DESC UDX12::Desc::DSV::Basic(DXGI_FORMAT format) {
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;

	ZeroMemory(&dsvDesc, sizeof(D3D12_DEPTH_STENCIL_VIEW_DESC));

	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Format = format;
	dsvDesc.Texture2D.MipSlice = 0;

	return dsvDesc;
}

D3D12_GRAPHICS_PIPELINE_STATE_DESC UDX12::Desc::PSO::Basic(
    ID3D12RootSignature* rootSig,
    D3D12_INPUT_ELEMENT_DESC* pInputElementDescs, UINT NumElements,
    ID3DBlob* VS,
    ID3DBlob* PS,
    DXGI_FORMAT rtvFormat,
    DXGI_FORMAT dsvFormat)
{
	return MRT(rootSig,
		pInputElementDescs, NumElements,
		VS, PS,
		1,
		rtvFormat,
		dsvFormat);
}

D3D12_GRAPHICS_PIPELINE_STATE_DESC UDX12::Desc::PSO::MRT(
	ID3D12RootSignature* rootSig,
	D3D12_INPUT_ELEMENT_DESC* pInputElementDescs, UINT NumElements,
	ID3DBlob* VS,
	ID3DBlob* PS,
	UINT rtNum,
	DXGI_FORMAT rtvFormat,
	DXGI_FORMAT dsvFormat)
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;

	ZeroMemory(&psoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));

	psoDesc.InputLayout = { pInputElementDescs, NumElements };
	psoDesc.pRootSignature = rootSig;
	psoDesc.VS = { reinterpret_cast<BYTE*>(VS->GetBufferPointer()), VS->GetBufferSize() };
	psoDesc.PS = { reinterpret_cast<BYTE*>(PS->GetBufferPointer()), PS->GetBufferSize() };
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = rtNum;
	for (UINT i = 0; i < rtNum; i++)
		psoDesc.RTVFormats[i] = rtvFormat;
	psoDesc.SampleDesc.Count = 1;
	psoDesc.SampleDesc.Quality = 0;
	psoDesc.DSVFormat = dsvFormat;

	return psoDesc;
}

D3D12_RESOURCE_DESC UDX12::Desc::RSRC::Basic(
	D3D12_RESOURCE_DIMENSION dimension,
	UINT64 Width,
	UINT Height,
	DXGI_FORMAT format,
	D3D12_RESOURCE_FLAGS flags)
{
	D3D12_RESOURCE_DESC desc;
	ZeroMemory(&desc, sizeof(D3D12_RESOURCE_DESC));

	desc.Dimension = dimension;
	desc.Alignment = 0;
	desc.Width = Width;
	desc.Height = Height;
	desc.DepthOrArraySize = 1;
	desc.MipLevels = 1;
	desc.Format = format;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	desc.Flags = flags;

	return desc;
}

D3D12_RESOURCE_DESC UDX12::Desc::RSRC::RT2D(
	UINT64 Width,
	UINT Height,
	DXGI_FORMAT format)
{
	return Basic(D3D12_RESOURCE_DIMENSION_TEXTURE2D, Width, Height, format, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);
}
