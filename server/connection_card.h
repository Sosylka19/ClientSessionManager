#include <string>


class ConnectionCard
{
public:
    uint16_t GetPort() const;
    void SetPort(uint16_t port);
private:
    uint16_t port_; 
};