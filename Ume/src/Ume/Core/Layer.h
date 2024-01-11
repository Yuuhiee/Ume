#pragma once

#include "Ume/Core/Base.h"
#include "Ume/Events/Event.h"
#include "Ume/Core/Timestep.h"

namespace Ume
{
    class UME_API Layer
    {
    public:
        Layer(const std::string &name = "Layer");
        virtual ~Layer();

        virtual void OnAttach(){};
        virtual void OnDetach(){};
        virtual void OnUpdate(Timestep timestep){};
        virtual void OnImGuiRender(){};
        virtual void OnEvent(Event &e){};

        inline const std::string &GetName() const { return m_DebugName; }

    private:
        std::string m_DebugName;
    };
}
