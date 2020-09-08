#ifndef DEF__ERROR_HPP__
#define DEF__ERROR_HPP__

namespace HeartBeat
{
    enum EErrorType
    {
        ET_UNKNOWN = -1,
        ET_NO_ERROR = 0,
        ET_KEY_NOT_MATCH,
        ET_CMD_NOT_SUPPORT,
        ET_CONTENT_ERROR,
        ET_REQUEST_ERROR,

        ET_DNS_UPDATE_ERROR = 1000
    };
}

#endif