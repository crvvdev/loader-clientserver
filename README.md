
# Loader project

Hello, this is a project that i was working on some time ago, as i am not going to use it anytime soon i decided to publish it for free, it's missing some core functionality like streaming "cheats" and some other relevant needed stuff. Anyways, it's a good example and it's helpful to show how to do things in the right tracks.


# Client

It's coded in C++ and it has a graphical interface using Win32 GUI.

 - Metadata gathering (general computer information such as GPU, CPU, OS, Motherboard, Disk drives, Network adapters and some other fingerprints methods)
 - Websocket client connection with server
 - Basic graphical user interface for login and user panel
 - [VMProtect](https://vmpsoft.com/) ready
 - Libraries used: [websocketpp](https://github.com/zaphoyd/websocketpp), [OpenSSL 3.0](https://github.com/openssl/openssl/releases/tag/openssl-3.4.0), [JSON for modern C++](https://github.com/nlohmann/json), [scope_guard](https://github.com/Neargye/scope_guard), [ASIO](https://github.com/boostorg/asio)

## Server

It's coded in C#, it's a simple Websocket server.

 - Can be easily configured using toml file (config/config.toml)
 - Logs are automatically saved on disk
 - .NET Core 8.0, so it's portable and work for both Linux and Windows
 - Nu.get packages: Serilog (for logging), Fleck (WebSocket) and Tomlyn (for TOML parsing).
 - Capable of handling version update for client and login
 - Connects to API endpoint to perform actions

## About

This is a sample project with a lot of functionalities stripped, if you're interested in a custom solution that fits your needs please contact me.

Implemented:

 - Versioning, login
 - Server/client networking
 - Metadata gathering
 - Automatically store credentials on disk upon successful login
 - Basic Win32 GUI API (Button, Checkbox, EditBox, ListView, ProgressBar, RichEditControl, Static, StatusBar, Window, Dialog and Thunk)
 - Makes use of solid and secure libraries like OpenSSL
 
 Missing:
 - TLS support (straighforward to be implemented)
 - Network protocol layer security (encrypt communication with strong symmetric encryption like AES)
 - Injection logic (cheat streaming, driver, etc)
 - Other security miscellaneous
 - API endpoint code
 - Some other specific customized solutions

## Images

Client

![2](https://github.com/user-attachments/assets/06080251-f84c-42f9-9502-799ed6b68b45)
![Sem título](https://github.com/user-attachments/assets/4aa4145a-be7f-4c28-a76e-0818ebe21058)

Server and config

![3](https://github.com/user-attachments/assets/8bba33a3-5efd-4846-96cb-ab4685c6b8cd)
![4](https://github.com/user-attachments/assets/46ccac44-24c6-4fe8-8bd6-1f3239ac79c0)

## Contact

Check out my [blog](https://crvvdev.github.io/)!

[<img align="middle" alt="Twitter" width="32px" src="https://cdn.jsdelivr.net/npm/simple-icons@v3/icons/twitter.svg" />][twitter] [<img align="middle" alt="LinkedIn" width="32px" src="https://cdn.jsdelivr.net/npm/simple-icons@v3/icons/linkedin.svg" />][linkedin] [<img align="middle" alt="Telegram" width="32px" src="https://cdn.jsdelivr.net/npm/simple-icons@v3/icons/telegram.svg" />][telegram] [<img align="middle" alt="Telegram" width="32px" src="https://cdn.jsdelivr.net/npm/simple-icons@v3/icons/discord.svg" />][discord]

[twitter]: https://twitter.com/crvvdev
[telegram]: https://t.me/crvvdev
[linkedin]: https://www.linkedin.com/in/ricardo-carvalho-4677b1194/
[discord]: https://discord.com/invite/Bxrk4EpQ

## Donate

USDC: 0xEF5F5a1D633fc2A26b88D0C1578FA3C7514D7eEd

ETH: 0xEF5F5a1D633fc2A26b88D0C1578FA3C7514D7eEd

Tether USD: 0xEF5F5a1D633fc2A26b88D0C1578FA3C7514D7eEd

Paypal: https://www.paypal.com/donate/?hosted_button_id=6Q3LTUE27NH6G
