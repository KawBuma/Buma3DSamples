#include "pch.h"
#include "LazyDelegate.h"

namespace buma
{

class DefaultDelegate : public IDelegate
{
public:
    DefaultDelegate()
        : events                {}
        , num_events_to_execute {}
        , events_to_execute     {}
    {
    }

    virtual ~DefaultDelegate()
    {
    }

    void RegisterEvent(const std::weak_ptr<IEvent>& _event) override
    {
        events.emplace_back(_event);
    }
    void RemoveEvent(const std::weak_ptr<IEvent>& _event) override
    {
        auto find = std::find_if(events.begin(), events.end(), [&_event](const std::weak_ptr<IEvent>& _ev) { return !_ev.expired() && (_ev.lock() == _event.lock()); });
        if (find != events.end())
            events.erase(find);
    }
    void ExecuteEvents(IEventArgs* _args) override
    {
        if (events.empty())
            return;

        if (events_to_execute.size() < events.size())
            events_to_execute.resize(events.size());

        EraseExpiredEvents();
        auto events_to_execute_data = events_to_execute.data();
        for (size_t i = 0; i < num_events_to_execute; i++)
        {
            events_to_execute_data[i]->Execute(_args);
        }
    }

private:
    void EraseExpiredEvents()
    {
        num_events_to_execute = 0;
        auto it_remove = std::remove_if(events.begin(), events.end(), [this](const std::weak_ptr<IEvent>& _event)
        {
            if (_event.expired())
                return true;

            events_to_execute.data()[num_events_to_execute++] = _event.lock().get();
            return false;
        });
        events.erase(it_remove, events.end());
    }

private:
    std::vector<std::weak_ptr<IEvent>>  events;

    size_t                              num_events_to_execute;
    std::vector<IEvent*>                events_to_execute;

};

std::shared_ptr<IDelegate> IDelegate::CreateDefaultDelegate()
{
    return std::make_shared<DefaultDelegate>();
}


}// namespace buma
