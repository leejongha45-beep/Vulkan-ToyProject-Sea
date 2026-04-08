#pragma once

class IRenderClass
{
public:
	virtual ~IRenderClass() = default;

	IRenderClass(const IRenderClass&)			 = delete;
	IRenderClass& operator=(const IRenderClass&) = delete;

	IRenderClass(IRenderClass&&)			= delete;
	IRenderClass& operator=(IRenderClass&&) = delete;

protected:
	IRenderClass() = default;

public:
	virtual void create()  = 0;
	virtual void destroy() = 0;
};