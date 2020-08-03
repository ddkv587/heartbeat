#include "command.hpp"

namespace HeartBeat
{
    const ::std::string CDNSCommand::s_strZoneIDFormat     = "curl -s -X GET \"https://api.cloudflare.com/client/v4/zones?name=%s&status=active\" \
                                                        -H \"Authorization: Bearer %s\" \
                                                        -H \"Content-Type: application/json\" | jq -r '{\"result\"}[] | .[0] | .id'";

    const ::std::string CDNSCommand::s_strRecordIDFormat   = "curl -s -X GET \"https://api.cloudflare.com/client/v4/zones/%s/dns_records?type=A&name=%s\" \
                                                            -H \"Authorization: Bearer %s\" \
                                                            -H \"Content-Type: application/json\" | jq -r '{\"result\"}[] | .[0] | .id'";

    const ::std::string CDNSCommand::s_strUpdateDNSFormat  = "curl -s -X PUT \"https://api.cloudflare.com/client/v4/zones/%s/dns_records/%s\" \
                                                            -H \"Authorization: Bearer %s\" \
                                                            -H \"Content-Type: application/json\" \
                                                            --data '{\"type\":\"%s\",\"name\":\"%s\",\"content\":\"%s\",\"ttl\":%d,\"proxied\":%s}'";
}