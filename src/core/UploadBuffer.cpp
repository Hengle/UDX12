#include <UDX12/UploadBuffer.h>

#include <DirectXHelpers.h>

using namespace Ubpa;

UDX12::UploadBuffer::UploadBuffer(ID3D12Device* device, UINT64 size, D3D12_RESOURCE_FLAGS flag)
    : size{ size }
{
    assert(size > 0);

    ThrowIfFailed(device->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Buffer(size, flag),
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&resource)));

    ThrowIfFailed(resource->Map(0, nullptr, reinterpret_cast<void**>(&mappedData)));
}

UDX12::UploadBuffer::~UploadBuffer() {
    if(resource)
        resource->Unmap(0, nullptr);
}

void UDX12::UploadBuffer::Set(UINT64 offset, const void* data, UINT64 size) {
    assert(size > 0 && data);
    assert(offset + size <= Size());
    memcpy(mappedData + offset, data, size);
}

void UDX12::UploadBuffer::CopyConstruct(
	size_t dstOffset, size_t srcOffset, size_t numBytes,
	ID3D12Device* device,
	ID3D12GraphicsCommandList* cmdList,
	D3D12_RESOURCE_STATES afterState,
	ID3D12Resource** pBuffer, // out com ptr
	D3D12_RESOURCE_FLAGS resFlags
) {
	assert(resource);
	auto desc = CD3DX12_RESOURCE_DESC::Buffer(numBytes, resFlags);

	CD3DX12_HEAP_PROPERTIES heapProperties(D3D12_HEAP_TYPE_DEFAULT);

	ComPtr<ID3D12Resource> res;
	ThrowIfFailed(device->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_GRAPHICS_PPV_ARGS(res.GetAddressOf())));

	CopyAssign(
		dstOffset, srcOffset, numBytes,
		cmdList, res.Get(), D3D12_RESOURCE_STATE_COPY_DEST
	);
	DirectX::TransitionResource(cmdList, res.Get(), D3D12_RESOURCE_STATE_COPY_DEST, afterState);

	*pBuffer = res.Detach();
}

void UDX12::UploadBuffer::MoveConstruct(
	size_t dstOffset, size_t srcOffset, size_t numBytes,
	ResourceDeleteBatch& deleteBatch,
	ID3D12Device* device,
	ID3D12GraphicsCommandList* cmdList,
	D3D12_RESOURCE_STATES afterState,
	ID3D12Resource** pBuffer, // out com ptr
	D3D12_RESOURCE_FLAGS resFlags
) {
	CopyConstruct(
		dstOffset, srcOffset, numBytes,
		device, cmdList, afterState, pBuffer, resFlags
	);
	Delete(deleteBatch);
}

void UDX12::UploadBuffer::CopyAssign(
	size_t dstOffset, size_t srcOffset, size_t numBytes,
	ID3D12GraphicsCommandList* cmdList,
    ID3D12Resource* dst,
    D3D12_RESOURCE_STATES state
) {
    assert(resource);
	assert(dst->GetDesc().Dimension == D3D12_RESOURCE_DIMENSION_BUFFER);

	DirectX::TransitionResource(cmdList, dst, state, D3D12_RESOURCE_STATE_COPY_DEST);
	cmdList->CopyBufferRegion(dst, dstOffset, resource.Get(), srcOffset, size);
	DirectX::TransitionResource(cmdList, dst, D3D12_RESOURCE_STATE_COPY_DEST, state);
}

void UDX12::UploadBuffer::MoveAssign(
	size_t dstOffset, size_t srcOffset, size_t numBytes,
	ResourceDeleteBatch& deleteBatch,
	ID3D12GraphicsCommandList* cmdList,
    ID3D12Resource* dst,
    D3D12_RESOURCE_STATES state
) {
	CopyAssign(
		dstOffset, srcOffset, numBytes,
		cmdList, dst, state
	);

	Delete(deleteBatch);
}


void UDX12::UploadBuffer::Delete(ResourceDeleteBatch& deleteBatch) {
	resource->Unmap(0, nullptr);

	deleteBatch.Add(resource.Get());

	resource.Detach();
	mappedData = nullptr;
	size = 0;
}

// ==================================================

UDX12::DynamicUploadBuffer::DynamicUploadBuffer(ID3D12Device* device, D3D12_RESOURCE_FLAGS flag)
    : device{ device }, flag{ flag }{}

ID3D12Resource* UDX12::DynamicUploadBuffer::GetResource() const noexcept {
    return buffer ? buffer->GetResource() : nullptr;
}

UINT64 UDX12::DynamicUploadBuffer::Size() const noexcept {
    return buffer ? buffer->Size() : 0;
}

void UDX12::DynamicUploadBuffer::Reserve(size_t size) {
	if (size <= Size())
		return;

	auto newBuffer = std::make_unique<UDX12::UploadBuffer>(device, size, flag);
    if(buffer)
        newBuffer->Set(0, buffer->GetMappedData(), buffer->Size());

    buffer = std::move(newBuffer);
}

void UDX12::DynamicUploadBuffer::FastReserve(size_t size) {
	if (size <= Size())
		return;

    buffer = std::make_unique<UDX12::UploadBuffer>(device, size, flag);
}

void UDX12::DynamicUploadBuffer::Set(UINT64 offset, const void* data, UINT64 size) {
    assert(buffer);
    buffer->Set(offset, data, size);
}

void UDX12::DynamicUploadBuffer::CopyConstruct(
	size_t dstOffset, size_t srcOffset, size_t numBytes,
	ID3D12Device* device,
	ID3D12GraphicsCommandList* cmdList,
	D3D12_RESOURCE_STATES afterState,
	ID3D12Resource** pBuffer, // out com ptr
	D3D12_RESOURCE_FLAGS resFlags
) {
	assert(buffer);
	buffer->CopyConstruct(
		dstOffset, srcOffset, numBytes,
		device, cmdList, afterState, pBuffer, resFlags
	);
}

void UDX12::DynamicUploadBuffer::MoveConstruct(
	size_t dstOffset, size_t srcOffset, size_t numBytes,
	ResourceDeleteBatch& deleteBatch,
	ID3D12Device* device,
	ID3D12GraphicsCommandList* cmdList,
	D3D12_RESOURCE_STATES afterState,
	ID3D12Resource** pBuffer, // out com ptr
	D3D12_RESOURCE_FLAGS resFlags
) {
	assert(buffer);
	buffer->MoveConstruct(
		dstOffset, srcOffset, numBytes,
		deleteBatch, device, cmdList, afterState, pBuffer, resFlags
	);
	buffer.reset();
}

void UDX12::DynamicUploadBuffer::CopyAssign(
	size_t dstOffset, size_t srcOffset, size_t numBytes,
	ID3D12GraphicsCommandList* cmdList,
	ID3D12Resource* dst,
	D3D12_RESOURCE_STATES state
) {
	assert(buffer);
	buffer->CopyAssign(
		dstOffset, srcOffset, numBytes,
		cmdList, dst, state
	);
}

void UDX12::DynamicUploadBuffer::MoveAssign(
	size_t dstOffset, size_t srcOffset, size_t numBytes,
	ResourceDeleteBatch& deleteBatch,
	ID3D12GraphicsCommandList* cmdList,
	ID3D12Resource* dst,
	D3D12_RESOURCE_STATES state
) {
	assert(buffer);
	buffer->MoveAssign(
		dstOffset, srcOffset, numBytes,
		deleteBatch, cmdList, dst, state
	);
	buffer.reset();
}
