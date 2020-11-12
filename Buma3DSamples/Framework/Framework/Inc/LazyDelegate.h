#pragma once

namespace buma
{

struct IEventArgs
{
    virtual ~IEventArgs() {}
};

struct IEvent : std::enable_shared_from_this<IEvent>
{
    virtual ~IEvent() {}
    virtual void Execute(IEventArgs* _args) = 0;
    void operator()(IEventArgs* _args) { Execute(_args); }
};

struct IDelegate
{
    virtual ~IDelegate() {}

    static std::shared_ptr<IDelegate> CreateDefaultDelegate();

    virtual void RegisterEvent(std::weak_ptr<IEvent> _event) = 0;
    void operator+=(std::weak_ptr<IEvent> _event) { RegisterEvent(_event); }

    virtual void RemoveEvent(std::weak_ptr<IEvent> _event) = 0;
    void operator-=(std::weak_ptr<IEvent> _event) { RemoveEvent(_event); }

    virtual void ExecuteEvents(IEventArgs* _args) = 0;
    void operator()(IEventArgs* _args) { ExecuteEvents(_args); }

};


}// namespace buma
