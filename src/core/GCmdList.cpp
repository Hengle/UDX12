#include <UDX12/GCmdList.h>

using namespace Ubpa;

void UDX12::GCmdList::Reset(ID3D12CommandAllocator* pAllocator,
    ID3D12PipelineState* pInitialState) {
    ThrowIfFailed(raw->Reset(pAllocator, pInitialState));
}

void UDX12::GCmdList::ResourceBarrier(ID3D12Resource* resource, D3D12_RESOURCE_STATES from, D3D12_RESOURCE_STATES to) {
    raw->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(resource,
        from, to));
}

void UDX12::GCmdList::RSSetViewport(D3D12_VIEWPORT viewport) {
    raw->RSSetViewports(1, &viewport);
}

void UDX12::GCmdList::RSSetScissorRect(D3D12_RECT rect) {
    raw->RSSetScissorRects(1, &rect);
}

void UDX12::GCmdList::ClearRenderTargetView(D3D12_CPU_DESCRIPTOR_HANDLE RenderTargetView, const FLOAT color[4]) {
    raw->ClearRenderTargetView(RenderTargetView, color, 0, nullptr);
}

void UDX12::GCmdList::ClearDepthStencilView(D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView) {
    raw->ClearDepthStencilView(DepthStencilView, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.f, 0, 0, nullptr);
}

void UDX12::GCmdList::OMSetRenderTarget(
    D3D12_CPU_DESCRIPTOR_HANDLE rendertarget,
    D3D12_CPU_DESCRIPTOR_HANDLE depthstencil)
{
    raw->OMSetRenderTargets(1, &rendertarget, true, &depthstencil);
}

void UDX12::GCmdList::DrawIndexed(
    UINT IndexCount,
    UINT StartIndexLocation,
    INT BaseVertexLocation)
{
    raw->DrawIndexedInstanced(IndexCount, 1, StartIndexLocation, BaseVertexLocation, 0);
}
