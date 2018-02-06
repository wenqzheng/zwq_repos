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

template<typename Strat>
struct StrategyModel:StrategyConcept
{
    StrategyModel(const Strat& strat):strategy(strat) {}
    virtual void do_something() const
    {
        strategy.do_something();
    }
    virtual std::string return_something() const
    {
        return typeid(Strat).name();
    }
private:
    Strat strategy;
};

std::shared_ptr<StrategyConcept> strategy;

public:
template<typename Strat>
Strategy(const Strat& strat)
    :strategy(std::make_shared<StrategyModel<Strat>>(strat))
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
