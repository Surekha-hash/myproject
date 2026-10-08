#pragma once

#include <string>
#include <sstream>
#include <vector>

namespace vse {

enum class MessageType {
    CALL_SETUP,
    CALL_ACCEPTED,
    CALL_REJECTED,
    CALL_RELEASE,
    BILLING_START,
    BILLING_STARTED,
    BILLING_STOP,
    BILLING_RESULT,
    ERROR
};

inline std::string messageTypeToString(MessageType type)
{
    switch (type) {
        case MessageType::CALL_SETUP:
            return "CALL_SETUP";

        case MessageType::CALL_ACCEPTED:
            return "CALL_ACCEPTED";

        case MessageType::CALL_REJECTED:
            return "CALL_REJECTED";

        case MessageType::CALL_RELEASE:
            return "CALL_RELEASE";

        case MessageType::BILLING_START:
            return "BILLING_START";

        case MessageType::BILLING_STARTED:
            return "BILLING_STARTED";

        case MessageType::BILLING_STOP:
            return "BILLING_STOP";

        case MessageType::BILLING_RESULT:
            return "BILLING_RESULT";

        case MessageType::ERROR:
            return "ERROR";
    }

    return "UNKNOWN";
}

inline std::vector<std::string> split(
    const std::string& input,
    char delimiter)
{
    std::vector<std::string> result;
    std::stringstream ss(input);
    std::string item;

    while (std::getline(ss, item, delimiter)) {
        result.push_back(item);
    }

    return result;
}

}
//Our protocol will initially be simple:

//CALL_SETUP|CALL-1001|46701112233|46709998877
//and:

//CALL_ACCEPTED|CALL-1001
//Billing:

//BILLING_START|CALL-1001

