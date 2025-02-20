#pragma once

#include <string>
#include <deque>
#include <condition_variable>
#include "table.h"

#define MQ_RESPONSE_MAX_COUNT (4*1024*1024)
#define MQ_SIZE 100
#define MQ_MAX_RETRY 10
#define MQ_POLL_TIMEOUT (1000)
#define MQ_WATERMARK 10000

namespace swss {

class ZmqMessageHandler
{
public:
    virtual ~ZmqMessageHandler() {};
    virtual void handleReceivedData(std::shared_ptr<KeyOpFieldsValuesTuple> pkco) = 0;
};

class ZmqServer
{
public:
    /* The default value of pop batch size is 128 */
    static constexpr int DEFAULT_POP_BATCH_SIZE = 128;

    ZmqServer(const std::string& endpoint);
    ~ZmqServer();

    void registerMessageHandler(
                                const std::string dbName,
                                const std::string tableName,
                                ZmqMessageHandler* handler);

private:
    void handleReceivedData(const char* buffer, const size_t size);

    void mqPollThread();
    
    ZmqMessageHandler* findMessageHandler(const std::string dbName, const std::string tableName);

    volatile bool m_runThread;

    std::shared_ptr<std::thread> m_mqPollThread;

    std::string m_endpoint;

    std::map<std::string, std::map<std::string, ZmqMessageHandler*>> m_HandlerMap;
};

}
