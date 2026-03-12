using System.Text.Json;

namespace cv4
{
    public class JsonLogger : IMyLogger
    {
        public async Task Log(string message)
        {
            string path = "log.json";
            List<string> data;

            if (!File.Exists(path))
            {
                data = new List<string>();
            }
            else
            {
                data = JsonSerializer.Deserialize<List<string>>(
                        await File.ReadAllTextAsync(path)
                    )!;
            }

            data.Add(message);

            await File.WriteAllTextAsync(path, JsonSerializer.Serialize(data));
        }
    }
}
