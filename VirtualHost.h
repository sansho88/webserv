//
// Created by Alexis Lavaud on 05/04/2023.
//

#ifndef WEBSERV_VIRTUALHOST_H
#define WEBSERV_VIRTUALHOST_H
#include "Http/RequestHandler.h"
#include <string>
#include <vector>
#include "Network/NetworkAddress4.h"

class ServerHost;
class MountPoint;

class VirtualHost : public IRequestHandler
{
public:
    typedef std::vector<std::string> tServerNameList;

    VirtualHost();
    ~VirtualHost();

    void AddListenAddress(const NetworkAddress4 &addr);

    const std::vector<NetworkAddress4> &GetBindAddresses() const
    {
        return m_bindAddresses;
    }

    void HandleRequest(Request *request, Response *response);

    MountPoint *GetRootMountPoint() const
    {
        return m_rootMountPoint;
    }

    const tServerNameList &GetServerNames() const;

    tServerNameList m_serverNames;
    std::vector<NetworkAddress4> m_bindAddresses;
    std::vector<MountPoint *> m_mountPoints;

private:
    void DispatchRequest(MountPoint *mountPoint, Request *request, Response *response);

    MountPoint *m_rootMountPoint;
};

#endif //WEBSERV_VIRTUALHOST_H
