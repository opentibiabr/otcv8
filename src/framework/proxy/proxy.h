#pragma once

#include "proxy_client.h"

class ProxyManager {
public:
    ProxyManager() : m_io(), m_guard(asio::make_work_guard(m_io))
    {

    }
    void init();
    void terminate();
    void clear();
    void setMaxActiveProxies(int value)
    {
        m_maxActiveProxies = value;
        if (m_maxActiveProxies < 1)
            m_maxActiveProxies = 1;
    }
    bool isActive();
    void addProxy(const std::string& host, uint16_t port, int priority);
    void removeProxy(const std::string& host, uint16_t port);
    uint32_t addSession(uint16_t port, std::function<void(ProxyPacketPtr)> recvCallback, std::function<void(std::error_code)> disconnectCallback);
    void removeSession(uint32_t sessionId);
    void send(uint32_t sessionId, ProxyPacketPtr packet);
    // tools
    std::map<std::string, uint32_t> getProxies();
    std::map<std::string, std::string> getProxiesDebugInfo();
    int getPing();

private:
    asio::io_context m_io;
    asio::executor_work_guard<asio::io_context::executor_type> m_guard;

    bool m_working = false;
    std::thread m_thread;

    int m_maxActiveProxies = 2;

    std::list<std::weak_ptr<Proxy>> m_proxies;
    std::list<std::weak_ptr<Session>> m_sessions;
};

extern ProxyManager g_proxy;
