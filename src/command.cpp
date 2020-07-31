#include "command.hpp"

namespace HeartBeat
{
    const ::std::string CCommand::s_strZoneIDFormat = "curl -s -X GET \"https://api.cloudflare.com/client/v4/zones?name=%s&status=active\" \
                                                        -H \"Authorization: Bearer %s\" \
                                                        -H \"Content-Type: application/json\" | jq -r '{\"result\"}[] | .[0] | .id'";

    const ::std::string CCommand::s_strRecordIDFormat = "curl -s -X GET \"https://api.cloudflare.com/client/v4/zones/%s/dns_records?type=A&name=%s\" \
                                                            -H \"Authorization: Bearer %s\" \
                                                            -H \"Content-Type: application/json\" | jq -r '{\"result\"}[] | .[0] | .id'";
}