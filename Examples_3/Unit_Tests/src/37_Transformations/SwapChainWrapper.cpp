#include "SwapChainWrapper.h"

Semaphore* SwapChainWrapper::getImageAcquiredSemaphore() { return imageAcquiredSemaphore; }

SwapChain* SwapChainWrapper::getSwapChain() { return swapChain; }
