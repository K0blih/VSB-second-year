#include <iostream>
#include <cstring>

class IPAddress {
public:
    IPAddress(uint8_t a, uint8_t b, uint8_t c, uint8_t d) {
        this->address = static_cast<uint32_t>(a) << 24 | 0;
    }

private:
    uint32_t address;
};

/*
 * Try to design APIs that cannot be used in a wrong way - such APIs are called "sound".
 *
 * Consider the network client class `Client1` below, which is very unsound.
 * You can create a client and then misuse it easily:
 * - send a message without calling connect or authenticate
 * - authenticate before connecting
 * - connecting multiple times
 * - forgetting to call close
 * - calling close multiple times
 *
 * These errors will only show up during runtime, not when compiling.
 */

/*
 * You have to call connect first, then authenticate, and then you can send messages.
 */
class Client1 {
public:
    Client1() = default;

    void connect(IPAddress address) {
        // some connect logic
    }

    void authenticate(std::string password) {
        // some authentication logic
    }

    void send_message(std::string msg) {}

    void close() {}
};

void use_client1() {
    Client1 client{};
    client.connect(IPAddress{1, 2, 3, 4});
//     client.send_message("asd"); // OOPS!
    client.authenticate("foo");
    client.send_message("asd");
//    client.connect(IPAddress{1, 2, 3, 5}); // OOPS!
    client.close();
//     client.send_message("asd"); // OOPS!
}

// What about disabling the option to misuse the API altogether?
// We can create a ConnectedClient class that will have the invariant that it is connected and
// authenticated. Therefore, we cannot misuse connect/authenticate.
// It will not be possible to create ConnectedClient without an active connection.
// It will not provide a close method, but it will close itself automatically in its destructor.
// Therefore, we cannot misuse close.

struct Connection { /* ... */ };

class ConnectedClient {
public:
    static ConnectedClient
    ConnectAndAuthenticate(IPAddress ipAddress, const std::string& password) {
        Connection connection;
        // connect(connection);
        // authenticate(connection, passowrd);
        return ConnectedClient{connection};
    }

    void send_message(std::string msg) {}

    ~ConnectedClient() {
        std::cout << "Disconnecting" << std::endl;
    }

private:
    explicit ConnectedClient(Connection connection) : connection(connection) {}

    Connection connection;
};

// If we want to allow valid substates (connected, but unauthenticated client), we could introduce
// more types:

struct AuthenticatedClient {
    void send_message(std::string msg) {}

    // ...
};

struct ConnectedClient1 {
public:
    explicit ConnectedClient1(Connection connection) : connection(connection) {}

    AuthenticatedClient authenticate(std::string password) {
        // perform authentication...
        return AuthenticatedClient{};
    }

private:
    Connection connection;
};

Connection connect(IPAddress ip) {
    return Connection{};
}

int main() {
    ConnectedClient1 client_conn = ConnectedClient1{connect(IPAddress{1, 2, 3, 4})};
    AuthenticatedClient client = client_conn.authenticate("foo");
    client.send_message("hello");

    return 0;
}
