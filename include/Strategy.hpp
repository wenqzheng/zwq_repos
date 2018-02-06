#include <memory>
#include <string>

class Strategy
{
struct StrategyConcept
{
    virtual ~StrategyConcept() {}
    virtual void do_something() const = 0;
    virtual std::string return_something() const = 0;
};

template<typename S>
struct StrategyModel:StrategyConcept
{
    StrategyModel(const S& s):strategy(s) {}
    virtual void do_something() const
    {
        strategy.do_something();
    }
    virtual std::string return_something() const
    {
        return typeid(S).name();
    }
private:
    S strategy;
};

std::shared_ptr<StrategyConcept> strategy;

public:
template<typename S>
Strategy(const S& s)
    :strategy(std::make_shared<StrategyModel<S>>(s))
{}

Strategy() = default;

std::string return_something() const
{
    return strategy->return_something();
}

void do_something() const
{
    strategy->do_something();
}

};
