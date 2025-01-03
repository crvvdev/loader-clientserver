using Serilog;
using ServerCore;
using Tomlyn.Model;
using Tomlyn;

namespace Server
{
    internal class Program
    {
        static readonly string configFilePath = "config/config.toml";
        static readonly string defaultHost = "0.0.0.0";
        static readonly int defaultPort = 50000;
        static readonly string defaultApiUrl = "";
        static readonly string defaultApiKey = "";

        static async Task Main(string[] args)
        {
            Console.Title = "Server";

            Log.Logger = new LoggerConfiguration()
                    .MinimumLevel.Debug()
                    .WriteTo.Console()
                    .WriteTo.File("Logs/server_.log", rollingInterval: RollingInterval.Day)
                    .CreateLogger();

            Log.Debug($"Parsing server configuration file");

            string host = defaultHost;
            int port = defaultPort;
            string apiUrl = defaultApiUrl;
            string apiKey = defaultApiKey;

            try
            {
                string configContent = File.ReadAllText(configFilePath);
                TomlTable configToml = Toml.Parse(configContent).ToModel();

                var server = configToml["server"] as TomlTable ?? new TomlTable();
                host = server["host"] as string ?? defaultHost;
                port = server["port"] != null ? Convert.ToInt32(server["port"]) : defaultPort;
                apiUrl = server["api_url"] as string ?? defaultApiUrl;
                apiKey = server["api_key"] as string ?? defaultApiKey;

                Log.Information($"Starting server...");
            }
            catch (Exception ex)
            {
                Log.Information($"Failed to parse config.toml: {ex.Message}");
                CreateDefaultConfig();
            }

            try
            {
                var cancellationTokenSource = new CancellationTokenSource();
                var server = new WebSocketServer(host, port, apiUrl, apiKey);
                cancellationTokenSource.Token.Register(() => { server.Stop(); });

                Console.CancelKeyPress += (sender, e) =>
                {
                    e.Cancel = true;
                    cancellationTokenSource.Cancel();
                };

                server.Start();
                Log.Debug("Server started. Press Ctrl+C to exit...");

                try
                {
                    await Task.Delay(Timeout.Infinite, cancellationTokenSource.Token);
                }
                catch (TaskCanceledException)
                {

                }
            }
            catch (Exception ex)
            {
                Log.Information($"Exception starting server: {ex.Message}");
            }

            Log.Information($"Server stopped.");
        }

        static void CreateDefaultConfig()
        {
            var defaultConfig = new TomlTable
            {
                ["server"] = new TomlTable
                {
                    ["host"] = defaultHost,
                    ["port"] = defaultPort,
                    ["api_url"] = defaultApiUrl,
                    ["api_key"] = defaultApiKey,
                }
            };

            string tomlContent = Tomlyn.Toml.FromModel(defaultConfig);

            Directory.CreateDirectory(Path.GetDirectoryName(configFilePath)!);
            File.WriteAllText(configFilePath, tomlContent);
        }
    }
}
