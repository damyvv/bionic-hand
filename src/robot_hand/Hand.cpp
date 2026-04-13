#include "Hand.hpp"

Hand::Hand(std::array<Finger, 5>&& fingers)
    : fingers(std::move(fingers))
{
}

void Hand::OpenFinger(FingerId fingerId, float percentage)
{
    GetFinger(fingerId).Open(percentage);
}

void Hand::OpenFinger(FingerId fingerId)
{
    GetFinger(fingerId).Open();
}

void Hand::CloseFinger(FingerId fingerId)
{
    GetFinger(fingerId).Close();
}

void Hand::OpenFingers(float percentage)
{
    for (auto& finger : fingers)
    {
        finger.Open(percentage);
    }
}

void Hand::OpenFingers()
{
    for (auto& finger : fingers)
    {
        finger.Open();
    }
}

void Hand::CloseFingers()
{
    for (auto& finger : fingers)
    {
        finger.Close();
    }
}

Finger& Hand::GetFinger(FingerId fingerId)
{
    really_assert(static_cast<std::size_t>(fingerId) < fingers.size());
    return fingers[static_cast<std::size_t>(fingerId)];
}
