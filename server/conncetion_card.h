#include <string>


class Connection
{
public:
    std::string GetIp() const;
    uint16_t GetPort() const;
private:
    struct Adress 
    {
        std::string ip_{"0.0.0.0"};
        uint16_t port_{80};
    };
    
};

class ConnectionCard: Connection
{
public:
    ConnectionCard(const std::string& name);
    
    std::string GetStatus() const;
    std::string GetName() const;

    void SetName(const std::string& name);
    void SetStatus(const std::string& status);
private:
    std::string name_;
    std::string status_{"offline"};
};