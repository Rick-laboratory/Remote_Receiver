#include "UDP_Receiver.h"
#include "command.hpp"
#include <iostream>
#include <stdexcept>
#include <vector>

namespace {
void require(bool condition, const char* message) {
    if (!condition) throw std::runtime_error(message);
}
} // namespace

int main() {
    try {
        for (const auto text : {"0 2 0", "1 65 0", "2 65 0", "3 -100 200", "4 -120 0", "5 0 0", "6 0 0"}) {
            (void)parse_command(text);
        }
        require(parse_command(std::string("3 12 34\0", 8)).second == 34, "Legacy NUL rejected.");
        std::vector<std::string> invalid{"", "7 0 0", "-1 0 0", "0 65535 0", "1 256 0",
            "1 65", "1 65 0 0", "1 65 0junk", "1 65 1", "6 1 0", "3 9999999999999 2"};
        invalid.emplace_back("1 65\0 0", 7);
        invalid.emplace_back(129, '1');
        for (const auto& text : invalid) {
            bool rejected = false;
            try { (void)parse_command(text); } catch (const std::invalid_argument&) { rejected = true; }
            require(rejected, "Malformed packet accepted.");
        }

        UDP_Receiver receiver(0);
        const SOCKET sender = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        require(sender != INVALID_SOCKET, "Cannot create test sender.");
        struct Close { SOCKET socket; ~Close() { closesocket(socket); } } close{sender};
        sockaddr_in destination{};
        destination.sin_family = AF_INET;
        destination.sin_port = htons(receiver.Port());
        destination.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
        for (const std::string& payload : {std::string("3 123 456"), std::string("5 0 0"), std::string(65507, 'x')}) {
            require(sendto(sender, payload.data(), static_cast<int>(payload.size()), 0,
                    reinterpret_cast<const sockaddr*>(&destination), sizeof(destination)) ==
                    static_cast<int>(payload.size()), "Send failed.");
            const auto received = receiver.Receive();
            require(received && received->payload == payload && received->sender == "127.0.0.1",
                    "Receive changed packet length/data or peer.");
        }
        require(!receiver.Receive(), "Idle receive should time out.");
        std::cout << "Protocol validation, exact UDP datagrams and timeout passed; no input applied.\n";
        return 0;
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
