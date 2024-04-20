#ifndef OFFSCREENRENDERPASS_H
#define OFFSCREENRENDERPASS_H

#include "RenderPass.h"

class OffScreenRenderPass : public RenderPass
{
public:

	OffScreenRenderPass(const Window& window);

	~OffScreenRenderPass();

	void Create() override;

private:
};


#endif // OFFSCREENRENDERPASS_H