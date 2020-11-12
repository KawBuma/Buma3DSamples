#include "pch.h"
#include "LazyDelegate.h"
#include <unordered_set>

namespace buma
{

class DefaultDelegate : public IDelegate
{
public:
    DefaultDelegate()
        : events {}
    {
    }

    virtual ~DefaultDelegate()
    {
    }

    void RegisterEvent(std::weak_ptr<IEvent> _event) override
    {
        events.emplace_back(_event);
    }
    void RemoveEvent(std::weak_ptr<IEvent> _event) override
    {
        auto find = std::find_if(events.begin(), events.end(), [&_event](const std::weak_ptr<IEvent>& _ev) { return !_ev.expired() && (_ev.lock() == _event.lock()); });
        if (find != events.end())
            events.erase(find);
    }
    void ExecuteEvents(IEventArgs* _args) override
    {
        if (events.empty())
            return;

        for (auto& i : events)
        {
            if (!i.expired())
                i.lock()->Execute(_args);
        }
        EraseExpiredEvents();
    }

private:
    void EraseExpiredEvents()
    {
        auto it_remove = std::remove_if(events.begin(), events.end(), [](const std::weak_ptr<IEvent>& _event) { return _event.expired(); });
        events.erase(it_remove, events.end());
    }

private:
    std::vector<std::weak_ptr<IEvent>>  events;

};

std::shared_ptr<IDelegate> IDelegate::CreateDefaultDelegate()
{
    return std::make_shared<DefaultDelegate>();
}


}// namespace buma
